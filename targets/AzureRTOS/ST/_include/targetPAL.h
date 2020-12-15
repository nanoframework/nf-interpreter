//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGETPAL_H_
#define _TARGETPAL_H_

#include <stm32l4xx_hal.h>

// Contains available GPIO ports for the current board
extern GPIO_TypeDef* GpioPorts[];

//Gets the GPIO port according to a pin number
#define GPIO_PORT(pin) (GpioPorts[pin/16])

//Gets the GPIO port according to a pin number
#define GPIO_PIN(pin) (pin - (pin%16)*16)

#endif // _TARGETPAL_H_
