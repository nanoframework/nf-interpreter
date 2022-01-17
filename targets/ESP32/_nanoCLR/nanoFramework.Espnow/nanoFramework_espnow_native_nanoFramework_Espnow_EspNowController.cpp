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

EspNowDataSentEventData Library_nanoFramework_espnow_native_nanoFramework_Espnow_EspNowController::dataSentEventData;
EspNowDataRecvEventData Library_nanoFramework_espnow_native_nanoFramework_Espnow_EspNowController::dataRecvEventData;

void Library_nanoFramework_espnow_native_nanoFramework_Espnow_EspNowController::DataSentCb(
    const uint8_t *mac_addr,
    esp_now_send_status_t status)
{
    DEBUG_FENTER();

    DEBUG_WRITELINE("Sending status:\t%d", status);

    memcpy(dataSentEventData.peer_mac, mac_addr, ESP_NOW_ETH_ALEN);
    dataSentEventData.status = status;

    DEBUG_WRITELINE("Posting managed SENT event");

    PostManagedEvent(EVENT_ESPNOW, 0, EVENT_ESPNOW_DATASENT, (CLR_UINT32)&dataSentEventData);

    DEBUG_FEXIT();
}

void Library_nanoFramework_espnow_native_nanoFramework_Espnow_EspNowController::DataRecvCb(
    const uint8_t *mac_addr,
    const uint8_t *incomingData,
    int len)
{
    DEBUG_FENTER();

    DEBUG_WRITELINE("Recv %d bytes from[0]:\t%d", len, incomingData[0]);

    memcpy(dataRecvEventData.peer_mac, mac_addr, ESP_NOW_ETH_ALEN);
    memcpy(dataRecvEventData.data, incomingData, len);
    dataRecvEventData.dataLen = len;

    DEBUG_WRITELINE("Posting managed RECV event");

    PostManagedEvent(EVENT_ESPNOW, 0, EVENT_ESPNOW_DATARECV, (CLR_UINT32)&dataRecvEventData);

    DEBUG_FEXIT();
}

HRESULT Library_nanoFramework_espnow_native_nanoFramework_Espnow_EspNowController::NativeInitialize___I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    DEBUG_FENTER();

    esp_err_t ret;

    DEBUG_WRITELINE("ESPNOW init");

    ret = esp_now_init();
    if (ret == ESP_OK)
    {

        DEBUG_WRITELINE("ESPNOW reg recvcb");

        ret = esp_now_register_recv_cb(DataRecvCb);
        if (ret == ESP_OK)
        {

            DEBUG_WRITELINE("ESPNOW reg sendcb");

            ret = esp_now_register_send_cb(DataSentCb);
        }
    }

    DEBUG_FEXIT_RET(ret);

    stack.SetResult_I4((int32_t)ret);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nanoFramework_espnow_native_nanoFramework_Espnow_EspNowController::NativeDispose___VOID__BOOLEAN(
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

HRESULT Library_nanoFramework_espnow_native_nanoFramework_Espnow_EspNowController::
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

HRESULT Library_nanoFramework_espnow_native_nanoFramework_Espnow_EspNowController::
    NativeEspNowAddPeer___I4__SZARRAY_U1__U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    DEBUG_FENTER();

    esp_err_t ret;

    CLR_RT_HeapBlock_Array *peerMacArg = stack.Arg1().DereferenceArray();
    char *peerMac = (char *)peerMacArg->GetFirstElement();

    uint8_t channel = (uint8_t)stack.Arg2().NumericByRef().u1;

    esp_now_peer_info_t peerInfo;
    memset((void *)&peerInfo, 0, sizeof(peerInfo));
    memcpy(peerInfo.peer_addr, peerMac, ESP_NOW_ETH_ALEN);
    peerInfo.ifidx = WIFI_IF_STA;
    peerInfo.channel = channel;
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

    ret = esp_now_add_peer(&peerInfo);

    DEBUG_FEXIT_RET(ret);

    stack.SetResult_I4((int32_t)ret);

    NANOCLR_NOCLEANUP_NOLABEL();
}
