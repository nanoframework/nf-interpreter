//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>

// Converts CMSIS sysTicks to .NET ticks (100 nanoseconds)
uint64_t HAL_Time_SysTicksToTime(uint64_t sysTicks) 
{
    // convert to microseconds from ST HAL SysTicks
    // need to convert from microseconds to 100 nanoseconds
    return (((sysTicks * (uint64_t)1000000) + (int64_t)1000 - 1) / (int64_t)1000) * 10;
}

// because HAL_Time_SysTicksToTime needs to be called from C we need a proxy to allow it to be called from 'C' code
extern "C" {
    
    uint64_t HAL_Time_SysTicksToTime_C(uint64_t sysTicks)
    {
        return HAL_Time_SysTicksToTime(sysTicks);
    }
}
