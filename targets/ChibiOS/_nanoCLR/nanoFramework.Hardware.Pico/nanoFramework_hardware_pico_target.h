//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NANOFRAMEWORK_HARDWARE_PICO_TARGET_H
#define NANOFRAMEWORK_HARDWARE_PICO_TARGET_H

#include <ch.h>
#include <hal.h>
#include <nanoHAL.h>
#include <nanoFramework_hardware_pico.h>

#if defined(RP2350)
#define PIO_MAX_BLOCK 2
#define PIO_MAX_PIN   47
#else
#define PIO_MAX_BLOCK 1
#define PIO_MAX_PIN   29
#endif

#if defined(TARGET_HAS_WIFI)
#define PIO_MIN_BLOCK 1
#else
#define PIO_MIN_BLOCK 0
#endif

#define NF_PICO_PIO_IRQ_PRIORITY    3
#define NF_PICO_PIO_FIFO_TIMEOUT_MS 1000u
#define NF_PICO_PIO_DRAIN_LIMIT     0x10000u
#define NF_PICO_PIO_IRQ_FLAG_COUNT  8

#define VALIDATE_PIO_BLOCK(block)                                                                                      \
    if ((block) < 0 || (block) > PIO_MAX_BLOCK)                                                                        \
    {                                                                                                                  \
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);                                                                     \
    }                                                                                                                  \
    if ((block) < PIO_MIN_BLOCK)                                                                                       \
    {                                                                                                                  \
        NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);                                                                    \
    }

#define VALIDATE_SM(sm)                                                                                                \
    if ((sm) < 0 || (sm) >= static_cast<int>(RP_PIO_NUM_STATE_MACHINES))                                               \
    {                                                                                                                  \
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);                                                                     \
    }

#define VALIDATE_NOT_DISPOSED(pThis)                                                                                   \
    FAULT_ON_NULL(pThis);                                                                                              \
    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)                                                                \
    {                                                                                                                  \
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);                                                                  \
    }

#endif // NANOFRAMEWORK_HARDWARE_PICO_TARGET_H
