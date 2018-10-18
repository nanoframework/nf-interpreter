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
    static const int FIELD_STATIC__s_instance = 0;
    static const int FIELD_STATIC__s_deviceCollection = 1;


    //--//

};

struct Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice
{
    static const int FIELD___syncLock = 1;
    static const int FIELD___deviceId = 2;
    static const int FIELD___connectionSettings = 3;
    static const int FIELD___disposed = 4;

    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);
    NANOCLR_NATIVE_DECLARE(DisposeNative___VOID);
    NANOCLR_NATIVE_DECLARE(NativeTransmit___WindowsDevicesI2cI2cTransferResult__SZARRAY_U1__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(GetDeviceSelector___STATIC__STRING);

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

#endif  //_WIN_DEV_I2C_NATIVE_H_
