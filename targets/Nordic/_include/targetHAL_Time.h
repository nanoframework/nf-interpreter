//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef TARGET_HAL_TIME_H
#define TARGET_HAL_TIME_H

// This is needed to provide types
#include <zephyr/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

    uint64_t HAL_Time_CurrentSysTicks();

#ifdef __cplusplus
}
#endif

#endif // TARGET_HAL_TIME_H
