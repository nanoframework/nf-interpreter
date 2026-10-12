//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_i2c_native.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::NativeDispose___VOID,
    Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::NativeInit___VOID,
    Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::NativeTransmit___SystemDeviceI2cI2cTransferResult__SystemReadOnlySpan_1__SystemSpan_1,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_I2c =
{
    "System.Device.I2c",
    0x3C0CD501,
    method_lookup,
    ARRAYSIZE(method_lookup)
};

// clang-format on
