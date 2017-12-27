//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


#include <targetPAL.h>
#include "win_dev_wifi_native.h"
#include "nf_rt_events_native.h"


///////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.Gpio.GpioPinDriveMode (in managed code) !!! //
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
// !!! KEEP IN SYNC WITH Windows.Devices.Gpio.GpioPinDriveMode (in managed code) !!! //
///////////////////////////////////////////////////////////////////////////////////////

    enum WiFiNetworkKind
    {
        Adhoc = 0,
        Any,
        Infrastructure
    };

 ///////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.Gpio.GpioPinDriveMode (in managed code) !!! //
///////////////////////////////////////////////////////////////////////////////////////

    enum WiFiReconnectionKind
    {
        Automatic = 0,
        Manual
    };


    //     private extern void NativeInit();

    HRESULT Library_win_dev_wifi_native_Windows_Devices_WiFi_WiFiAdapter::NativeInit___VOID( CLR_RT_StackFrame& stack )
    {
        NANOCLR_HEADER();


        NANOCLR_NOCLEANUP();
    }
  
  
    //    private extern void DisposeNative();

    HRESULT Library_win_dev_wifi_native_Windows_Devices_WiFi_WiFiAdapter::DisposeNative___VOID( CLR_RT_StackFrame& stack )
    {
        NANOCLR_HEADER();
 
        NANOCLR_NOCLEANUP();
    }


    
    // private extern WiFiNetworkReport NativeNetworkReport();
    HRESULT Library_win_dev_wifi_native_Windows_Devices_WiFi_WiFiAdapter::NativeNetworkReport___WindowsDevicesWiFiWiFiNetworkReport( CLR_RT_StackFrame& stack )
    {
        NANOCLR_HEADER();
        {
            uint16_t number = 0;

            esp_err_t res = esp_wifi_scan_get_ap_num( &number );
            if ( res == ESP_OK)
            {
                  NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            }


//TODO return report Array

            // Allocate room for ap records

        
  //          CLR_RT_HeapBlock& top   = stack.PushValue();
   
  //          NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( top, num, g_CLR_RT_WellKnownTypes.m_TypeStatic ));

  //    res = esp_wifi_scan_get_ap_records( &number, wifi_ap_record_t *ap_records)

        }
        NANOCLR_NOCLEANUP();
    }

 
    HRESULT  Library_win_dev_wifi_native_Windows_Devices_WiFi_WiFiAdapter::NativeConnect___WindowsDevicesWiFiWiFiConnectionResult__WindowsDevicesWiFiWiFiAvailableNetwork__WindowsDevicesWiFiWiFiReconnectionKind__STRING( CLR_RT_StackFrame& stack )
    {
        NANOCLR_HEADER();

        WiFiConnectionStatus Status = Success;

        esp_err_t res = esp_wifi_connect();
        switch(res)
        {
          default:
          case ESP_ERR_WIFI_NOT_INIT:
          case ESP_ERR_WIFI_NOT_STARTED:
          case ESP_ERR_WIFI_CONN:
                Status = UnspecifiedFailure;
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

        }

        // Return value to the managed application
        stack.SetResult_I4( Status );

        NANOCLR_NOCLEANUP();
    }


    //    private extern void NativeDisconnect();

   HRESULT Library_win_dev_wifi_native_Windows_Devices_WiFi_WiFiAdapter::NativeDisconnect___VOID( CLR_RT_StackFrame& stack )
    {
        NANOCLR_HEADER();


        esp_wifi_disconnect();

        
        NANOCLR_NOCLEANUP();
    }



    //    private extern void NativeScanAsync();

   HRESULT Library_win_dev_wifi_native_Windows_Devices_WiFi_WiFiAdapter::NativeScanAsync___VOID( CLR_RT_StackFrame& stack )
    {
        NANOCLR_HEADER();

        wifi_scan_config_t config;
        memset( &config, 0, sizeof(wifi_scan_config_t));

        config.scan_type = WIFI_SCAN_TYPE_PASSIVE;
        config.scan_time.passive = 500;                         // 500 ms

        esp_err_t res = esp_wifi_scan_start( &config, false );
        if ( res == ESP_OK)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }

        NANOCLR_NOCLEANUP();
    }


