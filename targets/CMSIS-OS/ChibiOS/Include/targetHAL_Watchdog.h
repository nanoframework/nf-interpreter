//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include <nanoHAL_Watchdog.h>
#include <target_common.h>

#ifndef _TARGETHAL_WATCHDOG_H_
#define _TARGETHAL_WATCHDOG_H_ 1    

#if (HAL_USE_WDG)
  #if (STM32_LSI_ENABLED == FALSE)
    #error "LSI clock must be enabled for independent watchdog (IWDG) to work"
  #endif
  
  // if the timeout isn't set at target level, set a default value (10 seconds)
  #if !defined(IWATCHDOG_TIMEOUT_MILLIS)
  #define IWATCHDOG_TIMEOUT_MILLIS        10000
  #endif

#endif

#endif
