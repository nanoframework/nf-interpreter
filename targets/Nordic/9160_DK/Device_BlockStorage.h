//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef DEVICE_BLOCKSTORAGE_H
#define DEVICE_BLOCKSTORAGE_H


// this device has 1 block storage devices
#define TARGET_BLOCKSTORAGE_COUNT 1

// SOC flash
#if (NCS_VERSION_MAJOR < 2)
#define FLASH_DRIVER_NAME DT_CHOSEN_ZEPHYR_FLASH_CONTROLLER_LABEL
#else
//#define FLASH_DRIVER DT_NODELABEL(flash0)
#define FLASH_DRIVER_NAME         DT_LABEL(DT_CHOSEN(zephyr_flash_controller))
#endif


#endif // DEVICE_BLOCKSTORAGE_H