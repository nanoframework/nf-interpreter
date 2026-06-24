// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

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

#include "nanoFramework_espnow_native.h"

EspNowDataSentEventData Library_nanoFramework_EspNow_native_nanoFramework_EspNow_EspNowController::dataSentEventData;
EspNowDataRecvEventData Library_nanoFramework_EspNow_native_nanoFramework_EspNow_EspNowController::dataRecvEventData;

static const int ESPNOW_ERR_ESPNOW_INIT = 10001;
static const int ESPNOW_ERR_INVALID_PEER = 10002;
static const int ESPNOW_ERR_ADD_PEER = 10003;

void Library_nanoFramework_EspNow_native_nanoFramework_EspNow_EspNowController::DataSentCb(
    const wifi_tx_info_t *tx_info,
    esp_now_send_status_t status)
{
    DEBUG_FENTER();

    DEBUG_WRITELINE("Sending status:\t%d", status);

    memcpy(dataSentEventData.peer_mac, tx_info->des_addr, ESP_NOW_ETH_ALEN);
    dataSentEventData.status = status;

    DEBUG_WRITELINE("Posting managed SENT event");

    PostManagedEvent(EVENT_ESPNOW, 0, EVENT_ESPNOW_DATASENT, (CLR_UINT32)&dataSentEventData);

    DEBUG_FEXIT();
}

void Library_nanoFramework_EspNow_native_nanoFramework_EspNow_EspNowController::DataRecvCb(
    const esp_now_recv_info *recv_info,
    const uint8_t *incomingData,
    int len)
{
    DEBUG_FENTER();

    if (len > 0)
    {
        DEBUG_WRITELINE("Recv %d bytes from[0]:\t%d", len, incomingData[0]);
    }
    else
    {
        DEBUG_WRITELINE("Recv %d bytes", len);
    }

    memcpy(dataRecvEventData.peer_mac, recv_info->src_addr, ESP_NOW_ETH_ALEN);
    memcpy(dataRecvEventData.data, incomingData, len);
    dataRecvEventData.dataLen = len;

    DEBUG_WRITELINE("Posting managed RECV event");

    PostManagedEvent(EVENT_ESPNOW, 0, EVENT_ESPNOW_DATARECV, (CLR_UINT32)&dataRecvEventData);

    DEBUG_FEXIT();
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

    DEBUG_FENTER();

    esp_now_unregister_recv_cb();
    esp_now_unregister_send_cb();
    esp_now_deinit();

    DEBUG_FEXIT();

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nanoFramework_EspNow_native_nanoFramework_EspNow_EspNowController::
    NativeEspNowSend___I4__SZARRAY_U1__SZARRAY_U1__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    DEBUG_FENTER();

    esp_err_t ret;

    CLR_RT_HeapBlock_Array *peerMacArg = stack.Arg1().DereferenceArray();
    char *peerMac = (char *)peerMacArg->GetFirstElement();

    CLR_RT_HeapBlock_Array *dataArg = stack.Arg2().DereferenceArray();
    char *data = (char *)dataArg->GetFirstElement();

    int32_t dataLen = stack.Arg3().NumericByRef().s4;

    DEBUG_WRITELINE(
        "sending %d: bytes to peer mac: %x:%x:%x:%x:%x:%x, data[0]: %x",
        dataLen,
        peerMac[0],
        peerMac[1],
        peerMac[2],
        peerMac[3],
        peerMac[4],
        peerMac[5],
        data[0]);

    ret = esp_now_send((const uint8_t *)peerMac, (const uint8_t *)data, dataLen);

    DEBUG_FEXIT_RET(ret);

    stack.SetResult_I4((int32_t)ret);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nanoFramework_EspNow_native_nanoFramework_EspNow_EspNowController::
    NativeEspNowAddPeer___I4__SZARRAY_U1__U1(CLR_RT_StackFrame &stack)
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
    uint8_t *peerMac = (uint8_t *)macArray->GetFirstElement();

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, peerMac, ESP_NOW_ETH_ALEN);
    peerInfo.channel = channel;
    peerInfo.ifidx = WIFI_IF_STA;
    peerInfo.encrypt = false;

    DEBUG_WRITELINE(
        "add_peer, mac: %x:%x:%x:%x:%x:%x, ch: %d",
        peerMac[0],
        peerMac[1],
        peerMac[2],
        peerMac[3],
        peerMac[4],
        peerMac[5],
        channel);

    esp_err_t result = esp_now_add_peer(&peerInfo);
    if (result != ESP_OK)
    {
        DEBUG_WRITELINE("esp_now_add_peer failed: %d", result);
        stack.SetResult_I4(ESPNOW_ERR_ADD_PEER);
        NANOCLR_NOCLEANUP_NOLABEL();
    }

    stack.SetResult_I4(0);

    NANOCLR_NOCLEANUP_NOLABEL();
}


