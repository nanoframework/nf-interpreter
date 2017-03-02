//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _NANOHAL_TIME_H_
#define _NANOHAL_TIME_H_ 1

#include <targetHAL_Time.h>

#define NANOHAL_TIME_CONVERSION_MICROSECONDS         1000000

//////////////////////////////////////////
// TODO delete these when working on #130
typedef unsigned int        UINT32;
typedef signed   int        INT32;
//////////////////////////////////////////

INT32   HAL_Time_TicksToTimeMicroSec(UINT32 Ticks);

#endif //_NANOHAL_TIME_H_
