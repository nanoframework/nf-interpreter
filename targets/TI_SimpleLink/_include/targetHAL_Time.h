//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGET_HAL_TIME_H
#define TARGET_HAL_TIME_H

#include <ti/sysbios/knl/Clock.h>

#define HAL_Time_CurrentSysTicks Clock_getTicks

#ifdef __cplusplus
extern "C"
{
#endif

    // Converts TI RTOS clock tick period to .NET ticks (100 nanoseconds)
    inline __attribute__((always_inline)) uint64_t HAL_Time_SysTicksToTime(uint64_t sysTicks)
    {
        return ((int64_t)sysTicks * (int64_t)Clock_tickPeriod) * 10;
    }

#ifdef __cplusplus
}
#endif

#endif // TARGET_HAL_TIME_H
