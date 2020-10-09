//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGETPAL_H_
#define _TARGETPAL_H_

#include <hal.h>


#if defined(STM32L0XX) || defined(STM32F0XX) || defined(STM32F4XX) || defined(STM32F7XX) || defined(STM32H7XX)

// Contains available GPIO ports for the current board
extern stm32_gpio_t* gpioPort[];

//Gets the GPIO according to a pin number
#define GPIO_PORT(pin) (gpioPort[pin/16])

#endif

#endif // _TARGETPAL_H_
