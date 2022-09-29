//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoWeak.h>
#include <targetHAL_Time.h>

// HACKHACK
#ifdef PATH_MAX
#undef PATH_MAX
#endif

#include <zephyr.h>
#include <time.h>
//#include <sys/time.h>

//#define CLOCK_TICKS_PER_SEC   CONFIG_SYS_CLOCK_TICKS_PER_SEC
#define CLOCK_TICKS_PER_SEC 1000

uint64_t HAL_Time_CurrentSysTicks()
{
    return (uint32_t)k_uptime_get_32();
}

// Converts Zephyr Tickcount to .NET ticks (100 nanoseconds)
uint64_t HAL_Time_SysTicksToTime(uint64_t sysTicks)
{
    return (((int64_t)sysTicks * (int64_t)1000000 + (int64_t)CLOCK_TICKS_PER_SEC - 1) / (int64_t)CLOCK_TICKS_PER_SEC) *
           10;
}

// because HAL_Time_SysTicksToTime needs to be called from C we need a proxy to allow it to be called from 'C' code
extern "C"
{

    uint64_t HAL_Time_SysTicksToTime_C(uint64_t sysTicks)
    {
        return HAL_Time_SysTicksToTime(sysTicks);
    }
}
