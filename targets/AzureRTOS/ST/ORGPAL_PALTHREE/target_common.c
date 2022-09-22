//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_v2.h>
#include "target_board.h"
#include "target_common.h"
#include <platform_target_capabilities.h>

HAL_SYSTEM_CONFIG HalSystemConfig = {
    {true}, // HAL_DRIVER_CONFIG_HEADER Header;

    1, // ConvertCOM_DebugHandle(1),
    0, // ConvertCOM_DebugHandle(0),
    921600,
    0, // STDIO = COM2 or COM1

    {RAM1_MEMORY_StartAddress, RAM1_MEMORY_Size},
    {FLASH1_MEMORY_StartAddress, FLASH1_MEMORY_Size}};

HAL_TARGET_CONFIGURATION g_TargetConfiguration;

// this target can use both JTAG and DFU for updates
inline GET_TARGET_CAPABILITIES(TargetCapabilities_JtagUpdate | TargetCapabilities_DfuUpdate);

// This target can have it's MAC address changed
bool Target_CanChangeMacAddress()
{
    return true;
}
