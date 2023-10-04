//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_i2c_native_target.h>

//////////
// I2C0 //
//////////

// pin configuration for I2C0
// port for I2C0_SCL is: PC1
// port for I2C0_SDA is: PC0

// GPIO alternate pin function is 4 for both pins (see Alternate Functionality mapping table in device datasheet)
I2C_CONFIG_PINS(0, gpioPortC, gpioPortC, 1, 0, 4, 4)

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

// GPIO alternate pin function is 7 for both pins (see Alternate Functionality mapping table in device datasheet)
I2C_CONFIG_PINS(2, gpioPortI, gpioPortI, 5, 4, 7, 7)
