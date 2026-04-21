//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// W25Q512 QSPI external flash driver using ChibiOS WSPIv1 (QUADSPIv1 LLD).
//
// Hardware: W25Q512 64 MB QSPI flash on the STM32F769 QUADSPI peripheral.
// Pin mapping (driven by board.c via ChibiOS PAL):
//   NCS  = PB6  AF10   (chip select, active low)
//   CLK  = PF10 AF9    (SCLK)
//   D0   = PD11 AF9    (SIO0 / MOSI)
//   D1   = PD12 AF9    (SIO1 / MISO)
//   D2   = PE2  AF9    (SIO2 / /WP)
//   D3   = PD13 AF9    (SIO3 / /HOLD)
//
// All flash operations use 1-line (SPI-compatible) indirect mode.
// QSPI clock: 216 MHz / 3 (prescaler=2 in mcuconf.h) = 72 MHz.
// Addressing: 24-bit (3-byte) — all MCUboot slot offsets are well below 16 MB.

#include "hal.h"
#include "w25q512_qspi_chibios.h"

// clang-format off

/* W25Q512 command set (1-line SPI mode) */
#define CMD_WRITE_ENABLE         0x06U
#define CMD_WRITE_DISABLE        0x04U
#define CMD_READ_STATUS1         0x05U
#define CMD_READ_JEDEC_ID        0x9FU
#define CMD_READ_DATA            0x03U
#define CMD_PAGE_PROGRAM         0x02U
#define CMD_BLOCK_ERASE_64K      0xD8U

/* W25Q512 Status Register 1 bits */
#define SR1_BUSY                 0x01U

/* JEDEC manufacturer + device ID for W25Q512 */
#define W25Q512_JEDEC_MFR        0xEFU
#define W25Q512_JEDEC_TYPE       0x40U
#define W25Q512_JEDEC_CAP        0x20U  // 2^0x20 = 64 MB

/* Flash geometry */
#define PAGE_SIZE                256U
#define BLOCK_SIZE_64K           (64U * 1024U)

// clang-format on

// WSPI driver instance (QUADSPIv1 LLD uses WSPID1).
static WSPIDriver *const s_wdrv = &WSPID1;

// WSPI configuration: indirect 1-line mode, 24-bit address, no alternate bytes.
// DCR: FSIZE = 25 (2^(25+1) = 64 MB), CSHT = 1 (2 QSPI cycles CS high).
// Prescaler is set in mcuconf.h (STM32_WSPI_QUADSPI1_PRESCALER_VALUE = 2).
static const WSPIConfig s_wspi_cfg = {
    .end_cb  = NULL,
    .dcr     = STM32_DCR_FSIZE(25U) | STM32_DCR_CSHT(1U),
};

// Helper: send a command with no address, no data.
static void wspi_cmd(uint32_t cmd)
{
    wspi_command_t desc = {
        .cmd   = cmd,
        .cfg   = WSPI_CFG_CMD_MODE_ONE_LINE | WSPI_CFG_ADDR_MODE_NONE |
                 WSPI_CFG_ALT_MODE_NONE     | WSPI_CFG_DATA_MODE_NONE,
        .addr  = 0U,
        .alt   = 0U,
        .dummy = 0U,
    };
    wspiCommand(s_wdrv, &desc);
}

// Helper: read @p len bytes after sending @p cmd (no address).
static void wspi_cmd_read(uint32_t cmd, uint8_t *buf, size_t len)
{
    wspi_command_t desc = {
        .cmd   = cmd,
        .cfg   = WSPI_CFG_CMD_MODE_ONE_LINE | WSPI_CFG_ADDR_MODE_NONE |
                 WSPI_CFG_ALT_MODE_NONE     | WSPI_CFG_DATA_MODE_ONE_LINE,
        .addr  = 0U,
        .alt   = 0U,
        .dummy = 0U,
    };
    wspiReceive(s_wdrv, &desc, len, buf);
}

// Helper: poll SR1_BUSY until the device is idle.
static void wait_busy(void)
{
    uint8_t sr;
    do
    {
        wspi_cmd_read(CMD_READ_STATUS1, &sr, 1U);
    } while (sr & SR1_BUSY);
}

// Helper: read @p len bytes from flash @p addr into @p buf.
static void wspi_read(uint32_t addr, uint8_t *buf, uint32_t len)
{
    wspi_command_t desc = {
        .cmd   = CMD_READ_DATA,
        .cfg   = WSPI_CFG_CMD_MODE_ONE_LINE  | WSPI_CFG_ADDR_MODE_ONE_LINE |
                 WSPI_CFG_ADDR_SIZE_24        | WSPI_CFG_ALT_MODE_NONE     |
                 WSPI_CFG_DATA_MODE_ONE_LINE,
        .addr  = addr,
        .alt   = 0U,
        .dummy = 0U,
    };
    wspiReceive(s_wdrv, &desc, (size_t)len, buf);
}

// Helper: program one partial page (up to PAGE_SIZE bytes, must not cross page boundary).
static bool wspi_page_program(uint32_t addr, const uint8_t *src, uint32_t len)
{
    wspi_cmd(CMD_WRITE_ENABLE);
    wait_busy();

    wspi_command_t desc = {
        .cmd   = CMD_PAGE_PROGRAM,
        .cfg   = WSPI_CFG_CMD_MODE_ONE_LINE  | WSPI_CFG_ADDR_MODE_ONE_LINE |
                 WSPI_CFG_ADDR_SIZE_24        | WSPI_CFG_ALT_MODE_NONE     |
                 WSPI_CFG_DATA_MODE_ONE_LINE,
        .addr  = addr,
        .alt   = 0U,
        .dummy = 0U,
    };
    wspiSend(s_wdrv, &desc, (size_t)len, src);
    wait_busy();

    return true;
}

bool w25q512_chibios_init(void)
{
    wspiStart(s_wdrv, &s_wspi_cfg);

    // Read and verify JEDEC ID.
    uint8_t id[3];
    wspi_cmd_read(CMD_READ_JEDEC_ID, id, 3U);

    if (id[0] != W25Q512_JEDEC_MFR || id[1] != W25Q512_JEDEC_TYPE || id[2] != W25Q512_JEDEC_CAP)
    {
        return false;
    }

    return true;
}

bool w25q512_chibios_read(uint8_t *dst, uint32_t addr, uint32_t len)
{
    wspi_read(addr, dst, len);
    return true;
}

bool w25q512_chibios_write(const uint8_t *src, uint32_t addr, uint32_t len)
{
    while (len > 0U)
    {
        // Bytes remaining in this page.
        uint32_t page_off = addr % PAGE_SIZE;
        uint32_t chunk = PAGE_SIZE - page_off;
        if (chunk > len)
        {
            chunk = len;
        }

        if (!wspi_page_program(addr, src, chunk))
        {
            return false;
        }

        src  += chunk;
        addr += chunk;
        len  -= chunk;
    }
    return true;
}

bool w25q512_chibios_erase(uint32_t addr)
{
    wspi_cmd(CMD_WRITE_ENABLE);
    wait_busy();

    wspi_command_t desc = {
        .cmd   = CMD_BLOCK_ERASE_64K,
        .cfg   = WSPI_CFG_CMD_MODE_ONE_LINE  | WSPI_CFG_ADDR_MODE_ONE_LINE |
                 WSPI_CFG_ADDR_SIZE_24        | WSPI_CFG_ALT_MODE_NONE     |
                 WSPI_CFG_DATA_MODE_NONE,
        .addr  = addr,
        .alt   = 0U,
        .dummy = 0U,
    };
    wspiCommand(s_wdrv, &desc);
    wait_busy();

    return true;
}
