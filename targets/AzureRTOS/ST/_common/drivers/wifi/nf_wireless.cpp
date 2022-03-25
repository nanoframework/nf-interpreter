//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nf_wireless.h>

// flag to store if Wi-Fi has been initialized
// static bool IsWifiInitialised = false;

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

    if (WIFI_Init() != WIFI_STATUS_OK)
    {
        return TX_START_ERROR;
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

    return WIFI_Connect((const char *)config->Ssid, (const char *)config->Password, ecn) == WIFI_STATUS_OK;
}

int NF_Wireless_Open(HAL_Configuration_NetworkInterface *config)
{
    // esp_err_t ec;
    // bool okToStartSmartConnect = false;

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
        return NF_Wireless_Start_Connect(wirelessConfig) == true ? 0 : SOCK_SOCKET_ERROR;

        // don't start smart connect
        // okToStartSmartConnect = false;
    }
    else
    {
        // clear flag
        // NF_ESP32_IsToConnect = false;
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
    //}

    return false;
}
