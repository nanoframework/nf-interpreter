// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include "nanoFramework_espnow_native.h"

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

static const int EVENT_ESPNOW_DATARECV = 2;
static const int ESPNOW_ERR_ESPNOW_INIT = 10001;
static const int ESPNOW_ERR_INVALID_PEER = 10002;
static const int ESPNOW_ERR_ADD_PEER = 10003;
static const int ESPNOW_ERR_RECEIVE_TIMEOUT = 10004;

struct EspNowPacketQueue
{
    EspNowPacket packets[NF_ESPNOW_RX_QUEUE_DEPTH];
    uint32_t head;
    uint32_t count;
    uint32_t overflow_count;
    SemaphoreHandle_t mutex;
    SemaphoreHandle_t data_available;
    uint32_t maximum_data_length;
};

static EspNowPacketQueue s_rxQueue = {};

static void ResetReceiveQueue(uint32_t maximumDataLength)
{
    s_rxQueue.head = 0;
    s_rxQueue.count = 0;
    s_rxQueue.overflow_count = 0;
    s_rxQueue.maximum_data_length = maximumDataLength;
}

static void DisposeReceiveQueue()
{
    if (s_rxQueue.data_available != NULL)
    {
        vSemaphoreDelete(s_rxQueue.data_available);
        s_rxQueue.data_available = NULL;
    }

    if (s_rxQueue.mutex != NULL)
    {
        vSemaphoreDelete(s_rxQueue.mutex);
        s_rxQueue.mutex = NULL;
    }

    ResetReceiveQueue(0);
}

void Library_nanoFramework_EspNow_native_nanoFramework_EspNow_EspNowController::DataSentCb(
    const wifi_tx_info_t *tx_info,
    esp_now_send_status_t status)
{
    // ESP-NOW delivery confirmation remains a native callback only. Send()
    // reports the result of esp_now_send() submission synchronously.
    (void)tx_info;
    (void)status;
}

void Library_nanoFramework_EspNow_native_nanoFramework_EspNow_EspNowController::DataRecvCb(
    const esp_now_recv_info *recv_info,
    const uint8_t *incomingData,
    int len)
{
    if (recv_info == NULL || incomingData == NULL || len < 0 || len > (int)NF_ESPNOW_MAX_DATA_LEN
        || s_rxQueue.mutex == NULL)
    {
        return;
    }

    if (xSemaphoreTake(s_rxQueue.mutex, 0) != pdTRUE)
    {
        s_rxQueue.overflow_count++;
        return;
    }

    if (s_rxQueue.count == NF_ESPNOW_RX_QUEUE_DEPTH)
    {
        // head points to the oldest packet, so overwrite it and advance head.
        s_rxQueue.head = (s_rxQueue.head + 1) % NF_ESPNOW_RX_QUEUE_DEPTH;
        s_rxQueue.count--;
        s_rxQueue.overflow_count++;
    }

    uint32_t writeIndex = (s_rxQueue.head + s_rxQueue.count) % NF_ESPNOW_RX_QUEUE_DEPTH;
    EspNowPacket &packet = s_rxQueue.packets[writeIndex];
    memcpy(packet.peer_mac, recv_info->src_addr, ESP_NOW_ETH_ALEN);
    memcpy(packet.data, incomingData, len);
    packet.data_len = (uint16_t)len;
    s_rxQueue.count++;

    xSemaphoreGive(s_rxQueue.mutex);
    xSemaphoreGive(s_rxQueue.data_available);

    // Only signal that data is available. The packet itself is read through
    // NativeReadPacket by managed code.
    PostManagedEvent(EVENT_ESPNOW, 0, EVENT_ESPNOW_DATARECV, 0);
}

HRESULT Library_nanoFramework_EspNow_native_nanoFramework_EspNow_EspNowController::NativeInitialize___I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    esp_err_t ret = esp_now_init();
    if (ret != ESP_OK)
    {
        stack.SetResult_I4(ESPNOW_ERR_ESPNOW_INIT);
        NANOCLR_NOCLEANUP_NOLABEL();
    }

    uint32_t espNowVersion = 1;
    ret = esp_now_get_version(&espNowVersion);
    if (ret != ESP_OK)
    {
        esp_now_deinit();
        stack.SetResult_I4(ESPNOW_ERR_ESPNOW_INIT);
        NANOCLR_NOCLEANUP_NOLABEL();
    }

    uint32_t maximumDataLength = ESP_NOW_MAX_DATA_LEN;
#ifdef ESP_NOW_MAX_DATA_LEN_V2
    if (espNowVersion >= 2)
    {
        maximumDataLength = ESP_NOW_MAX_DATA_LEN_V2;
    }
#endif

    s_rxQueue.mutex = xSemaphoreCreateMutex();
    s_rxQueue.data_available = xSemaphoreCreateBinary();
    if (s_rxQueue.mutex == NULL || s_rxQueue.data_available == NULL)
    {
        DisposeReceiveQueue();
        esp_now_deinit();
        stack.SetResult_I4(ESPNOW_ERR_ESPNOW_INIT);
        NANOCLR_NOCLEANUP_NOLABEL();
    }
    ResetReceiveQueue(maximumDataLength);

    ret = esp_now_register_recv_cb(DataRecvCb);
    if (ret == ESP_OK)
    {
        ret = esp_now_register_send_cb(DataSentCb);
    }

    if (ret != ESP_OK)
    {
        esp_now_unregister_recv_cb();
        esp_now_unregister_send_cb();
        DisposeReceiveQueue();
        esp_now_deinit();
        stack.SetResult_I4(ESPNOW_ERR_ESPNOW_INIT);
        NANOCLR_NOCLEANUP_NOLABEL();
    }

    stack.SetResult_I4(0);
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nanoFramework_EspNow_native_nanoFramework_EspNow_EspNowController::NativeDispose___VOID__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    esp_now_unregister_recv_cb();
    esp_now_unregister_send_cb();
    DisposeReceiveQueue();
    esp_now_deinit();

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nanoFramework_EspNow_native_nanoFramework_EspNow_EspNowController::
    NativeEspNowSend___I4__SZARRAY_U1__SZARRAY_U1__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array *peerMacArg = stack.Arg1().DereferenceArray();
    if (peerMacArg == NULL || peerMacArg->m_numOfElements != ESP_NOW_ETH_ALEN)
    {
        stack.SetResult_I4(ESPNOW_ERR_INVALID_PEER);
        NANOCLR_NOCLEANUP_NOLABEL();
    }

    CLR_RT_HeapBlock_Array *dataArg = stack.Arg2().DereferenceArray();
    if (dataArg == NULL)
    {
        stack.SetResult_I4(ESPNOW_ERR_INVALID_PEER);
        NANOCLR_NOCLEANUP_NOLABEL();
    }

    int32_t dataLen = stack.Arg3().NumericByRef().s4;
    if (dataLen < 0 || dataLen > (int32_t)dataArg->m_numOfElements
        || s_rxQueue.maximum_data_length == 0 || dataLen > (int32_t)s_rxQueue.maximum_data_length)
    {
        stack.SetResult_I4(ESPNOW_ERR_INVALID_PEER);
        NANOCLR_NOCLEANUP_NOLABEL();
    }

    esp_err_t ret = esp_now_send(
        (const uint8_t *)peerMacArg->GetFirstElement(),
        (const uint8_t *)dataArg->GetFirstElement(),
        dataLen);

    stack.SetResult_I4((int32_t)ret);
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nanoFramework_EspNow_native_nanoFramework_EspNow_EspNowController::
    NativeEspNowAddPeer___I4__SZARRAY_U1__U1__BOOLEAN__SZARRAY_U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array *macArray = stack.Arg1().DereferenceArray();
    if (macArray == NULL || macArray->m_numOfElements != ESP_NOW_ETH_ALEN)
    {
        stack.SetResult_I4(ESPNOW_ERR_INVALID_PEER);
        NANOCLR_NOCLEANUP_NOLABEL();
    }

    uint8_t channel = stack.Arg2().NumericByRef().u1;
    bool encrypted = stack.Arg3().NumericByRef().u1 != 0;
    uint8_t *peerMac = (uint8_t *)macArray->GetFirstElement();

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, peerMac, ESP_NOW_ETH_ALEN);
    peerInfo.channel = channel;
    peerInfo.ifidx = WIFI_IF_STA;
    peerInfo.encrypt = encrypted;

    if (encrypted)
    {
        CLR_RT_HeapBlock_Array *localMasterKey = stack.Arg4().DereferenceArray();
        if (localMasterKey == NULL || localMasterKey->m_numOfElements != ESP_NOW_KEY_LEN)
        {
            stack.SetResult_I4(ESPNOW_ERR_INVALID_PEER);
            NANOCLR_NOCLEANUP_NOLABEL();
        }

        memcpy(peerInfo.lmk, localMasterKey->GetFirstElement(), ESP_NOW_KEY_LEN);
    }

    esp_err_t result = esp_now_add_peer(&peerInfo);
    if (result != ESP_OK)
    {
        stack.SetResult_I4(ESPNOW_ERR_ADD_PEER);
        NANOCLR_NOCLEANUP_NOLABEL();
    }

    stack.SetResult_I4(0);
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nanoFramework_EspNow_native_nanoFramework_EspNow_EspNowController::NativeGetMaximumDataLength___I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    stack.SetResult_I4((int32_t)s_rxQueue.maximum_data_length);
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nanoFramework_EspNow_native_nanoFramework_EspNow_EspNowController::NativeReadPacket___I4__SZARRAY_U1__SZARRAY_U1__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array *peerMacArg = stack.Arg1().DereferenceArray();
    CLR_RT_HeapBlock_Array *dataArg = stack.Arg2().DereferenceArray();
    int32_t timeout = stack.Arg3().NumericByRef().s4;

    if (peerMacArg == NULL || peerMacArg->m_numOfElements != ESP_NOW_ETH_ALEN || dataArg == NULL
        || dataArg->m_numOfElements < s_rxQueue.maximum_data_length || timeout < 0)
    {
        stack.SetResult_I4(-ESPNOW_ERR_INVALID_PEER);
        NANOCLR_NOCLEANUP_NOLABEL();
    }

    while (true)
    {
        if (xSemaphoreTake(s_rxQueue.mutex, portMAX_DELAY) == pdTRUE)
        {
            if (s_rxQueue.count > 0)
            {
                EspNowPacket &packet = s_rxQueue.packets[s_rxQueue.head];
                memcpy(peerMacArg->GetFirstElement(), packet.peer_mac, ESP_NOW_ETH_ALEN);
                memcpy(dataArg->GetFirstElement(), packet.data, packet.data_len);
                int32_t dataLength = packet.data_len;
                s_rxQueue.head = (s_rxQueue.head + 1) % NF_ESPNOW_RX_QUEUE_DEPTH;
                s_rxQueue.count--;
                xSemaphoreGive(s_rxQueue.mutex);
                stack.SetResult_I4(dataLength);
                NANOCLR_NOCLEANUP_NOLABEL();
            }

            xSemaphoreGive(s_rxQueue.mutex);
        }

        if (timeout == 0 || xSemaphoreTake(s_rxQueue.data_available, pdMS_TO_TICKS(timeout)) != pdTRUE)
        {
            stack.SetResult_I4(-ESPNOW_ERR_RECEIVE_TIMEOUT);
            NANOCLR_NOCLEANUP_NOLABEL();
        }
    }
}

HRESULT Library_nanoFramework_EspNow_native_nanoFramework_EspNow_EspNowController::
    NativeGetReceiveOverflowCount___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint32_t overflowCount = 0;
    if (s_rxQueue.mutex != NULL && xSemaphoreTake(s_rxQueue.mutex, portMAX_DELAY) == pdTRUE)
    {
        overflowCount = s_rxQueue.overflow_count;
        xSemaphoreGive(s_rxQueue.mutex);
    }

    stack.SetResult_I4((int32_t)overflowCount);
    NANOCLR_NOCLEANUP_NOLABEL();
}
