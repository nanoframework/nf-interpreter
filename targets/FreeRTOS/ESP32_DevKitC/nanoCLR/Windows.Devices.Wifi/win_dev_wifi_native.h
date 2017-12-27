//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#ifndef _WIN_DEV_WIFI_NATIVE_H_
#define _WIN_DEV_WIFI_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>
#include <Esp32_os.h>


struct Library_win_dev_wifi_native_Windows_Devices_WiFi_WiFiAdapter
{
    static const int FIELD___syncLock = 1;
    static const int FIELD___disposedValue = 2;

    NANOCLR_NATIVE_DECLARE(DisposeNative___VOID);
    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);
    NANOCLR_NATIVE_DECLARE(NativeNetworkReport___WindowsDevicesWiFiWiFiNetworkReport);
    NANOCLR_NATIVE_DECLARE(NativeConnect___WindowsDevicesWiFiWiFiConnectionResult__WindowsDevicesWiFiWiFiAvailableNetwork__WindowsDevicesWiFiWiFiReconnectionKind__STRING);
    NANOCLR_NATIVE_DECLARE(NativeDisconnect___VOID);
    NANOCLR_NATIVE_DECLARE(NativeScanAsync___VOID);

    //--//

};

struct Library_win_dev_wifi_native_Windows_Devices_WiFi_WiFiAvailableNetwork
{
    static const int FIELD___ssid = 1;
    static const int FIELD___networkKind = 2;


    //--//

};

struct Library_win_dev_wifi_native_Windows_Devices_WiFi_WiFiConnectionResult
{
    static const int FIELD___ConnectionStatus = 1;

    //--//

};

struct Library_win_dev_wifi_native_Windows_Devices_WiFi_WiFiNetworkReport
{
    // Something wrong with this field. Possibly its backing field is missing (mandatory for nanoFramework).
    // <AvailableNetworks>k__BackingField
    //static const int FIELD__**THIS_FIELD_IS_NOT_CORRECT_CHECK_MANAGED_CODE** = 1;

    NANOCLR_NATIVE_DECLARE(get_AvailableNetworks___SZARRAY_WindowsDevicesWiFiWiFiAvailableNetwork);

    //--//

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Devices_Wifi;

#endif