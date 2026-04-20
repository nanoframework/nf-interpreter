//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Bare-metal AT25SF641 SPI flash driver
//
// GPIO / SPI peripheral configuration
//
//   RCC:
//     GPIOA, GPIOB, GPIOI, GPIOJ — AHB1 (RCC->AHB1ENR)
//     SPI1                        — APB2 (RCC->APB2ENR)
//
//   GPIO alternate functions (AF5 for SPI1):
//     PA5 (SCK)  — MODER=10 (AF), AFRL[23:20]=0101
//     PA6 (MISO) — MODER=10 (AF), AFRL[27:24]=0101
//     PB5 (MOSI) — MODER=10 (AF), AFRL[23:20]=0101
//
//   GPIO output (CS / WP / HOLD):
//     PI15 (CS)   — MODER=01 (output), OTYPER=0 (push-pull), OSPEEDR=01 (low)
//     PJ3  (WP#)  — MODER=01, driven high (not write-protected)
//     PJ4  (HOLD#)— MODER=01, driven high (not held)
//
//   SPI1 registers (APB2, base 0x40013000):
//     CR1: BR=000 (fPCLK/2 = 8 MHz @ HSI), MSTR=1, CPOL=0, CPHA=0, SSM=1, SSI=1
//     CR2: DS=0111 (8-bit), FRXTH=1 (RXNE fires when ≥ 1 byte)
//     CR1: SPE=1

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <stm32f769xx.h>

#include "at25sf641_spi_bare.h"

// AT25SF641 command bytes.
#define CMD_READ            0x03U
#define CMD_PAGE_PROG       0x02U
#define CMD_BLOCK_ERASE_64K 0xD8U
#define CMD_SECTOR_ERASE_4K 0x20U
#define CMD_WRITE_ENABLE    0x06U
#define CMD_READ_STATUS1    0x05U
#define CMD_RESUME_DEEP_PD  0xABU
#define CMD_READ_JEDEC_ID   0x9FU

// Timeout: large enough to cover the worst-case flash erase at 16 MHz HSI.
// SysTick is not configured in the bootloader; use a simple iteration counter.
// Roughly calibrated to ~4 seconds at HSI 16 MHz (conservative inner loop).
#define BUSY_TIMEOUT_LOOPS 0x08000000U

// CS pin manipulation (GPIOI, pin 15).
#define CS_SELECT()   (GPIOI->BSRR = (uint32_t)(1U << (15U + 16U))) // reset = drive low
#define CS_DESELECT() (GPIOI->BSRR = (uint32_t)(1U << 15U))         // set   = drive high

// ----------------------------------------------------------------------- //
// SPI polling helpers                                                      //
// ----------------------------------------------------------------------- //

static void spi1_tx_byte(uint8_t byte)
{
    // Wait for TXE (transmit buffer empty).
    while (!(SPI1->SR & SPI_SR_TXE))
    {
    }
    // Write one byte to the 8-bit data register.
    *(volatile uint8_t *)(&SPI1->DR) = byte;
}

static uint8_t spi1_rx_byte(void)
{
    // Clock in one byte by sending a dummy 0x00.
    spi1_tx_byte(0x00U);
    // Wait for RXNE (receive buffer not empty).
    while (!(SPI1->SR & SPI_SR_RXNE))
    {
    }
    return *(volatile uint8_t *)(&SPI1->DR);
}

static void spi1_flush_rx(void)
{
    // Drain any stale byte left in the RX FIFO.
    while (SPI1->SR & SPI_SR_RXNE)
    {
        (void)*(volatile uint8_t *)(&SPI1->DR);
    }
}

static void spi1_wait_not_busy(void)
{
    // Wait until SPI is no longer transmitting.
    while (SPI1->SR & SPI_SR_BSY)
    {
    }
}

// ----------------------------------------------------------------------- //
// GPIO / SPI1 initialisation                                               //
// ----------------------------------------------------------------------- //

static void spi1_gpio_init(void)
{
    // Enable GPIO and SPI1 clocks.
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOIEN | RCC_AHB1ENR_GPIOJEN;
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    __DSB();

    // PA5 (SCK) — AF5, push-pull, medium speed.
    GPIOA->MODER = (GPIOA->MODER & ~(3U << 10U)) | (2U << 10U);     // AF
    GPIOA->OSPEEDR = (GPIOA->OSPEEDR & ~(3U << 10U)) | (2U << 10U); // fast
    GPIOA->AFR[0] = (GPIOA->AFR[0] & ~(0xFU << 20U)) | (5U << 20U); // AF5

    // PA6 (MISO) — AF5, push-pull.
    GPIOA->MODER = (GPIOA->MODER & ~(3U << 12U)) | (2U << 12U);     // AF
    GPIOA->AFR[0] = (GPIOA->AFR[0] & ~(0xFU << 24U)) | (5U << 24U); // AF5

    // PB5 (MOSI) — AF5, push-pull, medium speed.
    GPIOB->MODER = (GPIOB->MODER & ~(3U << 10U)) | (2U << 10U);     // AF
    GPIOB->OSPEEDR = (GPIOB->OSPEEDR & ~(3U << 10U)) | (2U << 10U); // fast
    GPIOB->AFR[0] = (GPIOB->AFR[0] & ~(0xFU << 20U)) | (5U << 20U); // AF5

    // PI15 (CS) — GPIO output, push-pull, default high (deselected).
    GPIOI->BSRR = (uint32_t)(1U << 15U);                        // set high first
    GPIOI->MODER = (GPIOI->MODER & ~(3U << 30U)) | (1U << 30U); // output

    // PJ3 (WP#) — GPIO output, high (write-protect disabled).
    GPIOJ->BSRR = (uint32_t)(1U << 3U);
    GPIOJ->MODER = (GPIOJ->MODER & ~(3U << 6U)) | (1U << 6U);

    // PJ4 (HOLD#) — GPIO output, high (not held).
    GPIOJ->BSRR = (uint32_t)(1U << 4U);
    GPIOJ->MODER = (GPIOJ->MODER & ~(3U << 8U)) | (1U << 8U);
}

static void spi1_peripheral_init(void)
{
    // Reset SPI1.
    RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST;
    RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST;

    // CR1: Master, software CS (SSM=1, SSI=1), CPOL=0, CPHA=0, BR=000 (fPCLK/2).
    SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI;
    // BR=000 leaves default (fPCLK/2 = 8 MHz @ HSI 16 MHz).

    // CR2: 8-bit data size (DS=0111), RXNE fires on ≥ 1 byte (FRXTH=1).
    SPI1->CR2 = (7U << SPI_CR2_DS_Pos) | SPI_CR2_FRXTH;

    // Enable SPI1.
    SPI1->CR1 |= SPI_CR1_SPE;
}

// ----------------------------------------------------------------------- //
// Public API                                                               //
// ----------------------------------------------------------------------- //

bool at25sf641_bare_init(void)
{
    spi1_gpio_init();
    spi1_peripheral_init();

    // Resume from deep power down (required on cold boot).
    CS_SELECT();
    spi1_tx_byte(CMD_RESUME_DEEP_PD);
    spi1_wait_not_busy();
    CS_DESELECT();

    // AT25SF641 datasheet: device needs tRDP ≥ 3 µs after resume.
    // Use a simple delay loop (~20 µs at 16 MHz HSI).
    for (volatile uint32_t d = 0; d < 320U; d++)
    {
    }

    spi1_flush_rx();

    // Read JEDEC ID (0x9F) and verify manufacturer + device bytes.
    CS_SELECT();
    spi1_tx_byte(CMD_READ_JEDEC_ID);
    uint8_t mfr = spi1_rx_byte();
    uint8_t id1 = spi1_rx_byte();
    uint8_t id2 = spi1_rx_byte();
    spi1_wait_not_busy();
    CS_DESELECT();

    spi1_flush_rx();

    return (mfr == AT25SF641_MANUFACTURER_ID) && (id1 == AT25SF641_DEVICE_ID1) && (id2 == AT25SF641_DEVICE_ID2);
}

bool at25sf641_bare_wait_ready(void)
{
    uint32_t loops = 0;

    CS_SELECT();
    spi1_tx_byte(CMD_READ_STATUS1);

    while (true)
    {
        uint8_t status = spi1_rx_byte();
        if (!(status & AT25SF641_SR_BUSY))
        {
            break;
        }
        if (++loops > BUSY_TIMEOUT_LOOPS)
        {
            spi1_wait_not_busy();
            CS_DESELECT();
            spi1_flush_rx();
            return false;
        }
    }

    spi1_wait_not_busy();
    CS_DESELECT();
    spi1_flush_rx();
    return true;
}

bool at25sf641_bare_read(uint8_t *buf, uint32_t addr, uint32_t len)
{
    if (len == 0U)
    {
        return true;
    }

    CS_SELECT();
    spi1_tx_byte(CMD_READ);
    spi1_tx_byte((uint8_t)(addr >> 16U));
    spi1_tx_byte((uint8_t)(addr >> 8U));
    spi1_tx_byte((uint8_t)addr);

    // Drain the 4 command-byte echoes from the RX FIFO.
    spi1_flush_rx();

    for (uint32_t i = 0; i < len; i++)
    {
        buf[i] = spi1_rx_byte();
    }

    spi1_wait_not_busy();
    CS_DESELECT();
    spi1_flush_rx();
    return true;
}

bool at25sf641_bare_write(const uint8_t *buf, uint32_t addr, uint32_t len)
{
    uint32_t address = addr;

    while (len > 0U)
    {
        // Clamp write to the remaining bytes in the current page.
        uint32_t page_offset = address % AT25SF641_PAGE_SIZE;
        uint32_t space_in_page = AT25SF641_PAGE_SIZE - page_offset;
        uint32_t write_len = (len < space_in_page) ? len : space_in_page;

        // Write enable.
        CS_SELECT();
        spi1_tx_byte(CMD_WRITE_ENABLE);
        spi1_wait_not_busy();
        CS_DESELECT();
        spi1_flush_rx();

        // Page program command + 24-bit address.
        CS_SELECT();
        spi1_tx_byte(CMD_PAGE_PROG);
        spi1_tx_byte((uint8_t)(address >> 16U));
        spi1_tx_byte((uint8_t)(address >> 8U));
        // When writing a full page the LSB wraps per the AT25SF641 protocol.
        spi1_tx_byte((uint8_t)(write_len == AT25SF641_PAGE_SIZE ? address & 0xFFFFFF00U : address));

        for (uint32_t i = 0; i < write_len; i++)
        {
            spi1_tx_byte(buf[i]);
        }

        spi1_wait_not_busy();
        CS_DESELECT();
        spi1_flush_rx();

        if (!at25sf641_bare_wait_ready())
        {
            return false;
        }

        address += write_len;
        buf += write_len;
        len -= write_len;
    }

    return true;
}

bool at25sf641_bare_erase(uint32_t addr, bool large_block)
{
    // Write enable.
    CS_SELECT();
    spi1_tx_byte(CMD_WRITE_ENABLE);
    spi1_wait_not_busy();
    CS_DESELECT();
    spi1_flush_rx();

    // Erase command + 24-bit address.
    CS_SELECT();
    spi1_tx_byte(large_block ? CMD_BLOCK_ERASE_64K : CMD_SECTOR_ERASE_4K);
    spi1_tx_byte((uint8_t)(addr >> 16U));
    spi1_tx_byte((uint8_t)(addr >> 8U));
    spi1_tx_byte((uint8_t)addr);
    spi1_wait_not_busy();
    CS_DESELECT();
    spi1_flush_rx();

    return at25sf641_bare_wait_ready();
}
