//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _WIN_DEV_I2C_NATIVE_H_
#define _WIN_DEV_I2C_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>

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
    NANOCLR_NATIVE_DECLARE(DisposeNative___VOID);
    NANOCLR_NATIVE_DECLARE(NativeTransmit___WindowsDevicesI2cI2cTransferResult__SZARRAY_U1__SZARRAY_U1);

    //--//

    static void SetConfig(i2c_port_t bus, CLR_RT_HeapBlock* config);
};

struct Library_win_dev_i2c_native_Windows_Devices_I2c_I2cTransferResult
{
    static const int FIELD___bytesTransferred = 1;
    static const int FIELD___status = 2;


    //--//

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Devices_I2c;

#endif  //_WIN_DEV_I2C_NATIVE_H_