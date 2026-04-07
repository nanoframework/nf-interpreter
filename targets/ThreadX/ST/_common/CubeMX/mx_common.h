//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef MX_COMMON_H_
#define MX_COMMON_H_

#ifdef STM32L4XX
#include <stm32l4xx_hal.h>
#elif defined(STM32F7XX)
#include <stm32f7xx_hal.h>
#else
#error "Missing define with series name"
#endif

#include <nanoHAL_v2.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif // MX_COMMON_H_
