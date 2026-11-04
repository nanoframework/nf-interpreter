//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef HALCONF_NF_H
#define HALCONF_NF_H

// Enables the SD card
#if !defined(FATFS_HAL_DEVICE) || defined(__DOXYGEN__)
#define FATFS_HAL_DEVICE SDCD1
#endif

// enables STM32 Flash driver
#if !defined(HAL_NF_USE_STM32_FLASH)
#define HAL_NF_USE_STM32_FLASH TRUE
#endif

// enable USB MSD (from ChibiOS Contrib)
#define HAL_USBH_USE_MSD FALSE

#endif // HALCONF_NF_H
