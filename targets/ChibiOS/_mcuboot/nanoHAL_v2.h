//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// nanoHAL_v2.h stub for the standalone MCUboot bootloader binary.
//
// The nanoCLR build provides src/HAL/Include/nanoHAL_v2.h.  The MCUboot
// bootloader does not include the HAL source tree, so this stub provides the
// ASSERT macro used by target_ext_flash.c.

#ifndef NANOHAL_V2_STUB_H
#define NANOHAL_V2_STUB_H

// Trap on assertion failure with a debugger breakpoint.
#define ASSERT(x) do { if (!(x)) { __asm volatile("BKPT #0"); } } while(0)

#endif // NANOHAL_V2_STUB_H
