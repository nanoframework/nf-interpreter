//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>

#include <ti/sysbios/knl/Clock.h>

// Converts TI RTOS clock tick period to .NET ticks (100 nanoseconds)
uint64_t HAL_Time_SysTicksToTime(unsigned int sysTicks) 
{
    return ((int64_t)sysTicks * (int64_t)Clock_tickPeriod) * 10;
}

// because HAL_Time_SysTicksToTime needs to be called from C we need a proxy to allow it to be called from 'C' code
extern "C" {
    
    uint64_t HAL_Time_SysTicksToTime_C(unsigned int sysTicks)
    {
        return HAL_Time_SysTicksToTime(sysTicks);
    }
}
