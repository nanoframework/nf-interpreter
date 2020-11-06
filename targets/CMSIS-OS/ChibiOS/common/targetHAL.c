//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
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

// provide platform level "weak" implementations for all capabilities
__nfweak TARGET_CONFIG_UPDATE_REQUIRES_ERASE(true);
__nfweak TARGET_HAS_NANOBOOTER(true);
__nfweak TARGET_HAS_PROPRITARY_BOOTER(false);
__nfweak TARGET_IFU_CAPABLE(false);

// STM32 default capabiliy is JTAG update
// declared as "weak" to allow targets to provide hard implementation
__nfweak GET_TARGET_CAPABILITIES(TargetCapabilities_JtagUpdate);
