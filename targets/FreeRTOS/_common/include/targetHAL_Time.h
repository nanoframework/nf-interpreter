//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_HAL_TIME_H_
#define _TARGET_HAL_TIME_H_ 1

#include <stdint.h>
#include <targetHAL.h>
#include <FreeRTOS.h>
#include <task.h>

#ifdef __cplusplus
extern "C"
{
#endif

    uint64_t HAL_Time_CurrentSysTicks(void);

#ifdef __cplusplus
}
#endif

#endif //_TARGET_HAL_TIME_H_
