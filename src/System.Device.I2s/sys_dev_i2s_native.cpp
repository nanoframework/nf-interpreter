//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_i2s_native.h"

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
    NULL,
    NULL,
    Library_sys_dev_i2s_native_System_Device_I2s_I2sDevice::Read___VOID__SystemSpanByte,
    Library_sys_dev_i2s_native_System_Device_I2s_I2sDevice::Write___VOID__SystemSpanByte,
    NULL,
    Library_sys_dev_i2s_native_System_Device_I2s_I2sDevice::NativeInit___VOID,
    Library_sys_dev_i2s_native_System_Device_I2s_I2sDevice::NativeDispose___VOID,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_I2s =
{
    "System.Device.I2s",
    0x478490FE,
    method_lookup,
    { 100, 0, 0, 1 }
};

// clang-format on
