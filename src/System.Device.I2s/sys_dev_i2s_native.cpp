//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_i2s_native.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    Library_sys_dev_i2s_native_System_Device_I2s_I2sDevice::NativeDispose___VOID,
    Library_sys_dev_i2s_native_System_Device_I2s_I2sDevice::NativeInit___VOID,
    Library_sys_dev_i2s_native_System_Device_I2s_I2sDevice::Read___VOID__SystemSpan_1,
    Library_sys_dev_i2s_native_System_Device_I2s_I2sDevice::Write___VOID__SystemReadOnlySpan_1,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_I2s =
{
    "System.Device.I2s",
    0xB51CBE98,
    method_lookup,
    ARRAYSIZE(method_lookup)
};

// clang-format on
