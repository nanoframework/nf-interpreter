//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "win_dev_wifi_native.h"

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
    Library_win_dev_wifi_native_Windows_Devices_WiFi_WiFiAdapter::DisposeNative___VOID,
    Library_win_dev_wifi_native_Windows_Devices_WiFi_WiFiAdapter::NativeInit___VOID,
    Library_win_dev_wifi_native_Windows_Devices_WiFi_WiFiAdapter::NativeNetworkReport___WindowsDevicesWiFiWiFiNetworkReport,
    Library_win_dev_wifi_native_Windows_Devices_WiFi_WiFiAdapter::NativeConnect___WindowsDevicesWiFiWiFiConnectionResult__WindowsDevicesWiFiWiFiAvailableNetwork__WindowsDevicesWiFiWiFiReconnectionKind__STRING,
    Library_win_dev_wifi_native_Windows_Devices_WiFi_WiFiAdapter::NativeDisconnect___VOID,
    Library_win_dev_wifi_native_Windows_Devices_WiFi_WiFiAdapter::NativeScanAsync___VOID,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_win_dev_wifi_native_Windows_Devices_WiFi_WiFiNetworkReport::get_AvailableNetworks___SZARRAY_WindowsDevicesWiFiWiFiAvailableNetwork,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Devices_Wifi =
{
    "Windows.Devices.Wifi", 
    0xF0F3E985,
    method_lookup,
    { 1, 0, 0, 0 }
};
