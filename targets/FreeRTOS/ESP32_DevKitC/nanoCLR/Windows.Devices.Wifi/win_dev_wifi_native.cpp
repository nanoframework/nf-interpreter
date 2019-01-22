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
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_win_dev_wifi_native_Windows_Devices_WiFi_WiFiAdapter::DisposeNative___VOID,
    Library_win_dev_wifi_native_Windows_Devices_WiFi_WiFiAdapter::NativeInit___VOID,
    Library_win_dev_wifi_native_Windows_Devices_WiFi_WiFiAdapter::NativeConnect___WindowsDevicesWiFiWiFiConnectionStatus__STRING__STRING__WindowsDevicesWiFiWiFiReconnectionKind,
    Library_win_dev_wifi_native_Windows_Devices_WiFi_WiFiAdapter::NativeDisconnect___VOID,
    Library_win_dev_wifi_native_Windows_Devices_WiFi_WiFiAdapter::NativeScanAsync___VOID,
    Library_win_dev_wifi_native_Windows_Devices_WiFi_WiFiAdapter::GetNativeScanReport___SZARRAY_U1,
    NULL,
    Library_win_dev_wifi_native_Windows_Devices_WiFi_WiFiAdapter::NativeFindWirelessAdapters___STATIC__SZARRAY_U1,
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

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Devices_Wifi =
{
    "Windows.Devices.Wifi", 
    0x1B4DDB17,
    method_lookup,
    { 1, 0, 4, 3 }
};
