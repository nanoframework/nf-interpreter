//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "stdafx.h"

#include <nanoHAL_Types.h>
#include <nanoPAL_FileSystem.h>
#include <littlefs_FS_Driver.h>

extern FILESYSTEM_DRIVER_INTERFACE g_LITTLEFS_FILE_SYSTEM_DriverInterface;
extern STREAM_DRIVER_INTERFACE g_LITTLEFS_STREAM_DriverInterface;

FILESYSTEM_INTERFACES g_AvailableFSInterfaces[] = {
    {&g_LITTLEFS_FILE_SYSTEM_DriverInterface, &g_LITTLEFS_STREAM_DriverInterface},
};

const size_t g_InstalledFSCount = ARRAYSIZE(g_AvailableFSInterfaces);

uint32_t g_FS_NumVolumes;
STREAM_DRIVER_DETAILS *g_FS_DriverDetails;
FileSystemVolume *g_FS_Volumes;

void FS_AddVolumes()
{
    // 1 memory mapped volume
    g_FS_NumVolumes = 1;

    g_FS_Volumes = new FileSystemVolume[g_FS_NumVolumes];
    g_FS_DriverDetails = new STREAM_DRIVER_DETAILS[g_FS_NumVolumes];

    // memory mapped, drive I:, volume 0
    FileSystemVolumeList::AddVolume(
        &g_FS_Volumes[0],
        "I:",
        0,
        g_AvailableFSInterfaces[0].streamDriver,
        g_AvailableFSInterfaces[0].fsDriver,
        0,
        FALSE);
}

void FS_MountRemovableVolumes()
{
    // empty on purpose as we don't have removable volumes
}
