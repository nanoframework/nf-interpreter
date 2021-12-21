//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// This file includes the board specific Ethernet Initialisation

#include "NF_ESP32_Network.h"
#include "esp_netif_net_stack.h"

static const char *TAG = "wifi";

static wifi_mode_t wifiMode;

// flag to store if Wi-Fi has been initialized
static bool IsWifiInitialised = false;
// flag to signal if connect is to happen
bool NF_ESP32_IsToConnect = false;

//
//  Check what is the required Wi-Fi mode
//  Station only or AP only or Station and AP
//
//  See what network interfaces are enabled
//
wifi_mode_t NF_ESP32_CheckExpectedWifiMode()
{
    wifi_mode_t mode = WIFI_MODE_NULL;

    HAL_Configuration_Wireless80211 *wirelessConfig = NULL;
    HAL_Configuration_WirelessAP *wirelessAPConfig = NULL;

    HAL_Configuration_NetworkInterface *networkConfig =
        (HAL_Configuration_NetworkInterface *)platform_malloc(sizeof(HAL_Configuration_NetworkInterface));

    // check allocation
    if (networkConfig == NULL)
    {
        return WIFI_MODE_NULL;
    }

    // Check Wi-Fi station available
    if (g_TargetConfiguration.NetworkInterfaceConfigs->Count >= 1)
    {
        // get config Index 0 (Wireless station config in ESP32)
        if (ConfigurationManager_GetConfigurationBlock(networkConfig, DeviceConfigurationOption_Network, 0))
        {
            // Wireless Config with SSID setup
            if (networkConfig->InterfaceType == NetworkInterfaceType::NetworkInterfaceType_Wireless80211)
            {
                wirelessConfig = ConfigurationManager_GetWirelessConfigurationFromId(networkConfig->SpecificConfigId);

                if (wirelessConfig != NULL)
                {
                    if (wirelessConfig->Options & Wireless80211Configuration_ConfigurationOptions_Enable)
                    {
                        mode = WIFI_MODE_STA;
                    }
                }
            }
        }
    }

    // Check if AP config available
    if (g_TargetConfiguration.NetworkInterfaceConfigs->Count >= 2)
    {
        // get config Index 1 (Wireless AP config in ESP32)
        if (ConfigurationManager_GetConfigurationBlock(networkConfig, DeviceConfigurationOption_Network, 1))
        {
            // Wireless Config with SSID setup
            if (networkConfig->InterfaceType == NetworkInterfaceType::NetworkInterfaceType_WirelessAP)
            {
                wirelessAPConfig =
                    ConfigurationManager_GetWirelessAPConfigurationFromId(networkConfig->SpecificConfigId);

                if (wirelessAPConfig != NULL)
                {
                    if (wirelessAPConfig->Options & WirelessAPConfiguration_ConfigurationOptions_Enable)
                    {
                        // Use STATION + AP or just AP
                        mode = (mode == WIFI_MODE_STA) ? WIFI_MODE_APSTA : WIFI_MODE_AP;
                    }
                }
            }
        }
    }

    // this one is always set
    platform_free(networkConfig);

    // free this one, if it was allocated
    if (wirelessConfig)
    {
        platform_free(wirelessConfig);
    }

    return mode;
}

wifi_mode_t NF_ESP32_GetCurrentWifiMode()
{
    wifi_mode_t current_wifi_mode;

    esp_wifi_get_mode(&current_wifi_mode);

    return current_wifi_mode;
}

void NF_ESP32_DeinitWifi()
{
    // clear flags
    IsWifiInitialised = false;
    NF_ESP32_IsToConnect = false;

    esp_wifi_stop();

    esp_wifi_deinit();
}

esp_err_t NF_ESP32_InitaliseWifi()
{
    esp_err_t ec = ESP_OK;

    wifi_mode_t expectedWifiMode = NF_ESP32_CheckExpectedWifiMode();

    if (IsWifiInitialised)
    {
        // Check if we are running correct mode
        if (NF_ESP32_GetCurrentWifiMode() != expectedWifiMode)
        {
            // if not force a new initialization
            NF_ESP32_DeinitWifi();
        }
    }

    // Don't init if Wi-Fi Mode null (Disabled)
    if (expectedWifiMode == WIFI_MODE_NULL)
    {
        return ESP_FAIL;
    }

    if (!IsWifiInitialised)
    {
        // create Wi-Fi STA (ignoring return)
        esp_netif_create_default_wifi_sta();

        // We need to start the WIFI stack before the station can Connect
        // Also we can only get the NetIf number used by ESP IDF after it has been started.
        // Starting will also start the Soft- AP (if we have enabled it).
        // So make sure it configured as per wireless config otherwise we will get the default SSID
        if (expectedWifiMode & WIFI_MODE_AP)
        {
            // create AP (ignoring return)
            esp_netif_create_default_wifi_ap();
        }

        // Initialise WiFi, allocate resource for WiFi driver, such as WiFi control structure,
        // RX/TX buffer, WiFi NVS structure etc, this WiFi also start WiFi task.
        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        ec = esp_wifi_init(&cfg);

        if (ec != ESP_OK)
        {
            return ec;
        }

        // set Wi-Fi mode
        ec = esp_wifi_set_mode(expectedWifiMode);
        if (ec != ESP_OK)
        {
            return ec;
        }

        // start Wi-Fi
        ec = esp_wifi_start();
        if (ec != ESP_OK)
        {
            return ec;
        }

        // if need, config the AP
        // this can only be performed after Wi-Fi is started
        if (expectedWifiMode & WIFI_MODE_AP)
        {
            HAL_Configuration_NetworkInterface *networkConfig =
                (HAL_Configuration_NetworkInterface *)platform_malloc(sizeof(HAL_Configuration_NetworkInterface));
            if (networkConfig == NULL)
            {
                return ESP_FAIL;
            }

            if (ConfigurationManager_GetConfigurationBlock(networkConfig, DeviceConfigurationOption_Network, 1))
            {
                // take care of configuring Soft AP
                ec = NF_ESP32_WirelessAP_Configure(networkConfig);

                platform_free(networkConfig);
            }

            if (ec != ESP_OK)
            {
                return ec;
            }
        }

        IsWifiInitialised = true;
    }

    return ec;
}

esp_err_t NF_ESP32_Wireless_Start_Connect(HAL_Configuration_Wireless80211 *config)
{
    esp_err_t ec;

    // Connect directly
    wifi_config_t sta_config = {};

    hal_strncpy_s(
        (char *)sta_config.sta.ssid,
        sizeof(sta_config.sta.ssid),
        (char *)config->Ssid,
        hal_strlen_s((char *)config->Ssid));

    hal_strncpy_s(
        (char *)sta_config.sta.password,
        sizeof(sta_config.sta.password),
        (char *)config->Password,
        hal_strlen_s((char *)config->Password));

    sta_config.sta.bssid_set = false;

    ec = esp_wifi_set_config(WIFI_IF_STA, &sta_config);
    if (ec != ESP_OK)
    {
        return ec;
    }

    // set flag
    NF_ESP32_IsToConnect = true;

    // call disconnect to be sure that connect event is raised again
    esp_wifi_disconnect();

    // call connect
    esp_wifi_connect();

    return ESP_OK;
}

esp_err_t NF_ESP32_Wireless_Disconnect()
{
    esp_err_t ec;

    ec = esp_wifi_disconnect();

    if (ec != ESP_OK)
    {
        return ec;
    }

    return ESP_OK;
}

int NF_ESP32_Wireless_Open(HAL_Configuration_NetworkInterface *config)
{
    esp_err_t ec;
    bool okToStartSmartConnect = false;

    ec = NF_ESP32_InitaliseWifi();

    if (ec != ESP_OK)
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

    // Wireless station not enabled
    if (!(NF_ESP32_GetCurrentWifiMode() & WIFI_MODE_STA))
    {
        return SOCK_SOCKET_ERROR;
    }

    // sanity check for Wireless station disabled
    if (wirelessConfig->Options & Wireless80211Configuration_ConfigurationOptions_Disable)
    {
        return SOCK_SOCKET_ERROR;
    }

    // Connect if Auto connect and we have an SSID
    if ((wirelessConfig->Options & Wireless80211Configuration_ConfigurationOptions_AutoConnect) &&
        (hal_strlen_s((const char *)wirelessConfig->Ssid) > 0))
    {
        NF_ESP32_Wireless_Start_Connect(wirelessConfig);

        // don't start smart connect
        okToStartSmartConnect = false;
    }
    else
    {
        // clear flag
        NF_ESP32_IsToConnect = false;
    }

    if (okToStartSmartConnect &&
        (wirelessConfig->Options & Wireless80211Configuration_ConfigurationOptions_SmartConfig))
    {
        // Start Smart config (if enabled)
        NF_ESP32_Start_wifi_smart_config();

        // clear flag
        NF_ESP32_IsToConnect = false;
    }

    return NF_ESP32_Wait_NetNumber(IDF_WIFI_STA_DEF);
}

bool NF_ESP32_Wireless_Close()
{
    if (IsWifiInitialised)
    {
        esp_wifi_stop();
        esp_wifi_deinit();

        // clear flags
        IsWifiInitialised = false;
        NF_ESP32_IsToConnect = false;
    }

    return false;
}

// Start a scan
int NF_ESP32_Wireless_Scan()
{
    wifi_scan_config_t config = {};

    config.scan_type = WIFI_SCAN_TYPE_PASSIVE;
    // 500ms
    config.scan_time.passive = 500;

    // Start a Wi-Fi scan
    // When complete a Scan Complete event will be fired
    esp_err_t res = esp_wifi_scan_start(&config, false);

    return (int)res;
}

wifi_auth_mode_t MapAuthentication(AuthenticationType type)
{
    wifi_auth_mode_t mapAuth[] = {
        WIFI_AUTH_OPEN,        // 0 None
        WIFI_AUTH_OPEN,        // 1 EAP
        WIFI_AUTH_OPEN,        // 2 PEAP
        WIFI_AUTH_OPEN,        // 3 WCN
        WIFI_AUTH_OPEN,        // 4 Open
        WIFI_AUTH_OPEN,        // 5 Shared
        WIFI_AUTH_WEP,         // 6 WEP
        WIFI_AUTH_WPA_PSK,     // 7 WPA
        WIFI_AUTH_WPA_WPA2_PSK // 8 WPA2
    };

    return mapAuth[type];
}

esp_err_t NF_ESP32_WirelessAP_Configure(HAL_Configuration_NetworkInterface *config)
{
    esp_err_t ec;
    esp_netif_ip_info_t ip_info;

    esp_netif_t *espNetif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");

    ec = esp_netif_get_ip_info(espNetif, &ip_info);

    if (config->IPv4Address != 0)
    {
        ip_info.ip.addr = config->IPv4Address;
        ip_info.netmask.addr = config->IPv4NetMask;
        ip_info.gw.addr = config->IPv4GatewayAddress;

        ec = esp_netif_set_ip_info(espNetif, &ip_info);
    }
    else
    {
        config->IPv4Address = ip_info.ip.addr;
        config->IPv4NetMask = ip_info.netmask.addr;
        config->IPv4GatewayAddress = ip_info.gw.addr;
    }

    HAL_Configuration_WirelessAP *apConfig =
        ConfigurationManager_GetWirelessAPConfigurationFromId(config->SpecificConfigId);

    if (apConfig == 0)
    {
        return ESP_FAIL;
    }

    wifi_config_t ap_config = {0};

    hal_strncpy_s(
        (char *)ap_config.ap.ssid,
        sizeof(ap_config.ap.ssid),
        (char *)apConfig->Ssid,
        hal_strlen_s((char *)apConfig->Ssid));

    hal_strncpy_s(
        (char *)ap_config.ap.password,
        sizeof(ap_config.ap.password),
        (char *)apConfig->Password,
        hal_strlen_s((char *)apConfig->Password));

    ap_config.ap.ssid_len = hal_strlen_s((char *)apConfig->Ssid);
    ap_config.ap.channel = apConfig->Channel;
    ap_config.ap.ssid_hidden = (apConfig->Options & WirelessAPConfiguration_ConfigurationOptions_HiddenSSID) ? 1 : 0;
    ap_config.ap.authmode = MapAuthentication(apConfig->Authentication);

    if (hal_strlen_s((char *)ap_config.ap.password) == 0)
    {
        ap_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    // Max connections for ESP32
    ap_config.ap.max_connection = apConfig->MaxConnections;

    if (ap_config.ap.max_connection > ESP_WIFI_MAX_CONN_NUM)
    {
        ap_config.ap.max_connection = ESP_WIFI_MAX_CONN_NUM;
    }

    ap_config.ap.beacon_interval = 100;

    ec = esp_wifi_set_config(WIFI_IF_AP, &ap_config);
    if (ec != ESP_OK)
    {
        ESP_LOGE(TAG, "WiFi set AP config - result %d", ec);
    }

    return ec;
}

//
//	Open Wireless Soft AP
//
//  If AP is enabled it will have been configured and started running when the WiFI stack is initialised
//  All we need to do here is return the NetIf number used by ESP IDF
//  Also make sure Wi-Fi in initialised correctly if config is changed
int IRAM_ATTR NF_ESP32_WirelessAP_Open(HAL_Configuration_NetworkInterface *config)
{
    esp_err_t ec;

    // Initialise Wi-Fi stack if required
    ec = NF_ESP32_InitaliseWifi();

    if (ec != ESP_OK)
    {
        return SOCK_SOCKET_ERROR;
    }

    // AP mode enabled ?
    if (!(NF_ESP32_GetCurrentWifiMode() & WIFI_MODE_AP))
    {
        return SOCK_SOCKET_ERROR;
    }

    // Return NetIf number
    // FIXME find a better way to get the netif ptr
    // This becomes available on the event AP STARTED
    // for the moment we just wait for it
    return NF_ESP32_Wait_NetNumber(IDF_WIFI_AP_DEF);
}

//
//  Closing down AP
//
//  	Either config being updated  or shutdown
//  	Closing AP will also stop Station
//
bool NF_ESP32_WirelessAP_Close()
{
    NF_ESP32_DeinitWifi();

    return true;
}

// Wait for the network interface to become available
int NF_ESP32_Wait_NetNumber(int num)
{
    int number = 0;

    esp_netif_t *espNetif;

    while (true)
    {
        switch (num)
        {
            case IDF_WIFI_STA_DEF:
                espNetif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
                break;

            case IDF_WIFI_AP_DEF:
                espNetif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");
                break;

            case IDF_ETH_DEF:
                espNetif = esp_netif_get_handle_from_ifkey("ETH_DEF");
                break;

            default:
                // can't reach here
                HAL_AssertEx();
                break;
        }

        if (espNetif != NULL)
        {
            break;
        }

        vTaskDelay(20 / portTICK_PERIOD_MS);
    }

    return espNetif->lwip_netif->num;
}
