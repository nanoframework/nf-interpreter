//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2020 Raspberry Pi (Trading) Ltd. (BSD-3-Clause)
// See LICENSE file in the project root for full license information.
//

// CYW43 PIO SPI bus driver for ChibiOS on RP2040 (Pico W).
// Ported from the Pico SDK cyw43_bus_pio_spi.c reference implementation.
//
// This file provides BOTH the low-level SPI transport AND the higher-level
// register/byte read/write functions.  The generic cyw43_spi.c from the
// cyw43-driver repo must NOT be compiled — it assumes full-duplex SPI.
//
// CYW43439 half-duplex gSPI protocol:
//   GP23 = WL_REG_ON (power), GP24 = DATA/IRQ (bidirectional),
//   GP25 = CS (active low),   GP29 = CLK (via PIO sideset)

#include <ch.h>
#include <hal.h>
#include <string.h>
#include <assert.h>
#include "rp_pio.h"
#include "cyw43_configport.h"
#include "cyw43.h"
#include "cyw43_ll.h"
#include "cyw43_internal.h"
#include "cyw43_spi.h"

#if CYW43_USE_SPI

// ---------------------------------------------------------------------------
// Pin definitions
// ---------------------------------------------------------------------------
#define DATA_PIN  CYW43_PIN_WL_DATA_OUT  // GP24
#define CLK_PIN   CYW43_PIN_WL_CLK       // GP29
#define CS_PIN    CYW43_PIN_WL_CS        // GP25

// ---------------------------------------------------------------------------
// RP2040 hardware register definitions (direct access)
// ---------------------------------------------------------------------------
#define SIO_BASE        0xD0000000u
#define GPIO_OUT_SET    (*(volatile uint32_t *)(SIO_BASE + 0x014))
#define GPIO_OUT_CLR    (*(volatile uint32_t *)(SIO_BASE + 0x018))
#define GPIO_OE_SET     (*(volatile uint32_t *)(SIO_BASE + 0x024))
#define GPIO_OE_CLR     (*(volatile uint32_t *)(SIO_BASE + 0x028))
#define GPIO_IN_REG     (*(volatile uint32_t *)(SIO_BASE + 0x004))

#define IO_BANK0_BASE   0x40014000u
#define GPIO_CTRL(n)    (*(volatile uint32_t *)(IO_BANK0_BASE + 0x04 + 8 * (n)))

#define PADS_BANK0_BASE 0x4001C000u
#define PADS_GPIO(n)    (*(volatile uint32_t *)(PADS_BANK0_BASE + 0x04 + 4 * (n)))
#define PAD_OD          (1u << 7)
#define PAD_IE          (1u << 6)
#define PAD_DRIVE_2MA   (0u << 4)
#define PAD_DRIVE_4MA   (1u << 4)
#define PAD_DRIVE_8MA   (2u << 4)
#define PAD_DRIVE_12MA  (3u << 4)
#define PAD_PUE         (1u << 3)
#define PAD_PDE         (1u << 2)
#define PAD_SCHMITT     (1u << 1)
#define PAD_SLEWFAST    (1u << 0)

#define FUNCSEL_SIO     5u

// ---------------------------------------------------------------------------
// GPIO helpers
// ---------------------------------------------------------------------------
static inline void pin_high(uint32_t pin) { GPIO_OUT_SET = 1u << pin; }
static inline void pin_low(uint32_t pin)  { GPIO_OUT_CLR = 1u << pin; }
static inline void pin_output(uint32_t pin) { GPIO_OE_SET = 1u << pin; }
static inline void pin_input(uint32_t pin)  { GPIO_OE_CLR = 1u << pin; }

static void gpio_init_pin(uint32_t pin, bool output, uint32_t pad_cfg)
{
    GPIO_OUT_CLR = 1u << pin;
    if (output) GPIO_OE_SET = 1u << pin;
    else        GPIO_OE_CLR = 1u << pin;
    GPIO_CTRL(pin) = FUNCSEL_SIO;
    PADS_GPIO(pin) = pad_cfg;
}

// ---------------------------------------------------------------------------
// PIO SPI Program — spi_gap01_sample0 from Pico SDK
// ---------------------------------------------------------------------------
//   [0] lp:      out pins, 1       side 0
//   [1]          jmp x-- lp        side 1
//   [2] lp1_end: set pindirs, 0    side 0
//   [3]          nop               side 1
//   [4] lp2:     in pins, 1        side 0
//   [5]          jmp y-- lp2       side 1
//   [6] end:
static const uint16_t spi_program[] = {
    0x6001, // [0] out pins, 1    side 0
    0x1040, // [1] jmp x-- 0      side 1  (X--, addr patched at load)
    0xE080, // [2] set pindirs, 0 side 0
    0xB042, // [3] nop(mov y,y)   side 1
    0x4001, // [4] in pins, 1     side 0
    0x1084, // [5] jmp y-- 4      side 1  (Y--, addr patched at load)
};
#define SPI_PROGRAM_LEN     6
#define SPI_OFFSET_LP1_END  2
#define SPI_OFFSET_END      6

// Immediate-exec helpers (no sideset applied)
static inline uint16_t pio_encode_jmp(uint32_t addr)  { return (uint16_t)(addr & 0x1F); }
static inline uint16_t pio_encode_out_x_32(void)      { return 0x6020; }
static inline uint16_t pio_encode_out_y_32(void)      { return 0x6040; }
#define PIO_INSTR_SET_PINDIRS_1  0xE081u
#define PIO_INSTR_SET_PINDIRS_0  0xE080u
#define PIO_INSTR_SET_PINS_1     0xE001u
#define PIO_INSTR_MOV_PINS_NULL  0xA003u

// Read-modify-write EXECCTRL wrap bits only (matches SDK pio_sm_set_wrap).
// pioSmSetExecctrlX does a full register write which zeros all non-wrap bits.
static inline void pio_sm_set_wrap(const rp_pio_sm_t *smp, uint32_t wrap_bottom, uint32_t wrap_top)
{
    PIO_TypeDef *pio = smp->block->pio;
    uint32_t sm = smp->smidx;
    uint32_t val = pio->SM[sm].EXECCTRL;
    val &= ~(PIO_SM_EXECCTRL_WRAP_BOTTOM_Msk | PIO_SM_EXECCTRL_WRAP_TOP_Msk);
    val |= (wrap_bottom << PIO_SM_EXECCTRL_WRAP_BOTTOM_Pos) |
           (wrap_top << PIO_SM_EXECCTRL_WRAP_TOP_Pos);
    pio->SM[sm].EXECCTRL = val;
}

// ---------------------------------------------------------------------------
// PIO state
// ---------------------------------------------------------------------------
static const rp_pio_sm_t *pio_sm = NULL;
static int32_t pio_offset = -1;

// ---------------------------------------------------------------------------
// Byte-swap helpers
// ---------------------------------------------------------------------------
// Full byte reversal (emulates DMA bswap on each FIFO word)
static inline uint32_t bswap32(uint32_t x) { return __builtin_bswap32(x); }

// Pico SDK SWAP32 = rev16 = swap bytes within each 16-bit half
// Used only for the initial swap-mode register access before 32-bit bus switch
static inline uint32_t SWAP32(uint32_t x)
{
    uint32_t r;
    __asm volatile ("rev16 %0, %1" : "=l" (r) : "l" (x));
    return r;
}

// ---------------------------------------------------------------------------
// Command builder — identical to Pico SDK make_cmd / generic pack_cmd
// ---------------------------------------------------------------------------
static inline uint32_t make_cmd(bool write, bool inc, uint32_t fn,
                                uint32_t addr, uint32_t sz)
{
    return (uint32_t)write << 31 | (uint32_t)inc << 30 |
           fn << 28 | (addr & 0x1FFFFu) << 11 | sz;
}

// ---------------------------------------------------------------------------
// SPI diagnostic trace
// ---------------------------------------------------------------------------
#define SPI_TRACE_MAX_TXNS  8
#define SPI_TRACE_MAX_BYTES 16

typedef struct {
    uint8_t  tx_bytes[SPI_TRACE_MAX_BYTES];
    uint8_t  rx_bytes[SPI_TRACE_MAX_BYTES];
    uint16_t tx_len;
    uint16_t rx_len;
    uint16_t eff_tx_len;
    uint16_t eff_rx_len;
    uint32_t gpio_state;
    uint32_t raw_pio_rx;
} spi_trace_entry_t;

static spi_trace_entry_t spi_trace[SPI_TRACE_MAX_TXNS];
static volatile int spi_trace_count = 0;
static volatile uint32_t spi_txn_total = 0;

int  cyw43_spi_get_trace_count(void) { return spi_trace_count; }
uint32_t cyw43_spi_get_txn_total(void) { return spi_txn_total; }
const void *cyw43_spi_get_trace(int idx)
{
    if (idx < 0 || idx >= spi_trace_count) return NULL;
    return &spi_trace[idx];
}

// PIO register snapshot
static uint32_t diag_pio_pinctrl, diag_pio_shiftctrl, diag_pio_execctrl, diag_pio_clkdiv;
static uint32_t diag_pio_instr[SPI_PROGRAM_LEN];
static int32_t  diag_pio_offset;
static uint32_t diag_gpio_ctrl_data, diag_gpio_ctrl_clk, diag_gpio_ctrl_cs, diag_gpio_ctrl_regon;
static uint32_t diag_pads_data, diag_pads_clk;

uint32_t cyw43_spi_get_diag_pinctrl(void)   { return diag_pio_pinctrl; }
uint32_t cyw43_spi_get_diag_shiftctrl(void) { return diag_pio_shiftctrl; }
uint32_t cyw43_spi_get_diag_execctrl(void)  { return diag_pio_execctrl; }
uint32_t cyw43_spi_get_diag_clkdiv(void)    { return diag_pio_clkdiv; }
int32_t  cyw43_spi_get_diag_offset(void)    { return diag_pio_offset; }
uint32_t cyw43_spi_get_diag_instr(int idx)  { return (idx >= 0 && idx < SPI_PROGRAM_LEN) ? diag_pio_instr[idx] : 0; }

void cyw43_spi_capture_gpio_diag(void)
{
    diag_gpio_ctrl_data  = GPIO_CTRL(DATA_PIN);
    diag_gpio_ctrl_clk   = GPIO_CTRL(CLK_PIN);
    diag_gpio_ctrl_cs    = GPIO_CTRL(CS_PIN);
    diag_gpio_ctrl_regon = GPIO_CTRL(CYW43_PIN_WL_REG_ON);
    diag_pads_data = PADS_GPIO(DATA_PIN);
    diag_pads_clk  = PADS_GPIO(CLK_PIN);
}

uint32_t cyw43_spi_get_gpio_ctrl(int idx)
{
    switch (idx) {
        case 0: return diag_gpio_ctrl_data;
        case 1: return diag_gpio_ctrl_clk;
        case 2: return diag_gpio_ctrl_cs;
        case 3: return diag_gpio_ctrl_regon;
        case 4: return diag_pads_data;
        case 5: return diag_pads_clk;
        default: return 0;
    }
}

// =========================================================================
//  Low-level port functions
// =========================================================================

void cyw43_spi_gpio_setup(void)
{
    gpio_init_pin(CS_PIN, true, PAD_IE | PAD_DRIVE_4MA | PAD_SCHMITT);
    pin_high(CS_PIN);

    gpio_init_pin(CYW43_PIN_WL_REG_ON, true, PAD_IE | PAD_DRIVE_4MA | PAD_PUE | PAD_SCHMITT);
    pin_low(CYW43_PIN_WL_REG_ON);

    // DATA: SIO output LOW during reset (critical for gSPI mode detection).
    // No pull — between transfers, chip drives GP24 HIGH for interrupts.
    gpio_init_pin(DATA_PIN, true, PAD_IE | PAD_DRIVE_4MA | PAD_SCHMITT);
    pin_low(DATA_PIN);
}

void cyw43_spi_reset(void)
{
    pin_low(CYW43_PIN_WL_REG_ON);
    cyw43_delay_ms(20);
    pin_high(CYW43_PIN_WL_REG_ON);
    cyw43_delay_ms(250);

    // DATA → SIO input after power-on (matches Pico SDK, no pull)
    gpio_init_pin(DATA_PIN, false, PAD_IE | PAD_DRIVE_4MA | PAD_SCHMITT);
}

int cyw43_spi_init(cyw43_int_t *self)
{
    (void)self;
    pioInit();

    pio_sm = pioSmAlloc(RP_PIO0_BLOCK, RP_PIO_SM_ID_ANY, 0, NULL, NULL);
    if (pio_sm == NULL) return -1;

    // Load program
    uint16_t patched[SPI_PROGRAM_LEN];
    memcpy(patched, spi_program, sizeof(spi_program));
    rp_pio_program_t prog = { .instructions = patched,
                              .length = SPI_PROGRAM_LEN,
                              .origin = -1 };
    pio_offset = pioProgramLoad(pio_sm->block, &prog);
    if (pio_offset < 0) { pioSmFree(pio_sm); pio_sm = NULL; return -1; }

    // Patch JMP targets to absolute addresses
    PIO_TypeDef *pio = pio_sm->block->pio;
    patched[1] = 0x1040 | (uint16_t)(pio_offset + 0);  // jmp x-- lp
    patched[5] = 0x1080 | (uint16_t)(pio_offset + 4);  // jmp y-- lp2
    for (int i = 0; i < SPI_PROGRAM_LEN; i++)
        pio->INSTR_MEM[pio_offset + i] = patched[i];

    uint32_t sm = pio_sm->smidx;

    // Clock: 125 MHz / 2 = 62.5 MHz PIO → ~31 MHz SPI
    pioSmSetClkdivX(pio_sm, PIO_SM_CLKDIV(2, 0));

    // Shift: OUT left, IN left, autopull/autopush at 32 bits
    pioSmSetShiftctrlX(pio_sm,
        PIO_SM_SHIFTCTRL_AUTOPULL | PIO_SM_SHIFTCTRL_AUTOPUSH);

    // Pin mapping
    pioSmSetPinctrlX(pio_sm,
        (DATA_PIN << PIO_SM_PINCTRL_OUT_BASE_Pos) |
        (1u       << PIO_SM_PINCTRL_OUT_COUNT_Pos) |
        (DATA_PIN << PIO_SM_PINCTRL_SET_BASE_Pos) |
        (1u       << PIO_SM_PINCTRL_SET_COUNT_Pos) |
        (DATA_PIN << PIO_SM_PINCTRL_IN_BASE_Pos) |
        (CLK_PIN  << PIO_SM_PINCTRL_SIDESET_BASE_Pos) |
        (1u       << PIO_SM_PINCTRL_SIDESET_COUNT_Pos));

    // CLK pad: 12 mA, fast slew, Schmitt trigger
    PADS_GPIO(CLK_PIN) = PAD_IE | PAD_DRIVE_12MA | PAD_SLEWFAST | PAD_SCHMITT;
    pioSmSetPinFunctionX(pio_sm, CLK_PIN);

    // Set CLK as output via PIO (temp point SET at CLK)
    {
        uint32_t saved = pio->SM[sm].PINCTRL;
        pio->SM[sm].PINCTRL = (CLK_PIN << PIO_SM_PINCTRL_SET_BASE_Pos) |
                               (1u << PIO_SM_PINCTRL_SET_COUNT_Pos);
        pioSmExecX(pio_sm, PIO_INSTR_SET_PINDIRS_1);
        pio->SM[sm].PINCTRL = saved;
    }

    // DATA pad: pull-down and Schmitt (matches SDK gpio_set_pulls(DATA_IN, false, true))
    PADS_GPIO(DATA_PIN) = PAD_IE | PAD_DRIVE_4MA | PAD_PDE | PAD_SCHMITT;
    pioSmSetPinFunctionX(pio_sm, DATA_PIN);

    // DATA = output high initially (matches Pico SDK set pins, 1)
    pioSmExecX(pio_sm, PIO_INSTR_SET_PINDIRS_1);
    pioSmExecX(pio_sm, PIO_INSTR_SET_PINS_1);

    // Bypass input synchronizer for zero-delay reads
    pio->INPUT_SYNC_BYPASS |= (1u << DATA_PIN);

    // Diagnostics snapshot
    diag_pio_pinctrl   = pio->SM[sm].PINCTRL;
    diag_pio_shiftctrl = pio->SM[sm].SHIFTCTRL;
    diag_pio_execctrl  = pio->SM[sm].EXECCTRL;
    diag_pio_clkdiv    = pio->SM[sm].CLKDIV;
    diag_pio_offset    = pio_offset;
    for (int i = 0; i < SPI_PROGRAM_LEN; i++)
        diag_pio_instr[i] = patched[i];

    return 0;
}

void cyw43_spi_deinit(cyw43_int_t *self)
{
    (void)self;
    if (pio_sm != NULL) {
        pioSmDisableX(pio_sm);
        if (pio_offset >= 0)
            pioProgramUnload(pio_sm->block, pio_offset, SPI_PROGRAM_LEN);
        pioSmFree(pio_sm);
        pio_sm = NULL;
        pio_offset = -1;
    }
    pin_low(CYW43_PIN_WL_REG_ON);
}

void cyw43_spi_set_polarity(cyw43_int_t *self, int pol)
{
    (void)self; (void)pol;
}

// =========================================================================
//  Core SPI transfer — half-duplex PIO
//
//  Calling convention (matches Pico SDK cyw43_bus_pio_spi.c):
//    tx!=NULL, rx==NULL  →  write:  send tx_length bytes
//    tx==NULL, rx!=NULL  →  read:   command in rx[0..3], response in rx[4..]
//    tx!=NULL, rx!=NULL  →  send tx_length from tx, receive rx_length into rx
// =========================================================================

static void start_spi_comms(void)
{
    pioSmSetPinFunctionX(pio_sm, DATA_PIN);
    pioSmSetPinFunctionX(pio_sm, CLK_PIN);
    // Add pull-down on CLK for idle LOW between transfers (matches SDK gpio_pull_down)
    PADS_GPIO(CLK_PIN) |= PAD_PDE;
    pin_low(CS_PIN);
}

static void stop_spi_comms(void)
{
    pin_high(CS_PIN);
    // We need to wait a bit in case the IRQ line is incorrectly high
    // (matches Pico SDK IRQ_SAMPLE_DELAY_NS = 100ns)
    __asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;" ::: );
}

int cyw43_spi_transfer(cyw43_int_t *self, const uint8_t *tx, size_t tx_length,
                       uint8_t *rx, size_t rx_length)
{
    (void)self;
    if ((tx == NULL) && (rx == NULL)) {
        return CYW43_FAIL_FAST_CHECK(-CYW43_EINVAL);
    }
    if (pio_sm == NULL) return -1;
    spi_txn_total++;

    // Trace capture
    int tidx = -1;
    if (spi_trace_count < SPI_TRACE_MAX_TXNS) {
        tidx = spi_trace_count++;
        spi_trace_entry_t *t = &spi_trace[tidx];
        memset(t, 0, sizeof(*t));
        t->tx_len = (uint16_t)tx_length;
        t->rx_len = (uint16_t)rx_length;
        if (tx) {
            size_t c = tx_length < SPI_TRACE_MAX_BYTES ? tx_length : SPI_TRACE_MAX_BYTES;
            memcpy(t->tx_bytes, tx, c);
        } else if (rx) {
            size_t c = 4 < SPI_TRACE_MAX_BYTES ? 4 : SPI_TRACE_MAX_BYTES;
            memcpy(t->tx_bytes, rx, c);
        }
    }
    if (spi_trace_count <= 1) cyw43_spi_capture_gpio_diag();

    PIO_TypeDef *pio = pio_sm->block->pio;
    uint32_t sm = pio_sm->smidx;

    start_spi_comms();
    if (tidx >= 0) spi_trace[tidx].gpio_state = GPIO_IN_REG;

    if (rx != NULL) {
        // ---- TX+RX (read) ------------------------------------------------
        // SDK convention: tx==NULL means command is in rx[0..3]
        if (tx == NULL) { tx = rx; }

        if (tidx >= 0) {
            spi_trace[tidx].eff_tx_len = (uint16_t)tx_length;
            spi_trace[tidx].eff_rx_len = (uint16_t)rx_length;
        }

        assert(!(tx_length & 3));
        assert(!(((uintptr_t)tx) & 3));
        assert(!(((uintptr_t)rx) & 3));
        assert(!(rx_length & 3));

        uint32_t tx_bits = tx_length * 8;
        uint32_t rx_bits = (rx_length - tx_length) * 8;

        pioSmDisableX(pio_sm);
        pio_sm_set_wrap(pio_sm, pio_offset, pio_offset + SPI_OFFSET_END - 1);
        pioSmClearFifosX(pio_sm);
        pioSmExecX(pio_sm, PIO_INSTR_SET_PINDIRS_1);   // DATA = output

        pioSmRestartX(pio_sm);
        pioSmClkdivRestartX(pio_sm);

        pioSmPutX(pio_sm, tx_bits - 1);
        pioSmExecX(pio_sm, pio_encode_out_x_32());
        pioSmPutX(pio_sm, rx_bits - 1);
        pioSmExecX(pio_sm, pio_encode_out_y_32());
        pioSmExecX(pio_sm, pio_encode_jmp(pio_offset));
        pioSmEnableX(pio_sm);

        // Feed TX words
        const uint32_t *tx32 = (const uint32_t *)tx;
        for (uint32_t i = 0; i < tx_length / 4; i++) {
            while (pioSmIsTxFullX(pio_sm)) {}
            pioSmPutX(pio_sm, bswap32(tx32[i]));
        }

        // Read RX words
        uint32_t *rx32 = (uint32_t *)(rx + tx_length);
        uint32_t rx_words = (rx_length - tx_length) / 4;
        for (uint32_t i = 0; i < rx_words; i++) {
            while (pioSmIsRxEmptyX(pio_sm)) {}
            uint32_t raw = pioSmGetX(pio_sm);
            if (tidx >= 0 && i == 0) spi_trace[tidx].raw_pio_rx = raw;
            rx32[i] = bswap32(raw);
        }
        __asm volatile("" ::: "memory");

        memset(rx, 0, tx_length);  // zero command portion

    } else if (tx != NULL) {
        // ---- TX-only (write) ---------------------------------------------
        assert(!(((uintptr_t)tx) & 3));
        assert(!(tx_length & 3));
        uint32_t tx_bits = tx_length * 8;

        pioSmDisableX(pio_sm);
        pio_sm_set_wrap(pio_sm, pio_offset, pio_offset + SPI_OFFSET_LP1_END - 1);
        pioSmClearFifosX(pio_sm);
        pioSmExecX(pio_sm, PIO_INSTR_SET_PINDIRS_1);

        pioSmRestartX(pio_sm);
        pioSmClkdivRestartX(pio_sm);

        pioSmPutX(pio_sm, tx_bits - 1);
        pioSmExecX(pio_sm, pio_encode_out_x_32());
        pioSmPutX(pio_sm, 0);
        pioSmExecX(pio_sm, pio_encode_out_y_32());
        pioSmExecX(pio_sm, pio_encode_jmp(pio_offset));
        pioSmEnableX(pio_sm);

        const uint32_t *tx32 = (const uint32_t *)tx;
        for (uint32_t i = 0; i < tx_length / 4; i++) {
            while (pioSmIsTxFullX(pio_sm)) {}
            pioSmPutX(pio_sm, bswap32(tx32[i]));
        }

        // Wait for TX complete
        uint32_t stall = PIO_FDEBUG_TXSTALL(sm);
        pio->FDEBUG = stall;
        while (!(pio->FDEBUG & stall)) {}
        __asm volatile("" ::: "memory");

        pioSmDisableX(pio_sm);
        pioSmExecX(pio_sm, PIO_INSTR_SET_PINDIRS_0);  // DATA → input
    }

    // Set DATA output value to 0 for next transfer (matches SDK mov pins, null)
    pioSmExecX(pio_sm, PIO_INSTR_MOV_PINS_NULL);
    stop_spi_comms();

    // Trace: capture RX
    if (tidx >= 0 && rx != NULL) {
        size_t c = rx_length < SPI_TRACE_MAX_BYTES ? rx_length : SPI_TRACE_MAX_BYTES;
        memcpy(spi_trace[tidx].rx_bytes, rx, c);
    }
    return 0;
}

// =========================================================================
//  Higher-level SPI protocol functions — replace generic cyw43_spi.c
// =========================================================================

// --- Swap-mode register access (used before 32-bit bus mode switch) -------

uint32_t read_reg_u32_swap(cyw43_int_t *self, uint32_t fn, uint32_t reg)
{
    uint32_t buf[2] = {0};
    assert(fn != BACKPLANE_FUNCTION);
    buf[0] = SWAP32(make_cmd(false, true, fn, reg, 4));
    int ret = cyw43_spi_transfer(self, NULL, 4, (uint8_t *)buf, 8);
    if (ret != 0) return (uint32_t)ret;
    return SWAP32(buf[1]);
}

int write_reg_u32_swap(cyw43_int_t *self, uint32_t fn, uint32_t reg, uint32_t val)
{
    uint32_t buf[2];
    buf[0] = SWAP32(make_cmd(true, true, fn, reg, 4));
    buf[1] = SWAP32(val);
    int ret = cyw43_spi_transfer(self, (uint8_t *)buf, 8, NULL, 0);
    return ret;
}

// --- Normal 32-bit mode register access -----------------------------------
// Match Pico SDK _cyw43_read_reg / _cyw43_write_reg — these use a local
// buffer and always transfer 8 bytes (+ padding for backplane) instead of
// going through cyw43_read_bytes/cyw43_write_bytes.

static inline uint32_t _cyw43_read_reg(cyw43_int_t *self, uint32_t fn,
                                        uint32_t reg, uint32_t size)
{
    _Static_assert(CYW43_BACKPLANE_READ_PAD_LEN_BYTES % 4 == 0, "");
    // Padding for BACKPLANE reads, none for BUS/WLAN
    const uint32_t padding = (fn == BACKPLANE_FUNCTION)
                             ? CYW43_BACKPLANE_READ_PAD_LEN_BYTES : 0;
    // index = pad_words + cmd_word + data_word
    const int index = (CYW43_BACKPLANE_READ_PAD_LEN_BYTES / 4) + 1 + 1;
    uint32_t buf32[index];
    uint8_t *buf = (uint8_t *)buf32;

    buf32[0] = make_cmd(false, true, fn, reg, size);
    int ret = cyw43_spi_transfer(self, NULL, 4, buf, 8 + padding);
    if (ret != 0) return (uint32_t)ret;

    // Result is last word if padded, second word otherwise
    return buf32[padding > 0 ? index - 1 : 1];
}

int cyw43_read_reg_u8(cyw43_int_t *self, uint32_t fn, uint32_t reg)
{
    // After bswap32, wire byte 0 (the u8 value) is at the LSB of the result.
    // Mask to 8 bits to discard any garbage in upper bytes.
    return (int)(_cyw43_read_reg(self, fn, reg, 1) & 0xFF);
}

int cyw43_read_reg_u16(cyw43_int_t *self, uint32_t fn, uint32_t reg)
{
    // Match Pico SDK PIO: no extra byte swap.  bswap32 in cyw43_spi_transfer
    // maps the CYW43's LE wire bytes so the u16 value lands in the LOWER 16
    // bits of the returned uint32_t.  Callers store in uint16_t for correct
    // truncation.  The UPPER 16 bits contain piggybacked SPI status
    // (INTR_WITH_STATUS) and must be discarded.
    return _cyw43_read_reg(self, fn, reg, 2);
}

uint32_t cyw43_read_reg_u32(cyw43_int_t *self, uint32_t fn, uint32_t reg)
{
    return _cyw43_read_reg(self, fn, reg, 4);
}

static inline int _cyw43_write_reg(cyw43_int_t *self, uint32_t fn,
                                    uint32_t reg, uint32_t val, uint32_t size)
{
    uint32_t buf[2];
    buf[0] = make_cmd(true, true, fn, reg, size);
    buf[1] = val;
    return cyw43_spi_transfer(self, (uint8_t *)buf, 8, NULL, 0);
}

int cyw43_write_reg_u8(cyw43_int_t *self, uint32_t fn, uint32_t reg, uint32_t val)
{
    return _cyw43_write_reg(self, fn, reg, val, 1);
}

int cyw43_write_reg_u16(cyw43_int_t *self, uint32_t fn, uint32_t reg, uint16_t val)
{
    // Match Pico SDK PIO: no extra byte swap.  val is zero-extended to u32
    // in buf[1]; bswap32 in cyw43_spi_transfer then produces the LE wire
    // representation expected by the CYW43.
    return _cyw43_write_reg(self, fn, reg, val, 2);
}

int cyw43_write_reg_u32(cyw43_int_t *self, uint32_t fn, uint32_t reg, uint32_t val)
{
    return _cyw43_write_reg(self, fn, reg, val, 4);
}

// --- Bulk byte read/write (firmware download, data packets) ---------------

int cyw43_read_bytes(cyw43_int_t *self, uint32_t fn, uint32_t addr,
                     size_t len, uint8_t *dest)
{
    assert(fn != BACKPLANE_FUNCTION || (len <= CYW43_BUS_MAX_BLOCK_SIZE));
    const size_t aligned_len = (len + 3u) & ~3u;
    assert(aligned_len > 0 && aligned_len <= 0x7f8);
    assert(dest == self->spid_buf || dest < self->spid_buf ||
           dest >= (self->spid_buf + sizeof(self->spid_buf)));
    const uint32_t padding = (fn == BACKPLANE_FUNCTION)
                             ? CYW43_BACKPLANE_READ_PAD_LEN_BYTES : 0;

    // Command goes at spi_header[0] (backplane) or spi_header[pad_words] (bus/wlan)
    self->spi_header[padding > 0 ? 0 : (CYW43_BACKPLANE_READ_PAD_LEN_BYTES / 4)] =
        make_cmd(false, true, fn, addr, len);

    uint8_t *spi_buf = (uint8_t *)&self->spi_header[padding > 0 ? 0 :
                        (CYW43_BACKPLANE_READ_PAD_LEN_BYTES / 4)];

    int ret = cyw43_spi_transfer(self, NULL, 4, spi_buf, aligned_len + 4 + padding);
    if (ret != 0) return ret;

    if (dest != self->spid_buf) {
        memcpy(dest, self->spid_buf, len);
    }
    return 0;
}

int cyw43_write_bytes(cyw43_int_t *self, uint32_t fn, uint32_t addr,
                      size_t len, const uint8_t *src)
{
    assert(fn != BACKPLANE_FUNCTION || (len <= CYW43_BUS_MAX_BLOCK_SIZE));
    const size_t aligned_len = (len + 3u) & ~3u;
    assert(aligned_len > 0 && aligned_len <= 0x7f8);
    size_t xfer_len = 4u + aligned_len;

    // For WLAN_FUNCTION writes, wait for F2 to be ready (matches SDK)
    if (fn == WLAN_FUNCTION) {
        int f2_ready_attempts = 1000;
        while (f2_ready_attempts-- > 0) {
            uint32_t bus_status = cyw43_read_reg_u32(self, BUS_FUNCTION, SPI_STATUS_REGISTER);
            if (bus_status & STATUS_F2_RX_READY) break;
        }
        if (f2_ready_attempts <= 0) {
            return CYW43_FAIL_FAST_CHECK(-CYW43_EIO);
        }
    }

    if (src == self->spid_buf) {
        // Fast path: data already in spid_buf, just prepend command header
        self->spi_header[(CYW43_BACKPLANE_READ_PAD_LEN_BYTES / 4)] =
            make_cmd(true, true, fn, addr, len);
        return cyw43_spi_transfer(self,
            (uint8_t *)&self->spi_header[(CYW43_BACKPLANE_READ_PAD_LEN_BYTES / 4)],
            xfer_len, NULL, 0);
    } else {
        assert(src < self->spid_buf ||
               src >= (self->spid_buf + sizeof(self->spid_buf)));
        uint8_t *spi_buf = (uint8_t *)&self->spi_header[CYW43_BACKPLANE_READ_PAD_LEN_BYTES / 4];
        *(uint32_t *)spi_buf = make_cmd(true, true, fn, addr, len);
        memcpy(self->spid_buf, src, len);
        return cyw43_spi_transfer(self, spi_buf, xfer_len, NULL, 0);
    }
}

#endif // CYW43_USE_SPI
