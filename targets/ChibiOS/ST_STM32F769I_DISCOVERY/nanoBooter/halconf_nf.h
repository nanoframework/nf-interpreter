//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef HALCONF_NF_H
#define HALCONF_NF_H

// Enables the SD card
#if !defined(FATFS_HAL_DEVICE) || defined(__DOXYGEN__)
// this board requires SDCD2 not SDCD1
#define FATFS_HAL_DEVICE SDCD2
#endif

// enables STM32 Flash driver
#if !defined(HAL_NF_USE_STM32_FLASH)
#define HAL_NF_USE_STM32_FLASH TRUE
#endif

// Enable graphics low level hardware on STM32F769

#if !defined(HAL_DSI_MODULE_ENABLED)
#define HAL_DSI_MODULE_ENABLED TRUE
#endif

// enable USB MSD (from ChibiOS Contrib)
#define HAL_USBH_USE_MSD FALSE

#endif // HALCONF_NF_H
