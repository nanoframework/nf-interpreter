//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_i2c_native_target.h>

//////////
// I2C0 //
//////////

// Default Pico pin configuration for I2C0:
// SCL = GP5
// SDA = GP4
I2C_CONFIG_PINS(0, 5, 4)

//////////
// I2C1 //
//////////

// Default Pico pin configuration for I2C1:
// SCL = GP7
// SDA = GP6
I2C_CONFIG_PINS(1, 7, 6)
