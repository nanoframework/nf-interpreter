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

// Converts ticks (CMSIS sysTicks) to milliseconds
UINT64 HAL_Time_TicksToTimeMilliSec(UINT32 ticks) {
    
    // this is a rewrite of ChibiOS ST2MS(n) macro because it will overflow if doing the math using uint32_t
    return  (((ticks) * 1000ULL + (uint64_t)CH_CFG_ST_FREQUENCY - 1ULL) / (uint64_t)CH_CFG_ST_FREQUENCY);
}
