//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_i2c_native_target.h>

//////////
// I2C0 //
//////////

// SCL: PA1
// SDA: PA0

// GPIO alternate pin function is 0 for both pins (see alternate function mapping table in device datasheet)
I2C_CONFIG_PINS(0, gpioPortA, gpioPortA, 1, 0, 0, 0)

//////////
// I2C1 //
//////////

// SCL: PB12
// SDA: PB11

// GPIO alternate pin function is 0 for both pins (see alternate function mapping table in device datasheet)
I2C_CONFIG_PINS(1, gpioPortB, gpioPortB, 12, 11, 1, 1)

//////////
// I2C2 //
//////////

// SCL: PC15
// SDA: PC14

// GPIO alternate pin function is 0 for both pins (see alternate function mapping table in device datasheet)
I2C_CONFIG_PINS(2, gpioPortC, gpioPortC, 15, 14, 1, 1)
