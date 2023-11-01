//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <com_sky_nf_dev_i2c_native_target.h>

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

// SCL: PC5
// SDA: PC4

// GPIO alternate pin function is 0 for both pins (see alternate function mapping table in device datasheet)
I2C_CONFIG_PINS(1, gpioPortC, gpioPortC, 5, 4, 0, 0)
