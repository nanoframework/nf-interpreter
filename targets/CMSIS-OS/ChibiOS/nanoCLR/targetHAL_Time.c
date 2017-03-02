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

// Converts ticks (CMSIS sysTicks) to microseconds
INT32 HAL_Time_TicksToTimeMicroSec(UINT32 ticks) {
    // T[s] = 1 / f[Hz] 
    return (ticks / (osKernelSysTickFrequency * NANOHAL_TIME_CONVERSION_MICROSECONDS));
}
