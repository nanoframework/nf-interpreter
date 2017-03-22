//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_HAL_H_
#define _TARGET_HAL_H_

#include <target_board.h>

#if !defined(BUILD_RTM)

// FIXME IMPLEMENT
inline void HARD_Breakpoint() { };

#define HARD_BREAKPOINT()     HARD_Breakpoint()

// #if defined(_DEBUG)
// #define DEBUG_HARD_BREAKPOINT()     HARD_Breakpoint()
// #else
// #define DEBUG_HARD_BREAKPOINT()
// #endif

// #else

// #define HARD_BREAKPOINT()
// #define DEBUG_HARD_BREAKPOINT()

#endif  // !defined(BUILD_RTM)

#define NANOCLR_STOP() HARD_BREAKPOINT()

inline void HAL_AssertEx()
{
    __BKPT(0);
    while(true) { /*nop*/ }
}

extern int HeapBegin;
extern int HeapEnd;

// FIXME uncomment? declaration needed here as external?
// extern char * nanoCLR_Dat_Start;
// extern char * nanoCLR_Dat_End;

extern uint32_t __nanoImage_start__;
extern uint32_t __nanoImage_end__;
extern uint32_t __deployment_start__;
extern uint32_t __deployment_end__;

#endif //_TARGET_HAL_H_
