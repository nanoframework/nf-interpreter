//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_HAL_H_
#define _TARGET_HAL_H_

#include <target_board.h>
#include <nanoWeak.h>
#include <cmsis_gcc.h>

#define GLOBAL_LOCK(x)              chSysLock();
#define GLOBAL_UNLOCK(x);           chSysUnlock();
#define ASSERT_IRQ_MUST_BE_OFF()    // TODO need to determine if this needs implementation

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

inline void HARD_Breakpoint() 
{
    __BKPT(0);
    while(true) { /*nop*/ }
};

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

inline bool Target_HasNanoBooter() { return true; };

#define NANOCLR_STOP() CPU_Reset();

inline void HAL_AssertEx()
{
    __BKPT(0);
    while(true) { /*nop*/ }
}

// Provides information whether the configuration block storage requires erase command before sending the update command
// The 'weak' implementation for ChibiOS targets is true
// If a target implements the store differently it has to provide a 'strong' implementation of this.
__nfweak bool Target_ConfigUpdateRequiresErase() { return true; };

extern int HeapBegin;
extern int HeapEnd;

extern uint32_t __nanoImage_start__;
extern uint32_t __nanoImage_end__;
extern uint32_t __nanoConfig_start__;
extern uint32_t __nanoConfig_end__;
extern uint32_t __deployment_start__;
extern uint32_t __deployment_end__;

#endif //_TARGET_HAL_H_
