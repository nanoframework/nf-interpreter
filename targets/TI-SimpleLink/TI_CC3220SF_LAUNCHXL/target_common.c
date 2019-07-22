//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_v2.h>
#include "target_board.h"
#include "target_common.h"

HAL_SYSTEM_CONFIG HalSystemConfig =
{
    { true }, // HAL_DRIVER_CONFIG_HEADER Header;

    1, //ConvertCOM_DebugHandle(1),
    0,//ConvertCOM_DebugHandle(0),
    921600,
    0,  // STDIO = COM2 or COM1

    { RAM1_MEMORY_StartAddress, RAM1_MEMORY_Size },
    { FLASH1_MEMORY_StartAddress, FLASH1_MEMORY_Size }
};

HAL_TARGET_CONFIGURATION  g_TargetConfiguration;

// these addresses are the start and end address of flash region FLASH
uint32_t __nanoImage_start__ = FLASH1_MEMORY_StartAddress;
uint32_t __nanoImage_end__ = (FLASH1_MEMORY_StartAddress + FLASH1_MEMORY_Size);
