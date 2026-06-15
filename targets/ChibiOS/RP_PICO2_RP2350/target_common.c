//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_v2.h>
#include "target_board.h"
#include "target_common.h"

HAL_SYSTEM_CONFIG HalSystemConfig = {
    {true}, // HAL_DRIVER_CONFIG_HEADER Header;

    1, // DebuggerPort (COM1)
    0, // DebuggerPort (COM0, unused)
    921600, // DebuggerBaudrate
    0, // STDIO port (not used on this target)

    {RAM1_MEMORY_StartAddress, RAM1_MEMORY_Size},
    {FLASH1_MEMORY_StartAddress, FLASH1_MEMORY_Size}};

HAL_TARGET_CONFIGURATION g_TargetConfiguration;

bool Target_IFUCapable()
{
    return true;
}
