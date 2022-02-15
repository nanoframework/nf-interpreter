//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef SYS_DEV_WIFI_NATIVE_H
#define SYS_DEV_WIFI_NATIVE_H

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

typedef enum __nfpack WiFiConnectionStatus
{
    WiFiConnectionStatus_AccessRevoked = 0,
    WiFiConnectionStatus_InvalidCredential = 1,
    WiFiConnectionStatus_NetworkNotAvailable = 2,
    WiFiConnectionStatus_Success = 3,
    WiFiConnectionStatus_Timeout = 4,
    WiFiConnectionStatus_UnspecifiedFailure = 5,
    WiFiConnectionStatus_UnsupportedAuthenticationProtocol = 6,
} WiFiConnectionStatus;

////////////////////////////////////////////////
// moved to nanoPAL_Sockets.h for convenience //
////////////////////////////////////////////////
// typedef enum __nfpack WiFiEventType
// {
//     WiFiEventType_ScanComplete = 1,
// } WiFiEventType;

typedef enum __nfpack WiFiNetworkKind
{
    WiFiNetworkKind_Adhoc = 0,
    WiFiNetworkKind_Any = 1,
    WiFiNetworkKind_Infrastructure = 2,
} WiFiNetworkKind;

typedef enum __nfpack WiFiReconnectionKind
{
    WiFiReconnectionKind_Automatic = 0,
    WiFiReconnectionKind_Manual = 1,
} WiFiReconnectionKind;

struct Library_sys_dev_wifi_native_System_Device_WiFi_WiFiAvailableNetwork
{
    static const int FIELD___bsid = 1;
    static const int FIELD___ssid = 2;
    static const int FIELD___rssi = 3;
    static const int FIELD___networkKind = 4;

    //--//
};

struct Library_sys_dev_wifi_native_System_Device_WiFi_WiFiConnectionResult
{
    static const int FIELD___ConnectionStatus = 1;

    //--//
};

struct Library_sys_dev_wifi_native_System_Device_WiFi_WiFiAdapter
{
    static const int FIELD_STATIC__s_eventListener = 0;

    static const int FIELD___networkInterface = 1;
    static const int FIELD___syncLock = 2;
    static const int FIELD__AvailableNetworksChanged = 3;
    static const int FIELD___disposedValue = 4;

    NANOCLR_NATIVE_DECLARE(DisposeNative___VOID);
    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);
    NANOCLR_NATIVE_DECLARE(
        NativeConnect___SystemDeviceWiFiWiFiConnectionStatus__STRING__STRING__SystemDeviceWiFiWiFiReconnectionKind);
    NANOCLR_NATIVE_DECLARE(NativeDisconnect___VOID);
    NANOCLR_NATIVE_DECLARE(NativeScanAsync___VOID);
    NANOCLR_NATIVE_DECLARE(GetNativeScanReport___SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(NativeFindWirelessAdapters___STATIC__SZARRAY_U1);

    //--//
    static HRESULT GetNetInterfaceIndex(CLR_RT_StackFrame &stack, int *pNetIndex);
    static int StoreApRecordsToString(uint8_t *pTarget, wifi_ap_record_t *apRecords, uint16_t recordCount);
};

struct Library_sys_dev_wifi_native_System_Device_WiFi_WiFiNetworkReport
{
    static const int FIELD___wifiNetworks = 1;

    //--//
};

struct Library_sys_dev_wifi_native_nanoFramework_Networking_WiFiNetworkHelper
{
    static const int FIELD_STATIC___ipAddressAvailable = 1;
    static const int FIELD_STATIC___networkReady = 2;
    static const int FIELD_STATIC___requiresDateTime = 3;
    static const int FIELD_STATIC___ssid = 4;
    static const int FIELD_STATIC___password = 5;
    static const int FIELD_STATIC___wifi = 6;
    static const int FIELD_STATIC___reconnectionKind = 7;
    static const int FIELD_STATIC___ipConfiguration = 8;
    static const int FIELD_STATIC___networkHelperStatus = 9;
    static const int FIELD_STATIC___helperException = 10;
    static const int FIELD_STATIC___workingNetworkInterface = 11;
    static const int FIELD_STATIC___helperInstanciated = 12;

    //--//
};

struct Library_sys_dev_wifi_native_System_Device_WiFi_WiFiEvent
{
    static const int FIELD__EventType = 3;
    static const int FIELD__Time = 4;

    //--//
};

struct Library_sys_dev_wifi_native_System_Device_WiFi_WiFiEventListener
{
    static const int FIELD__wifiAdapters = 1;

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_WiFi;

#endif // SYS_DEV_WIFI_NATIVE_H
