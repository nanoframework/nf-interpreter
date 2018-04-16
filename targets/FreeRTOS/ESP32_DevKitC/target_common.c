//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_v2.h>
#include "target_board.h"
#include "target_common.h"

HAL_SYSTEM_CONFIG HalSystemConfig =
{
    { true }, // HAL_DRIVER_CONFIG_HEADER Header;

    //--//

//    {                                               // unsigned int      DebuggerPorts[MAX_DEBUGGERS];
        0//ConvertCOM_DebugHandle(0),           // DebuggerPort COM_HANDLE
//    }
    ,

//    {
        0//ConvertCOM_DebugHandle(0),           // MessagingPort COM_HANDLE
//    }
    ,

    0,//ConvertCOM_DebugHandle(0),              // DebugTextPort COM_HANDLE
    115200,                                     // USART_DefaultBaudRate
    0,  // STDIO = COM2 or COM1

    { RAM1_MEMORY_StartAddress, RAM1_MEMORY_Size },
    { FLASH1_MEMORY_StartAddress, FLASH1_MEMORY_Size }
};

HAL_TARGET_CONFIGURATION  g_TargetConfiguration;
