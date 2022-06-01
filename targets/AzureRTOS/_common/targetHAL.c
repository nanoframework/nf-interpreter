//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_v2.h>
#include <platform_target_capabilities.h>

void HAL_AssertEx()
{
    __asm("BKPT #0\n");
    while (true)
    { /*nop*/
    }
}

#if !defined(BUILD_RTM)

void HARD_Breakpoint()
{
    __asm("BKPT #0\n");
    while (true)
    { /*nop*/
    }
};

#endif // !defined(BUILD_RTM)

// provide platform level "weak" implementations for all capabilities
__nfweak TARGET_IFU_CAPABLE(false);

// STM32 default capability is JTAG update
// declared as "weak" to allow targets to provide hard implementation
__nfweak GET_TARGET_CAPABILITIES(TargetCapabilities_JtagUpdate);

// Azure RTOS targets can't change their MAC address
// Implemented as "weak" to allow it to be replaced with "hard" implementation at target level.
__nfweak bool Target_CanChangeMacAddress()
{
    return false;
}
