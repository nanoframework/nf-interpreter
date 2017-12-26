//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
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
    static const int FIELD_STATIC__DeviceCollection = 0;


    //--//

};

struct Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice
{
    static const int FIELD___syncLock = 1;
    static const int FIELD___deviceId = 2;
    static const int FIELD___connectionSettings = 3;
    static const int FIELD___i2cBus = 4;
    static const int FIELD___disposedValue = 5;

    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);
    NANOCLR_NATIVE_DECLARE(DisposeNative___VOID);
    NANOCLR_NATIVE_DECLARE(NativeTransmit___I4__SZARRAY_U1__U4__SZARRAY_U1__U4);
    NANOCLR_NATIVE_DECLARE(GetDeviceSelector___STATIC__STRING);

    //--//
    static void SetConfig(i2c_port_t bus, CLR_RT_HeapBlock* config);
};

struct Library_win_dev_i2c_native_Windows_Devices_I2c_I2cTransferResult
{
    static const int FIELD__BytesTransferred = 1;
    static const int FIELD__Status = 2;


    //--//

};

    extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Devices_I2c;

#endif  //_WIN_DEV_I2C_NATIVE_H_
