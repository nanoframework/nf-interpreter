//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_HAL_TIME_H_
#define _TARGET_HAL_TIME_H_

#include <nanoCLR_Headers.h>

#ifdef __cplusplus
extern "C"
{
#endif

uint32_t HAL_GetTick(void);

#ifdef __cplusplus
}
#endif

#define HAL_Time_CurrentSysTicks    HAL_GetTick           

#endif //_TARGET_HAL_TIME_H_
