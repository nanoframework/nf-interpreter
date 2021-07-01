//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_HAL_TIME_H_
#define _TARGET_HAL_TIME_H_ 1

#include  <sys/time.h>
#include  <time.h>
#include <esp32_os.h>
#include <stdint.h>
#include <targetHAL.h>

#ifdef __cplusplus
extern "C"
{
#endif

    uint64_t HAL_Time_CurrentSysTicks(void);

#ifdef __cplusplus
}
#endif

#define ESP32_TICKS_PER_MS(x)       ((x * (uint64_t)configTICK_RATE_HZ) / 1000)

#endif //_TARGET_HAL_TIME_H_
