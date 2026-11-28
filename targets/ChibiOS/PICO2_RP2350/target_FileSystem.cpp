//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_v2.h>
#include <nanoHAL_Types.h>
#include <nanoPAL_FileSystem.h>
#include <littlefs_FS_Driver.h>
#include "Target_System_IO_FileSystem.h"

extern FILESYSTEM_DRIVER_INTERFACE g_LITTLEFS_FILE_SYSTEM_DriverInterface;
extern STREAM_DRIVER_INTERFACE g_LITTLEFS_STREAM_DriverInterface;

#if HAL_USE_MMC_SPI
#include <fatfs_FS_Driver.h>
extern FILESYSTEM_DRIVER_INTERFACE g_FATFS_FILE_SYSTEM_DriverInterface;
extern STREAM_DRIVER_INTERFACE g_FATFS_STREAM_DriverInterface;
#endif

FILESYSTEM_INTERFACES g_AvailableFSInterfaces[] = {
    {&g_LITTLEFS_FILE_SYSTEM_DriverInterface, &g_LITTLEFS_STREAM_DriverInterface},
#if HAL_USE_MMC_SPI
    {&g_FATFS_FILE_SYSTEM_DriverInterface, &g_FATFS_STREAM_DriverInterface},
#endif
};

const size_t g_InstalledFSCount = ARRAYSIZE(g_AvailableFSInterfaces);

uint32_t g_FS_NumVolumes;
STREAM_DRIVER_DETAILS *g_FS_DriverDetails;
FileSystemVolume *g_FS_Volumes;

void FS_AddVolumes()
{
    g_FS_NumVolumes = 1;

    g_FS_Volumes = new FileSystemVolume[g_FS_NumVolumes];
    g_FS_DriverDetails = new STREAM_DRIVER_DETAILS[g_FS_NumVolumes];

    // Internal flash littlefs, drive I:, volume 0
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
    // SPI SD cards are mounted explicitly by the managed SDCard API.
}
