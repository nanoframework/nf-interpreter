//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef SYS_DEV_I2C_NATIVE_TARGET_H
#define SYS_DEV_I2C_NATIVE_TARGET_H

#include <ch.h>
#include <hal.h>
#include <nanoHAL.h>
#include <sys_dev_i2c_native.h>

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

////////////////////////////////////////////
// declaration of the the I2C PAL structs //
////////////////////////////////////////////
#if RP_I2C_USE_I2C0
extern NF_PAL_I2C I2C0_PAL;
#endif
#if RP_I2C_USE_I2C1
extern NF_PAL_I2C I2C1_PAL;
#endif

// RP2040 I2C pin configuration macro
// I2C function select = 3 on RP2040
#define I2C_CONFIG_PINS(num, scl_pin, sda_pin)                                                                         \
    void ConfigPins_I2C##num()                                                                                         \
    {                                                                                                                  \
        palSetPadMode(IOPORT1, scl_pin, PAL_MODE_ALTERNATE_I2C | PAL_RP_PAD_PUE);                                     \
        palSetPadMode(IOPORT1, sda_pin, PAL_MODE_ALTERNATE_I2C | PAL_RP_PAD_PUE);                                     \
    }

// RP2040 has 2 I2C buses (I2C0, I2C1)
void ConfigPins_I2C0();
void ConfigPins_I2C1();

#endif // SYS_DEV_I2C_NATIVE_TARGET_H
