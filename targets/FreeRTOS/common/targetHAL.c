//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoCLR_Headers.h>
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

// NXP targets don't declare any target capability
__nfweak GET_TARGET_CAPABILITIES(0);

// NXP doesn't have a proprietatry bootloader
inline TARGET_HAS_PROPRIETARY_BOOTER(false);

inline TARGET_IFU_CAPABLE(false);
