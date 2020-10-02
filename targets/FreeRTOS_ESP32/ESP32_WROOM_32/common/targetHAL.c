//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <esp32_os.h>
#include <nanoWeak.h>
#include <nanoHAL_v2.h>

inline void HAL_AssertEx()
{
    asm("break.n 1");
    while(true) { /*nop*/ }
}

#if !defined(BUILD_RTM)

inline void HARD_Breakpoint() 
{
    asm("break.n 1");
    while(true) { /*nop*/ }
};

#endif  // !defined(BUILD_RTM)

// Provides information whether the configuration block storage requires erase command before sending the update command
// ESP32 is storing this using its non-volatile storage therefore no erase is required.
__nfweak bool Target_ConfigUpdateRequiresErase()
{ 
    return false;
};

inline bool Target_HasNanoBooter()
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

