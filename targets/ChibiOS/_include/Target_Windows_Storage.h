//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGET_WINDOWS_STORAGE_H
#define TARGET_WINDOWS_STORAGE_H

#include <hal.h>

#if defined(HAL_USBH_USE_MSD) && (HAL_USBH_USE_MSD == TRUE)
#include "usbh/dev/msd.h"
#endif

#define SDCARD_POLLING_INTERVAL (10)
#define SDCARD_POLLING_DELAY    (500)

#define USB_MSD_POLLING_INTERVAL (1000)
#define USB_MSD_POLLING_DELAY    (1000)

// the drive indexes have to be used instead of fixed drive letters because a target can have one or more
// and those have to follow the sequence that is used in ChibiOS FatFS wrappers
// SD Card (or SPI) is 1st and USB MAS is 2nd (if SD Card is enabled)
// this is also mapped in the FatFS configuration
#if (HAL_USE_SDC == TRUE)

#define SD_CARD_DRIVE_INDEX         "0:"
#define SD_CARD_DRIVE_INDEX_NUMERIC (0)

#endif

#if (HAL_USE_SDC == TRUE) && defined(HAL_USBH_USE_MSD)

#define USB_MSD_DRIVE_INDEX         "1:"
#define USB_MSD_DRIVE_INDEX_NUMERIC (1)

#elif !#if (HAL_USE_SDC == TRUE) && defined(HAL_USBH_USE_MSD)

#define USB_MSD_DRIVE_INDEX         "0:"
#define USB_MSD_DRIVE_INDEX_NUMERIC (0)

#endif

#ifdef __cplusplus
extern "C"
{
#endif

    // declaration of storage working threads
    void SdCardWorkingThread(void const *argument);
    void UsbMsdWorkingThread(void const *argument);

#ifdef __cplusplus
}
#endif

#endif // TARGET_WINDOWS_STORAGE_H
