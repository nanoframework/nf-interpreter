//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_Time.h>

//////////////////////////////////////////
// TODO delete these when working on #130
typedef unsigned int        UINT32;
typedef signed   int        INT32;
//////////////////////////////////////////

// Converts CMSIS sysTicks to .NET ticks (100ns)
UINT64 HAL_Time_SysTicksToTime(UINT32 sysTicks) {
    
    // this is a rewrite of ChibiOS ST2US(n) macro because it will overflow if doing the math using uint32_t
    
    // convert to microseconds from CMSIS SysTicks
    uint64_t microsecondsFromSysTicks = (((sysTicks) * 1000000ULL + (uint64_t)CH_CFG_ST_FREQUENCY - 1ULL) / (uint64_t)CH_CFG_ST_FREQUENCY);

    // need to convert from microseconds to 100 nanoseconds
    return  microsecondsFromSysTicks * 10;
}
