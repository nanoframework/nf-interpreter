//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_HAL_H_
#define _TARGET_HAL_H_

#include <target_board.h>
#include <Board.h>
#include <gpio.h>
#include <ti/drivers/dpl/ClockP.h>
#include <ti/sysbios/knl/Task.h>

#define GLOBAL_LOCK()               uint32_t taskKey = Task_disable();
#define GLOBAL_UNLOCK()             Task_restore(taskKey);

// platform dependent delay
#define PLATFORM_DELAY(milliSecs)   ClockP_usleep(milliSecs * 1000);

// Definitions for Sockets/Network
#define GLOBAL_LOCK_SOCKETS(x)   

#define PLATFORM_DEPENDENT__SOCKETS_MAX_COUNT    16

#define LPCSTR  const char*

// these macros are to be used at entry/exit of native interrupt handlers
#define NATIVE_INTERRUPT_START  SystemState_SetNoLock( SYSTEM_STATE_ISR              );   \
                                SystemState_SetNoLock( SYSTEM_STATE_NO_CONTINUATIONS );
#define NATIVE_INTERRUPT_END    SystemState_ClearNoLock( SYSTEM_STATE_NO_CONTINUATIONS ); \
                                SystemState_ClearNoLock( SYSTEM_STATE_ISR              );

#define TRUE  true
#define FALSE false

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

// map TI SimpleLink calls to UART output to nanoFramework API
// this is valid only for debug buils
// release and RTM build don't call these
#if defined(BUILD_RTM)

#define UART_PRINT(x,...)
#define INFO_PRINT(x,...)

#else

#if defined(DEBUG) || defined(_DEBUG)

#define UART_PRINT      DebuggerPort_WriteProxy
#define INFO_PRINT      DebuggerPort_WriteProxy

#else

#define UART_PRINT(x,...)
#define INFO_PRINT(x,...)

#endif // DEBUG

#endif // defined(BUILD_RTM)

#define NANOCLR_STOP() HARD_BREAKPOINT()

/////////////////////////////////////////////////////////////////////////////////////////////////////
// DEBUGGER HELPER                                                                                 //
// The line below is meant to be used as helper on checking that the execution engine is running. //
// This can be inferred by checking if Events_WaitForEvents loop is running.                       //
// The implementation should is to be provided by each target at target_common.h.in                //
////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(BUILD_RTM)
    #define EVENTS_HEART_BEAT
#else
    #ifndef EVENTS_HEART_BEAT
    #define EVENTS_HEART_BEAT __asm__ __volatile__ ("nop")
    #endif // EVENTS_HEART_BEAT
#endif

extern int HeapBegin;
extern int HeapEnd;

extern uint32_t __nanoImage_start__;
extern uint32_t __nanoImage_end__;
extern uint32_t __nanoConfig_start__;
extern uint32_t __nanoConfig_end__;
extern uint32_t __deployment_start__;
extern uint32_t __deployment_end__;

#endif //_TARGET_HAL_H_
