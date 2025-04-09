//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_v2.h>
#include <nanoHAL_Types.h>
#include <nanoPAL_FileSystem.h>
#include <littlefs_FS_Driver.h>
#include <fatfs_FS_Driver.h>
#include "Target_System_IO_FileSystem.h"

extern FILESYSTEM_DRIVER_INTERFACE g_LITTLEFS_FILE_SYSTEM_DriverInterface;
extern STREAM_DRIVER_INTERFACE g_FATFS_STREAM_DriverInterface;

extern FILESYSTEM_DRIVER_INTERFACE g_FATFS_FILE_SYSTEM_DriverInterface;
extern STREAM_DRIVER_INTERFACE g_LITTLEFS_STREAM_DriverInterface;

#if HAL_USE_SDC
#include <target_storage_config.h>
extern "C" void SdCardDetectCallback(void *arg);
#endif

extern "C" void UsbMsdForceMount();

FILESYSTEM_INTERFACES g_AvailableFSInterfaces[] = {
    {&g_FATFS_FILE_SYSTEM_DriverInterface, &g_FATFS_STREAM_DriverInterface},
    {&g_LITTLEFS_FILE_SYSTEM_DriverInterface, &g_LITTLEFS_STREAM_DriverInterface},
};

const size_t g_InstalledFSCount = ARRAYSIZE(g_AvailableFSInterfaces);

uint32_t g_FS_NumVolumes;
STREAM_DRIVER_DETAILS *g_FS_DriverDetails;
FileSystemVolume *g_FS_Volumes;

void FS_AddVolumes()
{
    // 1 SPI flash devices
    // 2 SPI flash devices
    g_FS_NumVolumes = 2;

    g_FS_Volumes = new FileSystemVolume[g_FS_NumVolumes];
    g_FS_DriverDetails = new STREAM_DRIVER_DETAILS[g_FS_NumVolumes];

    // W25Q128, drive I:, volume 0
    FileSystemVolumeList::AddVolume(
        &g_FS_Volumes[0],
        "I:",
        0,
        g_AvailableFSInterfaces[1].streamDriver,
        g_AvailableFSInterfaces[1].fsDriver,
        0,
        FALSE);
}

void FS_MountRemovableVolumes()
{

#if HAL_USE_SDC
    SdCardDetectCallback(&SD_CARD_DRIVER);
#endif

#if defined(HAL_USBH_USE_MSD) && (HAL_USBH_USE_MSD == TRUE)
    UsbMsdForceMount();
#endif
}
