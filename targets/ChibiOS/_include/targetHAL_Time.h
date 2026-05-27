//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGET_HAL_TIME_H
#define TARGET_HAL_TIME_H

#include <cmsis_os.h>

#define HAL_Time_CurrentSysTicks chVTGetTimeStamp

#ifdef __cplusplus
extern "C"
{
#endif

    // Converts CMSIS sysTicks to .NET ticks (100 nanoseconds)
    inline __attribute__((always_inline)) uint64_t HAL_Time_SysTicksToTime(uint64_t sysTicks)
    {
        // convert to microseconds from CMSIS SysTicks
        // this is a rewrite of ChibiOS TIME_I2US(n) macro because it will overflow if doing the math using uint32_t
        // need to convert from microseconds to 100 nanoseconds
        return (((sysTicks * (uint64_t)1000000) + (int64_t)CH_CFG_ST_FREQUENCY - 1) / (int64_t)CH_CFG_ST_FREQUENCY) *
               10;
    }

#ifdef __cplusplus
}
#endif

#endif // TARGET_HAL_TIME_H
