//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NANOFRAMEWORK_HARDWARE_PICO_NATIVE_TARGET_H
#define NANOFRAMEWORK_HARDWARE_PICO_NATIVE_TARGET_H

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

// Releases a PIO block (and the IO/PAD banks) from reset on first use; idempotent. Defined in
// the PioBlock translation unit so the reset state is shared across all entry points.
void PioEnsureOutOfReset(int blockIndex);

#endif // NANOFRAMEWORK_HARDWARE_PICO_NATIVE_TARGET_H
