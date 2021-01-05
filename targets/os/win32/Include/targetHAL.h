//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_HAL_H_
#define _TARGET_HAL_H_

// #include <nanoHAL_Power.h>

// set min possible number of sockets
#define PLATFORM_DEPENDENT__SOCKETS_MAX_COUNT 1

#if defined(_WIN32)
#define NANOCLR_STOP() ::DebugBreak()
#pragma warning(error : 4706) // error C4706: assignment within conditional expression
#endif

#if !defined(BUILD_RTM)

inline void __cdecl HARD_Breakpoint()
{
    if (::IsDebuggerPresent())
    {
        ::DebugBreak();
    }
}

#define HARD_BREAKPOINT() HARD_Breakpoint()

inline bool Target_ConfigUpdateRequiresErase()
{
    return true;
}

// #if defined(_DEBUG)
// #define DEBUG_HARD_BREAKPOINT()     HARD_Breakpoint()
// #else
// #define DEBUG_HARD_BREAKPOINT()
// #endif

// #else

// #define HARD_BREAKPOINT()
// #define DEBUG_HARD_BREAKPOINT()

#endif // !defined(BUILD_RTM)

inline bool Target_HasNanoBooter()
{
    return false;
};

inline bool Target_IFUCapable()
{
    return false;
};

inline bool Target_HasProprietaryBooter()
{
    return false;
};

inline uint32_t GetPlatformCapabilities()
{
    return 0;
};

inline uint32_t GetTargetCapabilities()
{
    return 0;
};

inline bool RequestToLaunchProprietaryBootloader()
{
    return false;
};

inline bool RequestToLaunchNanoBooter()
{
    return false;
};

inline uint32_t CPU_TicksPerSecond()
{
    return 1000000000;
}

inline uint64_t CPU_MicrosecondsToTicks(UINT64 uSec)
{
    return uSec * 1000;
}

#endif //_TARGET_HAL_H_
