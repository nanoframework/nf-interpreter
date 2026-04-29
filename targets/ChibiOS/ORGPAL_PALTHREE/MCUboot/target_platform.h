//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// target_platform.h stub for the standalone MCUboot bootloader binary on ORGPAL_PALTHREE.
//
// The nanoCLR build generates this file from CMake with board-specific settings.
// The MCUboot build does not go through that pipeline, so this hand-written stub
// provides the symbols that ffconf.h needs to compute FF_VOLUMES.

#ifndef TARGET_PLATFORM_H
#define TARGET_PLATFORM_H

#if !defined(HAL_USE_SDC)
#define HAL_USE_SDC TRUE
#endif

#if !defined(HAL_USBH_USE_MSD)
#define HAL_USBH_USE_MSD FALSE
#endif

#endif // TARGET_PLATFORM_H
