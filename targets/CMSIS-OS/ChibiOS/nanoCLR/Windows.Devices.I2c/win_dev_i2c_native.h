//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _WIN_DEV_I2C_NATIVE_H_
#define _WIN_DEV_I2C_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>
#include <hal.h>

struct Library_win_dev_i2c_native_Windows_Devices_I2c_I2cConnectionSettings
{
    static const int FIELD___slaveAddress = 1;
    static const int FIELD___busSpeed = 2;
    static const int FIELD___sharingMode = 3;


    //--//

};

struct Library_win_dev_i2c_native_Windows_Devices_I2c_I2cController
{
    static const int FIELD___syncLock = 1;
    static const int FIELD___controllerId = 2;
    static const int FIELD__s_deviceCollection = 3;

    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);
    NANOCLR_NATIVE_DECLARE(GetDeviceSelector___STATIC__STRING);

    //--//

};

struct Library_win_dev_i2c_native_Windows_Devices_I2c_I2cControllerManager
{
    static const int FIELD_STATIC___syncLock = 0;
    static const int FIELD_STATIC__s_controllersCollection = 1;


    //--//

};

struct Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice
{
    static const int FIELD___syncLock = 1;
    static const int FIELD___deviceId = 2;
    static const int FIELD___connectionSettings = 3;
    static const int FIELD___disposed = 4;

    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);
    NANOCLR_NATIVE_DECLARE(NativeDispose___VOID__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeTransmit___WindowsDevicesI2cI2cTransferResult__SZARRAY_U1__SZARRAY_U1);

    //--//

    static void GetI2cConfig(CLR_RT_HeapBlock* managedConfig, I2CConfig* llConfig);
    static bool IsLongRunningOperation(uint16_t writeSize, uint16_t readSize, float byteTime, uint32_t& estimatedDurationMiliseconds);
};

struct Library_win_dev_i2c_native_Windows_Devices_I2c_I2cTransferResult
{
    static const int FIELD___bytesTransferred = 1;
    static const int FIELD___status = 2;


    //--//

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Devices_I2c;

// struct representing the I2C 
struct NF_PAL_I2C
{
    I2CDriver*  Driver;
    I2CConfig   Configuration;
    thread_t*   WorkingThread;
    i2caddr_t   Address;
    float ByteTime;

    uint8_t* WriteBuffer;
    uint8_t  WriteSize;

    uint8_t* ReadBuffer;
    uint8_t  ReadSize;
};

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
#define I2C_CONFIG_PINS(num, gpio_port_scl, gpio_port_sda, scl_pin, sda_pin, alternate_function) void ConfigPins_I2C##num() \
{ \
    palSetPadMode(gpio_port_scl, scl_pin, (PAL_MODE_ALTERNATE(alternate_function) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_OTYPE_OPENDRAIN) ); \
    palSetPadMode(gpio_port_sda, sda_pin, (PAL_MODE_ALTERNATE(alternate_function) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_OTYPE_OPENDRAIN)); \
}

//////////////////////////////////////////////////////////////////////////////////////////////
// when an I2C is defined the declarations below will have the real function/configuration //
// in the target folder @ target_windows_devices_i2c_config.cpp                             //
//////////////////////////////////////////////////////////////////////////////////////////////
void ConfigPins_I2C1();
void ConfigPins_I2C2();
void ConfigPins_I2C3();
void ConfigPins_I2C4();

#endif  //_WIN_DEV_I2C_NATIVE_H_
