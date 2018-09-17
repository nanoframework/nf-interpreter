//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGETPAL_H_
#define _TARGETPAL_H_

#include <hal.h>


#if defined(STM32L0xx_MCUCONF) || defined(STM32F0xx_MCUCONF) || defined(STM32F4xx_MCUCONF) || defined(STM32F7xx_MCUCONF) || defined(STM32H7xx_MCUCONF)

// Contains available GPIO ports for the current board
extern stm32_gpio_t* gpioPort[];

//Gets the GPIO according to a pin number
#define GPIO_PORT(pin) (gpioPort[pin/16])

#endif // defined(STM32F0xx) || defined(STM32F4xx) || defined(STM32F7xx) || defined(STM32H7xx)

#endif // _TARGETPAL_H_
