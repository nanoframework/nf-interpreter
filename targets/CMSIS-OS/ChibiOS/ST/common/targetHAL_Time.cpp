//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>

// Converts CMSIS sysTicks to .NET ticks (100 nanoseconds)
uint64_t HAL_Time_SysTicksToTime(unsigned int sysTicks) 
{
    // convert to microseconds from CMSIS SysTicks
    // this is a rewrite of ChibiOS TIME_I2US(n) macro because it will overflow if doing the math using uint32_t
    // need to convert from microseconds to 100 nanoseconds
    return (((sysTicks * (uint64_t)1000000) + (int64_t)CH_CFG_ST_FREQUENCY - 1) / (int64_t)CH_CFG_ST_FREQUENCY) * 10;
}

// because HAL_Time_SysTicksToTime needs to be called from C we need a proxy to allow it to be called from 'C' code
extern "C" {
    
    uint64_t HAL_Time_SysTicksToTime_C(unsigned int sysTicks)
    {
        return HAL_Time_SysTicksToTime(sysTicks);
    }
}

// implementation required for STM Cube package
// Provides a tick value in millisecond.
extern "C" {

uint32_t HAL_GetTick(void)
{
  return TIME_I2MS(chVTGetSystemTimeX());
}

}

// implementation required for STM Cube package
// This function provides minimum delay (in milliseconds).
extern "C" {

void HAL_Delay(uint32_t delay)
{
    systime_t start = chVTGetSystemTime();
    systime_t end = start + TIME_MS2I(delay);
    
    do
    {
        // do nothing until the timeout expires
        chThdYield();
    }
    while(chVTIsSystemTimeWithin(start, end));
}

}