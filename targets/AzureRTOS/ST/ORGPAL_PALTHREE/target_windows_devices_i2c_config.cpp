//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <win_dev_i2c_native_target.h>

//////////
// I2C2 //
//////////

// pin configuration for I2C2
// port for SCL pin is: GPIOH
// port for SDA pin is: GPIOH
// SCL pin: is GPIOH_4
// SDA pin: is GPIOH_5
// GPIO alternate pin function is 4 (see alternate function mapping table in device datasheet)
I2C_CONFIG_PINS(2, GPIOH, GPIOH, 4, 5, 4)

//////////
// I2C3 //
//////////

// pin configuration for I2C3
// port for SCL pin is: GPIOA
// port for SDA pin is: GPIOH
// SCL pin: is GPIOA_8
// SDA pin: is GPIOH_8
// GPIO alternate pin function is 4 (see alternate function mapping table in device datasheet)
I2C_CONFIG_PINS(3, GPIOA, GPIOH, 8, 8, 4)
