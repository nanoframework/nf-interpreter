//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _GPIO_PORT
#define _GPIO_PORT

#include <hal.h>


// Contains available GPIO ports for the current board
extern stm32_gpio_t* gpioPort[];

//Gets the GPIO according to a pin number
#define GPIO_PORT(pin) (gpioPort[pin/16])

#endif
