//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_v2.h>
#include <nanoHAL_Types.h>
#include <nanoPAL_FileSystem.h>
#include <fatfs_FS_Driver.h>
#include "Target_System_IO_FileSystem.h"

extern FILESYSTEM_DRIVER_INTERFACE g_FATFS_FILE_SYSTEM_DriverInterface;

#if HAL_USE_SDC
#include <target_storage_config.h>
extern "C" void SdCardDetectCallback(void *arg);
#endif

extern "C" void UsbMsdForceMount();

FILESYSTEM_INTERFACES g_AvailableFSInterfaces[] = {
    // This is the place to add the available file systems to the system
};

const size_t g_InstalledFSCount = ARRAYSIZE(g_AvailableFSInterfaces);

void FS_AddVolumes()
{
    // This is the place to add volumes to the system
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
