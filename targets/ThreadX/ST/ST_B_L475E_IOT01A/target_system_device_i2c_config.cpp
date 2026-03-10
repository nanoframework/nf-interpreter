//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_i2c_native_target.h>

//////////
// I2C1 //
//////////

// Note on ST_B_L457EIOT01A I2C1 connects to CN1 ARDUINO connector pins 10 and 9

// pin configuration for I2C1 on GPIOB AF4
// port for I2C1_SCL is: GPIOB_8 : I2C1_SCL pin CN1_10
// port for I2C1_SDA is: GPIOB_9 : I2C1_SDA pin CN1_9

// GPIO alternate pin function is 4 (see alternate function mapping table in device datasheet)
I2C_CONFIG_PINS(1, GPIOB, GPIOB, 8, 9, 4)

//////////
// I2C2 //
//////////

// Note on ST_B_L457EIOT01A I2C2 connects to the following devices
// Modules     | Description                             | SAD[6:0] + R/W | I2C write address | I2C read address
// HTS221      | Capacitive digital sensor for           |
//             | relative humidity and temperature       | 1011111x       | 0xBE              | 0xBF
// LIS3MDL     | 3-axis magnetometer                     | 0011110x       | 0x3C              | 0x3D
// LPS22HB     | MEMS nano pressure sensor               | 1011101x       | 0xBA              | 0xBB
// LSM6DSL     | 3D accelerometer and 3D gyroscope       | 1101010x       | 0xD4              | 0xD5
// VL53L0X     | Time-of-Flight ranging and gesture      |
//             | detection sensor                        | 0101001x       | 0x52              | 0x53
// M24SR64-Y   | Dynamic NFC/RFID tag IC                 | 1010110x       | 0xAC              | 0xAD
// STSAFE-A100 | -                                       | 0100000x       | 0x40              | 0x41

// pin configuration for I2C2 on GPIOB AF4
// port for I2C2_SCL is: GPIOB_10 : I2C2_SCL INTERNAL-I2C2_SCL
// port for I2C2_SDA is: GPIOB_11 : I2C2_SDA INTERNAL-I2C2_SDA

// GPIO alternate pin function is 4 (see alternate function mapping table in device datasheet)
I2C_CONFIG_PINS(2, GPIOB, GPIOB, 10, 11, 4)

//////////
// I2C3 //
//////////

// Note on ST_B_L457EIOT01A I2C3 connects to CN4 ARDUINO connector pins 5 and 6

// pin configuration for I2C3 on GPIOC AF4
// port for I2C1_SCL is: GPIOC_0 : ADC / I2C3_SCL pin CN4_5
// port for I2C1_SDA is: GPIOC_1 : ADC / I2C3_SDA pin CN4_6

// GPIO alternate pin function is 4 (see alternate function mapping table in device datasheet)
I2C_CONFIG_PINS(3, GPIOC, GPIOC, 0, 1, 4)