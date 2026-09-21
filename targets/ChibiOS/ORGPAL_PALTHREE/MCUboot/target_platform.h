//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// target_platform.h stub for the standalone MCUboot bootloader binary on ORGPAL_PALTHREE.
//
// The nanoCLR build generates this file from CMake with board-specific settings.

#ifndef TARGET_PLATFORM_H
#define TARGET_PLATFORM_H

#if !defined(HAL_USE_SDC)
#if defined(CONFIG_NF_FEATURE_MCUBOOT_HAS_SDCARD) && CONFIG_NF_FEATURE_MCUBOOT_HAS_SDCARD
#define HAL_USE_SDC TRUE
#else
#define HAL_USE_SDC FALSE
#endif
#endif

#if !defined(HAL_USBH_USE_MSD)
#if defined(CONFIG_NF_FEATURE_MCUBOOT_HAS_USB_MSD) && CONFIG_NF_FEATURE_MCUBOOT_HAS_USB_MSD
#define HAL_USBH_USE_MSD TRUE
#else
#define HAL_USBH_USE_MSD FALSE
#endif
#endif

#endif // TARGET_PLATFORM_H
