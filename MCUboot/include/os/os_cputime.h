//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// os/os_cputime.h — Mynewt CPU-timer stub for MCUboot boot_serial.c.
//
// boot_serial.c calls os_cputime_delay_usecs(250000) in bs_reset() before
// hal_system_reset() to allow any pending serial output to flush. On our
// bare-metal ChibiOS platform we implement this as a simple busy-wait loop.
// Exact timing is not required — any delay in the 100ms–500ms range is fine.

#ifndef OS_OS_CPUTIME_H
#define OS_OS_CPUTIME_H

#include <stdint.h>

// Busy-wait for approximately the given number of microseconds.
// Loop tuned for Cortex-M7 at ~216 MHz with -Og: ~43 iterations/µs.
static inline void os_cputime_delay_usecs(uint32_t usecs)
{
    volatile uint32_t count = usecs * 43U;
    while (count--)
    {
    }
}

#endif // OS_OS_CPUTIME_H
