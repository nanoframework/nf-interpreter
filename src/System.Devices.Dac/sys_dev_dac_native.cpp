//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_dac_native.h"

static const CLR_RT_MethodHandler method_lookup[] = {
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
    Library_sys_dev_dac_native_System_Device_Dac_DacController::NativeOpenChannel___VOID__I4,
    Library_sys_dev_dac_native_System_Device_Dac_DacController::NativeGetChannelCount___I4,
    Library_sys_dev_dac_native_System_Device_Dac_DacController::NativeGetResolutionInBits___I4,
    Library_sys_dev_dac_native_System_Device_Dac_DacController::NativeInit___VOID,
    NULL,
    NULL,
    Library_sys_dev_dac_native_System_Device_Dac_DacController::GetDeviceSelector___STATIC__STRING,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_sys_dev_dac_native_System_Device_Dac_DacChannel::NativeWriteValue___VOID__U2,
    Library_sys_dev_dac_native_System_Device_Dac_DacChannel::NativeDispose___VOID__BOOLEAN,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_Dac =
    {"System.Device.Dac", 0x5ED97E79, method_lookup, {100, 0, 0, 6}};
