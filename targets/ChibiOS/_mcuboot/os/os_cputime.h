//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// ChibiOS-specific Mynewt CPU-timer shim for MCUboot boot_serial.c.

#ifndef OS_OS_CPUTIME_H
#define OS_OS_CPUTIME_H

#include <stdint.h>
#include "ch.h"

static inline void os_cputime_delay_usecs(uint32_t usecs)
{
    chThdSleepMicroseconds(usecs);
}

#endif // OS_OS_CPUTIME_H
