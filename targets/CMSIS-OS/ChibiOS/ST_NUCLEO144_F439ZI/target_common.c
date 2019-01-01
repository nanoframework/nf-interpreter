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

    1,  //ConvertCOM_DebugHandle(1),
    0,  //ConvertCOM_DebugHandle(0),
    115200,
    0,  // STDIO = COM2 or COM1

    { RAM1_MEMORY_StartAddress, RAM1_MEMORY_Size },
    { FLASH1_MEMORY_StartAddress, FLASH1_MEMORY_Size }
};

HAL_TARGET_CONFIGURATION  g_TargetConfiguration;
