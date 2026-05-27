//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGET_HAL_TIME_H
#define TARGET_HAL_TIME_H

#include "FreeRTOS.h"
#include "task.h"

#define HAL_Time_CurrentSysTicks           xTaskGetTickCount

#endif //TARGET_HAL_TIME_H
