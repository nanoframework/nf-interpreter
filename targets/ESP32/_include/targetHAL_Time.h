//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGET_HAL_TIME_H
#define TARGET_HAL_TIME_H

#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>

#define HAL_Time_CurrentSysTicks xTaskGetTickCount
#define ESP32_TICKS_PER_MS(x)    ((x * (uint64_t)configTICK_RATE_HZ) / 1000)

#endif //TARGET_HAL_TIME_H
