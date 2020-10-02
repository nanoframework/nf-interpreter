//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_HAL_H_
#define _TARGET_HAL_H_

#include <target_board.h>
#include <lwipopts.h>
#include <cmsis_gcc.h>
#include <nanoHAL_v2.h>
#include <hal.h>

// platform dependent delay
#define PLATFORM_DELAY(milliSecs) osDelay(milliSecs);

// Definitions for Sockets/Network
#define GLOBAL_LOCK_SOCKETS(x)

// get number of sockets from lwIP options
#define PLATFORM_DEPENDENT__SOCKETS_MAX_COUNT MEMP_NUM_NETCONN

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

extern int my_lock_counter;

#define GLOBAL_LOCK()                                                                                                  \
    {                                                                                                                  \
        if (port_is_isr_context())                                                                                     \
            chSysLockFromISR();                                                                                        \
        else                                                                                                           \
            chSysLock();

#define GLOBAL_UNLOCK()                                                                                                \
    if (port_is_isr_context())                                                                                         \
        chSysUnlockFromISR();                                                                                          \
    else                                                                                                               \
        chSysUnlock();                                                                                                 \
    }

#endif //_TARGET_HAL_H_

//#define GLOBAL_LOCK()              chSysLock();
//#define GLOBAL_UNLOCK();           chSysUnlock();
