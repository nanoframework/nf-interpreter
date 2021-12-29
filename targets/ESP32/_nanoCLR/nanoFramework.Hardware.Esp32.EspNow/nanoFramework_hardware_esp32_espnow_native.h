//-----------------------------------------------------------------------------
//
//                   ** WARNING! ** 
//    This file was generated automatically by a tool.
//    Re-running the tool will overwrite this file.
//    You should copy this file to a custom location
//    before adding any customization in the copy to
//    prevent loss of your changes when the tool is
//    re-run.
//
//-----------------------------------------------------------------------------

#ifndef _NANOFRAMEWORK_HARDWARE_ESP32_ESPNOW_NATIVE_H_
#define _NANOFRAMEWORK_HARDWARE_ESP32_ESPNOW_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

#include <esp_wifi.h>
#include <esp_now.h>

#define EVENT_ESP32_ESPNOW_DATASENT 1
#define EVENT_ESP32_ESPNOW_DATARECV 2


struct EspNowDataSentEventData
{
    uint8_t peer_mac[ESP_NOW_ETH_ALEN]; 
    esp_now_send_status_t status;
};

struct EspNowDataRecvEventData
{
    uint8_t peer_mac[ESP_NOW_ETH_ALEN]; 
    uint8_t data[ESP_NOW_MAX_DATA_LEN];
    int dataLen;
};

struct Library_nanoFramework_hardware_esp32_espnow_native_nanoFramework_Hardware_Esp32_EspNow_DataReceivedEventArgs
{
    static const int FIELD__PeerMac = 1;
    static const int FIELD__Data = 2;
    static const int FIELD__DataLen = 3;

    //--//

};

struct Library_nanoFramework_hardware_esp32_espnow_native_nanoFramework_Hardware_Esp32_EspNow_DataSentEventArgs
{
    static const int FIELD__PeerMac = 1;
    static const int FIELD__Status = 2;

    //--//

};

struct Library_nanoFramework_hardware_esp32_espnow_native_nanoFramework_Hardware_Esp32_EspNow_EspNowController
{
    static const int FIELD_STATIC__BROADCASTMAC = 0;

    static const int FIELD__DataSent = 1;
    static const int FIELD__DataReceived = 2;
    static const int FIELD__isDisposed = 3;

    NANOCLR_NATIVE_DECLARE(NativeInitialize___I4);
    NANOCLR_NATIVE_DECLARE(NativeDispose___VOID__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeEspNowSend___I4__SZARRAY_U1__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(NativeEspNowAddPeer___I4__SZARRAY_U1__U1);

    //--//
    static EspNowDataSentEventData dataSentEventData;
    static EspNowDataRecvEventData dataRecvEventData;

    static void DataSentCb(const uint8_t *mac_addr, esp_now_send_status_t status);
    static void DataRecvCb(const uint8_t * mac, const uint8_t *incomingData, int len);
};

struct Library_nanoFramework_hardware_esp32_espnow_native_nanoFramework_Hardware_Esp32_EspNow_EspNowController__DataRecvEventInternal
{
    static const int FIELD__PeerMac = 3;
    static const int FIELD__Data = 4;
    static const int FIELD__DataLen = 5;

    //--//

};

struct Library_nanoFramework_hardware_esp32_espnow_native_nanoFramework_Hardware_Esp32_EspNow_EspNowController__DataSentEventInternal
{
    static const int FIELD__PeerMac = 3;
    static const int FIELD__Status = 4;

    //--//

};

struct Library_nanoFramework_hardware_esp32_espnow_native_nanoFramework_Hardware_Esp32_EspNow_EspNowController__EspNowEventHandler
{
    static const int FIELD__controllerInstance = 1;

    NANOCLR_NATIVE_DECLARE(ProcessEvent___nanoFrameworkRuntimeEventsBaseEvent__U4__U4__SystemDateTime);

    //--//

    static HRESULT CopyByteArrayToCLRArray(CLR_RT_HeapBlock &target, uint8_t *src, CLR_UINT32 length);
    static HRESULT ProcessEvent_DataSent(CLR_RT_StackFrame &stack);
    static HRESULT ProcessEvent_DataRecv(CLR_RT_StackFrame &stack);

};


extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Hardware_Esp32_EspNow;

#endif  //_NANOFRAMEWORK_HARDWARE_ESP32_ESPNOW_NATIVE_H_
