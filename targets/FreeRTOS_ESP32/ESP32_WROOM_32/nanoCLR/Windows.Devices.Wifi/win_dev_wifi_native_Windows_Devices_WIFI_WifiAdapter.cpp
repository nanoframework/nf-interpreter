//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <targetPAL.h>
#include "win_dev_wifi_native.h"
#include "nf_rt_events_native.h"

///////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.WiFi (in managed code) !!! //
///////////////////////////////////////////////////////////////////////////////////////
struct ScanRecord 
{
  uint8_t  bssid[6];
  uint8_t  ssid[33]; 
  uint8_t  rssi;
  uint8_t  authMode;
  uint8_t  cypherType;
};

///////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.WiFi (in managed code) !!! //
///////////////////////////////////////////////////////////////////////////////////////
enum WiFiConnectionStatus
{
    AccessRevoked = 0,
    InvalidCredential,
    NetworkNotAvailable,
    Success,
    Timeout,
    UnspecifiedFailure,
    UnsupportedAuthenticationProtocol
};

///////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.WiFi (in managed code) !!! //
///////////////////////////////////////////////////////////////////////////////////////
enum WiFiNetworkKind
{
    Adhoc = 0,
    Any,
    Infrastructure
};

 ///////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.WiFi (in managed code) !!! //
///////////////////////////////////////////////////////////////////////////////////////
enum WiFiReconnectionKind
{
    Automatic = 0,
    Manual
};

HRESULT Library_win_dev_wifi_native_Windows_Devices_WiFi_WiFiAdapter::DisposeNative___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    (void)stack;  

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_win_dev_wifi_native_Windows_Devices_WiFi_WiFiAdapter::NativeInit___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    (void)stack;  

    NANOCLR_NOCLEANUP_NOLABEL();
}

//
//  Pickup Net interface index and do checks
//
HRESULT Library_win_dev_wifi_native_Windows_Devices_WiFi_WiFiAdapter::GetNetInterfaceIndex(CLR_RT_StackFrame& stack, int * pNetIndex)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
            
        if(pThis[ Library_win_dev_wifi_native_Windows_Devices_WiFi_WiFiAdapter::FIELD___disposedValue ].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }
        
        *pNetIndex = pThis[ FIELD___networkInterface ].NumericByRefConst().s4;
    }
    NANOCLR_NOCLEANUP();
}

//
// Connect to Wireless connection to passed SSID / passPhase 
//
HRESULT Library_win_dev_wifi_native_Windows_Devices_WiFi_WiFiAdapter::NativeConnect___WindowsDevicesWiFiWiFiConnectionStatus__STRING__STRING__WindowsDevicesWiFiWiFiReconnectionKind( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        const char*  szSsid;   
        const char*  szPassPhase;   
        int          reconnectionKind;
        esp_err_t    res;
        int          netIndex;

        NANOCLR_CHECK_HRESULT( GetNetInterfaceIndex(stack, &netIndex ) );

        WiFiConnectionStatus Status = Success;
        
        // Get SSID
        szSsid = stack.Arg1().RecoverString(); FAULT_ON_NULL(szSsid);

        // Get Password
        szPassPhase = stack.Arg2().RecoverString(); FAULT_ON_NULL(szPassPhase);

        // Reconnect kind
        reconnectionKind = stack.Arg3().NumericByRef().s4;

        res = (esp_err_t)Network_Interface_Connect(netIndex, szSsid, szPassPhase, reconnectionKind);
        switch(res)
        {
            case ESP_OK:
                Status = Success;
                break;
            case ESP_ERR_WIFI_SSID:
                Status = NetworkNotAvailable;
                break;
            case ESP_ERR_WIFI_PASSWORD:
                Status = InvalidCredential;
                break;
            case ESP_ERR_WIFI_TIMEOUT:
                Status = Timeout;
                break;

            default:
            case ESP_ERR_WIFI_NOT_INIT:
            case ESP_ERR_WIFI_NOT_STARTED:
            case ESP_ERR_WIFI_CONN:
                Status = UnspecifiedFailure;
                break;
        }

        // Return value to the managed application
        stack.SetResult_I4( Status );
    }
    NANOCLR_NOCLEANUP();
}

//
//  Disconnect WiFi connection
//
HRESULT Library_win_dev_wifi_native_Windows_Devices_WiFi_WiFiAdapter::NativeDisconnect___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        int netIndex;

        NANOCLR_CHECK_HRESULT( GetNetInterfaceIndex(stack, &netIndex ) );

        Network_Interface_Disconnect(netIndex);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_wifi_native_Windows_Devices_WiFi_WiFiAdapter::NativeScanAsync___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    { 
        int netIndex;

        NANOCLR_CHECK_HRESULT( GetNetInterfaceIndex(stack, &netIndex ) );

        // Start scan 
        if ( Network_Interface_Start_Scan(netIndex) == false)
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
int  StoreApRecordsToString( uint8_t * pTarget, wifi_ap_record_t * apRecords, uint16_t recordCount )
{
    int index;

    if ( pTarget != 0 ) 
    {
        // Store record count as first of returned byte[] 
        *pTarget++ = (uint8_t)recordCount;      
    
        ScanRecord * pScanRec = (ScanRecord *)pTarget;
    
        for( index=0; index < recordCount; index++)
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
HRESULT Library_win_dev_wifi_native_Windows_Devices_WiFi_WiFiAdapter::GetNativeScanReport___SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    // Temporary ap record storage
    wifi_ap_record_t * ap_records = 0; 
    {
        CLR_RT_HeapBlock&   top = stack.PushValueAndClear();           
        CLR_RT_HeapBlock_Array* array;
        CLR_UINT8*  buf;
        uint16_t number = 0;
        int rlen = 2;

        esp_err_t res = esp_wifi_scan_get_ap_num( &number );
        if ( res != ESP_OK)
        {
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        if ( number != 0 )
        { 
            // Allocate room for ap records
            ap_records = (wifi_ap_record_t *)platform_malloc(sizeof(wifi_ap_record_t) * number );
            if (ap_records == 0) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);

            res = esp_wifi_scan_get_ap_records( &number, ap_records);
            if ( res != ESP_OK)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            }

            // Store AP records in string and return to managed code to sort out
            // MAC, SSID, RSSI, Auth mode, 
        
            // Find out how big a string is required
            rlen = StoreApRecordsToString( 0, ap_records, number );
        }

        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( top, rlen, g_CLR_RT_WellKnownTypes.m_UInt8 ));
        array = top.DereferenceArray();
        buf   = array->GetFirstElement();

        // Store into returned uint8 array
        StoreApRecordsToString( buf, ap_records, number );
    }
  
    NANOCLR_CLEANUP();
  
    if(ap_records)
    {
        platform_free(ap_records);
    }
  
    NANOCLR_CLEANUP_END();
}

//
//  Find wireless adapters in network config
//  return count of adapters found
//
int  FindWirelessAdaptersStore( uint8_t * pTarget )
{
    int wirelessCount = 0;

    for( int index=0; index < g_TargetConfiguration.NetworkInterfaceConfigs->Count; index++)
    {
        HAL_Configuration_NetworkInterface* pNetInterface = g_TargetConfiguration.NetworkInterfaceConfigs->Configs[index];

        if (pNetInterface->InterfaceType == NetworkInterfaceType_Wireless80211)
        {
            wirelessCount++;
            if ( pTarget != 0) *pTarget++ = (uint8_t)index;
        }
    }

    return wirelessCount;
}

HRESULT Library_win_dev_wifi_native_Windows_Devices_WiFi_WiFiAdapter::NativeFindWirelessAdapters___STATIC__SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock&   top = stack.PushValueAndClear();           
        CLR_RT_HeapBlock_Array* array;
        CLR_UINT8*  buf;
        int rlen;

        // Find number of wireless adapters so we can allocated return array 
        rlen = FindWirelessAdaptersStore( 0 );

        // Return interface indexes
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( top, rlen, g_CLR_RT_WellKnownTypes.m_UInt8 ));
        array = top.DereferenceArray();
        buf   = array->GetFirstElement();

        // Store adapters indexes in to returned array
        FindWirelessAdaptersStore( buf );
    }
    NANOCLR_NOCLEANUP();
}
