//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_i2c_native.h"

// clang-format off

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
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::NativeInit___VOID,
    Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::NativeDispose___VOID,
    Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::NativeTransmit___SystemDeviceI2cI2cTransferResult__SystemSpanByte__SystemSpanByte,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_I2c =
{
    "System.Device.I2c",
    0xFA806D33,
    method_lookup,
    { 100, 0, 0, 1 }
};

// clang-format on
