//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <esp_smartconfig.h>

static const int ESPTOUCH_DONE_BIT = BIT1;
static const char *TAG = "sc";

static EventGroupHandle_t sc_wifi_event_group;

static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    smartconfig_event_got_ssid_pswd_t *gotSsidAndPswd;
    wifi_config_t wifiConfig;

    // we only care about SC_EVENTs
    if (event_base == SC_EVENT)
    {
        switch (event_id)
        {
            case SC_EVENT_FOUND_CHANNEL:
                ESP_LOGI(TAG, "SC_EVENT_FOUND_CHANNEL");
                break;

            case SC_EVENT_GOT_SSID_PSWD:
                ESP_LOGI(TAG, "SC_EVENT_GOT_SSID_PSWD");
                break;

                // get event data
                gotSsidAndPswd = (smartconfig_event_got_ssid_pswd_t *)event_data;

                // clear memory for wifiConfig
                memset(&wifiConfig, 0, sizeof(wifiConfig));

                // copy from event
                memcpy(wifiConfig.sta.ssid, gotSsidAndPswd->ssid, sizeof(wifiConfig.sta.ssid));
                memcpy(wifiConfig.sta.password, gotSsidAndPswd->password, sizeof(wifiConfig.sta.password));

                ESP_LOGI(TAG, "SSID:%s", wifiConfig.sta.ssid);
                ESP_LOGI(TAG, "PASSWORD:%s", wifiConfig.sta.password);

                // Try to connect and Save config
                Network_Interface_Start_Connect(
                    0,
                    (const char *)wifiConfig.sta.ssid,
                    (const char *)wifiConfig.sta.password,
                    NETWORK_CONNECT_SAVE_CONFIG + NETWORK_CONNECT_RECONNECT);

                // done here
                break;

            case SC_EVENT_SEND_ACK_DONE:
                ESP_LOGI(TAG, "SC_EVENT_SEND_ACK_DONE");

                // set bit to stop smart config task
                xEventGroupSetBits(sc_wifi_event_group, ESPTOUCH_DONE_BIT);

                break;

            case SC_EVENT_SCAN_DONE:
                ESP_LOGI(TAG, "SC_EVENT_SCAN_DONE");
                break;
        }
    }
}

void smartconfig_task(void *parm)
{
    EventBits_t uxBits;

    ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH));

    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_smartconfig_start(&cfg));

    sc_wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));

    while (1)
    {
        uxBits = xEventGroupWaitBits(sc_wifi_event_group, ESPTOUCH_DONE_BIT, true, false, portMAX_DELAY);

        if (uxBits & ESPTOUCH_DONE_BIT)
        {
            ESP_LOGI(TAG, "smartconfig over");

            esp_smartconfig_stop();

            vEventGroupDelete(sc_wifi_event_group);

            vTaskDelete(NULL);
        }
    }
}

void NF_ESP32_Start_wifi_smart_config(void)
{
    xTaskCreate(smartconfig_task, "smartconfig_task", 4096, NULL, 3, NULL);
}
