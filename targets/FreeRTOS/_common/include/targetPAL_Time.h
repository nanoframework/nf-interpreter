//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGET_PAL_TIME_H
#define TARGET_PAL_TIME_H

// Converts FreeRTOS Tickcount to .NET ticks (100 nanoseconds)
inline __attribute__((always_inline)) uint64_t HAL_Time_SysTicksToTime(uint64_t sysTicks)
{
    return sysTicks * configTICK_RATE_HZ * 10;
}

// because HAL_Time_SysTicksToTime needs to be called from C we need a proxy to allow it to be called from 'C' code
#ifdef __cplusplus
extern "C"
{
#endif

    inline __attribute__((always_inline)) uint64_t HAL_Time_SysTicksToTime_C(uint64_t sysTicks)
    {
        return sysTicks * configTICK_RATE_HZ * 10;
    }

#ifdef __cplusplus
}
#endif

#endif // TARGET_PAL_TIME_H
