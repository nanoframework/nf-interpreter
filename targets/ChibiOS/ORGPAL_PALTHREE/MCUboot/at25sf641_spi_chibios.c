//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// at25sf641_spi_chibios.c — AT25SF641 8 MB SPI flash driver using ChibiOS SPIv2 HAL.
//
// Hardware: SPI1 on ORGPAL_PALTHREE (STM32F769ZI).
//   SCK  = PA5  AF5
//   MISO = PA6  AF5
//   MOSI = PB5  AF5
//   CS   = PI15 (GPIO output, active-LOW, managed manually)
//   WP   = PJ3  (held HIGH by board.c — write-protect inactive)
//   HOLD = PJ4  (held HIGH by board.c — hold inactive)
//
// All pins are initialised by board.c / boardInit() before this driver is called.
// SPI1 clock: PCLK2 = 108 MHz, BR=001 (÷4) → 27 MHz (AT25SF641 max 100 MHz).

#include "hal.h"
#include "at25sf641_spi_chibios.h"

// AT25SF641 opcodes
#define AT25_CMD_READ_ID        0x9FU
#define AT25_CMD_READ           0x03U
#define AT25_CMD_WRITE_ENABLE   0x06U
#define AT25_CMD_PAGE_PROGRAM   0x02U
#define AT25_CMD_SECTOR_ERASE   0x20U   // 4 kB
#define AT25_CMD_BLOCK_ERASE    0xD8U   // 64 kB
#define AT25_CMD_READ_STATUS    0x05U

#define AT25_STATUS_BUSY        0x01U

#define AT25_PAGE_SIZE          256U
#define AT25_JEDEC_MFR          0x1FU   // Adesto/Renesas
#define AT25_JEDEC_TYPE         0x32U   // AT25SF641
#define AT25_JEDEC_CAP          0x17U   // 8 MB

// SPI1 configuration for AT25SF641.
// Master mode, SSM+SSI (software slave management), CPOL=0, CPHA=0.
// BR=001 → fPCLK2/4 = 108/4 = 27 MHz.
// DS=0111 (8-bit), RXFIFO threshold = 1 byte.
static const SPIConfig s_spi1cfg = {
    .end_cb  = NULL,
    .cr1     = SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_BR_0,
    .cr2     = (7U << SPI_CR2_DS_Pos) | SPI_CR2_FRXTH,
};

static inline void cs_assert(void)
{
    palClearPad(GPIOI, GPIOI_FLASH_SPI1_CS);
}

static inline void cs_release(void)
{
    palSetPad(GPIOI, GPIOI_FLASH_SPI1_CS);
}

static inline void spi_send_byte(uint8_t b)
{
    spiSend(&SPID1, 1, &b);
}

static inline uint8_t spi_recv_byte(void)
{
    uint8_t b;
    spiReceive(&SPID1, 1, &b);
    return b;
}

static void send_addr(uint32_t addr)
{
    uint8_t buf[3] = {
        (uint8_t)(addr >> 16),
        (uint8_t)(addr >> 8),
        (uint8_t)(addr),
    };
    spiSend(&SPID1, 3, buf);
}

static bool wait_ready(void)
{
    // Poll SR1.BUSY until the device is idle (max ~300 ms for block erase).
    for (uint32_t i = 0; i < 1000000UL; i++)
    {
        cs_assert();
        spi_send_byte(AT25_CMD_READ_STATUS);
        uint8_t sr = spi_recv_byte();
        cs_release();

        if ((sr & AT25_STATUS_BUSY) == 0U)
        {
            return true;
        }
    }
    return false;
}

static void write_enable(void)
{
    cs_assert();
    spi_send_byte(AT25_CMD_WRITE_ENABLE);
    cs_release();
}

bool at25sf641_bare_init(void)
{
    spiStart(&SPID1, &s_spi1cfg);

    // Read JEDEC ID and verify the device is present.
    cs_assert();
    spi_send_byte(AT25_CMD_READ_ID);
    uint8_t id[3];
    spiReceive(&SPID1, 3, id);
    cs_release();

    return (id[0] == AT25_JEDEC_MFR) && (id[1] == AT25_JEDEC_TYPE) && (id[2] == AT25_JEDEC_CAP);
}

bool at25sf641_bare_read(uint8_t *dst, uint32_t addr, uint32_t len)
{
    if (len == 0U)
    {
        return true;
    }

    cs_assert();
    spi_send_byte(AT25_CMD_READ);
    send_addr(addr);
    spiReceive(&SPID1, len, dst);
    cs_release();

    return true;
}

bool at25sf641_bare_write(const uint8_t *src, uint32_t addr, uint32_t len)
{
    while (len > 0U)
    {
        // Writes must not cross a 256-byte page boundary.
        uint32_t page_offset = addr & (AT25_PAGE_SIZE - 1U);
        uint32_t chunk = AT25_PAGE_SIZE - page_offset;
        if (chunk > len)
        {
            chunk = len;
        }

        write_enable();

        cs_assert();
        spi_send_byte(AT25_CMD_PAGE_PROGRAM);
        send_addr(addr);
        spiSend(&SPID1, chunk, src);
        cs_release();

        if (!wait_ready())
        {
            return false;
        }

        src  += chunk;
        addr += chunk;
        len  -= chunk;
    }

    return true;
}

bool at25sf641_bare_erase(uint32_t addr, bool block4k)
{
    write_enable();

    cs_assert();
    spi_send_byte(block4k ? AT25_CMD_SECTOR_ERASE : AT25_CMD_BLOCK_ERASE);
    send_addr(addr);
    cs_release();

    return wait_ready();
}
