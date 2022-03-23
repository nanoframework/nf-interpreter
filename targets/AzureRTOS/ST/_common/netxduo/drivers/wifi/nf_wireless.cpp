//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nf_wireless.h>

// flag to store if Wi-Fi has been initialized
// static bool IsWifiInitialised = false;

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

    // ec = NF_ESP32_InitaliseWifi();

    // if (ec != ESP_OK)
    // {
    //     return SOCK_SOCKET_ERROR;
    // }

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
        NF_Wireless_Start_Connect(wirelessConfig);

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
