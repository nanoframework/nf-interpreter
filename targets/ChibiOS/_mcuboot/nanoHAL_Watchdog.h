//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Minimal nanoHAL_Watchdog.h stub for the MCUboot bootloader build context.
// Watchdog_Reset() is used in AT25SF641_EraseChip(), which MCUboot does not
// call. Provided as a no-op so the translation unit links cleanly.

#ifndef NANOHAL_WATCHDOG_H
#define NANOHAL_WATCHDOG_H

static inline void Watchdog_Reset(void)
{
}

#endif // NANOHAL_WATCHDOG_H
