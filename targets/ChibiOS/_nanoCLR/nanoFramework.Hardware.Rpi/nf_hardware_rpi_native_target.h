//
// Copyright (c) .NET nanoFramework PIO contributors
//
// Shared helpers for the RP2040/RP2350 PIO native interop. Include after the CMSIS header
// (rp2040.h / rp2350.h) so PIO_TypeDef and PIO0/PIO1/PIO2 are defined.
//

#ifndef NF_HARDWARE_RPI_NATIVE_TARGET_H
#define NF_HARDWARE_RPI_NATIVE_TARGET_H

// Maps a PIO block index to its register block, or nullptr for an invalid index.
// RP2040 exposes PIO0/PIO1; RP2350 adds PIO2.
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

#endif // NF_HARDWARE_RPI_NATIVE_TARGET_H
