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

#include "nanoFramework_hardware_esp32_espnow_native.h"

EspNowDataSentEventData Library_nanoFramework_hardware_esp32_espnow_native_nanoFramework_Hardware_Esp32_EspNow_EspNowController::
    dataSentEventData;
EspNowDataRecvEventData Library_nanoFramework_hardware_esp32_espnow_native_nanoFramework_Hardware_Esp32_EspNow_EspNowController::
    dataRecvEventData;


void Library_nanoFramework_hardware_esp32_espnow_native_nanoFramework_Hardware_Esp32_EspNow_EspNowController::
    DataSentCb(const uint8_t *mac_addr, esp_now_send_status_t status) 
{

#ifdef DEBUG_ESPNOW
    {
        char temporaryStringBuffer[64];
        int realStringSize=snprintf(temporaryStringBuffer, sizeof(temporaryStringBuffer), "\r\n***Sending status:\t%d\r\n", status);
        CLR_EE_DBG_EVENT_BROADCAST( CLR_DBG_Commands_c_Monitor_Message, realStringSize, temporaryStringBuffer, WP_Flags_c_NonCritical | WP_Flags_c_NoCaching );
    }
#endif

    memcpy(dataSentEventData.peer_mac, mac_addr, ESP_NOW_ETH_ALEN);
    dataSentEventData.status = status;

#ifdef DEBUG_ESPNOW
    {
        char temporaryStringBuffer[64];
        int realStringSize=snprintf(temporaryStringBuffer, sizeof(temporaryStringBuffer), "\r\n***Posting managed SENT event\r\n");
        CLR_EE_DBG_EVENT_BROADCAST( CLR_DBG_Commands_c_Monitor_Message, realStringSize, temporaryStringBuffer, WP_Flags_c_NonCritical | WP_Flags_c_NoCaching );
    }
#endif

    PostManagedEvent(EVENT_ESP32_ESPNOW, 0, EVENT_ESP32_ESPNOW_DATASENT, (CLR_UINT32)&dataSentEventData);
}

void Library_nanoFramework_hardware_esp32_espnow_native_nanoFramework_Hardware_Esp32_EspNow_EspNowController::
    DataRecvCb(const uint8_t * mac_addr, const uint8_t *incomingData, int len) 
{

#ifdef DEBUG_ESPNOW
    {
      char temporaryStringBuffer[64];
      int realStringSize=snprintf(temporaryStringBuffer, sizeof(temporaryStringBuffer), "\r\n***-> Recv %d bytes from:\t%d\r\n", len, incomingData[0]);
      CLR_EE_DBG_EVENT_BROADCAST( CLR_DBG_Commands_c_Monitor_Message, realStringSize, temporaryStringBuffer, WP_Flags_c_NonCritical | WP_Flags_c_NoCaching );
    }
#endif

    memcpy(dataRecvEventData.peer_mac, mac_addr, ESP_NOW_ETH_ALEN);
    memcpy(dataRecvEventData.data, incomingData, len);
    dataRecvEventData.dataLen = len;

#ifdef DEBUG_ESPNOW
    {
        char temporaryStringBuffer[64];
        int realStringSize=snprintf(temporaryStringBuffer, sizeof(temporaryStringBuffer), "\r\n***Posting managed RECV event\r\n");
        CLR_EE_DBG_EVENT_BROADCAST( CLR_DBG_Commands_c_Monitor_Message, realStringSize, temporaryStringBuffer, WP_Flags_c_NonCritical | WP_Flags_c_NoCaching );
    }
#endif

    PostManagedEvent(EVENT_ESP32_ESPNOW, 0, EVENT_ESP32_ESPNOW_DATARECV, (CLR_UINT32)&dataRecvEventData);

}

HRESULT Library_nanoFramework_hardware_esp32_espnow_native_nanoFramework_Hardware_Esp32_EspNow_EspNowController::
    NativeInitialize___I4( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

#ifdef DEBUG_ESPNOW
    {
      char temporaryStringBuffer[64];
      int realStringSize=snprintf(temporaryStringBuffer, sizeof(temporaryStringBuffer), "\r\n***NativeInitialize entry\r\n");
      CLR_EE_DBG_EVENT_BROADCAST( CLR_DBG_Commands_c_Monitor_Message, realStringSize, temporaryStringBuffer, WP_Flags_c_NonCritical | WP_Flags_c_NoCaching );
    }
#endif

    esp_err_t ret;

    esp_wifi_stop();
    esp_wifi_deinit();
    esp_netif_create_default_wifi_sta();

#ifdef DEBUG_ESPNOW
    {
      char temporaryStringBuffer[64];
      int realStringSize=snprintf(temporaryStringBuffer, sizeof(temporaryStringBuffer), "\r\n***NativeInitialize: WiFi init\r\n");
      CLR_EE_DBG_EVENT_BROADCAST( CLR_DBG_Commands_c_Monitor_Message, realStringSize, temporaryStringBuffer, WP_Flags_c_NonCritical | WP_Flags_c_NoCaching );
    }
#endif

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ret = esp_wifi_init(&cfg);
    if (ret == ESP_OK)
    {

#ifdef DEBUG_ESPNOW
        {
        char temporaryStringBuffer[64];
        int realStringSize=snprintf(temporaryStringBuffer, sizeof(temporaryStringBuffer), "\r\n***NativeInitialize: WiFi mode set\r\n");
        CLR_EE_DBG_EVENT_BROADCAST( CLR_DBG_Commands_c_Monitor_Message, realStringSize, temporaryStringBuffer, WP_Flags_c_NonCritical | WP_Flags_c_NoCaching );
        }
#endif

        ret = esp_wifi_set_mode(WIFI_MODE_STA);
        if (ret == ESP_OK)
        {

#ifdef DEBUG_ESPNOW
            {
            char temporaryStringBuffer[64];
            int realStringSize=snprintf(temporaryStringBuffer, sizeof(temporaryStringBuffer), "\r\n***NativeInitialize: WiFi disconnect\r\n");
            CLR_EE_DBG_EVENT_BROADCAST( CLR_DBG_Commands_c_Monitor_Message, realStringSize, temporaryStringBuffer, WP_Flags_c_NonCritical | WP_Flags_c_NoCaching );
            }
#endif

            ret = esp_wifi_disconnect();
            if (ret == ESP_OK)
            {

#ifdef DEBUG_ESPNOW
                {
                char temporaryStringBuffer[64];
                int realStringSize=snprintf(temporaryStringBuffer, sizeof(temporaryStringBuffer), "\r\n***NativeInitialize: ESPNOW init\r\n");
                CLR_EE_DBG_EVENT_BROADCAST( CLR_DBG_Commands_c_Monitor_Message, realStringSize, temporaryStringBuffer, WP_Flags_c_NonCritical | WP_Flags_c_NoCaching );
                }
#endif

                ret = esp_now_init();
                if (ret == ESP_OK)
                {

#ifdef DEBUG_ESPNOW
                    {
                    char temporaryStringBuffer[64];
                    int realStringSize=snprintf(temporaryStringBuffer, sizeof(temporaryStringBuffer), "\r\n***NativeInitialize: register recvcb\r\n");
                    CLR_EE_DBG_EVENT_BROADCAST( CLR_DBG_Commands_c_Monitor_Message, realStringSize, temporaryStringBuffer, WP_Flags_c_NonCritical | WP_Flags_c_NoCaching );
                    }
#endif

                    ret = esp_now_register_recv_cb(DataRecvCb);
                    if (ret == ESP_OK)
                    {

#ifdef DEBUG_ESPNOW
                        {
                        char temporaryStringBuffer[64];
                        int realStringSize=snprintf(temporaryStringBuffer, sizeof(temporaryStringBuffer), "\r\n***NativeInitialize: register sendcb\r\n");
                        CLR_EE_DBG_EVENT_BROADCAST( CLR_DBG_Commands_c_Monitor_Message, realStringSize, temporaryStringBuffer, WP_Flags_c_NonCritical | WP_Flags_c_NoCaching );
                        }
#endif

                        ret = esp_now_register_send_cb(DataSentCb);
                    }
                }
            }
        }
    }

#ifdef DEBUG_ESPNOW
    {
      char temporaryStringBuffer[64];
      int realStringSize=snprintf(temporaryStringBuffer, sizeof(temporaryStringBuffer), "\r\n***NativeInitialize ret: %d\r\n", ret);
      CLR_EE_DBG_EVENT_BROADCAST( CLR_DBG_Commands_c_Monitor_Message, realStringSize, temporaryStringBuffer, WP_Flags_c_NonCritical | WP_Flags_c_NoCaching );
    }
#endif

    stack.SetResult_I4((int32_t)ret);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nanoFramework_hardware_esp32_espnow_native_nanoFramework_Hardware_Esp32_EspNow_EspNowController::
    NativeDispose___VOID__BOOLEAN( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

#ifdef DEBUG_ESPNOW
    {
      char temporaryStringBuffer[64];
      int realStringSize=snprintf(temporaryStringBuffer, sizeof(temporaryStringBuffer), "\r\n***NativeDispose entry\r\n");
      CLR_EE_DBG_EVENT_BROADCAST( CLR_DBG_Commands_c_Monitor_Message, realStringSize, temporaryStringBuffer, WP_Flags_c_NonCritical | WP_Flags_c_NoCaching );
    }
#endif

    esp_now_unregister_recv_cb();
    esp_now_unregister_send_cb();
    esp_now_deinit();

#ifdef DEBUG_ESPNOW
    {
      char temporaryStringBuffer[64];
      int realStringSize=snprintf(temporaryStringBuffer, sizeof(temporaryStringBuffer), "\r\n***NativeDispose: exit\r\n");
      CLR_EE_DBG_EVENT_BROADCAST( CLR_DBG_Commands_c_Monitor_Message, realStringSize, temporaryStringBuffer, WP_Flags_c_NonCritical | WP_Flags_c_NoCaching );
    }
#endif

    NANOCLR_NOCLEANUP_NOLABEL();
}


HRESULT Library_nanoFramework_hardware_esp32_espnow_native_nanoFramework_Hardware_Esp32_EspNow_EspNowController::
    NativeEspNowSend___I4__SZARRAY_U1__SZARRAY_U1__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#ifdef DEBUG_ESPNOW
    {
      char temporaryStringBuffer[64];
      int realStringSize=snprintf(temporaryStringBuffer, sizeof(temporaryStringBuffer), "\r\n***NativeEspNowSend entry\r\n");
      CLR_EE_DBG_EVENT_BROADCAST( CLR_DBG_Commands_c_Monitor_Message, realStringSize, temporaryStringBuffer, WP_Flags_c_NonCritical | WP_Flags_c_NoCaching );
    }
#endif

    esp_err_t ret;

    CLR_RT_HeapBlock_Array *peerMacArg = stack.Arg1().DereferenceArray();
    char *peerMac = (char *)peerMacArg->GetFirstElement();

    CLR_RT_HeapBlock_Array *dataArg = stack.Arg2().DereferenceArray();
    char *data = (char *)dataArg->GetFirstElement();

    int32_t dataLen = stack.Arg3().NumericByRef().s4;

#ifdef DEBUG_ESPNOW
    {
      char temporaryStringBuffer[64];
      int realStringSize=snprintf(temporaryStringBuffer, sizeof(temporaryStringBuffer), "\r\n***NativeEspNowSend: sending %d bytes\r\n", dataLen);
      CLR_EE_DBG_EVENT_BROADCAST( CLR_DBG_Commands_c_Monitor_Message, realStringSize, temporaryStringBuffer, WP_Flags_c_NonCritical | WP_Flags_c_NoCaching );
    }
#endif

    ret = esp_now_send((const uint8_t *)peerMac, (const uint8_t *)data, dataLen);

#ifdef DEBUG_ESPNOW
    {
      char temporaryStringBuffer[64];
      int realStringSize=snprintf(temporaryStringBuffer, sizeof(temporaryStringBuffer), "\r\n***NativeEspNowSend ret: %d\r\n", ret);
      CLR_EE_DBG_EVENT_BROADCAST( CLR_DBG_Commands_c_Monitor_Message, realStringSize, temporaryStringBuffer, WP_Flags_c_NonCritical | WP_Flags_c_NoCaching );
    }
#endif

    stack.SetResult_I4((int32_t)ret);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nanoFramework_hardware_esp32_espnow_native_nanoFramework_Hardware_Esp32_EspNow_EspNowController::
    NativeEspNowAddPeer___I4__SZARRAY_U1__U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#ifdef DEBUG_ESPNOW
    {
      char temporaryStringBuffer[64];
      int realStringSize=snprintf(temporaryStringBuffer, sizeof(temporaryStringBuffer), "\r\n***NativeEspNowAddPeer entry\r\n");
      CLR_EE_DBG_EVENT_BROADCAST( CLR_DBG_Commands_c_Monitor_Message, realStringSize, temporaryStringBuffer, WP_Flags_c_NonCritical | WP_Flags_c_NoCaching );
    }
#endif

    esp_err_t ret;

    CLR_RT_HeapBlock_Array *peerMacArg = stack.Arg1().DereferenceArray();
    char *peerMac = (char *)peerMacArg->GetFirstElement();

    uint8_t channel = (uint8_t)stack.Arg1().NumericByRef().u1;

    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, peerMac, 6);
    peerInfo.channel = channel;
    peerInfo.encrypt = false;

#ifdef DEBUG_ESPNOW
    {
      char temporaryStringBuffer[64];
      int realStringSize=snprintf(temporaryStringBuffer, sizeof(temporaryStringBuffer), "\r\n***NativeEspNowAddPeer: add_peer\r\n");
      CLR_EE_DBG_EVENT_BROADCAST( CLR_DBG_Commands_c_Monitor_Message, realStringSize, temporaryStringBuffer, WP_Flags_c_NonCritical | WP_Flags_c_NoCaching );
    }
#endif

    ret = esp_now_add_peer(&peerInfo);

#ifdef DEBUG_ESPNOW
    {
      char temporaryStringBuffer[64];
      int realStringSize=snprintf(temporaryStringBuffer, sizeof(temporaryStringBuffer), "\r\n***NativeEspNowAddPeer ret: %d\r\n", ret);
      CLR_EE_DBG_EVENT_BROADCAST( CLR_DBG_Commands_c_Monitor_Message, realStringSize, temporaryStringBuffer, WP_Flags_c_NonCritical | WP_Flags_c_NoCaching );
    }
#endif

    stack.SetResult_I4((int32_t)ret);

    NANOCLR_NOCLEANUP_NOLABEL();
}
