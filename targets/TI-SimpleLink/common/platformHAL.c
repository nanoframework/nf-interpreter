//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

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
