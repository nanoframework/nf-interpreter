//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGET_HAL_H
#define TARGET_HAL_H

// #include <nanoHAL_Power.h>

#define PLATFORM_DELAY(milliSecs) Sleep(milliSecs);

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
    return 100000000;
}

inline uint64_t CPU_MicrosecondsToTicks(UINT64 uSec)
{
    return uSec * 10;
}

inline uint64_t CPU_MillisecondsToTicks(UINT64 uSec)
{
    return uSec * 10 * 1000;
}

#endif //TARGET_HAL_H
