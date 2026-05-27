//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_dac_native.h"

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
    nullptr,
    nullptr,
    Library_sys_dev_dac_native_System_Device_Dac_DacController::NativeOpenChannel___VOID__I4,
    Library_sys_dev_dac_native_System_Device_Dac_DacController::NativeGetChannelCount___I4,
    Library_sys_dev_dac_native_System_Device_Dac_DacController::NativeGetResolutionInBits___I4,
    Library_sys_dev_dac_native_System_Device_Dac_DacController::NativeInit___VOID,
    nullptr,
    nullptr,
    Library_sys_dev_dac_native_System_Device_Dac_DacController::GetDeviceSelector___STATIC__STRING,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    Library_sys_dev_dac_native_System_Device_Dac_DacChannel::NativeWriteValue___VOID__U2,
    Library_sys_dev_dac_native_System_Device_Dac_DacChannel::NativeDispose___VOID__BOOLEAN,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_Dac =
{
    "System.Device.Dac",
    0x435307F5,
    method_lookup,
    { 100, 2, 0, 0 }
};

// clang-format on
