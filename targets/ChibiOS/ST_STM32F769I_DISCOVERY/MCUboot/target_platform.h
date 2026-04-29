//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Stub for MCUboot build — satisfies the #include <target_platform.h> in ffconf.h.
// Defines HAL_USE_SDC and HAL_USBH_USE_MSD so that FF_VOLUMES evaluates correctly.

#ifndef TARGET_PLATFORM_H
#define TARGET_PLATFORM_H

#if !defined(HAL_USE_SDC)
#define HAL_USE_SDC     TRUE
#endif

#if !defined(HAL_USBH_USE_MSD)
#define HAL_USBH_USE_MSD FALSE
#endif

#endif // TARGET_PLATFORM_H
