//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <stdint.h>
#include <stdbool.h>
#include <cmsis_gcc.h>
#include <nanoHAL_v2.h>
#include <platform_target_capabilities.h>

void HAL_AssertEx()
{
    __BKPT(0);
    while (true)
    { /*nop*/
    }
}

#if !defined(BUILD_RTM)

void HARD_Breakpoint()
{
    __BKPT(0);
    while (true)
    { /*nop*/
    }
};

#endif // !defined(BUILD_RTM)

// The 'weak' implementation for NXP targets is true
// If a target implements the store differently it has to provide a 'strong' implementation of this.
__nfweak TARGET_CONFIG_UPDATE_REQUIRES_ERASE(true);

// NXP targets implement nanoBooter
__nfweak TARGET_HAS_NANOBOOTER(true);

// NXP targets don't declare any target capability
__nfweak GET_TARGET_CAPABILITIES(0);

// NXP doesn't have a proprietatry bootloader
inline TARGET_HAS_PROPRITARY_BOOTER(false);

inline TARGET_IFU_CAPABLE(false);
