//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _ST_HAL_GPIO_HELPERS_H_
#define _ST_HAL_GPIO_HELPERS_H_

#include <stm32l4xx_hal.h>

// taken from stm32l4xx_hal_gpio.c
#define GPIO_MODE             (0x00000003u)
#define ANALOG_MODE           (0x00000008u)
#define EXTI_MODE             (0x10000000u)
#define GPIO_MODE_IT          (0x00010000u)
#define GPIO_MODE_EVT         (0x00020000u)
#define RISING_EDGE           (0x00100000u)
#define FALLING_EDGE          (0x00200000u)
#define GPIO_OUTPUT_TYPE      (0x00000010u)

#ifdef __cplusplus
extern "C" {
#endif

void HAL_GPIO_GetConfig(GPIO_TypeDef *gpiox, GPIO_InitTypeDef *gpioInit);
void HAL_GPIO_SetConfig(GPIO_TypeDef *gpiox, GPIO_InitTypeDef *gpioInit);

#ifdef __cplusplus
}
#endif

#endif  //_ST_HAL_GPIO_HELPERS_H_
