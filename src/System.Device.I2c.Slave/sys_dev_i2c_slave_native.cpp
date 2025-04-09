//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_i2c_slave_native.h"

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
    Library_sys_dev_i2c_slave_native_System_Device_I2c_I2cSlaveDevice::NativeInit___VOID,
    Library_sys_dev_i2c_slave_native_System_Device_I2c_I2cSlaveDevice::NativeDispose___VOID,
    Library_sys_dev_i2c_slave_native_System_Device_I2c_I2cSlaveDevice::NativeTransmit___I4__SystemSpanByte__SystemSpanByte__I4,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_I2c_Slave =
{
    "System.Device.I2c.Slave",
    0x4238164B,
    method_lookup,
    { 1, 0, 0, 0 }
};

// clang-format on
