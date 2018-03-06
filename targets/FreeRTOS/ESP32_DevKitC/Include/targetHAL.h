//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_HAL_H_
#define _TARGET_HAL_H_

#include <target_board.h>

//TODO: implement
//#define GLOBAL_LOCK(x)              chSysLock();
//#define GLOBAL_UNLOCK(x);           chSysUnlock();
#define GLOBAL_LOCK(x)             
#define GLOBAL_UNLOCK(x);          
#define ASSERT_IRQ_MUST_BE_OFF()   // TODO need to determine if this needs implementation

// Defininitions for Sockets/Network
#define GLOBAL_LOCK_SOCKETS(x)       
#define SOCKETS_MAX_COUNT 16
#define LPCSTR  const uint8_t*
#define NETWORK_INTERFACE_COUNT 2


// these macros are to be used at entry/exit of native interrupt handlers
#define NATIVE_INTERRUPT_START  SystemState_SetNoLock( SYSTEM_STATE_ISR              );   \
                                SystemState_SetNoLock( SYSTEM_STATE_NO_CONTINUATIONS );
#define NATIVE_INTERRUPT_END    SystemState_ClearNoLock( SYSTEM_STATE_NO_CONTINUATIONS ); \
                                SystemState_ClearNoLock( SYSTEM_STATE_ISR              );


// TODO: Doesn't seem to be defined anywhere, used  clr corlib math
#define INT32 int32_t
#define TRUE  true
#define FALSE false
#define fmod(x,y) (1)


#if !defined(BUILD_RTM)

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
 //   __BKPT(0);
    while(true) { /*nop*/ }
}

extern int HeapBegin;
extern int HeapEnd;

// FIXME uncomment? declaration needed here as external?
// extern char * nanoCLR_Dat_Start;
// extern char * nanoCLR_Dat_End;

extern uint32_t __nanoImage_start__;
extern uint32_t __nanoImage_end__;
extern uint32_t __nanoConfig_start__;
extern uint32_t __nanoConfig_end__;
extern uint32_t __deployment_start__;
extern uint32_t __deployment_end__;

#endif //_TARGET_HAL_H_
