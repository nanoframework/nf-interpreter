//
// Copyright (c) 2017 The nanoFramework project contributors
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
    Library_win_dev_i2c_native_Windows_Devices_I2c_I2cController::NativeInit___VOID,
    NULL,
    Library_win_dev_i2c_native_Windows_Devices_I2c_I2cController::GetDeviceSelector___STATIC__STRING,
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
    NULL,
    Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::NativeInit___VOID,
    Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::DisposeNative___VOID,
    Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::NativeTransmit___WindowsDevicesI2cI2cTransferResult__SZARRAY_U1__SZARRAY_U1,
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
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Devices_I2c =
{
    "Windows.Devices.I2c", 
    0xA44C698B,
    method_lookup,
    { 1, 1, 3, 1 }
};
