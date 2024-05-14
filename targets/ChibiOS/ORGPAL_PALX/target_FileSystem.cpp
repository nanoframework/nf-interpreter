//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_v2.h>
#include <nanoHAL_Types.h>
#include <nanoPAL_FileSystem.h>
#include <littlefs_FS_Driver.h>

FILESYSTEM_DRIVER_INTERFACE g_LITTLEFS_FILE_SYSTEM_DriverInterface = {
    &LITTLEFS_FS_Driver::FindOpen,
    &LITTLEFS_FS_Driver::FindNext,
    &LITTLEFS_FS_Driver::FindClose,

    &LITTLEFS_FS_Driver::GetFileInfo,

    &LITTLEFS_FS_Driver::CreateDirectory,
    &LITTLEFS_FS_Driver::Move,
    &LITTLEFS_FS_Driver::Delete,

    &LITTLEFS_FS_Driver::GetAttributes,
    &LITTLEFS_FS_Driver::SetAttributes,

    &LITTLEFS_FS_Driver::Format,
    &LITTLEFS_FS_Driver::LoadMedia,
    &LITTLEFS_FS_Driver::GetSizeInfo,
    &LITTLEFS_FS_Driver::FlushAll,
    &LITTLEFS_FS_Driver::GetVolumeLabel,

    "LITTLEFS",
    0,
};

extern STREAM_DRIVER_INTERFACE g_LITTLEFS_STREAM_DriverInterface;

FILESYSTEM_INTERFACES g_AvailableFSInterfaces[] = {
    // { &g_WINDOWS_FILE_SYSTEM_DriverInterface       , &g_WINDOWS_STREAMING_DriverInterface },
    {&g_LITTLEFS_FILE_SYSTEM_DriverInterface, &g_LITTLEFS_STREAM_DriverInterface},
};

const size_t g_InstalledFSCount = ARRAYSIZE(g_AvailableFSInterfaces);

uint32_t g_FS_NumVolumes;
STREAM_DRIVER_DETAILS *g_FS_DriverDetails;
FileSystemVolume *g_FS_Volumes;

void FS_AddVolumes()
{
    // 2 SPI flash devices
    // 1 SD card
    // 1 USB MSD device
    g_FS_NumVolumes = 1;

    g_FS_Volumes = (FileSystemVolume *)platform_malloc(sizeof(FileSystemVolume) * g_FS_NumVolumes);

    // sanity check
    if (g_FS_Volumes == NULL)
    {
        platform_free(g_FS_DriverDetails);

        return;
    }

    // clear the memory
    memset(g_FS_Volumes, 0, sizeof(FileSystemVolume) * g_FS_NumVolumes);

    g_FS_DriverDetails = (STREAM_DRIVER_DETAILS *)platform_malloc(sizeof(STREAM_DRIVER_DETAILS) * g_FS_NumVolumes);

    // sanity check
    if (g_FS_DriverDetails == NULL)
    {
        return;
    }

    // clear the memory
    memset(g_FS_DriverDetails, 0, sizeof(STREAM_DRIVER_DETAILS) * g_FS_NumVolumes);

    // AT25SF641, drive I:, volume 0
    FileSystemVolumeList::AddVolume(
        &g_FS_Volumes[0],
        "I:",
        0,
        &g_LITTLEFS_STREAM_DriverInterface,
        &g_LITTLEFS_FILE_SYSTEM_DriverInterface,
        0,
        FALSE);

    // W25Q128, drive F:, volume 1
    FileSystemVolumeList::AddVolume(
        &g_FS_Volumes[1],
        "J:",
        0,
        &g_LITTLEFS_STREAM_DriverInterface,
        &g_LITTLEFS_FILE_SYSTEM_DriverInterface,
        1,
        FALSE);
}
