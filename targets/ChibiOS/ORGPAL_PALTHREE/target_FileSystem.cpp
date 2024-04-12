//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_v2.h>
#include <nanoHAL_Types.h>
#include <nanoPAL_FileSystem.h>

FILESYSTEM_INTERFACES g_AvailableFSInterfaces[] =
{
    // { &g_WINDOWS_FILE_SYSTEM_DriverInterface       , &g_WINDOWS_STREAMING_DriverInterface },
    // { &g_EMULATOR_FAT32_FILE_SYSTEM_DriverInterface, &g_FAT32_STREAM_DriverInterface      },
};

const size_t g_InstalledFSCount = ARRAYSIZE(g_AvailableFSInterfaces);

void FS_AddVolumes()
{
    // AT25SF641
    FS_MountVolume("LFS1", 0, 0);

    // W25Q128
    FS_MountVolume("LFS2", 0, 0);
}
