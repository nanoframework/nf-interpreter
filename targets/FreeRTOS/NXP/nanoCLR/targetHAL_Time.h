//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_HAL_TIME_H_
#define _TARGET_HAL_TIME_H_ 1

#include "FreeRTOS.h"
#include "task.h"

#define HAL_Time_CurrentSysTicks           xTaskGetTickCount

#endif //_TARGET_HAL_TIME_H_
