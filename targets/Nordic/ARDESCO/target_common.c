//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_v2.h>
#include <target_board.h>
#include <target_common.h>
#include <platform_target_capabilities.h>

HAL_SYSTEM_CONFIG HalSystemConfig = {
    {true}, // HAL_DRIVER_CONFIG_HEADER Header;

    1,      // dbg_uart. Ignored. dbg uart port set in nflib_main
    0,      // dbg_txt.  Ignored. Set to default zephyr console
    921600, // Ignored. Dbg port speed set in device overlay file. (TODO:: recognize this val)
    0,      // STDIO = COM2 or COM1

    {0, 0},       // RAM Start addr and size.   Filled in by nflib_main
    {0, 0x800000} // Flash Start addr and size.
};

// Network configuration pointer
HAL_TARGET_CONFIGURATION g_TargetConfiguration;
