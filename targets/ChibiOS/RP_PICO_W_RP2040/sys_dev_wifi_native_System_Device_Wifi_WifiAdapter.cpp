//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// ChibiOS WiFi adapter native implementation for Pico W (CYW43439).
// Implements the System.Device.Wifi managed API using the CYW43 driver.

#include <sys_dev_wifi_native.h>
#include <nf_rt_events_native.h>

extern "C" {
#include <nf_lwipthread_wifi.h>
#include <cyw43_configport.h>
}

////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH System.Device.Wifi (in managed code) !!! //
////////////////////////////////////////////////////////////////////////////////////
struct ScanRecord
{
    uint8_t bssid[6];
    uint8_t ssid[33];
    uint8_t rssi;
    uint8_t authMode;
    uint8_t cypherType;
};

// ---------------------------------------------------------------------------
// GetNetInterfaceIndex — helper to retrieve network interface index
// ---------------------------------------------------------------------------
HRESULT Library_sys_dev_wifi_native_System_Device_Wifi_WifiAdapter::GetNetInterfaceIndex(
    CLR_RT_StackFrame &stack,
    int *pNetIndex)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        if (pThis[Library_sys_dev_wifi_native_System_Device_Wifi_WifiAdapter::FIELD___disposedValue]
                .NumericByRef()
                .u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        *pNetIndex = pThis[FIELD___networkInterface].NumericByRefConst().s4;
    }
    NANOCLR_NOCLEANUP();
}

// ---------------------------------------------------------------------------
// NativeSetDeviceName
// ---------------------------------------------------------------------------
HRESULT Library_sys_dev_wifi_native_System_Device_Wifi_WifiAdapter::NativeSetDeviceName___VOID__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        // hostname is set via lwIP netif, not separately configurable on CYW43
        (void)stack;
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

// ---------------------------------------------------------------------------
// DisposeNative
// ---------------------------------------------------------------------------
HRESULT Library_sys_dev_wifi_native_System_Device_Wifi_WifiAdapter::DisposeNative___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    (void)stack;
    NANOCLR_NOCLEANUP_NOLABEL();
}

// ---------------------------------------------------------------------------
// NativeInit
// ---------------------------------------------------------------------------
HRESULT Library_sys_dev_wifi_native_System_Device_Wifi_WifiAdapter::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    (void)stack;
    // CYW43 is initialized by the lwIP WiFi thread on startup
    NANOCLR_NOCLEANUP_NOLABEL();
}

// ---------------------------------------------------------------------------
// NativeConnect — connect to a WiFi network
// ---------------------------------------------------------------------------
HRESULT Library_sys_dev_wifi_native_System_Device_Wifi_WifiAdapter::
    NativeConnect___SystemDeviceWifiWifiConnectionStatus__STRING__STRING__SystemDeviceWifiWifiReconnectionKind(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        const char *szSsid;
        const char *szPassPhrase;
        int netIndex;
        WifiConnectionStatus status = WifiConnectionStatus_UnspecifiedFailure;

        NANOCLR_CHECK_HRESULT(GetNetInterfaceIndex(stack, &netIndex));

        szSsid = stack.Arg1().RecoverString();
        FAULT_ON_NULL(szSsid);

        szPassPhrase = stack.Arg2().RecoverString();
        // password can be NULL for open networks

        // Initiate WiFi connection via CYW43 driver
        int result = Network_Interface_Start_Connect(
            netIndex, szSsid, szPassPhrase ? szPassPhrase : "", 0);

        if (result == 0)
        {
            // Wait for connection to complete (poll with timeout)
            int timeout_ms = 15000;
            int poll_ms = 100;
            int elapsed = 0;

            while (elapsed < timeout_ms)
            {
                int connectResult = Network_Interface_Connect_Result(netIndex);
                if (connectResult >= 0)
                {
                    status = (connectResult == 0)
                                 ? WifiConnectionStatus_Success
                                 : WifiConnectionStatus_UnspecifiedFailure;
                    break;
                }
                chThdSleepMilliseconds(poll_ms);
                elapsed += poll_ms;
            }

            if (elapsed >= timeout_ms)
            {
                status = WifiConnectionStatus_Timeout;
            }
        }
        else
        {
            status = WifiConnectionStatus_NetworkNotAvailable;
        }

        stack.SetResult_I4(status);
    }
    NANOCLR_NOCLEANUP();
}

// ---------------------------------------------------------------------------
// NativeDisconnect — disconnect from current WiFi network
// ---------------------------------------------------------------------------
HRESULT Library_sys_dev_wifi_native_System_Device_Wifi_WifiAdapter::NativeDisconnect___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int netIndex;
        NANOCLR_CHECK_HRESULT(GetNetInterfaceIndex(stack, &netIndex));
        Network_Interface_Disconnect(netIndex);
    }
    NANOCLR_NOCLEANUP();
}

// ---------------------------------------------------------------------------
// NativeScanAsync — initiate a WiFi network scan
// ---------------------------------------------------------------------------
HRESULT Library_sys_dev_wifi_native_System_Device_Wifi_WifiAdapter::NativeScanAsync___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int netIndex;
        NANOCLR_CHECK_HRESULT(GetNetInterfaceIndex(stack, &netIndex));

        int startScanResult = Network_Interface_Start_Scan(netIndex);

        if (startScanResult != 0) // StartScanOutcome_Success
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }
    }
    NANOCLR_NOCLEANUP();
}

// ---------------------------------------------------------------------------
// GetNativeScanReport — return scan results as byte array
// ---------------------------------------------------------------------------
HRESULT Library_sys_dev_wifi_native_System_Device_Wifi_WifiAdapter::GetNativeScanReport___SZARRAY_U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock &top = stack.PushValueAndClear();

        // WiFi scan results are not yet available in the CYW43 driver integration.
        // Return empty result for now.
        uint16_t number = 0;
        int rlen = sizeof(uint16_t);

        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(top, rlen, g_CLR_RT_WellKnownTypes.m_UInt8));

        CLR_RT_HeapBlock_Array *array = top.DereferenceArray();
        CLR_UINT8 *buf = array->GetFirstElement();
        *buf = (uint8_t)number;
    }
    NANOCLR_NOCLEANUP();
}

// ---------------------------------------------------------------------------
// NativeFindWirelessAdapters — return array of WiFi interface indices
// ---------------------------------------------------------------------------
HRESULT Library_sys_dev_wifi_native_System_Device_Wifi_WifiAdapter::NativeFindWirelessAdapters___STATIC__SZARRAY_U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock_Array *array;
        CLR_UINT8 *arrayOfIndexes;
        int index;
        int interfaceCount = 0;

        CLR_RT_HeapBlock &top = stack.PushValueAndClear();

        HAL_Configuration_NetworkInterface *netInterfaceConfig =
            (HAL_Configuration_NetworkInterface *)platform_malloc(sizeof(HAL_Configuration_NetworkInterface));

        if (netInterfaceConfig == NULL)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }

        for (index = 0; index < g_TargetConfiguration.NetworkInterfaceConfigs->Count; index++)
        {
            if (!ConfigurationManager_GetConfigurationBlock(
                    netInterfaceConfig, DeviceConfigurationOption_Network, index))
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
            }

            if (netInterfaceConfig->InterfaceType == NetworkInterfaceType_Wireless80211)
            {
                interfaceCount++;
            }
        }

        NANOCLR_CHECK_HRESULT(
            CLR_RT_HeapBlock_Array::CreateInstance(top, interfaceCount, g_CLR_RT_WellKnownTypes.m_UInt8));
        array = top.DereferenceArray();
        arrayOfIndexes = array->GetFirstElement();

        for (index = 0; index < g_TargetConfiguration.NetworkInterfaceConfigs->Count; index++)
        {
            if (!ConfigurationManager_GetConfigurationBlock(
                    netInterfaceConfig, DeviceConfigurationOption_Network, index))
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
            }

            if (netInterfaceConfig->InterfaceType == NetworkInterfaceType_Wireless80211)
            {
                *arrayOfIndexes = index;
                arrayOfIndexes++;
            }
        }

        platform_free(netInterfaceConfig);
    }
    NANOCLR_NOCLEANUP();
}
