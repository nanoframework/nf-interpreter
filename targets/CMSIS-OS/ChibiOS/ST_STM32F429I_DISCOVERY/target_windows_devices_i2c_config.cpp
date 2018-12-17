//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "win_dev_i2c_native.h"

//////////
// I2C3 //
//////////

// pin configuration for I2C3
// port for SCL pin is: GPIOA
// port for SDA pin is: GPIOC
// SCL pin: is GPIOA_8
// SDA pin: is GPIOC_9
// GPIO alternate pin function is 4 (see alternate function mapping table in device datasheet)
I2C_CONFIG_PINS(3, GPIOA, GPIOC, 8, 9, 4)
