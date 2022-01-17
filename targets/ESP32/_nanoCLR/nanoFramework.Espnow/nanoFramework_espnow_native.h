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

#ifndef _NANOFRAMEWORK_ESPNOW_NATIVE_H_
#define _NANOFRAMEWORK_ESPNOW_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

#include <esp_interface.h>
#include <esp_wifi.h>
#include <esp_now.h>

//#define DEBUG_ESPNOW 1

#define DEBUG_FENTER()     DEBUG_WRITELINE("entry")
#define DEBUG_FEXIT()      DEBUG_WRITELINE("exit")
#define DEBUG_FEXIT_RET(v) DEBUG_WRITELINE("return: %d", v)

#if defined(DEBUG_ESPNOW)
#define DEBUG_WRITELINE(...)                                                                                           \
    {                                                                                                                  \
        char temporaryStringBuffer1[64];                                                                               \
        snprintf(temporaryStringBuffer1, sizeof(temporaryStringBuffer1), __VA_ARGS__);                                 \
        char temporaryStringBuffer2[128];                                                                              \
        int realStringSize = snprintf(                                                                                 \
            temporaryStringBuffer2,                                                                                    \
            sizeof(temporaryStringBuffer2),                                                                            \
            "\r\n[%s] %s\r\n",                                                                                         \
            __func__,                                                                                                  \
            temporaryStringBuffer1);                                                                                   \
        CLR_EE_DBG_EVENT_BROADCAST(                                                                                    \
            CLR_DBG_Commands_c_Monitor_Message,                                                                        \
            realStringSize,                                                                                            \
            temporaryStringBuffer2,                                                                                    \
            WP_Flags_c_NonCritical | WP_Flags_c_NoCaching);                                                            \
    }

#else
#define DEBUG_WRITELINE(...) ;
#endif

#define EVENT_ESPNOW_DATASENT 1
#define EVENT_ESPNOW_DATARECV 2

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

struct Library_nanoFramework_espnow_native_nanoFramework_Espnow_DataReceivedEventArgs
{
    static const int FIELD__PeerMac = 1;
    static const int FIELD__Data = 2;
    static const int FIELD__DataLen = 3;

    //--//
};

struct Library_nanoFramework_espnow_native_nanoFramework_Espnow_DataRecvEventInternal
{
    static const int FIELD__PeerMac = 3;
    static const int FIELD__Data = 4;
    static const int FIELD__DataLen = 5;

    //--//
};

struct Library_nanoFramework_espnow_native_nanoFramework_Espnow_DataSentEventArgs
{
    static const int FIELD__PeerMac = 1;
    static const int FIELD__Status = 2;

    //--//
};

struct Library_nanoFramework_espnow_native_nanoFramework_Espnow_DataSentEventInternal
{
    static const int FIELD__PeerMac = 3;
    static const int FIELD__Status = 4;

    //--//
};

struct Library_nanoFramework_espnow_native_nanoFramework_Espnow_EspNowController
{
    static const int FIELD_STATIC__BROADCASTMAC = 0;

    static const int FIELD__DataSent = 1;
    static const int FIELD__DataReceived = 2;
    static const int FIELD__isDisposed = 3;
    static const int FIELD__eventHandler = 4;

    NANOCLR_NATIVE_DECLARE(NativeInitialize___I4);
    NANOCLR_NATIVE_DECLARE(NativeDispose___VOID__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeEspNowSend___I4__SZARRAY_U1__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(NativeEspNowAddPeer___I4__SZARRAY_U1__U1);

    //--//
    static EspNowDataSentEventData dataSentEventData;
    static EspNowDataRecvEventData dataRecvEventData;

    static void DataSentCb(const uint8_t *mac_addr, esp_now_send_status_t status);
    static void DataRecvCb(const uint8_t *mac, const uint8_t *incomingData, int len);
};

struct Library_nanoFramework_espnow_native_nanoFramework_Espnow_EspNowController__EspNowEventHandler
{
    static const int FIELD__controllerInstance = 1;

    NANOCLR_NATIVE_DECLARE(ProcessEvent___nanoFrameworkRuntimeEventsBaseEvent__U4__U4__SystemDateTime);

    //--//

    static HRESULT CopyByteArrayToCLRArray(CLR_RT_HeapBlock &target, uint8_t *src, CLR_UINT32 length);
    static HRESULT ProcessEvent_DataSent(CLR_RT_StackFrame &stack);
    static HRESULT ProcessEvent_DataRecv(CLR_RT_StackFrame &stack);
};

struct Library_nanoFramework_espnow_native_nanoFramework_Espnow_EspNowException
{
    static const int FIELD__esp_err = 5;

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Espnow;

#endif //_NANOFRAMEWORK_ESPNOW_NATIVE_H_
