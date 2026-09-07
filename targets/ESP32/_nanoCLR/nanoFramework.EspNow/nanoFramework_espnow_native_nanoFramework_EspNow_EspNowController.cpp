// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include "nanoFramework_espnow_native.h"

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

// how long the receive callback (WiFi task) and the managed-thread readers are willing to wait
// on the ring buffer mutex - short and bounded on both sides, so neither can stall the other.
static const TickType_t EspNowMutexTimeoutTicks = pdMS_TO_TICKS(20);
static const TickType_t EspNowRecvCbMutexTimeoutTicks = pdMS_TO_TICKS(2);

struct EspNowPacketQueue
{
    EspNowPacket packets[NF_ESPNOW_RX_QUEUE_DEPTH];
    uint32_t head;
    uint32_t count;
    uint32_t overflow_count;
    SemaphoreHandle_t mutex;
    uint32_t maximum_data_length;
    bool initialized;
    bool readPending;
};

static EspNowPacketQueue s_rxQueue = {};

struct EspNowSendStatusLatch
{
    uint8_t peer_mac[ESP_NOW_ETH_ALEN];
    EspNowSendStatus status;
};

static EspNowSendStatusLatch s_lastSendStatus = {};

static void FreeReceiveQueueSlots()
{
    for (uint32_t i = 0; i < NF_ESPNOW_RX_QUEUE_DEPTH; i++)
    {
        if (s_rxQueue.packets[i].data != NULL)
        {
            platform_free(s_rxQueue.packets[i].data);
            s_rxQueue.packets[i].data = NULL;
        }
    }
}

static bool AllocateReceiveQueueSlots(uint32_t maximumDataLength)
{
    for (uint32_t i = 0; i < NF_ESPNOW_RX_QUEUE_DEPTH; i++)
    {
        s_rxQueue.packets[i].data = (uint8_t *)platform_malloc(maximumDataLength);

        if (s_rxQueue.packets[i].data == NULL)
        {
            FreeReceiveQueueSlots();
            return false;
        }
    }

    return true;
}

void Library_nf_espnow_nanoFramework_EspNow_EspNowController::DataSentCb(
    const wifi_tx_info_t *tx_info,
    esp_now_send_status_t status)
{
    if (!s_rxQueue.initialized || tx_info == NULL)
    {
        return;
    }

    GLOBAL_LOCK();
    memcpy(s_lastSendStatus.peer_mac, tx_info->des_addr, ESP_NOW_ETH_ALEN);
    s_lastSendStatus.status = (EspNowSendStatus)status;
    GLOBAL_UNLOCK();

    PostManagedEvent(EVENT_ESPNOW, 0, 0, (uint32_t)EspNowEventType_DataSent);
}

void Library_nf_espnow_nanoFramework_EspNow_EspNowController::DataRecvCb(
    const esp_now_recv_info *recv_info,
    const uint8_t *incomingData,
    int len)
{
    if (!s_rxQueue.initialized || recv_info == NULL || incomingData == NULL || len < 0 ||
        (uint32_t)len > s_rxQueue.maximum_data_length || s_rxQueue.mutex == NULL)
    {
        return;
    }

    if (xSemaphoreTake(s_rxQueue.mutex, EspNowRecvCbMutexTimeoutTicks) != pdTRUE)
    {
        s_rxQueue.overflow_count++;
        return;
    }

    if (!s_rxQueue.initialized)
    {
        // disposed while we were waiting for the mutex
        xSemaphoreGive(s_rxQueue.mutex);
        return;
    }

    bool wasReadPending = s_rxQueue.readPending;

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

    Events_Set(SYSTEM_EVENT_FLAG_ESPNOW);

    // only raise the managed DataReceived notification when nobody is already blocked in a
    // synchronous ReadPacket() call - otherwise the event handler and the blocked reader would
    // race to drain the same packet.
    if (!wasReadPending)
    {
        PostManagedEvent(EVENT_ESPNOW, 0, 0, (uint32_t)EspNowEventType_DataReceived);
    }
}

HRESULT Library_nf_espnow_nanoFramework_EspNow_EspNowController::NativeInitialize___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    if (s_rxQueue.initialized)
    {
        stack.SetResult_I4((int32_t)ESP_ERR_INVALID_STATE);
        NANOCLR_NOCLEANUP_NOLABEL();
    }

    esp_err_t ret = esp_now_init();
    if (ret != ESP_OK)
    {
        stack.SetResult_I4((int32_t)ret);
        NANOCLR_NOCLEANUP_NOLABEL();
    }

    uint32_t espNowVersion = 1;
    ret = esp_now_get_version(&espNowVersion);
    if (ret != ESP_OK)
    {
        esp_now_deinit();
        stack.SetResult_I4((int32_t)ret);
        NANOCLR_NOCLEANUP_NOLABEL();
    }

    uint32_t maximumDataLength = ESP_NOW_MAX_DATA_LEN;
#ifdef ESP_NOW_MAX_DATA_LEN_V2
    if (espNowVersion >= 2)
    {
        maximumDataLength = ESP_NOW_MAX_DATA_LEN_V2;
    }
#endif

    if (!AllocateReceiveQueueSlots(maximumDataLength))
    {
        esp_now_deinit();
        stack.SetResult_I4((int32_t)ESP_ERR_NO_MEM);
        NANOCLR_NOCLEANUP_NOLABEL();
    }

    if (s_rxQueue.mutex == NULL)
    {
        s_rxQueue.mutex = xSemaphoreCreateMutex();
    }

    if (s_rxQueue.mutex == NULL)
    {
        FreeReceiveQueueSlots();
        esp_now_deinit();
        stack.SetResult_I4((int32_t)ESP_ERR_NO_MEM);
        NANOCLR_NOCLEANUP_NOLABEL();
    }

    s_rxQueue.head = 0;
    s_rxQueue.count = 0;
    s_rxQueue.overflow_count = 0;
    s_rxQueue.maximum_data_length = maximumDataLength;
    s_rxQueue.readPending = false;

    memset(&s_lastSendStatus, 0, sizeof(s_lastSendStatus));

    ret = esp_now_register_recv_cb(DataRecvCb);
    if (ret == ESP_OK)
    {
        ret = esp_now_register_send_cb(DataSentCb);
    }

    if (ret != ESP_OK)
    {
        esp_now_unregister_recv_cb();
        esp_now_unregister_send_cb();
        FreeReceiveQueueSlots();
        vSemaphoreDelete(s_rxQueue.mutex);
        s_rxQueue.mutex = NULL;
        esp_now_deinit();
        stack.SetResult_I4((int32_t)ret);
        NANOCLR_NOCLEANUP_NOLABEL();
    }

    s_rxQueue.initialized = true;

    stack.SetResult_I4(0);
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_espnow_nanoFramework_EspNow_EspNowController::NativeDispose___VOID__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    if (s_rxQueue.initialized)
    {
        s_rxQueue.initialized = false;

        Events_Set(SYSTEM_EVENT_FLAG_ESPNOW);

        esp_now_unregister_recv_cb();
        esp_now_unregister_send_cb();
        esp_now_deinit();

        if (s_rxQueue.mutex != NULL && xSemaphoreTake(s_rxQueue.mutex, EspNowMutexTimeoutTicks) == pdTRUE)
        {
            xSemaphoreGive(s_rxQueue.mutex);
        }

        FreeReceiveQueueSlots();

        if (s_rxQueue.mutex != NULL)
        {
            vSemaphoreDelete(s_rxQueue.mutex);
            s_rxQueue.mutex = NULL;
        }

        s_rxQueue.head = 0;
        s_rxQueue.count = 0;
        s_rxQueue.overflow_count = 0;
        s_rxQueue.maximum_data_length = 0;
        s_rxQueue.readPending = false;
    }

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_espnow_nanoFramework_EspNow_EspNowController::NativeEspNowSend___I4__SZARRAY_U1__SZARRAY_U1__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int32_t dataLen;
    esp_err_t ret;

    CLR_RT_HeapBlock_Array *peerMacArg = stack.Arg1().DereferenceArray();
    CLR_RT_HeapBlock_Array *dataArg = stack.Arg2().DereferenceArray();
    dataLen = stack.Arg3().NumericByRef().s4;

    if (peerMacArg == NULL || peerMacArg->m_numOfElements != ESP_NOW_ETH_ALEN || dataArg == NULL || dataLen < 0 ||
        (uint32_t)dataLen > dataArg->m_numOfElements)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    if (!s_rxQueue.initialized)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    ret = esp_now_send(
        (const uint8_t *)peerMacArg->GetFirstElement(),
        (const uint8_t *)dataArg->GetFirstElement(),
        dataLen);

    stack.SetResult_I4((int32_t)ret);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_espnow_nanoFramework_EspNow_EspNowController::
    NativeEspNowAddPeer___I4__SZARRAY_U1__U1__BOOLEAN__SZARRAY_U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint8_t channel;
    bool encrypted;
    uint8_t *peerMac;
    esp_err_t result;
    esp_now_peer_info_t peerInfo = {};

    CLR_RT_HeapBlock_Array *macArray = stack.Arg1().DereferenceArray();

    if (macArray == NULL || macArray->m_numOfElements != ESP_NOW_ETH_ALEN)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    channel = stack.Arg2().NumericByRef().u1;
    encrypted = stack.Arg3().NumericByRef().u1 != 0;
    peerMac = (uint8_t *)macArray->GetFirstElement();

    memcpy(peerInfo.peer_addr, peerMac, ESP_NOW_ETH_ALEN);
    peerInfo.channel = channel;
    peerInfo.ifidx = WIFI_IF_STA;
    peerInfo.encrypt = encrypted;

    if (encrypted)
    {
        CLR_RT_HeapBlock_Array *localMasterKey = stack.Arg4().DereferenceArray();
        if (localMasterKey == NULL || localMasterKey->m_numOfElements != ESP_NOW_KEY_LEN)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        memcpy(peerInfo.lmk, localMasterKey->GetFirstElement(), ESP_NOW_KEY_LEN);
    }

    result = esp_now_add_peer(&peerInfo);

    stack.SetResult_I4((int32_t)result);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_espnow_nanoFramework_EspNow_EspNowController::NativeGetMaximumDataLength___I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    stack.SetResult_I4((int32_t)s_rxQueue.maximum_data_length);
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_espnow_nanoFramework_EspNow_EspNowController::NativeReadPacket___I4__SZARRAY_U1__SZARRAY_U1__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock hbTimeout;
    int64_t *timeoutTicks;
    bool eventResult = true;

    CLR_RT_HeapBlock_Array *peerMacArg = stack.Arg1().DereferenceArray();
    CLR_RT_HeapBlock_Array *dataArg = stack.Arg2().DereferenceArray();
    int32_t timeout = stack.Arg3().NumericByRef().s4;

    if (peerMacArg == NULL || peerMacArg->m_numOfElements != ESP_NOW_ETH_ALEN || dataArg == NULL || timeout < 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    if (!s_rxQueue.initialized || s_rxQueue.mutex == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    hbTimeout.SetInteger((CLR_INT64)timeout * TIME_CONVERSION__TO_MILLISECONDS);
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeoutTicks));

    while (true)
    {
        if (!s_rxQueue.initialized || s_rxQueue.mutex == NULL)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }

        if (xSemaphoreTake(s_rxQueue.mutex, EspNowMutexTimeoutTicks) == pdTRUE)
        {
            if (s_rxQueue.count > 0)
            {
                EspNowPacket &packet = s_rxQueue.packets[s_rxQueue.head];

                if ((uint32_t)dataArg->m_numOfElements < packet.data_len)
                {
                    s_rxQueue.readPending = false;
                    xSemaphoreGive(s_rxQueue.mutex);
                    NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                }

                memcpy(peerMacArg->GetFirstElement(), packet.peer_mac, ESP_NOW_ETH_ALEN);
                memcpy(dataArg->GetFirstElement(), packet.data, packet.data_len);
                int32_t dataLength = (int32_t)packet.data_len;

                s_rxQueue.head = (s_rxQueue.head + 1) % NF_ESPNOW_RX_QUEUE_DEPTH;
                s_rxQueue.count--;
                s_rxQueue.readPending = false;

                xSemaphoreGive(s_rxQueue.mutex);

                stack.SetResult_I4(dataLength);

                // pop the timeout heap block pushed by SetupTimeoutFromTicks
                stack.PopValue();

                NANOCLR_SET_AND_LEAVE(S_OK);
            }

            if (timeout == 0)
            {
                // non-blocking poll and nothing available - not an error
                s_rxQueue.readPending = false;
                xSemaphoreGive(s_rxQueue.mutex);

                stack.SetResult_I4(0);
                stack.PopValue();

                NANOCLR_SET_AND_LEAVE(S_OK);
            }

            // about to block: let DataRecvCb know a blocking read is outstanding, so it skips
            // the managed DataReceived notification for the packet this call is about to pick up
            s_rxQueue.readPending = true;

            xSemaphoreGive(s_rxQueue.mutex);
        }

        Events_Get(SYSTEM_EVENT_FLAG_ESPNOW);

        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeoutTicks, Event_EspNow, eventResult));

        if (!eventResult)
        {
            if (s_rxQueue.mutex != NULL && xSemaphoreTake(s_rxQueue.mutex, EspNowMutexTimeoutTicks) == pdTRUE)
            {
                s_rxQueue.readPending = false;
                xSemaphoreGive(s_rxQueue.mutex);
            }

            NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
        }
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_espnow_nanoFramework_EspNow_EspNowController::NativeGetReceiveOverflowCount___I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint32_t overflowCount = 0;
    if (s_rxQueue.mutex != NULL && xSemaphoreTake(s_rxQueue.mutex, EspNowMutexTimeoutTicks) == pdTRUE)
    {
        overflowCount = s_rxQueue.overflow_count;
        xSemaphoreGive(s_rxQueue.mutex);
    }

    stack.SetResult_I4((int32_t)overflowCount);
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_espnow_nanoFramework_EspNow_EspNowController::NativeReadSendStatus___I4__SZARRAY_U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array *peerMacArg = stack.Arg1().DereferenceArray();
    if (peerMacArg == NULL || peerMacArg->m_numOfElements != ESP_NOW_ETH_ALEN)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    EspNowSendStatus status;

    GLOBAL_LOCK();
    memcpy(peerMacArg->GetFirstElement(), s_lastSendStatus.peer_mac, ESP_NOW_ETH_ALEN);
    status = s_lastSendStatus.status;
    GLOBAL_UNLOCK();

    stack.SetResult_I4((int32_t)status);

    NANOCLR_NOCLEANUP();
}
