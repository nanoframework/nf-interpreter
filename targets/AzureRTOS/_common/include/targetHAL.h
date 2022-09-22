//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_HAL_H_
#define _TARGET_HAL_H_

#include <target_board.h>
#include <nanoHAL_v2.h>

///
/// @brief Convert milliseconds to TX ticks.
///
///
#define TX_TICKS_PER_MILLISEC(milliSecs) ((milliSecs * TX_TIMER_TICKS_PER_SECOND) / 1000)

// platform dependent delay
#define PLATFORM_DELAY(milliSecs) tx_thread_sleep((milliSecs * TX_TIMER_TICKS_PER_SECOND) / 1000);

// Definitions for Sockets/Network
#define GLOBAL_LOCK_SOCKETS(x)

// get number of sockets from NetX BSD (NX_BSD_MAX_SOCKETS)
#define PLATFORM_DEPENDENT__SOCKETS_MAX_COUNT 16

#define LPCSTR const char *

#if !defined(BUILD_RTM)

#define HARD_BREAKPOINT() HARD_Breakpoint()

// #if defined(_DEBUG)
// #define DEBUG_HARD_BREAKPOINT()     HARD_Breakpoint()
// #else
// #define DEBUG_HARD_BREAKPOINT()
// #endif

// #else

// #define HARD_BREAKPOINT()
// #define DEBUG_HARD_BREAKPOINT()

#endif // !defined(BUILD_RTM)

/////////////////////////////////////////////////////////////////////////////////////////////////////
// DEBUGGER HELPER                                                                                 //
// The line below is meant to be used as helper on checking that the execution engine is running.  //
// This can be inferred by checking if Events_WaitForEvents loop is running.                       //
// The implementation should is to be provided by each target at target_common.h.in                //
////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(BUILD_RTM)
#define EVENTS_HEART_BEAT
#else
#ifndef EVENTS_HEART_BEAT
#define EVENTS_HEART_BEAT asm("nop")
#endif // EVENTS_HEART_BEAT
#endif

#define NANOCLR_STOP() CPU_Reset();

extern int HeapBegin;
extern int HeapEnd;

extern uint32_t __nanoImage_start__;
extern uint32_t __nanoImage_end__;
extern uint32_t __nanoConfig_start__;
extern uint32_t __nanoConfig_end__;
extern uint32_t __deployment_start__;
extern uint32_t __deployment_end__;

#ifdef __cplusplus
extern "C"
{
#endif

    extern unsigned int interrupt_save;

#ifdef __cplusplus
}
#endif

#define GLOBAL_LOCK()   TX_DISABLE
#define GLOBAL_UNLOCK() TX_RESTORE

#endif //_TARGET_HAL_H_
