//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NANOFRAMEWORK_HARDWARE_PICO_NATIVE_TARGET_H
#define NANOFRAMEWORK_HARDWARE_PICO_NATIVE_TARGET_H

static inline PIO_TypeDef *PioFromIndex(int index)
{
    switch (index)
    {
        case 0:
            return PIO0;
        case 1:
            return PIO1;
#if defined(RP2350)
        case 2:
            return PIO2;
#endif
        default:
            return nullptr;
    }
}

#if defined(RP2350)
#define PIO_MAX_BLOCK 2
#define PIO_MAX_PIN   47
#else
#define PIO_MAX_BLOCK 1
#define PIO_MAX_PIN   29
#endif

#if defined(RP_PIO_REQUIRED)
#define PIO_MIN_BLOCK 1
#else
#define PIO_MIN_BLOCK 0
#endif

#define VALIDATE_PIO_BLOCK(block)                                                                                      \
    if ((block) < 0 || (block) > PIO_MAX_BLOCK)                                                                        \
    {                                                                                                                  \
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);                                                                \
    }                                                                                                                  \
    if ((block) < PIO_MIN_BLOCK)                                                                                       \
    {                                                                                                                  \
        NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);                                                                    \
    }

#define VALIDATE_SM(sm)                                                                                                \
    if ((sm) < 0 || (sm) > 3)                                                                                          \
    {                                                                                                                  \
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);                                                                \
    }

#define VALIDATE_NOT_DISPOSED(stack)                                                                   \
    {                                                                                                  \
        CLR_RT_HeapBlock *pThis = (stack).This();                                                      \
        FAULT_ON_NULL(pThis);                                                                          \
        if (pThis[FIELD___disposed].NumericByRef().u1 != 0)                                            \
        {                                                                                              \
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);                                              \
        }                                                                                              \
    }

#endif // NANOFRAMEWORK_HARDWARE_PICO_NATIVE_TARGET_H
