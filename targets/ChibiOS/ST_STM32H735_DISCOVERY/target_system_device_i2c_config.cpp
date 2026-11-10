//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_i2c_native_target.h>

//////////
// I2C4 //
//////////

// pin configuration for I2C4 (STMOD+ connector, shared with on-board touch panel and audio codec)
// port for SCL pin is: GPIOF
// port for SDA pin is: GPIOF
// SCL pin: is GPIOF_14
// SDA pin: is GPIOF_15
// GPIO alternate pin function is 4 (see alternate function mapping table in device datasheet)
I2C_CONFIG_PINS(4, GPIOF, GPIOF, 14, 15, 4)
