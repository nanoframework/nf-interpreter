//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _NANOHAL_TIME_H_
#define _NANOHAL_TIME_H_ 1

#include <targetHAL_Time.h>

#define NANOHAL_TIME_CONVERSION_MICRO_TO_SECONDS                1000000
#define NANOHAL_TIME_CONVERSION_MICRO_TO_HUNDREDS_NANOSECONDS   10000

//////////////////////////////////////////
// TODO delete these when working on #130
typedef unsigned long long  UINT64;
typedef unsigned int        UINT32;
//////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

UINT64 HAL_Time_TicksToTimeMilliSec(UINT32 Ticks);

#ifdef __cplusplus
}
#endif

#endif //_NANOHAL_TIME_H_
