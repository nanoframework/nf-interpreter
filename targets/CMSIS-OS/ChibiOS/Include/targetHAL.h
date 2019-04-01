//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_HAL_H_
#define _TARGET_HAL_H_

#include <target_board.h>
#include <cmsis_gcc.h>
#include <nanoHAL_v2.h>

#define GLOBAL_LOCK(x)              chSysLock();
#define GLOBAL_UNLOCK(x);           chSysUnlock();
#define ASSERT_IRQ_MUST_BE_OFF()    // TODO need to determine if this needs implementation

// platform dependent delay
#define PLATFORM_DELAY(milliSecs)   osDelay(milliSecs);

// Definitions for Sockets/Network
#define GLOBAL_LOCK_SOCKETS(x)       

#define PLATFORM_DEPENDENT__SOCKETS_MAX_COUNT    16

#define LPCSTR  const char*

// these macros are to be used at entry/exit of native interrupt handlers
#define NATIVE_INTERRUPT_START  SystemState_SetNoLock( SYSTEM_STATE_ISR              );   \
                                SystemState_SetNoLock( SYSTEM_STATE_NO_CONTINUATIONS );
#define NATIVE_INTERRUPT_END    SystemState_ClearNoLock( SYSTEM_STATE_NO_CONTINUATIONS ); \
                                SystemState_ClearNoLock( SYSTEM_STATE_ISR              );

#if !defined(BUILD_RTM)

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
    #define EVENTS_HEART_BEAT __NOP()
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

#endif //_TARGET_HAL_H_
