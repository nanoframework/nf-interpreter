//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGETPAL_H_
#define _TARGETPAL_H_

#include <stm32l4xx_hal.h>

// Contains available GPIO ports for the current board
extern GPIO_TypeDef* gpioPort[];

//Gets the GPIO according to a pin number
#define GPIO_PORT(pin) (gpioPort[pin/16])

#endif // _TARGETPAL_H_
