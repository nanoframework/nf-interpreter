//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_HAL_H_
#define _TARGET_HAL_H_

#include <target_board.h>
#include <esp32_os.h>

extern portMUX_TYPE globalLockMutex;
#define GLOBAL_LOCK(x)              portENTER_CRITICAL(&globalLockMutex);
#define GLOBAL_UNLOCK(x)            portEXIT_CRITICAL(&globalLockMutex);
#define ASSERT_IRQ_MUST_BE_OFF()   // TODO need to determine if this needs implementation

// platform dependent delay
#define PLATFORM_DELAY(milliSecs)   vTaskDelay(milliSecs);

// Definitions for Sockets/Network
#define GLOBAL_LOCK_SOCKETS(x)   

#define PLATFORM_DEPENDENT__SOCKETS_MAX_COUNT    16

#define LPCSTR  const char*


// these macros are to be used at entry/exit of native interrupt handlers
#define NATIVE_INTERRUPT_START  SystemState_SetNoLock( SYSTEM_STATE_ISR              );   \
                                SystemState_SetNoLock( SYSTEM_STATE_NO_CONTINUATIONS );
#define NATIVE_INTERRUPT_END    SystemState_ClearNoLock( SYSTEM_STATE_NO_CONTINUATIONS ); \
                                SystemState_ClearNoLock( SYSTEM_STATE_ISR              );


// TODO: Doesn't seem to be defined anywhere, used  clr corlib math
#define INT32 int32_t
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

#define NANOCLR_STOP() HARD_BREAKPOINT()


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
    #define EVENTS_HEART_BEAT __asm__ __volatile__ ("nop")
    #endif // EVENTS_HEART_BEAT
#endif

extern int HeapBegin;
extern int HeapEnd;

#endif //_TARGET_HAL_H_
