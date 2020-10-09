//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _WIN_DEV_I2C_NATIVE_TARGET_H_
#define _WIN_DEV_I2C_NATIVE_TARGET_H_

#include <win_dev_i2c_native.h>
#include <hal.h>

// struct representing the I2C
typedef struct NF_PAL_I2C_
{
    I2CDriver *Driver;
    I2CConfig Configuration;
    thread_t *WorkingThread;
    i2caddr_t Address;
    float ByteTime;

    uint8_t *WriteBuffer;
    uint8_t WriteSize;

    uint8_t *ReadBuffer;
    uint8_t ReadSize;
} NF_PAL_I2C;

///////////////////////////////////////////
// declaration of the the I2C PAL strucs //
///////////////////////////////////////////
#if STM32_I2C_USE_I2C1
extern NF_PAL_I2C I2C1_PAL;
#endif
#if STM32_I2C_USE_I2C2
extern NF_PAL_I2C I2C2_PAL;
#endif
#if STM32_I2C_USE_I2C3
extern NF_PAL_I2C I2C3_PAL;
#endif
#if STM32_I2C_USE_I2C4
extern NF_PAL_I2C I2C4_PAL;
#endif

// the following macro defines a function that configures the GPIO pins for a STM32 I2C peripheral
// it gets called in the Windows_Devices_I2c_I2cDevice::NativeInit function
// this is required because the I2C peripherals can use multiple GPIO configuration combinations
#define I2C_CONFIG_PINS(num, gpio_port_scl, gpio_port_sda, scl_pin, sda_pin, alternate_function)                       \
    void ConfigPins_I2C##num()                                                                                         \
    {                                                                                                                  \
        palSetPadMode(                                                                                                 \
            gpio_port_scl,                                                                                             \
            scl_pin,                                                                                                   \
            (PAL_MODE_ALTERNATE(alternate_function) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_OTYPE_OPENDRAIN));          \
        palSetPadMode(                                                                                                 \
            gpio_port_sda,                                                                                             \
            sda_pin,                                                                                                   \
            (PAL_MODE_ALTERNATE(alternate_function) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_OTYPE_OPENDRAIN));          \
    }

//////////////////////////////////////////////////////////////////////////////////////////////
// when an I2C is defined the declarations below will have the real function/configuration //
// in the target folder @ target_windows_devices_i2c_config.cpp                             //
//////////////////////////////////////////////////////////////////////////////////////////////
void ConfigPins_I2C1();
void ConfigPins_I2C2();
void ConfigPins_I2C3();
void ConfigPins_I2C4();

#endif //_WIN_DEV_I2C_NATIVE_TARGET_H_
