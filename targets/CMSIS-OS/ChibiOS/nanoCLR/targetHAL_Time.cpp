//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_Types.h>
#include <nanoHAL_Time.h>

// Converts CMSIS sysTicks to .NET ticks (100 nanoseconds)
signed __int64 HAL_Time_SysTicksToTime(unsigned int sysTicks) {
    
    // this is a rewrite of ChibiOS ST2US(n) macro because it will overflow if doing the math using uint32_t
    
    // convert to microseconds from CMSIS SysTicks
    int64_t microsecondsFromSysTicks = (((sysTicks) * 1000000ULL + (int64_t)CH_CFG_ST_FREQUENCY - 1ULL) / (int64_t)CH_CFG_ST_FREQUENCY);

    // need to convert from microseconds to 100 nanoseconds
    return  microsecondsFromSysTicks * 10;
}
