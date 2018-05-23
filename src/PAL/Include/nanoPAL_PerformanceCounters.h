//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _PERFORMANCE_COUNTERS_H_
#define _PERFORMANCE_COUNTERS_H_ 1

#include <nanoWeak.h>

#ifdef __cplusplus
extern "C" {
#endif

void        PAL_PerformanceCounter_Initialize();
void        PAL_PerformanceCounter_Uninitialize();
uint32_t    PAL_PerformanceCounter();

#ifdef __cplusplus
}
#endif

#endif //_PERFORMANCE_COUNTERS_H_
