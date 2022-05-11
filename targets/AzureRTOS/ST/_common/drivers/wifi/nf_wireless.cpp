//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nf_wireless.h>

TX_QUEUE WiFiConnectQueue;
uint32_t WiFiConnectQueueStorage = 0;
TX_MUTEX WiFiMutex;

#define WIFI_CONNECT_THREAD_STACK_SIZE 2048
TX_THREAD wifiConnectThread;
uint32_t wifiConnectThreadStack[WIFI_CONNECT_THREAD_STACK_SIZE / sizeof(uint32_t)];

// flag to store if Wi-Fi has been initialized
static bool NF_WirelessInitialized = false;

__attribute__((noreturn)) void WiFiConnectWorker_entry(uint32_t parameter)
{
    (void)parameter;
    uint32_t wifiRequest;
    uint16_t configIndex;

    // loop until thread receives a request to terminate
    while (1)
    {
        if (tx_queue_receive(&WiFiConnectQueue, &wifiRequest, TX_WAIT_FOREVER) == TX_SUCCESS)
        {
            if ((wifiRequest & WIFI_REQUEST_MASK) == WIFI_REQUEST_CONNECT_STATION)
            {
                // store the config index
                configIndex = (wifiRequest & ~WIFI_REQUEST_MASK);

                // request to connect to a Wi-Fi network
                // sanity check
                if (g_TargetConfiguration.Wireless80211Configs->Count == 0 ||
                    g_TargetConfiguration.Wireless80211Configs->Count < configIndex)
                {
                    // non existing config index
                    continue;
                }

                if (NF_WirelessInitialized &&
                    !NF_Wireless_Start_Connect(g_TargetConfiguration.Wireless80211Configs->Configs[configIndex]))
                {
                    // failed to connect
                    // wait 2 seconds before trying again
                    tx_thread_sleep(TX_TICKS_PER_MILLISEC(2000));

                    // reschedule the request
                    // no need to wait for the queue to be empty neither check for success
                    // if the queue is full, that's because there is a newer request
                    tx_queue_send(&WiFiConnectQueue, &wifiRequest, TX_NO_WAIT);
                }
                else
                {
                    // connect successful
                    // fire notification event
                    NF_ConnectResult = 1;
                    NF_ConnectInProgress = false;

                    // fire event for Wi-Fi station job complete
                    Events_Set(SYSTEM_EVENT_FLAG_WIFI_STATION);

                    Network_PostEvent(
                        NetworkChange_NetworkEventType_AvailabilityChanged,
                        NetworkChange_NetworkEvents_NetworkAvailable,
                        0);
                }
            }

            // pass control to the OS
            tx_thread_sleep(TX_TICKS_PER_MILLISEC(1));
        }
    }

    // this function never returns
}

uint8_t NF_Wireless_InitaliseWifi()
{
    // wifi_mode_t expectedWifiMode = NF_ESP32_CheckExpectedWifiMode();

    // if (IsWifiInitialised)
    // {
    //     // Check if we are running correct mode
    //     if (NF_ESP32_GetCurrentWifiMode() != expectedWifiMode)
    //     {
    //         // if not force a new initialization
    //         NF_ESP32_DeinitWifi();
    //     }
    // }

    // create queue for WiFi connect requests
    if (tx_queue_create(&WiFiConnectQueue, NULL, TX_1_ULONG, &WiFiConnectQueueStorage, 1 * sizeof(ULONG)) != TX_SUCCESS)
    {
        return TX_START_ERROR;
    }

    // create mutex for WiFi API
    if (tx_mutex_create(&WiFiMutex, NULL, TX_NO_INHERIT) != TX_SUCCESS)
    {
        return TX_START_ERROR;
    }

    // create WiFi connect worker thread

    if (tx_thread_create(
            &wifiConnectThread,
            NULL,
            WiFiConnectWorker_entry,
            0,
            wifiConnectThreadStack,
            WIFI_CONNECT_THREAD_STACK_SIZE,
            5,
            5,
            TX_NO_TIME_SLICE,
            TX_AUTO_START) != TX_SUCCESS)
    {
        return TX_START_ERROR;
    }

    if (!NF_WirelessInitialized)
    {
        if (WIFI_Init() == WIFI_STATUS_OK)
        {
            NF_WirelessInitialized = true;
        }
    }

    // // Don't init if Wi-Fi Mode null (Disabled)
    // if (expectedWifiMode == WIFI_MODE_NULL)
    // {
    //     return ESP_FAIL;
    // }

    // if (!IsWifiInitialised)
    // {
    //     // create Wi-Fi STA (ignoring return)
    //     esp_netif_create_default_wifi_sta();

    //     // We need to start the WIFI stack before the station can Connect
    //     // Also we can only get the NetIf number used by ESP IDF after it has been started.
    //     // Starting will also start the Soft- AP (if we have enabled it).
    //     // So make sure it configured as per wireless config otherwise we will get the default SSID
    //     if (expectedWifiMode & WIFI_MODE_AP)
    //     {
    //         // create AP (ignoring return)
    //         esp_netif_create_default_wifi_ap();
    //     }

    //     // Initialise WiFi, allocate resource for WiFi driver, such as WiFi control structure,
    //     // RX/TX buffer, WiFi NVS structure etc, this WiFi also start WiFi task.
    //     wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    //     ec = esp_wifi_init(&cfg);

    //     if (ec != ESP_OK)
    //     {
    //         return ec;
    //     }

    //     // set Wi-Fi mode
    //     ec = esp_wifi_set_mode(expectedWifiMode);
    //     if (ec != ESP_OK)
    //     {
    //         return ec;
    //     }

    //     // start Wi-Fi
    //     ec = esp_wifi_start();
    //     if (ec != ESP_OK)
    //     {
    //         return ec;
    //     }

    //     // if need, config the AP
    //     // this can only be performed after Wi-Fi is started
    //     if (expectedWifiMode & WIFI_MODE_AP)
    //     {
    //         HAL_Configuration_NetworkInterface *networkConfig =
    //             (HAL_Configuration_NetworkInterface *)platform_malloc(sizeof(HAL_Configuration_NetworkInterface));
    //         if (networkConfig == NULL)
    //         {
    //             return ESP_FAIL;
    //         }

    //         if (ConfigurationManager_GetConfigurationBlock(networkConfig, DeviceConfigurationOption_Network, 1))
    //         {
    //             // take care of configuring Soft AP
    //             ec = NF_ESP32_WirelessAP_Configure(networkConfig);

    //             platform_free(networkConfig);
    //         }

    //         if (ec != ESP_OK)
    //         {
    //             return ec;
    //         }
    //     }

    //     IsWifiInitialised = true;
    // }

    return TX_SUCCESS;
}

bool NF_Wireless_Start_Connect(HAL_Configuration_Wireless80211 *config)
{
    WIFI_Ecn_t ecn;

    switch (config->Authentication)
    {
        case AuthenticationType_Open:
            ecn = WIFI_ECN_OPEN;
            break;

        case AuthenticationType_Shared:

        case AuthenticationType_WEP:
            ecn = WIFI_ECN_WEP;
            break;
        case AuthenticationType_WPA:
            ecn = WIFI_ECN_WPA_PSK;
            break;
        case AuthenticationType_WPA2:
            ecn = WIFI_ECN_WPA2_PSK;
            break;

        default:
            return false;
    }

    // better disconnect first
    if (WIFI_IsConnected() == WIFI_STATUS_OK)
    {
        WIFI_Disconnect();
    }

    return WIFI_Connect((const char *)config->Ssid, (const char *)config->Password, ecn) == WIFI_STATUS_OK;
}

int NF_Wireless_Open(HAL_Configuration_NetworkInterface *config)
{
    // esp_err_t ec;
    // bool okToStartSmartConnect = false;
    uint32_t wifiRequest = 0;

    if (NF_Wireless_InitaliseWifi() != TX_SUCCESS)
    {
        return SOCK_SOCKET_ERROR;
    }

    // Get Wireless config
    HAL_Configuration_Wireless80211 *wirelessConfig =
        ConfigurationManager_GetWirelessConfigurationFromId(config->SpecificConfigId);

    if (wirelessConfig == NULL)
    {
        return SOCK_SOCKET_ERROR;
    }

    // check if we have the MAC address stored in the configuration block
    if (config->MacAddress[0] == 0xFF)
    {
        // OK to ignore the return value, no harm done if it fails
        if (WIFI_GetMAC_Address(&config->MacAddress[0]) == WIFI_STATUS_OK)
        {
            // store the MAC address in the configuration block
            ConfigurationManager_UpdateConfigurationBlock(config, DeviceConfigurationOption_Network, 0);
        }
    }

    // // Wireless station not enabled
    // if (!(NF_ESP32_GetCurrentWifiMode() & WIFI_MODE_STA))
    // {
    //     return SOCK_SOCKET_ERROR;
    // }

    // sanity check for Wireless station disabled
    if (wirelessConfig->Options & Wireless80211Configuration_ConfigurationOptions_Disable)
    {
        return SOCK_SOCKET_ERROR;
    }

    // Connect if Auto connect and we have an SSID
    if ((wirelessConfig->Options & Wireless80211Configuration_ConfigurationOptions_AutoConnect) &&
        (hal_strlen_s((const char *)wirelessConfig->Ssid) > 0))
    {
        // request to connect to WiFi
        wifiRequest = (WIFI_REQUEST_CONNECT_STATION | config->SpecificConfigId);
        tx_queue_send(&WiFiConnectQueue, &wifiRequest, TX_NO_WAIT);

        // don't start smart connect
        // okToStartSmartConnect = false;
    }

    // TODO check if we're good with a single interface
    return 0;
}

bool NF_Wireless_Close()
{
    // if (IsWifiInitialised)
    //{
    // wifi_stop();

    // // clear flags
    // IsWifiInitialised = false;
    // NF_ESP32_IsToConnect = false;
    // NF_WirelessInitialized = false;
    //}

    return false;
}
