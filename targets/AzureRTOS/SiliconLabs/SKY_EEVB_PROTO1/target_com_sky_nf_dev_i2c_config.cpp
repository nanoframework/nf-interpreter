//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <com_sky_nf_dev_i2c_native_target.h>

//////////
// I2C2 //
//////////

// Devices available in GG11 dev kit
// Modules     | Description                              |
// Si7021      | Relative Humidity and Temperature Sensor |
// Si7210      | Hall-Effect Sensor                       |

// pin configuration for I2C2
// port for I2C2_SCL is: GPIOI_5
// port for I2C2_SDA is: GPIOI_4

// GPIO alternate pin function is 7 for both pins (see alternate function mapping table in device datasheet)
I2C_CONFIG_PINS(2, gpioPortI, gpioPortI, 5, 4, 7, 7)
