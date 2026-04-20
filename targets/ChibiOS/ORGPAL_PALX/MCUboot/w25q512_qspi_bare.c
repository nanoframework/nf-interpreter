//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Bare-metal W25Q512 QSPI flash driver
//
// STM32F769NI QUADSPI peripheral (AHB3, base 0xA0001000).
// Reference: RM0410 Rev 5, Chapter 13 (Quad-SPI interface).
//
// All operations use 1-line (IMODE=1, ADMODE=1, DMODE=1) indirect mode.
// QUADSPI clock: HCLK/2 = 8 MHz (prescaler=1 at 16 MHz HSI).
//
// QUADSPI CCR register layout (key fields):
//   [27:26] FMODE  — functional mode: 00=indirect write, 01=indirect read
//   [25:24] DMODE  — data mode:       00=none, 01=1-line, 10=2-line, 11=4-line
//   [22:18] DCYC   — dummy cycles
//   [13:12] ADSIZE — address size:    00=8-bit, 10=24-bit, 11=32-bit
//   [11:10] ADMODE — address mode (same encoding as DMODE)
//   [9:8]   IMODE  — instruction mode (same encoding as DMODE)
//   [7:0]   INSTRUCTION
//
// QUADSPI DCR register:
//   [20:16] FSIZE  — 2^(FSIZE+1) byte device size; W25Q512=64 MB → FSIZE=25
//   [10:8]  CSHT   — CS high time: CSHT+1 AHB3 cycles
//   [0]     CKMODE — 0=mode 0 (CPOL=0, CPHA=0)

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include <stm32f769xx.h>

#include "w25q512_qspi_bare.h"

// ----------------------------------------------------------------------- //
// W25Q512 command set                                                      //
// ----------------------------------------------------------------------- //
#define CMD_RESET_ENABLE     0x66U
#define CMD_RESET_MEMORY     0x99U
#define CMD_JEDEC_ID         0x9FU
#define CMD_READ             0x03U // Slow read (max 50 MHz, no dummy cycles)
#define CMD_WRITE_ENABLE     0x06U
#define CMD_READ_STATUS_REG1 0x05U
#define CMD_PAGE_PROGRAM     0x02U
#define CMD_BLOCK_ERASE_64K  0xD8U

#define W25Q512_MANUFACTURER 0xEFU
#define W25Q512_DEVICE_ID1   0x40U
#define W25Q512_DEVICE_ID2   0x20U

#define W25Q512_PAGE_SIZE 256U
#define W25Q512_SR1_WIP   0x01U // Write In Progress
#define W25Q512_SR1_WREN  0x02U // Write Enable Latch

// ----------------------------------------------------------------------- //
// QUADSPI CCR field helpers                                                //
// ----------------------------------------------------------------------- //
#define QSPI_FMODE_INDIRECT_WRITE (0U << 26U)
#define QSPI_FMODE_INDIRECT_READ  (1U << 26U)
#define QSPI_DMODE_NONE           (0U << 24U)
#define QSPI_DMODE_1_LINE         (1U << 24U)
#define QSPI_ADSIZE_24BIT         (2U << 12U)
#define QSPI_ADMODE_NONE          (0U << 10U)
#define QSPI_ADMODE_1_LINE        (1U << 10U)
#define QSPI_IMODE_1_LINE         (1U << 8U)

// ----------------------------------------------------------------------- //
// GPIO helpers                                                             //
// ----------------------------------------------------------------------- //

// Set GPIO pin to alternate function mode (MODER = 10) and set AF.
static void gpio_set_af(GPIO_TypeDef *port, uint32_t pin, uint32_t af)
{
    // MODER: 2 bits per pin
    port->MODER = (port->MODER & ~(3U << (pin * 2U))) | (2U << (pin * 2U));
    // OSPEEDR: 3 = very high speed
    port->OSPEEDR |= (3U << (pin * 2U));
    // AFR: 4 bits per pin; pins 0-7 → AFRL, pins 8-15 → AFRH
    if (pin < 8U)
    {
        port->AFR[0] = (port->AFR[0] & ~(0xFU << (pin * 4U))) | (af << (pin * 4U));
    }
    else
    {
        uint32_t p = pin - 8U;
        port->AFR[1] = (port->AFR[1] & ~(0xFU << (p * 4U))) | (af << (p * 4U));
    }
}

// ----------------------------------------------------------------------- //
// Internal helpers                                                         //
// ----------------------------------------------------------------------- //

// Wait until QUADSPI is not busy (TC flag set or BUSY clear for auto-polling).
static bool qspi_wait_tc(void)
{
    // Poll until Transfer Complete flag (bit 1 in SR) is set or error (bit 0).
    uint32_t timeout = 0x00FFFFFFU;

    while (!(QUADSPI->SR & QUADSPI_SR_TCF))
    {
        if (QUADSPI->SR & QUADSPI_SR_TEF)
        {
            QUADSPI->FCR = QUADSPI_FCR_CTEF;
            return false;
        }
        if (--timeout == 0U)
        {
            return false;
        }
    }

    QUADSPI->FCR = QUADSPI_FCR_CTCF;
    return true;
}

// Wait until QUADSPI BUSY flag is clear (after write/erase complete).
static bool qspi_wait_not_busy(void)
{
    uint32_t timeout = 0x00FFFFFFU;

    while (QUADSPI->SR & QUADSPI_SR_BUSY)
    {
        if (--timeout == 0U)
        {
            return false;
        }
    }
    return true;
}

// Send a command with no address and no data (e.g. RESET_ENABLE, WRITE_ENABLE).
static bool qspi_send_cmd(uint8_t cmd)
{
    if (!qspi_wait_not_busy())
    {
        return false;
    }

    QUADSPI->CCR = QSPI_FMODE_INDIRECT_WRITE | QSPI_DMODE_NONE | QSPI_ADMODE_NONE | QSPI_IMODE_1_LINE | cmd;

    return qspi_wait_not_busy();
}

// Read len bytes (1-32) from the QUADSPI DR using command cmd with no address.
static bool qspi_read_no_addr(uint8_t cmd, uint8_t *buf, uint32_t len)
{
    if (!qspi_wait_not_busy())
    {
        return false;
    }

    QUADSPI->DLR = len - 1U;
    QUADSPI->CCR = QSPI_FMODE_INDIRECT_READ | QSPI_DMODE_1_LINE | QSPI_ADMODE_NONE | QSPI_IMODE_1_LINE | (uint32_t)cmd;

    // Read byte by byte from DR (8-bit access).
    for (uint32_t i = 0; i < len; i++)
    {
        uint32_t timeout = 0x000FFFFFU;
        // Wait for at least 1 byte available in FIFO (FTF or TCF).
        while (!((QUADSPI->SR & QUADSPI_SR_FTF) || (QUADSPI->SR & QUADSPI_SR_TCF)))
        {
            if (QUADSPI->SR & QUADSPI_SR_TEF)
            {
                QUADSPI->FCR = QUADSPI_FCR_CTEF;
                return false;
            }
            if (--timeout == 0U)
            {
                return false;
            }
        }
        buf[i] = *(__IO uint8_t *)(&QUADSPI->DR);
    }

    QUADSPI->FCR = QUADSPI_FCR_CTCF;
    return true;
}

// Read W25Q512 Status Register 1 byte.
static bool qspi_read_sr1(uint8_t *sr)
{
    return qspi_read_no_addr(CMD_READ_STATUS_REG1, sr, 1U);
}

// Wait until the W25Q512 Write-In-Progress bit clears (polling SR1.WIP).
// Timeout loop — no watchdog in bootloader.
static bool qspi_wait_flash_ready(void)
{
    uint8_t sr;
    uint32_t attempts = 0x00FFFFFFU;

    do
    {
        if (!qspi_read_sr1(&sr))
        {
            return false;
        }
        if (--attempts == 0U)
        {
            return false;
        }
    } while (sr & W25Q512_SR1_WIP);

    return true;
}

// Send WRITE_ENABLE and verify the WEL bit is set.
static bool qspi_write_enable(void)
{
    if (!qspi_send_cmd(CMD_WRITE_ENABLE))
    {
        return false;
    }

    uint8_t sr;
    if (!qspi_read_sr1(&sr))
    {
        return false;
    }

    return (sr & W25Q512_SR1_WREN) != 0U;
}

// ----------------------------------------------------------------------- //
// Public API                                                               //
// ----------------------------------------------------------------------- //

bool w25q512_bare_init(void)
{
    // 1. Enable GPIO clocks: GPIOB (PB6), GPIOD (PD11/12/13), GPIOE (PE2), GPIOF (PF10).
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOEEN | RCC_AHB1ENR_GPIOFEN;
    __DSB();

    // 2. Enable QUADSPI clock on AHB3.
    RCC->AHB3ENR |= RCC_AHB3ENR_QSPIEN;
    __DSB();

    // 3. Configure GPIO pins for QUADSPI alternate function.
    //    PB6  = NCS  (AF10)
    gpio_set_af(GPIOB, 6U, 10U);
    //    PF10 = CLK  (AF9)
    gpio_set_af(GPIOF, 10U, 9U);
    //    PD11 = D0   (AF9)
    gpio_set_af(GPIOD, 11U, 9U);
    //    PD12 = D1   (AF9)
    gpio_set_af(GPIOD, 12U, 9U);
    //    PE2  = D2   (AF9)
    gpio_set_af(GPIOE, 2U, 9U);
    //    PD13 = D3   (AF9)
    gpio_set_af(GPIOD, 13U, 9U);

    // 4. Configure QUADSPI peripheral.
    //    Disable first to write CR/DCR.
    QUADSPI->CR = 0U;

    //    DCR: FSIZE=25 (64 MB), CSHT=1 (CS high 2 cycles), CKMODE=0 (mode 0).
    QUADSPI->DCR = (25U << QUADSPI_DCR_FSIZE_Pos) | (1U << QUADSPI_DCR_CSHT_Pos);

    //    CR: PRESCALER=1 (HCLK/2), FTHRES=3 (4-byte FIFO threshold), SSHIFT=1, EN=1.
    QUADSPI->CR = (1U << QUADSPI_CR_PRESCALER_Pos) | (3U << QUADSPI_CR_FTHRES_Pos) | QUADSPI_CR_SSHIFT | QUADSPI_CR_EN;
    __DSB();

    // 5. Software reset sequence (in case the device is in an unknown state).
    //    Must issue Reset Enable then Reset Memory; wait for WIP to clear.
    if (!qspi_send_cmd(CMD_RESET_ENABLE))
    {
        return false;
    }
    if (!qspi_send_cmd(CMD_RESET_MEMORY))
    {
        return false;
    }

    // After software reset, device needs ~30 µs recovery (tRST).
    // At 16 MHz, a simple spin loop of ~1000 iterations is ~62 µs — sufficient.
    for (volatile uint32_t i = 0; i < 1000U; i++)
    {
        __NOP();
    }

    // 6. Verify JEDEC ID: EF 40 20 (W25Q512JV).
    uint8_t jedec[3];
    if (!qspi_read_no_addr(CMD_JEDEC_ID, jedec, 3U))
    {
        return false;
    }

    if (jedec[0] != W25Q512_MANUFACTURER || jedec[1] != W25Q512_DEVICE_ID1 || jedec[2] != W25Q512_DEVICE_ID2)
    {
        return false;
    }

    return true;
}

bool w25q512_bare_read(uint8_t *dst, uint32_t addr, uint32_t len)
{
    if (len == 0U)
    {
        return true;
    }

    if (!qspi_wait_not_busy())
    {
        return false;
    }

    QUADSPI->DLR = len - 1U;
    QUADSPI->CCR = QSPI_FMODE_INDIRECT_READ | QSPI_DMODE_1_LINE | QSPI_ADSIZE_24BIT | QSPI_ADMODE_1_LINE |
                   QSPI_IMODE_1_LINE | CMD_READ;
    // Writing AR triggers the transfer.
    QUADSPI->AR = addr;

    uint8_t *p = dst;
    uint32_t remaining = len;

    while (remaining > 0U)
    {
        uint32_t timeout = 0x000FFFFFU;

        while (!((QUADSPI->SR & QUADSPI_SR_FTF) || (QUADSPI->SR & QUADSPI_SR_TCF)))
        {
            if (QUADSPI->SR & QUADSPI_SR_TEF)
            {
                QUADSPI->FCR = QUADSPI_FCR_CTEF;
                return false;
            }
            if (--timeout == 0U)
            {
                return false;
            }
        }

        *p++ = *(__IO uint8_t *)(&QUADSPI->DR);
        remaining--;
    }

    QUADSPI->FCR = QUADSPI_FCR_CTCF;
    return true;
}

bool w25q512_bare_write(const uint8_t *src, uint32_t addr, uint32_t len)
{
    if (len == 0U)
    {
        return true;
    }

    uint32_t current_addr = addr;
    const uint8_t *p = src;
    uint32_t remaining = len;

    while (remaining > 0U)
    {
        // Page-align: clip write to current 256-byte page boundary.
        uint32_t page_offset = current_addr % W25Q512_PAGE_SIZE;
        uint32_t chunk = W25Q512_PAGE_SIZE - page_offset;
        if (chunk > remaining)
        {
            chunk = remaining;
        }

        if (!qspi_write_enable())
        {
            return false;
        }

        if (!qspi_wait_not_busy())
        {
            return false;
        }

        QUADSPI->DLR = chunk - 1U;
        QUADSPI->CCR = QSPI_FMODE_INDIRECT_WRITE | QSPI_DMODE_1_LINE | QSPI_ADSIZE_24BIT | QSPI_ADMODE_1_LINE |
                       QSPI_IMODE_1_LINE | CMD_PAGE_PROGRAM;
        QUADSPI->AR = current_addr;

        for (uint32_t i = 0; i < chunk; i++)
        {
            uint32_t timeout = 0x000FFFFFU;
            // Wait for FIFO not full (FTF set or FIFO level < 32).
            while (!(QUADSPI->SR & QUADSPI_SR_FTF) && (QUADSPI->SR & QUADSPI_SR_BUSY))
            {
                if (--timeout == 0U)
                {
                    return false;
                }
            }
            *(__IO uint8_t *)(&QUADSPI->DR) = *p++;
        }

        if (!qspi_wait_tc())
        {
            return false;
        }

        if (!qspi_wait_flash_ready())
        {
            return false;
        }

        current_addr += chunk;
        remaining -= chunk;
    }

    return true;
}

bool w25q512_bare_erase(uint32_t addr)
{
    if (!qspi_write_enable())
    {
        return false;
    }

    if (!qspi_wait_not_busy())
    {
        return false;
    }

    QUADSPI->CCR = QSPI_FMODE_INDIRECT_WRITE | QSPI_DMODE_NONE | QSPI_ADSIZE_24BIT | QSPI_ADMODE_1_LINE |
                   QSPI_IMODE_1_LINE | CMD_BLOCK_ERASE_64K;
    QUADSPI->AR = addr;

    if (!qspi_wait_tc())
    {
        return false;
    }

    return qspi_wait_flash_ready();
}
