//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef SYS_DEV_I2C_NATIVE_TARGET_H
#define SYS_DEV_I2C_NATIVE_TARGET_H

#include <em_device.h>
#include <sl_i2cspm.h>

#include <targetPAL.h>
#include <nanoHAL.h>
#include <sys_dev_i2c_native.h>
#include <target_system_device_i2c_config.h>

// receiver thread
#define I2C_THREAD_STACK_SIZE 256
#define I2C_THREAD_PRIORITY   5

// set missing defines
#ifndef GECKO_USE_I2C0
#define GECKO_USE_I2C0 FALSE
#endif
#ifndef GECKO_USE_I2C1
#define GECKO_USE_I2C1 FALSE
#endif
#ifndef GECKO_USE_I2C2
#define GECKO_USE_I2C2 FALSE
#endif

// struct representing the I2C
typedef struct NF_PAL_I2C
{
    I2CSPM_Init_TypeDef *Configuration;
    TX_THREAD *WorkingThread;
    uint32_t *WorkingThreadStack;
    I2C_TransferReturn_TypeDef TransactionResult;
    uint16_t Address;
    float ByteTime;

    uint8_t *WriteBuffer;
    uint8_t WriteSize;

    uint8_t *ReadBuffer;
    uint8_t ReadSize;
} NF_PAL_I2C;

////////////////////////////////////////////
// declaration of the the I2C PAL structs //
////////////////////////////////////////////
#if defined(I2C0) && (GECKO_USE_I2C0 == TRUE)
extern NF_PAL_I2C I2C0_PAL;
#endif
#if defined(I2C1) && (GECKO_USE_I2C1 == TRUE)
extern NF_PAL_I2C I2C1_PAL;
#endif
#if defined(I2C2) && (GECKO_USE_I2C2 == TRUE)
extern NF_PAL_I2C I2C2_PAL;
#endif

#if defined(_SILICON_LABS_32B_SERIES_1)

// the following macro defines a function that configures the GPIO pins for a Gecko I2C peripheral
// it gets called in the System_Device_I2c_I2cDevice::NativeInit function
// this is required because the I2C peripherals can use multiple GPIO configuration combinations
#define I2C_CONFIG_PINS(num, gpio_port_scl, gpio_port_sda, scl_pin, sda_pin, scl_port_location, sda_port_location)     \
    void ConfigPins_I2C##num()                                                                                         \
    {                                                                                                                  \
        I2C##num##_PAL.Configuration->sclPort = gpio_port_scl;                                                         \
        I2C##num##_PAL.Configuration->sdaPort = gpio_port_sda;                                                         \
        I2C##num##_PAL.Configuration->sclPin = scl_pin;                                                                \
        I2C##num##_PAL.Configuration->sdaPin = sda_pin;                                                                \
        I2C##num##_PAL.Configuration->portLocationScl = scl_port_location;                                             \
        I2C##num##_PAL.Configuration->portLocationSda = sda_port_location;                                             \
    }

#else
#error                                                                                                                 \
    "Only _SILICON_LABS_32B_SERIES_1 is supported at this time. To add support for other series declaration above has to be updated"
#endif

//////////////////////////////////////////////////////////////////////////////////////////////
// when an I2C is defined the declarations below will have the real function/configuration //
// in the target folder @ target_windows_devices_i2c_config.cpp                             //
//////////////////////////////////////////////////////////////////////////////////////////////
void ConfigPins_I2C0();
void ConfigPins_I2C1();
void ConfigPins_I2C2();

#endif // SYS_DEV_I2C_NATIVE_TARGET_H
