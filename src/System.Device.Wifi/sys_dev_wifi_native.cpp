//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_wifi_native.h"

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
    nullptr,
    nullptr,
    Library_sys_dev_wifi_native_System_Device_Wifi_WifiAdapter::DisposeNative___VOID,
    Library_sys_dev_wifi_native_System_Device_Wifi_WifiAdapter::NativeInit___VOID,
    Library_sys_dev_wifi_native_System_Device_Wifi_WifiAdapter::NativeConnect___SystemDeviceWifiWifiConnectionStatus__STRING__STRING__SystemDeviceWifiWifiReconnectionKind,
    Library_sys_dev_wifi_native_System_Device_Wifi_WifiAdapter::NativeDisconnect___VOID,
    Library_sys_dev_wifi_native_System_Device_Wifi_WifiAdapter::NativeScanAsync___VOID,
    Library_sys_dev_wifi_native_System_Device_Wifi_WifiAdapter::GetNativeScanReport___SZARRAY_U1,
    Library_sys_dev_wifi_native_System_Device_Wifi_WifiAdapter::NativeSetDeviceName___VOID__STRING,
    nullptr,
    Library_sys_dev_wifi_native_System_Device_Wifi_WifiAdapter::NativeFindWirelessAdapters___STATIC__SZARRAY_U1,
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
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_Wifi =
{
    "System.Device.Wifi",
    0x3D0D06BD,
    method_lookup,
    { 100, 2, 0, 0 }
};

// clang-format on
