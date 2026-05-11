// Licensed to the .NET Foundation under one or more agreements.
// See LICENSE file in the project root for full license information.

#ifndef TARGET_PAL_TIME_H
#define TARGET_PAL_TIME_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Time_SetCompare is declared in nanoPAL_Time.h (shared PAL header).
// It stores a compare value for the HAL completion timer queue.
// The POSIX implementation is in targetPAL_Time.cpp.

#ifdef __cplusplus
}
#endif

#endif // TARGET_PAL_TIME_H
