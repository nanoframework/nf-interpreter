//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>

#include <ti/sysbios/knl/Clock.h>

// Converts TI RTOS clock tick period to .NET ticks (100 nanoseconds)
uint64_t HAL_Time_SysTicksToTime(uint64_t sysTicks)
{
    return ((int64_t)sysTicks * (int64_t)Clock_tickPeriod) * 10;
}
