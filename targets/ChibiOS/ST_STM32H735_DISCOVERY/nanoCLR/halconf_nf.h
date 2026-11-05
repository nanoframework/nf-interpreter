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

//  Enables the FSMC subsystem.
#if !defined(HAL_NF_USE_FSMC)
#define HAL_NF_USE_FSMC FALSE
#endif

// enables STM32 QSPI driver
#if !defined(HAL_NF_USE_STM32_QSPI)
#define HAL_NF_USE_STM32_QSPI FALSE
#endif

#endif // HALCONF_NF_H
