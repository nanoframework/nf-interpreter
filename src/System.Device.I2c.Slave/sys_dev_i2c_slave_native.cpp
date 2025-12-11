//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_i2c_slave_native.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    Library_sys_dev_i2c_slave_native_System_Device_I2c_I2cSlaveDevice::NativeInit___VOID,
    Library_sys_dev_i2c_slave_native_System_Device_I2c_I2cSlaveDevice::NativeDispose___VOID,
    Library_sys_dev_i2c_slave_native_System_Device_I2c_I2cSlaveDevice::NativeTransmit___I4__SystemSpan_1__SystemReadOnlySpan_1__I4,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_I2c_Slave =
{
    "System.Device.I2c.Slave",
    0x984281A1,
    method_lookup,
    { 100, 2, 0, 0 }
};

// clang-format on
