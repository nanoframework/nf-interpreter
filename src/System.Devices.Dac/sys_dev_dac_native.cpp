//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#include "sys_dev_dac_native.h"


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
    Library_sys_dev_dac_native_System_Devices_Dac_DacChannel::NativeWriteValue___I4, //TODO: needs changing to bool with double
    Library_sys_dev_dac_native_System_Devices_Dac_DacChannel::NativeDisposeChannel___VOID,
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
    Library_sys_dev_dac_native_System_Devices_Dac_DacController::NativeOpenChannel___VOID__I4,
    Library_sys_dev_dac_native_System_Devices_Dac_DacController::NativeGetChannelCount___I4,
    Library_sys_dev_dac_native_System_Devices_Dac_DacController::NativeGetMaxValue___I4,
    Library_sys_dev_dac_native_System_Devices_Dac_DacController::NativeGetMinValue___I4,
    Library_sys_dev_dac_native_System_Devices_Dac_DacController::NativeIsChannelModeSupported___BOOLEAN__I4,
    Library_sys_dev_dac_native_System_Devices_Dac_DacController::NativeGetResolutionInBits___I4,
    Library_sys_dev_dac_native_System_Devices_Dac_DacController::NativeInit___VOID,
    NULL,
    Library_sys_dev_dac_native_System_Devices_Dac_DacController::GetDeviceSelector___STATIC__STRING,
    NULL,
    NULL,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Devices_Dac =
{
    "System.Devices.Dac", 
    0xA84B23D2,
    method_lookup,
    { 100, 0, 0, 1 }
};
