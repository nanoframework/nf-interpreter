//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//
#include <nanoHAL_Watchdog.h>

#ifndef _TARGETHAL_WATCHDOG_H_
#define _TARGETHAL_WATCHDOG_H_ 1    
    #if (HAL_USE_WDG)
        #if (STM32_LSI_ENABLED == FALSE)
            #error "LSI clock must be enabled for independent watchdog (IWDG) to work"
        #endif
        #ifndef IWATCHDOG_CLK_TYPICAL
            #error "Must define the macro IWATCHDOG_CLK_TYPICAL and provide the watchdog clock frequency in hertz"
        #endif
        #ifndef IWATCHDOG_TIMEOUT_MILLIS
            #error "Must define the macro IWATCHDOG_TIMEOUT_MILLIS, watchdog timeout in millis. Currently minimum timeout supported is 1000 millis"
        #endif
    #endif
#endif
