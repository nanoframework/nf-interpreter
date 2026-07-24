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

void PioEnsureOutOfReset(int blockIndex);

#endif // NANOFRAMEWORK_HARDWARE_PICO_NATIVE_TARGET_H
