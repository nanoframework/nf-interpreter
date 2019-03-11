//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_devices_can_native.h"

static const CLR_RT_MethodHandler method_lookup[] =
{
    NULL,
    NULL,
    NULL,
    NULL,
    Library_nf_devices_can_native_nanoFramework_Devices_Can_CanController::WriteMessage___VOID__nanoFrameworkDevicesCanCanMessage,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_nf_devices_can_native_nanoFramework_Devices_Can_CanController::GetMessage___nanoFrameworkDevicesCanCanMessage,
    Library_nf_devices_can_native_nanoFramework_Devices_Can_CanController::DisposeNative___VOID,
    Library_nf_devices_can_native_nanoFramework_Devices_Can_CanController::NativeInit___VOID,
    Library_nf_devices_can_native_nanoFramework_Devices_Can_CanController::NativeUpdateCallbacks___VOID,
    NULL,
    Library_nf_devices_can_native_nanoFramework_Devices_Can_CanController::GetDeviceSelector___STATIC__STRING,
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
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Devices_Can =
{
    "nanoFramework.Devices.Can", 
    0xD2E9416A,
    method_lookup,
    { 1, 0, 3, 0 }
};
