//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// CYW43 PIO-based SPI port driver for ChibiOS on RP2040.
// Implements the port-level SPI functions required by the CYW43 driver's cyw43_spi.c:
//   cyw43_spi_init, cyw43_spi_deinit, cyw43_spi_gpio_setup, cyw43_spi_reset,
//   cyw43_spi_set_polarity, cyw43_spi_transfer
//
// The CYW43439 on Pico W uses a non-standard half-duplex SPI protocol where GP24
// is bidirectional (MOSI/MISO), requiring the RP2040 PIO state machine.

#include <ch.h>
#include <hal.h>
#include <string.h>
#include "cyw43_configport.h"
#include "cyw43_ll.h"
#include "cyw43_internal.h"
#include "cyw43_spi.h"
#include "rp_pio.h"

// ---------------------------------------------------------------------------
// PIO program for CYW43 SPI half-duplex communication
// Side-set: GP29 (clock), OUT/IN: GP24 (data), CS: GP25 (software)
// ---------------------------------------------------------------------------
static const uint16_t cyw43_pio_spi_program[] = {
    // .wrap_target
    0x80a0, // 0: pull block          side 0
    0x6101, // 1: out pins, 1         side 0 [1]
    0x1701, // 2: jmp !osre, 1        side 1 [1]
    0xe080, // 3: set pindirs, 0      side 0     (switch to input)
    0x4701, // 4: in pins, 1          side 1 [1]
    0x8160, // 5: push iffull noblock side 0 [1]
    0x0044, // 6: jmp y--, 4          side 0
    0xe081, // 7: set pindirs, 1      side 0     (switch to output)
    // .wrap
};

#define PIO_PROGRAM_LEN (sizeof(cyw43_pio_spi_program) / sizeof(cyw43_pio_spi_program[0]))

static const rp_pio_program_t cyw43_pio_program = {
    .instructions = cyw43_pio_spi_program,
    .length = PIO_PROGRAM_LEN,
    .origin = -1,
};

// PIO state
static const rp_pio_sm_t *smp = NULL;
static const rp_pio_block_t *pio_block = NULL;
static int32_t pio_offset = -1;

// ---------------------------------------------------------------------------
// Low-level helpers
// ---------------------------------------------------------------------------

static inline void cs_low(void)
{
    palClearPad(IOPORT1, CYW43_PIN_WL_CS);
}

static inline void cs_high(void)
{
    palSetPad(IOPORT1, CYW43_PIN_WL_CS);
}

// Reconfigure PIO SM for a write or read transfer of bit_count bits
static void pio_spi_configure(bool is_write, uint32_t bit_count)
{
    pioSmDisableX(smp);
    pioSmSetFrequencyX(smp, 66000000); // ~33 MHz SPI (2 PIO cycles/bit)

    uint32_t pinctrl =
        (1U << 26) |                        // 1 OUT pin
        (CYW43_PIN_WL_DATA_OUT << 0) |      // OUT base = GP24
        (1U << 29) |                         // 1 side-set pin
        (CYW43_PIN_WL_CLK << 10) |           // side-set base = GP29
        (CYW43_PIN_WL_DATA_OUT << 15);       // IN base = GP24
    pioSmSetPinctrlX(smp, pinctrl);

    // TX: shift left (MSB first), auto-pull at 32; RX: shift left, auto-push at 32
    uint32_t shiftctrl =
        (1U << 19) | // AUTOPULL
        (0U << 16) | // PULL_THRESH=32
        (1U << 18) | // OUT_SHIFTDIR=left
        (1U << 17) | // AUTOPUSH
        (0U << 20) | // PUSH_THRESH=32
        (0U << 22);  // IN_SHIFTDIR=left
    pioSmSetShiftctrlX(smp, shiftctrl);

    uint32_t wb = (uint32_t)pio_offset;
    uint32_t wt = (uint32_t)pio_offset + PIO_PROGRAM_LEN - 1;
    pioSmSetExecctrlX(smp, (wb << 7) | (wt << 12));

    if (!is_write && bit_count > 0)
    {
        uint32_t y_val = bit_count - 1;
        if (y_val <= 31)
        {
            pioSmExecX(smp, (uint16_t)(0xe040 | (y_val & 0x1f))); // set y, val
        }
        else
        {
            pioSmPutX(smp, y_val);
            pioSmExecX(smp, 0x80a0); // pull block
            pioSmExecX(smp, 0xa042); // mov y, osr
        }
    }

    pioSmClearFifosX(smp);
    pioSmSetPCX(smp, (uint32_t)pio_offset);
    pioSmEnableX(smp);
}

// ---------------------------------------------------------------------------
// Port-level SPI functions (called by cyw43_spi.c from the driver)
// ---------------------------------------------------------------------------

void cyw43_spi_gpio_setup(void)
{
    // GP25 (CS) - output, high (deasserted)
    palSetPadMode(IOPORT1, CYW43_PIN_WL_CS, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPad(IOPORT1, CYW43_PIN_WL_CS);

    // GP23 (WL_REG_ON) - output, low initially
    palSetPadMode(IOPORT1, CYW43_PIN_WL_REG_ON, PAL_MODE_OUTPUT_PUSHPULL);
    palClearPad(IOPORT1, CYW43_PIN_WL_REG_ON);
}

void cyw43_spi_reset(void)
{
    // Power cycle the CYW43439
    palClearPad(IOPORT1, CYW43_PIN_WL_REG_ON);
    cyw43_delay_ms(20);
    palSetPad(IOPORT1, CYW43_PIN_WL_REG_ON);
    cyw43_delay_ms(250);
}

int cyw43_spi_init(cyw43_int_t *self)
{
    (void)self;

    pioInit();
    pio_block = RP_PIO0_BLOCK;

    smp = pioSmAlloc(pio_block, 0, 2, NULL, NULL);
    if (smp == NULL)
        return -1;

    pio_offset = pioProgramLoad(pio_block, &cyw43_pio_program);
    if (pio_offset < 0)
    {
        pioSmFree(smp);
        smp = NULL;
        return -1;
    }

    // Configure PIO function on GP24 (data) and GP29 (clock)
    pioSmSetPinFunctionX(smp, CYW43_PIN_WL_DATA_OUT);
    palSetPadMode(IOPORT1, CYW43_PIN_WL_DATA_OUT, PAL_RP_PAD_DRIVE4 | PAL_RP_GPIO_OE);

    pioSmSetPinFunctionX(smp, CYW43_PIN_WL_CLK);
    palSetPadMode(IOPORT1, CYW43_PIN_WL_CLK, PAL_RP_PAD_DRIVE4 | PAL_RP_GPIO_OE);

    return 0;
}

void cyw43_spi_deinit(cyw43_int_t *self)
{
    (void)self;

    palClearPad(IOPORT1, CYW43_PIN_WL_REG_ON);

    if (smp != NULL)
    {
        pioSmDisableX(smp);
        pioSmFree(smp);
        smp = NULL;
    }
    if (pio_offset >= 0)
    {
        pioProgramUnload(pio_block, pio_offset, PIO_PROGRAM_LEN);
        pio_offset = -1;
    }
}

void cyw43_spi_set_polarity(cyw43_int_t *self, int pol)
{
    (void)self;
    (void)pol;
    // CYW43 SPI uses CPOL=0, CPHA=0 exclusively via PIO; no runtime change needed
}

int cyw43_spi_transfer(cyw43_int_t *self, const uint8_t *tx, size_t tx_length, uint8_t *rx, size_t rx_length)
{
    (void)self;

    cs_low();

    // Write phase
    if (tx != NULL && tx_length > 0)
    {
        pio_spi_configure(true, tx_length * 8);

        uint32_t words = (tx_length + 3) / 4;
        for (uint32_t i = 0; i < words; i++)
        {
            uint32_t word = 0;
            uint32_t base = i * 4;
            for (uint32_t b = 0; b < 4 && (base + b) < tx_length; b++)
            {
                word |= ((uint32_t)tx[base + b]) << (24 - b * 8);
            }
            pioSmPut(smp, word);
        }

        // Wait for TX FIFO to drain
        while (!pioSmIsTxEmptyX(smp))
        {
            chThdYield();
        }
        // Small delay for last bits to clock out
        cyw43_delay_us(2);
    }

    // Read phase
    if (rx != NULL && rx_length > 0)
    {
        pio_spi_configure(false, rx_length * 8);

        uint32_t words = (rx_length + 3) / 4;
        for (uint32_t i = 0; i < words; i++)
        {
            uint32_t word = pioSmGet(smp);
            uint32_t base = i * 4;
            for (uint32_t b = 0; b < 4 && (base + b) < rx_length; b++)
            {
                rx[base + b] = (uint8_t)(word >> (24 - b * 8));
            }
        }
    }

    cs_high();

    return 0;
}
