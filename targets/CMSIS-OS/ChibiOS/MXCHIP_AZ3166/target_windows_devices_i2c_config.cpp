//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "win_dev_i2c_native.h"

//////////
// I2C1 //
//////////

// pin configuration for I2C1
// port for SCL pin is: GPIOA
// port for SDA pin is: GPIOC
// SCL pin: is GPIOB_8
// SDA pin: is GPIOB_9
// GPIO alternate pin function is 4 (see alternate function mapping table in device datasheet)
I2C_CONFIG_PINS(1, GPIOB, GPIOB, 8, 9, 4)
