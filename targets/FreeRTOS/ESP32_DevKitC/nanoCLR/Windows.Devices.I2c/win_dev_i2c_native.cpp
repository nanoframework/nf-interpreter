//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "win_dev_i2c_native.h"


static const CLR_RT_MethodHandler method_lookup[] =
{
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::NativeInit___VOID,
    Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::DisposeNative___VOID,
    Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::NativeTransmit___I4__SZARRAY_U1__U4__SZARRAY_U1__U4,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::GetDeviceSelector___STATIC__STRING,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Devices_I2c =
{
    "Windows.Devices.I2c", 
    0xC5ABF45E,
    method_lookup
};

