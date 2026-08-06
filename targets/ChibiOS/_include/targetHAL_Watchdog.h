//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGETHAL_WATCHDOG_H
#define TARGETHAL_WATCHDOG_H

#if (HAL_USE_WDG)
  #if !defined(RP2040) && !defined(RP2350)
  #if (STM32_LSI_ENABLED == FALSE)
    #error "LSI clock must be enabled for independent watchdog (IWDG) to work"
  #endif
  #endif

  // if the timeout isn't set at target level, set a default value (10 seconds)
  #if !defined(IWATCHDOG_TIMEOUT_MILLIS)
  #define IWATCHDOG_TIMEOUT_MILLIS        10000
  #endif

#endif

#ifdef __cplusplus
extern "C" {
#endif
void Watchdog_Init(void);
void Watchdog_Reset(void);
#ifdef __cplusplus
}
#endif

#endif
