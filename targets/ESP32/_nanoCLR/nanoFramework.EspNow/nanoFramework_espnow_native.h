// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NANOFRAMEWORK_ESPNOW_NATIVE_H
#define NANOFRAMEWORK_ESPNOW_NATIVE_H

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>
#include <nanoHAL.h>

#include <esp_interface.h>
#include <esp_wifi.h>
#include <esp_now.h>

#ifdef ESP_NOW_MAX_DATA_LEN_V2
#define NF_ESPNOW_MAX_DATA_LEN ESP_NOW_MAX_DATA_LEN_V2
#else
#define NF_ESPNOW_MAX_DATA_LEN ESP_NOW_MAX_DATA_LEN
#endif

static constexpr uint32_t NF_ESPNOW_RX_QUEUE_DEPTH = 10;

typedef enum __nfpack EspNowEventType
{
    EspNowEventType_DataSent = 1,
    EspNowEventType_DataReceived = 2,
} EspNowEventType;

typedef enum __nfpack EspNowSendStatus
{
    EspNowSendStatus_Success = 0,
    EspNowSendStatus_Fail = 1,
} EspNowSendStatus;

struct EspNowPacket
{
    uint8_t peer_mac[ESP_NOW_ETH_ALEN];
    uint8_t *data;
    uint16_t data_len;
};

struct Library_nf_espnow_nanoFramework_EspNow_DataReceivedEventArgs
{
    static const int FIELD___peerMac = 1;
    static const int FIELD___data = 2;
    static const int FIELD___dataLen = 3;

    //--//
};

struct Library_nf_espnow_nanoFramework_EspNow_DataSentEventArgs
{
    static const int FIELD___peerMac = 1;
    static const int FIELD___status = 2;

    //--//
};

struct Library_nf_espnow_nanoFramework_EspNow_EspNowController
{
    static const int FIELD_STATIC__s_instance = 0;
    static const int FIELD_STATIC__s_syncLock = 1;
    static const int FIELD_STATIC__s_eventListener = 2;

    static const int FIELD___disposed = 1;
    static const int FIELD___syncLock = 2;
    static const int FIELD___callbacksDataReceivedEvent = 3;
    static const int FIELD___callbacksDataSentEvent = 4;

    NANOCLR_NATIVE_DECLARE(NativeInitialize___I4);
    NANOCLR_NATIVE_DECLARE(NativeDispose___VOID__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeEspNowSend___I4__SZARRAY_U1__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(NativeEspNowAddPeer___I4__SZARRAY_U1__U1__BOOLEAN__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(NativeGetMaximumDataLength___I4);
    NANOCLR_NATIVE_DECLARE(NativeReadPacket___I4__SZARRAY_U1__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(NativeGetReceiveOverflowCount___I4);
    NANOCLR_NATIVE_DECLARE(NativeReadSendStatus___I4__SZARRAY_U1);

    //--//
    static void DataSentCb(const wifi_tx_info_t *tx_info, esp_now_send_status_t status);
    static void DataRecvCb(const esp_now_recv_info *recv_info, const uint8_t *incomingData, int len);
};

struct Library_nf_espnow_nanoFramework_EspNow_EspNowEvent
{
    static const int FIELD__EventType = 3;

    //--//
};

struct Library_nf_espnow_nanoFramework_EspNow_EspNowEventListener
{
    static const int FIELD___controller = 1;

    //--//
};

struct Library_nf_espnow_nanoFramework_EspNow_EspNowException
{
    static const int FIELD__esp_err = 5;

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_EspNow;

#endif // NANOFRAMEWORK_ESPNOW_NATIVE_H
