//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_HAL_TIME_H_
#define _TARGET_HAL_TIME_H_ 1

#include  <sys/time.h>
#include  <time.h>

#include <esp32_os.h>

#define HAL_Time_CurrentSysTicks    xTaskGetTickCount
#define ESP32_TICKS_PER_MS(x)       ((x * (uint64_t)configTICK_RATE_HZ) / 1000)

#endif //_TARGET_HAL_TIME_H_
