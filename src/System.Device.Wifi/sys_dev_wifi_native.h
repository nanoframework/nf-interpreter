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

typedef enum __nfpack WifiConnectionStatus
{
    WifiConnectionStatus_AccessRevoked = 0,
    WifiConnectionStatus_InvalidCredential = 1,
    WifiConnectionStatus_NetworkNotAvailable = 2,
    WifiConnectionStatus_Success = 3,
    WifiConnectionStatus_Timeout = 4,
    WifiConnectionStatus_UnspecifiedFailure = 5,
    WifiConnectionStatus_UnsupportedAuthenticationProtocol = 6,
} WifiConnectionStatus;

////////////////////////////////////////////////
// moved to nanoPAL_Sockets.h for convenience //
////////////////////////////////////////////////
// typedef enum __nfpack WifiEventType
// {
//     WifiEventType_ScanComplete = 1,
// } WifiEventType;

typedef enum __nfpack WifiNetworkKind
{
    WifiNetworkKind_Adhoc = 0,
    WifiNetworkKind_Any = 1,
    WifiNetworkKind_Infrastructure = 2,
} WifiNetworkKind;

typedef enum __nfpack WifiReconnectionKind
{
    WifiReconnectionKind_Automatic = 0,
    WifiReconnectionKind_Manual = 1,
} WifiReconnectionKind;

struct Library_sys_dev_wifi_native_System_Device_Wifi_WifiConnectionResult
{
    static const int FIELD___ConnectionStatus = 1;

    //--//
};

struct Library_sys_dev_wifi_native_System_Device_Wifi_WifiAdapter
{
    static const int FIELD_STATIC__s_eventListener = 0;

    static const int FIELD___networkInterface = 1;
    static const int FIELD___syncLock = 2;
    static const int FIELD__AvailableNetworksChanged = 3;
    static const int FIELD___disposedValue = 4;

    NANOCLR_NATIVE_DECLARE(DisposeNative___VOID);
    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);
    NANOCLR_NATIVE_DECLARE(
        NativeConnect___SystemDeviceWifiWifiConnectionStatus__STRING__STRING__SystemDeviceWifiWifiReconnectionKind);
    NANOCLR_NATIVE_DECLARE(NativeDisconnect___VOID);
    NANOCLR_NATIVE_DECLARE(NativeScanAsync___VOID);
    NANOCLR_NATIVE_DECLARE(GetNativeScanReport___SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(NativeFindWirelessAdapters___STATIC__SZARRAY_U1);

    //--//
    static HRESULT GetNetInterfaceIndex(CLR_RT_StackFrame &stack, int *pNetIndex);
};

struct Library_sys_dev_wifi_native_System_Device_Wifi_WifiNetworkReport
{
    static const int FIELD___WifiNetworks = 1;

    //--//
};

struct Library_sys_dev_Wifi_native_nanoFramework_Networking_WifiNetworkHelper
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

struct Library_sys_dev_wifi_native_System_Device_Wifi_WifiEvent
{
    static const int FIELD__EventType = 3;
    static const int FIELD__Time = 4;

    //--//
};

struct Library_sys_dev_wifi_native_System_Device_Wifi_WifiEventListener
{
    static const int FIELD__WifiAdapters = 1;

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_Wifi;

#endif // SYS_DEV_WIFI_NATIVE_H
