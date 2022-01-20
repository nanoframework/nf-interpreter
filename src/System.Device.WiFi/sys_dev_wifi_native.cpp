//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_wifi_native.h"

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
    NULL,
    NULL,
    NULL,
    Library_sys_dev_wifi_native_System_Device_WiFi_WiFiAdapter::DisposeNative___VOID,
    Library_sys_dev_wifi_native_System_Device_WiFi_WiFiAdapter::NativeInit___VOID,
    Library_sys_dev_wifi_native_System_Device_WiFi_WiFiAdapter::NativeConnect___SystemDeviceWiFiWiFiConnectionStatus__STRING__STRING__SystemDeviceWiFiWiFiReconnectionKind,
    Library_sys_dev_wifi_native_System_Device_WiFi_WiFiAdapter::NativeDisconnect___VOID,
    Library_sys_dev_wifi_native_System_Device_WiFi_WiFiAdapter::NativeScanAsync___VOID,
    Library_sys_dev_wifi_native_System_Device_WiFi_WiFiAdapter::GetNativeScanReport___SZARRAY_U1,
    NULL,
    Library_sys_dev_wifi_native_System_Device_WiFi_WiFiAdapter::NativeFindWirelessAdapters___STATIC__SZARRAY_U1,
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

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_WiFi =
{
    "System.Device.WiFi",
    0x5C9E06C4,
    method_lookup,
    { 100, 0, 6, 3 }
};

// clang-format on
