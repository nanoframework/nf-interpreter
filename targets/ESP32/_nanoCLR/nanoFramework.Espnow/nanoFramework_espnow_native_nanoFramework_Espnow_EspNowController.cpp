// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nanoFramework_espnow_native.h"

static const int EVENT_ESPNOW_DATASENT = 1;
static const int EVENT_ESPNOW_DATARECV = 2;
static const int ESPNOW_ERR_ESPNOW_INIT = 10001;
static const int ESPNOW_ERR_INVALID_PEER = 10002;
static const int ESPNOW_ERR_ADD_PEER = 10003;

void Library_nanoFramework_EspNow_native_nanoFramework_EspNow_EspNowController::DataSentCb(
    const wifi_tx_info_t *tx_info,
    esp_now_send_status_t status)
{
    EspNowDataSentEventData *dataSentEventData = (EspNowDataSentEventData *)platform_malloc(sizeof(EspNowDataSentEventData));
    if (dataSentEventData == NULL)
    {
        return;
    }

    memcpy(dataSentEventData->peer_mac, tx_info->des_addr, ESP_NOW_ETH_ALEN);
    dataSentEventData->status = status;

    PostManagedEvent(EVENT_ESPNOW, 0, EVENT_ESPNOW_DATASENT, (CLR_UINT32)dataSentEventData);
}

void Library_nanoFramework_EspNow_native_nanoFramework_EspNow_EspNowController::DataRecvCb(
    const esp_now_recv_info *recv_info,
    const uint8_t *incomingData,
    int len)
{
    EspNowDataRecvEventData *dataRecvEventData = (EspNowDataRecvEventData *)platform_malloc(sizeof(EspNowDataRecvEventData));
    if (dataRecvEventData == NULL)
    {
        return;
    }

    memcpy(dataRecvEventData->peer_mac, recv_info->src_addr, ESP_NOW_ETH_ALEN);
    memcpy(dataRecvEventData->data, incomingData, len);
    dataRecvEventData->dataLen = len;

    PostManagedEvent(EVENT_ESPNOW, 0, EVENT_ESPNOW_DATARECV, (CLR_UINT32)dataRecvEventData);
}

HRESULT Library_nanoFramework_EspNow_native_nanoFramework_EspNow_EspNowController::NativeInitialize___I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    esp_err_t ret;

    ret = esp_now_init();
    if (ret != ESP_OK)
    {
        stack.SetResult_I4(ESPNOW_ERR_ESPNOW_INIT);
        NANOCLR_NOCLEANUP_NOLABEL();
    }

    ret = esp_now_register_recv_cb(DataRecvCb);
    if (ret == ESP_OK)
    {
        ret = esp_now_register_send_cb(DataSentCb);
    }

    if (ret != ESP_OK)
    {
        esp_now_unregister_recv_cb();
        esp_now_unregister_send_cb();
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
    esp_now_deinit();

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nanoFramework_EspNow_native_nanoFramework_EspNow_EspNowController::
    NativeEspNowSend___I4__SZARRAY_U1__SZARRAY_U1__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    esp_err_t ret;

    CLR_RT_HeapBlock_Array *peerMacArg = stack.Arg1().DereferenceArray();
    if (peerMacArg == NULL || peerMacArg->m_numOfElements != ESP_NOW_ETH_ALEN)
    {
        stack.SetResult_I4(ESPNOW_ERR_INVALID_PEER);
        NANOCLR_NOCLEANUP_NOLABEL();
    }

    char *peerMac = (char *)peerMacArg->GetFirstElement();

    CLR_RT_HeapBlock_Array *dataArg = stack.Arg2().DereferenceArray();
    if (dataArg == NULL)
    {
        stack.SetResult_I4(ESPNOW_ERR_INVALID_PEER);
        NANOCLR_NOCLEANUP_NOLABEL();
    }

    int32_t dataLen = stack.Arg3().NumericByRef().s4;
    if (dataLen < 0 || dataLen > (int32_t)dataArg->m_numOfElements)
    {
        stack.SetResult_I4(ESPNOW_ERR_INVALID_PEER);
        NANOCLR_NOCLEANUP_NOLABEL();
    }

    char *data = (char *)dataArg->GetFirstElement();

    ret = esp_now_send((const uint8_t *)peerMac, (const uint8_t *)data, dataLen);

    stack.SetResult_I4((int32_t)ret);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nanoFramework_EspNow_native_nanoFramework_EspNow_EspNowController::
    NativeEspNowAddPeer___I4__SZARRAY_U1__U1__BOOLEAN__SZARRAY_U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array *macArray = stack.Arg1().DereferenceArray();
    if (macArray == NULL)
    {
        stack.SetResult_I4(ESPNOW_ERR_INVALID_PEER);
        NANOCLR_NOCLEANUP_NOLABEL();
    }

    if (macArray->m_numOfElements != ESP_NOW_ETH_ALEN)
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


