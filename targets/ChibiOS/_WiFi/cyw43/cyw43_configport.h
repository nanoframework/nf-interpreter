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
// Feature configuration
// ---------------------------------------------------------------------------
#define CYW43_LWIP            1
#define CYW43_SPI_PIO         1
#define CYW43_USE_SPI         1
#define CYW43_LOGIC_DEBUG     0
#define CYW43_USE_STATS       0
#define CYW43_ENABLE_BLUETOOTH 0

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
// GP24 is also used for host wake (data in) — same physical pin
#define CYW43_PIN_WL_HOST_WAKE     24U

// What CYW43 GPIO controls the LED on Pico W
#define CYW43_WL_GPIO_LED_PIN      0
#define CYW43_WL_GPIO_COUNT        3

// ---------------------------------------------------------------------------
// PIN macros — map to ChibiOS PAL
// RP2040 has a single GPIO port: IOPORT1
// ---------------------------------------------------------------------------

#define CYW43_HAL_PIN_MODE_INPUT  PAL_MODE_INPUT
#define CYW43_HAL_PIN_MODE_OUTPUT PAL_MODE_OUTPUT_PUSHPULL
#define CYW43_HAL_PIN_PULL_NONE   0
#define CYW43_HAL_PIN_PULL_UP     1
#define CYW43_HAL_PIN_PULL_DOWN   2

static inline void cyw43_hal_pin_config(uint32_t pin, uint32_t mode, uint32_t pull, uint32_t value)
{
    (void)pull;
    (void)value;
    if (mode == CYW43_HAL_PIN_MODE_OUTPUT)
    {
        palSetPadMode(IOPORT1, pin, PAL_MODE_OUTPUT_PUSHPULL);
        if (value)
            palSetPad(IOPORT1, pin);
        else
            palClearPad(IOPORT1, pin);
    }
    else
    {
        palSetPadMode(IOPORT1, pin, PAL_MODE_INPUT);
    }
}

static inline int cyw43_hal_pin_read(uint32_t pin)
{
    return palReadPad(IOPORT1, pin);
}

static inline void cyw43_hal_pin_low(uint32_t pin)
{
    palClearPad(IOPORT1, pin);
}

static inline void cyw43_hal_pin_high(uint32_t pin)
{
    palSetPad(IOPORT1, pin);
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
// Thread safety — using ChibiOS mutex
// ---------------------------------------------------------------------------
extern mutex_t cyw43_mutex;

#define CYW43_THREAD_ENTER    chMtxLock(&cyw43_mutex)
#define CYW43_THREAD_EXIT     chMtxUnlock(&cyw43_mutex)
#define CYW43_THREAD_LOCK_CHECK

// ---------------------------------------------------------------------------
// SDPCM wait / IOCTL wait — using ChibiOS binary semaphores
// ---------------------------------------------------------------------------
extern binary_semaphore_t cyw43_sdpcm_sem;
extern binary_semaphore_t cyw43_ioctl_sem;

#define CYW43_SDPCM_SEND_COMMON_WAIT           chBSemWaitTimeout(&cyw43_sdpcm_sem, TIME_MS2I(1000))
#define CYW43_SDPCM_SEND_COMMON_WAIT_CHECK     /* nothing */
#define CYW43_DO_IOCTL_WAIT                     chBSemWaitTimeout(&cyw43_ioctl_sem, TIME_MS2I(2000))
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
// MAC address — retrieved from OTP or set a default
// ---------------------------------------------------------------------------
#define CYW43_HAL_MAC_WLAN0  0

static inline void cyw43_hal_get_mac(int itf, uint8_t mac[6])
{
    (void)itf;
    // Default MAC — will be overridden by OTP from CYW43439
    mac[0] = 0xDE;
    mac[1] = 0xAD;
    mac[2] = 0xBE;
    mac[3] = 0xEF;
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

// Assert
#define CYW43_ASSERT(expr) osalDbgAssert(expr, "cyw43 assert")

// Printf for debug
#define CYW43_PRINTF(...) /* disabled by default */

// Array size helper
#ifndef CYW43_ARRAY_SIZE
#define CYW43_ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#endif

#endif // CYW43_CONFIGPORT_H
