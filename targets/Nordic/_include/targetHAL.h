//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGET_HAL_H
#define TARGET_HAL_H

#include <zephyr.h>
#include <target_board.h>

// global mutex protecting the internal state of the interpreter, including event flags
//(db) TODO:: Not doing anything now. May need a mutex here
#define GLOBAL_LOCK()
#define GLOBAL_UNLOCK()

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

// platform dependent delay
#define PLATFORM_DELAY(milliSecs) k_sleep(K_MSEC(milliSecs));

// Definitions for Sockets/Network
#define GLOBAL_LOCK_SOCKETS(x)

// get number of sockets from zephyr config process
#define PLATFORM_DEPENDENT__SOCKETS_MAX_COUNT CONFIG_NET_MAX_CONN

#define LPCSTR const char *

#if !defined(BUILD_RTM)

#define HARD_BREAKPOINT() HARD_Breakpoint()

#if defined(_DEBUG)
#define DEBUG_HARD_BREAKPOINT() HARD_Breakpoint()
#else
#define DEBUG_HARD_BREAKPOINT()
#endif

#else

#define HARD_BREAKPOINT()
#define DEBUG_HARD_BREAKPOINT()

#endif // !defined(BUILD_RTM)

#define NANOCLR_STOP() CPU_Reset();

/////////////////////////////////////////////////////////////////////////////////////////////////////
// DEBUGGER HELPER                                                                                 //
// The line bellow is meant to be used as helper on checking that the execution engine is running. //
// This can be inferred by checking if Events_WaitForEvents loop is running.                       //
// The implementation should is to be provided by each target at target_common.h.in                //
////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(BUILD_RTM)
#define EVENTS_HEART_BEAT
#else
#ifndef EVENTS_HEART_BEAT
#define EVENTS_HEART_BEAT __asm__ __volatile__("nop")
#endif // EVENTS_HEART_BEAT
#endif

extern int HeapBegin;
extern int HeapEnd;

extern uint32_t __flash_start__;
extern uint32_t __nanoImage_start__;
extern uint32_t __nanoImage_end__;
extern uint32_t __nanoConfig_start__;
extern uint32_t __nanoConfig_end__;
extern uint32_t __deployment_start__;
extern uint32_t __deployment_end__;

#endif // TARGET_HAL_H
