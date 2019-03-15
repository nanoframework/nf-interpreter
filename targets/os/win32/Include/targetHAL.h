//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_HAL_H_
#define _TARGET_HAL_H_

// #include <nanoHAL_Power.h>

#if defined(_WIN32)
#define NANOCLR_STOP() ::DebugBreak()
#pragma warning( error : 4706 ) // error C4706: assignment within conditional expression
#endif

#if !defined(BUILD_RTM)

inline void __cdecl HARD_Breakpoint()
{
    if(::IsDebuggerPresent())
    {
        ::DebugBreak();
    }
}

#define HARD_BREAKPOINT()     HARD_Breakpoint()

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

#endif  // !defined(BUILD_RTM)


inline bool Target_HasNanoBooter() { return false; };

#endif //_TARGET_HAL_H_
