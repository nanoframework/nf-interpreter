//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <nanoWeak.h>

void HAL_AssertEx()
{
    __BKPT(0);
    while(true) { /*nop*/ }
}

#if !defined(BUILD_RTM)

void HARD_Breakpoint() 
{
    __BKPT(0);
    while(true) { /*nop*/ }
};

#endif  // !defined(BUILD_RTM)

// Provides information whether the configuration block storage requires erase command before sending the update command
// The 'weak' implementation for ChibiOS targets is true
// If a target implements the store differently it has to provide a 'strong' implementation of this.
__nfweak bool Target_ConfigUpdateRequiresErase()
{
    return true;
};

bool Target_HasNanoBooter()
{ 
    return false; 
};
