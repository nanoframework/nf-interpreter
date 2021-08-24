//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef PERFORMANCE_COUNTERS_H
#define PERFORMANCE_COUNTERS_H

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

#endif //PERFORMANCE_COUNTERS_H
