//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_wifi_native.h>
#include <nf_rt_events_native.h>
#include <esp_wifi_types.h>

///////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH System.Device.WiFi (in managed code) !!! //
///////////////////////////////////////////////////////////////////////////////////////
struct ScanRecord
{
    uint8_t bssid[6];
    uint8_t ssid[33];
    uint8_t rssi;
    uint8_t authMode;
    uint8_t cypherType;
};

HRESULT Library_sys_dev_wifi_native_System_Device_WiFi_WiFiAdapter::DisposeNative___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    (void)stack;

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_wifi_native_System_Device_WiFi_WiFiAdapter::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    (void)stack;

    NANOCLR_NOCLEANUP_NOLABEL();
}

//
//  Pickup Net interface index and do checks
//
HRESULT Library_sys_dev_wifi_native_System_Device_WiFi_WiFiAdapter::GetNetInterfaceIndex(
    CLR_RT_StackFrame &stack,
    int *pNetIndex)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        if (pThis[Library_sys_dev_wifi_native_System_Device_WiFi_WiFiAdapter::FIELD___disposedValue]
                .NumericByRef()
                .u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        *pNetIndex = pThis[FIELD___networkInterface].NumericByRefConst().s4;
    }
    NANOCLR_NOCLEANUP();
}

//
// Connect to Wireless connection to passed SSID / passPhase
//
HRESULT Library_sys_dev_wifi_native_System_Device_WiFi_WiFiAdapter::
    NativeConnect___SystemDeviceWiFiWiFiConnectionStatus__STRING__STRING__SystemDeviceWiFiWiFiReconnectionKind(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        const char *szSsid;
        const char *szPassPhase;
        int reconnectionKind;
        esp_err_t res;
        int netIndex;
        CLR_RT_HeapBlock hbTimeout;
        CLR_INT64 *timeout;
        bool eventResult = true;
        WiFiConnectionStatus Status = WiFiConnectionStatus_UnspecifiedFailure;

        NANOCLR_CHECK_HRESULT(GetNetInterfaceIndex(stack, &netIndex));

        if (stack.m_customState == 0)
        {
            NANOCLR_CHECK_HRESULT(GetNetInterfaceIndex(stack, &netIndex));

            // Get SSID
            szSsid = stack.Arg1().RecoverString();
            FAULT_ON_NULL(szSsid);

            // Get Password
            szPassPhase = stack.Arg2().RecoverString();
            FAULT_ON_NULL(szPassPhase);

            // Reconnect kind
            reconnectionKind = stack.Arg3().NumericByRef().s4;

            // Set timeout for connect
            hbTimeout.SetInteger((CLR_INT64)20000 * TIME_CONVERSION__TO_MILLISECONDS);

            res = (esp_err_t)Network_Interface_Start_Connect(netIndex, szSsid, szPassPhase, reconnectionKind);
            if (res != ESP_OK)
            {
                Status = WiFiConnectionStatus_UnspecifiedFailure;
                eventResult = false;
            }
        }

        // Wait for connect to finish
        while (eventResult)
        {
            int connectResult = Network_Interface_Connect_Result(netIndex);
            if (connectResult >= 0)
            {
                // Map ESP32 wifi reason code to WiFiConnectionStatus
                switch (connectResult)
                {
                    case 0:
                        Status = WiFiConnectionStatus_Success;
                        break;

                    case WIFI_REASON_NO_AP_FOUND:
                        Status = WiFiConnectionStatus_NetworkNotAvailable;
                        break;

                    case WIFI_REASON_AUTH_EXPIRE:
                    case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
                    case WIFI_REASON_BEACON_TIMEOUT:
                    case WIFI_REASON_AUTH_FAIL:
                    case WIFI_REASON_ASSOC_FAIL:
                    case WIFI_REASON_HANDSHAKE_TIMEOUT:
                        Status = WiFiConnectionStatus_InvalidCredential;
                        break;

                    default:
                        Status = WiFiConnectionStatus_UnspecifiedFailure;
                        break;
                }
                break;
            }

            // Get timeout
            NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));

            // non-blocking wait allowing other threads to run while we wait for the Spi transaction to complete
            NANOCLR_CHECK_HRESULT(
                g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_Wifi_Station, eventResult));

            if (!eventResult)
            {
                // Timeout
                Status = WiFiConnectionStatus_Timeout;
                break;
            }
        }

        // Return value to the managed application
        stack.SetResult_I4(Status);
    }
    NANOCLR_NOCLEANUP();
}

//
//  Disconnect WiFi connection
//
HRESULT Library_sys_dev_wifi_native_System_Device_WiFi_WiFiAdapter::NativeDisconnect___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int netIndex;

        NANOCLR_CHECK_HRESULT(GetNetInterfaceIndex(stack, &netIndex));

        Network_Interface_Disconnect(netIndex);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_wifi_native_System_Device_WiFi_WiFiAdapter::NativeScanAsync___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int netIndex;

        NANOCLR_CHECK_HRESULT(GetNetInterfaceIndex(stack, &netIndex));

        // Start scan
        if (Network_Interface_Start_Scan(netIndex) == false)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }
    }
    NANOCLR_NOCLEANUP();
}

//
//  Stores Ap records to target string.
//  if pTarget == 0 then just calculates length of target
//
//  unint_8[6]  MAC;
//  unint_8[33] SSID
//  unit8_t     RSSI
//
//  Return : length in pTarget
int Library_sys_dev_wifi_native_System_Device_WiFi_WiFiAdapter::StoreApRecordsToString(
    uint8_t *pTarget,
    wifi_ap_record_t *apRecords,
    uint16_t recordCount)
{
    int index;

    if (pTarget != 0)
    {
        // Store record count as first of returned byte[]
        *pTarget++ = (uint8_t)recordCount;

        ScanRecord *pScanRec = (ScanRecord *)pTarget;

        for (index = 0; index < recordCount; index++)
        {
            memcpy(pScanRec->bssid, apRecords[index].bssid, 6);
            memcpy(pScanRec->ssid, apRecords[index].ssid, 33);
            pScanRec->rssi = apRecords[index].rssi;
            pScanRec->authMode = apRecords[index].authmode;
            pScanRec->cypherType = apRecords[index].pairwise_cipher;
            pScanRec++;
        }
    }

    // Fixed length records so we can calculate length
    return (recordCount * sizeof(ScanRecord) + sizeof(uint16_t));
}

// private extern WiFiNetworkReport NativeNetworkReport();
HRESULT Library_sys_dev_wifi_native_System_Device_WiFi_WiFiAdapter::GetNativeScanReport___SZARRAY_U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    // Temporary ap record storage
    wifi_ap_record_t *ap_records = 0;
    {
        CLR_RT_HeapBlock &top = stack.PushValueAndClear();
        CLR_RT_HeapBlock_Array *array;
        CLR_UINT8 *buf;
        uint16_t number = 0;
        int rlen = 2;

        esp_err_t res = esp_wifi_scan_get_ap_num(&number);
        if (res != ESP_OK)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        if (number != 0)
        {
            // Allocate room for ap records
            ap_records = (wifi_ap_record_t *)platform_malloc(sizeof(wifi_ap_record_t) * number);
            if (ap_records == 0)
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);

            res = esp_wifi_scan_get_ap_records(&number, ap_records);
            if (res != ESP_OK)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            }

            // Store AP records in string and return to managed code to sort out
            // MAC, SSID, RSSI, Auth mode,

            // Find out how big a string is required
            rlen = StoreApRecordsToString(0, ap_records, number);
        }

        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(top, rlen, g_CLR_RT_WellKnownTypes.m_UInt8));
        array = top.DereferenceArray();
        buf = array->GetFirstElement();

        // Store into returned uint8 array
        StoreApRecordsToString(buf, ap_records, number);
    }

    NANOCLR_CLEANUP();

    if (ap_records)
    {
        platform_free(ap_records);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_sys_dev_wifi_native_System_Device_WiFi_WiFiAdapter::NativeFindWirelessAdapters___STATIC__SZARRAY_U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array *array;
    CLR_UINT8 *arrayOfIndexes;
    int index;
    int interfaceCount = 0;

    // add return object to stack
    CLR_RT_HeapBlock &top = stack.PushValueAndClear();

    HAL_Configuration_NetworkInterface *netInterfaceConfig =
        (HAL_Configuration_NetworkInterface *)platform_malloc(sizeof(HAL_Configuration_NetworkInterface));

    // check allocation
    if (netInterfaceConfig == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    // 1st pass: find number of wireless adapters
    for (index = 0; index < g_TargetConfiguration.NetworkInterfaceConfigs->Count; index++)
    {
        if (!ConfigurationManager_GetConfigurationBlock(netInterfaceConfig, DeviceConfigurationOption_Network, index))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }

        // check if this is a Wireless80211 adapter
        if (netInterfaceConfig->InterfaceType == NetworkInterfaceType_Wireless80211)
        {
            interfaceCount++;
        }
    }

    // build array with indexes of Wireless interfaces
    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(top, interfaceCount, g_CLR_RT_WellKnownTypes.m_UInt8));
    array = top.DereferenceArray();
    arrayOfIndexes = array->GetFirstElement();

    // 2nd pass: grab the index of the of the wireless adapters
    for (index = 0; index < g_TargetConfiguration.NetworkInterfaceConfigs->Count; index++)
    {
        if (!ConfigurationManager_GetConfigurationBlock(netInterfaceConfig, DeviceConfigurationOption_Network, index))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }

        // check if this is a Wireless80211 adapter
        if (netInterfaceConfig->InterfaceType == NetworkInterfaceType_Wireless80211)
        {
            // store index
            *arrayOfIndexes = index;

            // move to next position in array of indexes
            arrayOfIndexes++;
        }
    }

    NANOCLR_NOCLEANUP();
}
