//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _NANOHAL_V2_H_
#define _NANOHAL_V2_H_ 1

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <nanoWeak.h>

#include <nanoHAL_Network.h>
#include <nanoHAL_Power.h>
#include <nanoHAL_ConfigurationManager.h>

typedef enum SYSTEM_STATE
{
    SYSTEM_STATE_ISR,
    SYSTEM_STATE_NO_CONTINUATIONS,
    SYSTEM_STATE_TOTAL_STATES
} SYSTEM_STATE_type;

//////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Microsoft.SPOT.Hardware.SleepLevel !!! //
//////////////////////////////////////////////////////////////////
typedef enum SLEEP_LEVEL
{
    SLEEP_LEVEL__AWAKE         = 0x00,
    SLEEP_LEVEL__SELECTIVE_OFF = 0x10,
    SLEEP_LEVEL__SLEEP         = 0x20,
    SLEEP_LEVEL__DEEP_SLEEP    = 0x30,
    SLEEP_LEVEL__OFF           = 0x40,
} SLEEP_LEVEL_type;

//These events match emulator events in Framework\Tools\Emulator\Events.cs

#define SYSTEM_EVENT_FLAG_COM_IN                    0x00000001
#define SYSTEM_EVENT_FLAG_COM_OUT                   0x00000002
#define SYSTEM_EVENT_FLAG_STORAGE_IO                0x00000004
#define SYSTEM_EVENT_FLAG_SYSTEM_TIMER              0x00000010
//#define SYSTEM_EVENT_FLAG_TIMER1                    0x00000020
//#define SYSTEM_EVENT_FLAG_TIMER2                    0x00000040
//#define SYSTEM_EVENT_FLAG_BUTTON                    0x00000080
#define SYSTEM_EVENT_FLAG_GENERIC_PORT              0x00000100
//#define SYSTEM_EVENT_FLAG_UNUSED_0x00000200         0x00000200
//#define SYSTEM_EVENT_FLAG_UNUSED_0x00000400         0x00000400
#define SYSTEM_EVENT_FLAG_NETWORK                   0x00000800
//#define SYSTEM_EVENT_FLAG_TONE_COMPLETE             0x00001000
//#define SYSTEM_EVENT_FLAG_TONE_BUFFER_EMPTY         0x00002000
#define SYSTEM_EVENT_FLAG_SOCKET                    0x00004000
#define SYSTEM_EVENT_FLAG_ONEWIRE_MASTER            0x00008000
//#define SYSTEM_EVENT_FLAG_SPI                       0x00008000
//#define SYSTEM_EVENT_FLAG_OEM_RESERVED_1            0x00020000
//#define SYSTEM_EVENT_FLAG_OEM_RESERVED_2            0x00040000
//#define SYSTEM_EVENT_FLAG_UNUSED_0x00080000         0x00080000
//#define SYSTEM_EVENT_FLAG_UNUSED_0x00100000         0x00100000







//#define SYSTEM_EVENT_FLAG_UNUSED_0x00200000         0x00200000
//#define SYSTEM_EVENT_FLAG_UNUSED_0x00400000         0x00400000
//#define SYSTEM_EVENT_FLAG_UNUSED_0x00800000         0x00800000
//#define SYSTEM_EVENT_FLAG_UNUSED_0x01000000         0x01000000
#define SYSTEM_EVENT_FLAG_SPI_MASTER                0x02000000
#define SYSTEM_EVENT_FLAG_I2C_MASTER                0x04000000
#define SYSTEM_EVENT_HW_INTERRUPT                   0x08000000
#define SYSTEM_EVENT_FLAG_DEBUGGER_ACTIVITY         0x20000000
#define SYSTEM_EVENT_FLAG_MESSAGING_ACTIVITY        0x40000000
//#define SYSTEM_EVENT_FLAG_UNUSED_0x80000000         0x80000000
#define SYSTEM_EVENT_FLAG_ALL                       0xFFFFFFFF


////////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH nanoFramework.Runtime.Events.EventCategory (in managed code) !!! //
////////////////////////////////////////////////////////////////////////////////////////////

#define EVENT_UNKNOWN     0
#define EVENT_CUSTOM      10
#define EVENT_GPIO        20
#define EVENT_SERIAL      30
#define EVENT_NETWORK     40
#define EVENT_WIFI        50
#define EVENT_CAN         60
#define EVENT_STORAGE     70

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////


// COM_HANDLE Defines a type representing both a port type or "transport" and a port number
// The COM_HANDLE is a multi bit field value with the following bit fields usage
//    |--------+--------+--------+--------|
//    |33222222|22221111|111111  |        |
//    |10987654|32109876|54321098|76543210| bit position
//    |--------+--------+--------+--------|
//    |00000000|00000000|TTTTTTTT|pppppppp| ( transport != USB_TRANSPORT )
//    |--------+--------+--------+--------|
//    |00000000|00000000|TTTTTTTT|cccppppp| ( transport == USB_TRANSPORT )
//    |--------+--------+--------+--------|
// 
// where:
//    T => Transport type
//              USART_TRANSPORT => 1
//                USB_TRANSPORT => 2
//             SOCKET_TRANSPORT => 3
//              DEBUG_TRANSPORT => 4
//                LCD_TRANSPORT => 5
//        FLASH_WRITE_TRANSPORT => 6
//          MESSAGING_TRANSPORT => 7
//            GENERIC_TRANSPORT => 8
//    p => port instance number 
//        Port instances in the handle are 1 based. (e.g. p == 0 is invalid except when T == 0 )
//    c -> Controller instance number ( USB_TRANSPORT only )
//
//    NULL_PORT => T==0 && p == 0
//
// GENERIC_TRANSPORT is any custom port that isn't one of the above, they 
// are implemented for the DebugPort_xxxx APIs and the port number is 
// an index into a const global table of port interfaces (structure of
// function pointers) These allow custom extensions to the normal transports
// without needing to continue defining additional transport types and modifying
// switch on transport code. To keep compatibility high and code churn low, the
// previous legacy transports remain though they should be considered deprecated.
typedef int COM_HANDLE;

typedef struct HAL_DRIVER_CONFIG_HEADER
{
    unsigned int Enable;
}HAL_DRIVER_CONFIG_HEADER;

typedef struct HAL_SYSTEM_MEMORY_CONFIG
{
    unsigned int Base;
    unsigned int Size;
}HAL_SYSTEM_MEMORY_CONFIG;

typedef struct HAL_SYSTEM_CONFIG
{
    HAL_DRIVER_CONFIG_HEADER Header;

    //--//

    COM_HANDLE               DebuggerPort;
    // communication channel for debug messages in the debugger
    // which may be VS, MFDEPLOY, etc... Accessed via debug_printf
    // in the HAL/PAL and System.Diagnostics.Debug.Print() in managed
    // applications
    COM_HANDLE               DebugTextPort;

    unsigned int                   USART_DefaultBaudRate;
    // internal HAL/PAL debug/tracing channel, this is separate
    // to allow tracing messages in the driver that implements
    // the transport for the Debugger and DebugTextPort. This
    // channel is accessed via hal_printf() in the HAL/PAL
    COM_HANDLE               stdio;

    HAL_SYSTEM_MEMORY_CONFIG RAM1;
    HAL_SYSTEM_MEMORY_CONFIG FLASH1;

}HAL_SYSTEM_CONFIG;

extern HAL_SYSTEM_CONFIG  HalSystemConfig;

#ifdef __cplusplus
extern "C" {
#endif

void nanoHAL_Initialize_C();
void nanoHAL_Uninitialize_C();
void HeapLocation_C(unsigned char** baseAddress, unsigned int* sizeInBytes);

// Call to the external memory configuration and initialization function
// If a target has external memory it has to provide the implementation for it.
void Target_ExternalMemoryInit();

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

void CPU_Reset();
void CPU_Sleep(SLEEP_LEVEL_type level, uint64_t wakeEvents);
void CPU_SetPowerMode(PowerLevel_type powerLevel);

#ifdef __cplusplus
}
#endif

//--//

//
// This has to be extern "C" because we want to use platform implemented malloc 
//
#ifdef __cplusplus
extern "C" {
#endif

void* platform_malloc ( size_t size             );
void  platform_free   ( void*  ptr              );
void* platform_realloc( void*  ptr, size_t size );

#ifdef __cplusplus
}
#endif

void SystemState_Set  ( SYSTEM_STATE_type newState );
void SystemState_Clear( SYSTEM_STATE_type state    );
bool SystemState_Query( SYSTEM_STATE_type state    );

//--//


#ifdef __cplusplus
extern "C" {
#endif

void SystemState_SetNoLock  ( SYSTEM_STATE_type state );
void SystemState_ClearNoLock( SYSTEM_STATE_type state );
bool SystemState_QueryNoLock( SYSTEM_STATE_type state );

#ifdef __cplusplus
}
#endif

//--//

#define HAL_COMPLETION_IDLE_VALUE    0x0000FFFFFFFFFFFFull

// provide platform dependent delay to CLR code
#if defined(_WIN32)
#define OS_DELAY(milliSecs);
#else
#define OS_DELAY(milliSecs)         PLATFORM_DELAY(milliSecs)
#endif

//--//
// Function macros

#ifdef __cplusplus
extern "C" {
#endif

void HAL_Assert  ( const char* Func, int Line, const char* File );
// HAL_AssertEx is to be defined at platform layer
void HAL_AssertEx();

#ifdef __cplusplus
}
#endif

#if defined(PLATFORM_ARM) || defined(PLATFORM_ESP32)
    #if !defined(BUILD_RTM)
        #define       ASSERT(i)  { if(!(i)) HAL_AssertEx(); }
        #define _SIDE_ASSERTE(i) { if(!(i)) HAL_AssertEx(); }
    #endif
#else
    #if defined(_DEBUG)
#if !defined _ASSERTE
#error
#endif
        #define       ASSERT(i)  _ASSERTE(i)
        #define _SIDE_ASSERTE(i) _ASSERTE(i)
    #endif
#endif

#ifndef ASSERT
#define ASSERT(i)
#endif

#ifndef _ASSERTE
#define _ASSERTE(expr) ASSERT(expr)
#endif

#ifndef _SIDE_ASSERTE
#define _SIDE_ASSERTE(expr) (expr)
#endif

#if STATIC_ASSERT_SUPPORTED
#define CT_ASSERT_STRING( x ) #x
#define CT_ASSERT_UNIQUE_NAME(e,name)static_assert( (e), CT_ASSERT_STRING( name ) "@" __FILE__ CT_ASSERT_STRING(__LINE__) ); 
#define CT_ASSERT(e) static_assert( (e), __FILE__ CT_ASSERT_STRING(__LINE__) );
#else
// CT_ASSERT (compile-time assert) macro is used to test condition at compiler time and generate
// compiler error if condition is bool.
// Example: CT_ASSERT( sizeof( unsigned int ) == 2 ) would cause compilation error.
//          CT_ASSERT( sizeof( unsigned int ) == 4 ) compiles without error.
// Since this declaration is just typedef - it does not create any CPU code.
//
// Reason for CT_ASSERT_UNIQUE_NAME
// The possible problem with the macro - it creates multiple identical typedefs.
// It is not a problem in global scope, but if macro is used inside of struct - it generates warnings.
// CT_ASSERT_UNIQUE_NAME is the same in essence, but it provides a way to customize the name of the type.
#define CT_ASSERT_UNIQUE_NAME(e,name) typedef char __CT_ASSERT__##name[(e)?1:-1];
#define CT_ASSERT(e)                  CT_ASSERT_UNIQUE_NAME(e,nanoclr)
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(BUILD_RTM)

void debug_printf( const char *format, ... );

#else

__inline void debug_printf( const char *format, ... ) {}

#endif  // !defined(BUILD_RTM)

#ifdef __cplusplus
}
#endif
//--//

#ifdef __cplusplus
extern "C" {
#endif

bool Target_HasNanoBooter();
void HARD_Breakpoint();
bool Target_ConfigUpdateRequiresErase();

#ifdef __cplusplus
}
#endif

// Watchdog driver
#include <nanoHAL_Watchdog.h>

#include <nanoHAL_Windows_Storage.h>

#endif // _NANOHAL_V2_H_
