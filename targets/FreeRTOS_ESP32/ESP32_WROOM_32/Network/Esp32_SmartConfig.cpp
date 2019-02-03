//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include "Esp32_os.h"
#include "esp_smartconfig.h"
 
static const int ESPTOUCH_DONE_BIT = BIT1;
static const char *TAG = "sc";

static EventGroupHandle_t sc_wifi_event_group;

static  void sc_callback(smartconfig_status_t status, void *pdata)
{
    switch (status) {
        case SC_STATUS_WAIT:
            ESP_LOGI(TAG, "SC_STATUS_WAIT");
            break;

        case SC_STATUS_FIND_CHANNEL:
            ESP_LOGI(TAG, "SC_STATUS_FINDING_CHANNEL");
            break;

        case SC_STATUS_GETTING_SSID_PSWD:
            ESP_LOGI(TAG, "SC_STATUS_GETTING_SSID_PSWD");
            break;

        case SC_STATUS_LINK:
            ESP_LOGI(TAG, "SC_STATUS_LINK");
            {
                wifi_config_t *wifi_config = (wifi_config_t *)pdata;
                ESP_LOGI(TAG, "SSID:%s", wifi_config->sta.ssid);
                ESP_LOGI(TAG, "PASSWORD:%s", wifi_config->sta.password);

                // Try to connect and Save config
                Network_Interface_Connect(  0,  
                                            (const char *)wifi_config->sta.ssid, 
                                            (const char *)wifi_config->sta.password, 
                                            NETWORK_CONNECT_SAVE_CONFIG + NETWORK_CONNECT_RECONNECT 
                                         );

            }
            break;
  
        case SC_STATUS_LINK_OVER:
            ESP_LOGI(TAG, "SC_STATUS_LINK_OVER");
            if (pdata != NULL) 
            {
                uint8_t phone_ip[4] = { 0 };
                memcpy(phone_ip, (uint8_t* )pdata, 4);
                ESP_LOGI(TAG, "Phone ip: %d.%d.%d.%d\n", phone_ip[0], phone_ip[1], phone_ip[2], phone_ip[3]);
            }
            xEventGroupSetBits(sc_wifi_event_group, ESPTOUCH_DONE_BIT);
            break;

        default:
            break;
    }
}

void  smartconfig_task(void * parm)
{
    EventBits_t uxBits;

    sc_wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_smartconfig_set_type(SC_TYPE_ESPTOUCH) );
    ESP_ERROR_CHECK( esp_smartconfig_start(sc_callback) );
    while (1) {
        uxBits = xEventGroupWaitBits(sc_wifi_event_group, ESPTOUCH_DONE_BIT, true, false, portMAX_DELAY); 
        if(uxBits & ESPTOUCH_DONE_BIT) {
            ESP_LOGI(TAG, "smartconfig over");
            esp_smartconfig_stop();
            vEventGroupDelete(sc_wifi_event_group);
            vTaskDelete(NULL);
        }
    }
}


void Start_wifi_smart_config(void)
{
    xTaskCreate(smartconfig_task, "smartconfig_task", 4096, NULL, 3, NULL);
}


