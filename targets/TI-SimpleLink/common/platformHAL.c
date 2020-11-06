//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_v2.h>
#include <nanoWeak.h>
#include <stdbool.h>

void HAL_AssertEx()
{
    __asm__("BKPT");
    while (true)
    { /*nop*/
    }
}

#if !defined(BUILD_RTM)

void HARD_Breakpoint()
{
    __asm__("BKPT");
    while (true)
    { /*nop*/
    }
};

#endif // !defined(BUILD_RTM)

// Provides information whether the configuration block storage requires erase command before sending the update command
// The 'weak' implementation for TI SimpleLink targets is false
// If a target implements the store differently it has to provide a 'strong' implementation of this.
__nfweak TARGET_CONFIG_UPDATE_REQUIRES_ERASE(false);

// TI SimpleLink targets do not implement nanoBooter
__nfweak TARGET_HAS_NANOBOOTER(false);

// declarations of target capabilities
// TI SimpleLink targets don't declare any target capability
// If a target has something to declare it has to provide a 'strong' implementation of this.
__nfweak GET_TARGET_CAPABILITIES(0);

inline TARGET_IFU_CAPABLE(false);
