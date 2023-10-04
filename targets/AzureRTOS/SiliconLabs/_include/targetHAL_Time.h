//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_HAL_TIME_H_
#define _TARGET_HAL_TIME_H_

#include <tx_api.h>

#define HAL_Time_CurrentSysTicks tx_time_get

#ifdef __cplusplus
extern "C"
{
#endif

    // Converts CMSIS sysTicks to .NET ticks (100 nanoseconds)
    inline __attribute__((always_inline)) uint64_t HAL_Time_SysTicksToTime(uint64_t sysTicks)
    {
        // need to convert Azure RTOS ticks to 100 nanoseconds
        return sysTicks * TX_TIMER_TICKS_PER_SECOND * 10;
    }

    // because HAL_Time_SysTicksToTime needs to be called from C we need a proxy to allow it to be called from 'C' code
    inline __attribute__((always_inline)) uint64_t HAL_Time_SysTicksToTime_C(uint64_t sysTicks)
    {
        return sysTicks * TX_TIMER_TICKS_PER_SECOND * 10;
    }

#ifdef __cplusplus
}
#endif

#endif //_TARGET_HAL_TIME_H_
