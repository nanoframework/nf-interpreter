//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// CYW43 driver configuration port for ChibiOS/RT on RP2040 (Pico W).
// This header maps the cyw43-driver HAL requirements to ChibiOS primitives.

#ifndef CYW43_CONFIGPORT_H
#define CYW43_CONFIGPORT_H

#include <ch.h>
#include <hal.h>
#include <string.h>
#include <stdlib.h>

// Ensure static_assert is available in C mode
#if !defined(__cplusplus) && !defined(static_assert)
#define static_assert _Static_assert
#endif

// ---------------------------------------------------------------------------
// Logging — redirect CYW43 driver printf/warn to our debug_printf
// ---------------------------------------------------------------------------
extern void debug_printf(const char *format, ...);
#define CYW43_PRINTF(...) debug_printf(__VA_ARGS__)

// ---------------------------------------------------------------------------
// Feature configuration
// ---------------------------------------------------------------------------
#define CYW43_LWIP            1
#define CYW43_SPI_PIO         1
#define CYW43_USE_SPI         1
#define CYW43_LOGIC_DEBUG     0
#define CYW43_USE_STATS       0
#define CYW43_ENABLE_BLUETOOTH 0
#define CYW43_USE_OTP_MAC     1

#ifndef CYW43_HOST_NAME
#define CYW43_HOST_NAME       "PicoW"
#endif

// WiFi interface indices are defined as enums in cyw43_ll.h (CYW43_ITF_STA, CYW43_ITF_AP)

// Firmware inclusion — the combined firmware blob for CYW43439
// Note: path is relative, resolved via cyw43_driver_SOURCE_DIR in include paths
#define CYW43_CHIPSET_FIRMWARE_INCLUDE_FILE "firmware/w43439A0_7_95_49_00_combined.h"
#define CYW43_WIFI_NVRAM_INCLUDE_FILE       "firmware/wifi_nvram_43439.h"

// ---------------------------------------------------------------------------
// Pico W GPIO assignments for CYW43
// ---------------------------------------------------------------------------
#define CYW43_PIN_WL_REG_ON        23U
#define CYW43_PIN_WL_DATA_OUT      24U
#define CYW43_PIN_WL_CS            25U
#define CYW43_PIN_WL_CLK           29U
// GP24 is also used for interrupt signaling — same physical pin as DATA.
// Define BOTH HOST_WAKE and IRQ to match the Pico SDK:
//   - HOST_WAKE → sets INTERRUPT_POLARITY_HIGH in SPI_BUS_CONTROL so the
//     chip drives GP24 HIGH when it has events (between SPI transfers)
//   - HOST_WAKE also sets host_interrupt_pin_active=1 so the driver's
//     interrupt-pin gate in cyw43_ll_sdpcm_poll_device checks for HIGH
// Between transfers DATA is input with no pull — chip can drive it.
#define CYW43_PIN_WL_HOST_WAKE    24U
#define CYW43_PIN_WL_IRQ          24U

// What CYW43 GPIO controls the LED on Pico W
#define CYW43_WL_GPIO_LED_PIN      0
#define CYW43_WL_GPIO_COUNT        3

// ---------------------------------------------------------------------------
// PIN macros — direct RP2040 register access
// We bypass ChibiOS PAL to preserve pad electrical config (drive strength,
// pulls, Schmitt) that cyw43_spi_init sets up.  Only change direction (OE)
// and output level via SIO registers; never touch FUNCSEL or PADS.
// ---------------------------------------------------------------------------
#define SIO_BASE_ADDR       0xD0000000
#define SIO_GPIO_OUT_SET    (*(volatile uint32_t *)(SIO_BASE_ADDR + 0x014))
#define SIO_GPIO_OUT_CLR    (*(volatile uint32_t *)(SIO_BASE_ADDR + 0x018))
#define SIO_GPIO_OE_SET     (*(volatile uint32_t *)(SIO_BASE_ADDR + 0x024))
#define SIO_GPIO_OE_CLR     (*(volatile uint32_t *)(SIO_BASE_ADDR + 0x028))
#define SIO_GPIO_IN_REG     (*(volatile uint32_t *)(SIO_BASE_ADDR + 0x004))

#define CYW43_HAL_PIN_MODE_INPUT  0
#define CYW43_HAL_PIN_MODE_OUTPUT 1
#define CYW43_HAL_PIN_PULL_NONE   0
#define CYW43_HAL_PIN_PULL_UP     1
#define CYW43_HAL_PIN_PULL_DOWN   2

static inline void cyw43_hal_pin_config(uint32_t pin, uint32_t mode, uint32_t pull, uint32_t value)
{
    (void)pull;
    // Only change direction and output level. FUNCSEL and pad config
    // are set once by cyw43_spi_init / cyw43_spi_gpio_setup and must
    // not be overwritten.
    if (mode == CYW43_HAL_PIN_MODE_OUTPUT)
    {
        if (value)
            SIO_GPIO_OUT_SET = 1U << pin;
        else
            SIO_GPIO_OUT_CLR = 1U << pin;
        SIO_GPIO_OE_SET = 1U << pin;
    }
    else
    {
        SIO_GPIO_OE_CLR = 1U << pin;
    }
}

static inline int cyw43_hal_pin_read(uint32_t pin)
{
    // For the HOST_WAKE / IRQ pin (GP24), always return "active" (1).
    // This forces cyw43_ll_sdpcm_poll_device to skip its fast-path
    // (which checks GP24 and returns early when it reads 0) and instead
    // read the SPI status register — the definitive source of truth for
    // pending F2 data.
    //
    // On Pico W, GP24 is shared between SPI DATA and the interrupt line.
    // Between SPI transactions the PIO leaves the pad configured as an
    // input, but GPIO_IN consistently reads 0 during the poll loop even
    // when the CYW43 has pending events (the SPI trace confirms GP24 goes
    // HIGH inside SPI transactions that assert CS).  The exact cause is
    // still under investigation; forcing the full path costs one extra
    // SPI status read per poll cycle (~2 µs at 31 MHz) which is negligible
    // on a 10 ms poll interval.
    if (pin == CYW43_PIN_WL_HOST_WAKE)
        return 1;
    return (SIO_GPIO_IN_REG >> pin) & 1;
}

static inline void cyw43_hal_pin_low(uint32_t pin)
{
    SIO_GPIO_OUT_CLR = 1U << pin;
}

static inline void cyw43_hal_pin_high(uint32_t pin)
{
    SIO_GPIO_OUT_SET = 1U << pin;
}

static inline void cyw43_hal_pin_config_irq_falling(uint32_t pin, int enable)
{
    (void)pin;
    (void)enable;
    // IRQ on falling edge not used in polled mode
}

// ---------------------------------------------------------------------------
// Timing
// ---------------------------------------------------------------------------
static inline uint32_t cyw43_hal_ticks_us(void)
{
    // Convert ChibiOS system ticks to microseconds
    return (uint32_t)TIME_I2US(chVTGetSystemTimeX());
}

static inline uint32_t cyw43_hal_ticks_ms(void)
{
    return (uint32_t)TIME_I2MS(chVTGetSystemTimeX());
}

static inline void cyw43_delay_ms(uint32_t ms)
{
    chThdSleepMilliseconds(ms);
}

static inline void cyw43_delay_us(uint32_t us)
{
    // ChibiOS doesn't have sub-millisecond sleep on all platforms,
    // but the RP2040 port supports microsecond precision via osalThreadSleepMicroseconds
    if (us >= 1000)
    {
        chThdSleepMilliseconds(us / 1000);
    }
    else if (us > 0)
    {
        // Busy-wait for short delays (< 1ms)
        systime_t start = chVTGetSystemTimeX();
        systime_t target = TIME_US2I(us);
        while (chTimeDiffX(start, chVTGetSystemTimeX()) < target)
        {
            // spin
        }
    }
}

// ---------------------------------------------------------------------------
// Thread safety — recursive lock (CYW43 driver nests THREAD_ENTER/EXIT)
// ---------------------------------------------------------------------------
extern void cyw43_thread_enter(void);
extern void cyw43_thread_exit(void);

#define CYW43_THREAD_ENTER    cyw43_thread_enter()
#define CYW43_THREAD_EXIT     cyw43_thread_exit()
#define CYW43_THREAD_LOCK_CHECK

// ---------------------------------------------------------------------------
// SDPCM wait / IOCTL wait — release lock, wait on semaphore, re-lock
// (matches Pico SDK cyw43_arch_wait_for_work_until pattern)
// ---------------------------------------------------------------------------
extern void cyw43_sdpcm_wait(void);
extern void cyw43_ioctl_wait(void);

#define CYW43_SDPCM_SEND_COMMON_WAIT           cyw43_sdpcm_wait()
#define CYW43_SDPCM_SEND_COMMON_WAIT_CHECK     /* nothing */
#define CYW43_DO_IOCTL_WAIT                     cyw43_ioctl_wait()
#define CYW43_DO_IOCTL_WAIT_CHECK               /* nothing */

// ---------------------------------------------------------------------------
// Memory allocation
// ---------------------------------------------------------------------------
#define CYW43_MALLOC(size)       chHeapAlloc(NULL, size)
#define CYW43_FREE(ptr)          chHeapFree(ptr)

// Fallback to standard if heap allocator not available
#ifndef CYW43_MALLOC
#define CYW43_MALLOC(size)       malloc(size)
#define CYW43_FREE(ptr)          free(ptr)
#endif

// ---------------------------------------------------------------------------
// Error codes expected by cyw43_ll.c
// ---------------------------------------------------------------------------
#define CYW43_EPERM     1
#define CYW43_EIO       5
#define CYW43_EINVAL    22
#define CYW43_ETIMEDOUT 110

// ---------------------------------------------------------------------------
// MIN macro if not provided
// ---------------------------------------------------------------------------
#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

// ---------------------------------------------------------------------------
// Internal poll scheduling
// ---------------------------------------------------------------------------
// Called when the CYW43 driver wants its poll function called soon.
// We signal the WiFi lwIP thread to handle it.
extern void cyw43_schedule_internal_poll_dispatch(void (*func)(void));
extern void cyw43_post_poll_hook(void);

// ---------------------------------------------------------------------------
// MAC address — read from cyw43_state after firmware init (OTP MAC)
// ---------------------------------------------------------------------------
#define CYW43_HAL_MAC_WLAN0  0

// Implemented in cyw43_arch_chibios.c — must read from cyw43_state.mac
// (which requires cyw43_t to be fully defined, not possible in this header).
void cyw43_hal_get_mac(int itf, uint8_t mac[6]);

// Generate a locally-administered MAC address as fallback when OTP is unset
static inline void cyw43_hal_generate_laa_mac(int itf, uint8_t mac[6])
{
    (void)itf;
    // Locally administered, unicast MAC address
    mac[0] = 0x02;
    mac[1] = 0xCF;
    mac[2] = 0x43;
    mac[3] = 0x39;
    mac[4] = 0x00;
    mac[5] = 0x01;
}

// ---------------------------------------------------------------------------
// lwIP integration callbacks — declared in cyw43.h with proper types
// Do NOT redeclare here; the driver headers provide the correct signatures.
// ---------------------------------------------------------------------------
void cyw43_cb_process_ethernet(void *self, int itf, size_t len, const uint8_t *buf);

// ---------------------------------------------------------------------------
// Misc
// ---------------------------------------------------------------------------

// Byte swap helper
#ifndef __builtin_bswap32
static inline uint32_t __builtin_bswap32_impl(uint32_t x)
{
    return ((x & 0x000000FF) << 24) | ((x & 0x0000FF00) << 8) | ((x & 0x00FF0000) >> 8) |
           ((x & 0xFF000000) >> 24);
}
#define __builtin_bswap32 __builtin_bswap32_impl
#endif

// After each poll cycle, signal semaphores so waiting threads can proceed
#define CYW43_POST_POLL_HOOK cyw43_post_poll_hook();

// Assert
#define CYW43_ASSERT(expr) osalDbgAssert(expr, "cyw43 assert")

// Printf for debug — route through debug_printf for CYW43_WARN visibility
extern void debug_printf(const char *format, ...);
#define CYW43_PRINTF(...) debug_printf(__VA_ARGS__)

// Array size helper
#ifndef CYW43_ARRAY_SIZE
#define CYW43_ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#endif

#endif // CYW43_CONFIGPORT_H
