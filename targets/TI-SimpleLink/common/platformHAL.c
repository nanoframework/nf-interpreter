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
    while(true) { /*nop*/ }
}

#if !defined(BUILD_RTM)

void HARD_Breakpoint() 
{
    __asm__("BKPT");
    while(true) { /*nop*/ }
};

#endif  // !defined(BUILD_RTM)


// Provides information whether the configuration block storage requires erase command before sending the update command
// The 'weak' implementation for TI SimpleLink targets is false
// If a target implements the store differently it has to provide a 'strong' implementation of this.
__nfweak bool Target_ConfigUpdateRequiresErase()
{
    return false;
};

bool Target_HasNanoBooter()
{ 
    return false; 
};

// declarations of platform capabilities
uint32_t GetPlatformCapabilities()
{ 
    return 0; 
};

// declarations of target capabilities
// If a target has something to declare it has to provide a 'strong' implementation of this.
__nfweak uint32_t GetTargetCapabilities()
{ 
    return 0; 
};
