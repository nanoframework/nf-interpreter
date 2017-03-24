//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _NANOHAL_V2_H_
#define _NANOHAL_V2_H_ 1

#include <stdio.h>
#include <stdbool.h>

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
    COM_HANDLE               MessagingPort;
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

#endif // _NANOHAL_V2_H_
