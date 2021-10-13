
//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "NF_ESP32_Network.h"
#include <esp32_ethernet_options.h>

//
// Works with the Target_NetworkConfig to map the Network_Interface_XXXXX calls to the correct driver

bool NF_ESP32_ConnectInProgress = false;
int NF_ESP32_ConnectResult = 0;

bool StoreConfigBlock(
    DeviceConfigurationOption configType,
    uint32_t configurationIndex,
    void *pConfigBlock,
    size_t writeSize);

bool Network_Interface_Bind(int index)
{
    (void)index;

    return true;
}

int Network_Interface_Open(int index)
{
    HAL_Configuration_NetworkInterface networkConfiguration;

    // load network interface configuration from storage
    if (!ConfigurationManager_GetConfigurationBlock(
            (void *)&networkConfiguration,
            DeviceConfigurationOption_Network,
            index))
    {
        // failed to load configuration
        // FIXME output error?
        return SOCK_SOCKET_ERROR;
    }
    _ASSERTE(networkConfiguration.StartupAddressMode > 0);

    switch (networkConfiguration.InterfaceType)
    {
        // Wi-Fi (STA)
        case NetworkInterfaceType_Wireless80211:
            return NF_ESP32_Wireless_Open(&networkConfiguration);

        // Wi-Fi (Soft AP)
        case NetworkInterfaceType_WirelessAP:
            return NF_ESP32_WirelessAP_Open(&networkConfiguration);

#ifdef ESP32_ETHERNET_SUPPORT
        // Ethernet
        case NetworkInterfaceType_Ethernet:
            return NF_ESP32_Ethernet_Open(&networkConfiguration);
#endif
    }

    return SOCK_SOCKET_ERROR;
}

bool Network_Interface_Close(int index)
{
    HAL_Configuration_NetworkInterface networkConfiguration;

    // load network interface configuration from storage
    if (!ConfigurationManager_GetConfigurationBlock(
            (void *)&networkConfiguration,
            DeviceConfigurationOption_Network,
            index))
    {
        // failed to load configuration
        // FIXME output error?
        return SOCK_SOCKET_ERROR;
    }

    switch (networkConfiguration.InterfaceType)
    {
        // Wireless
        case NetworkInterfaceType_Wireless80211:
            return NF_ESP32_Wireless_Close();

        // Soft AP
        case NetworkInterfaceType_WirelessAP:
            return NF_ESP32_WirelessAP_Close();

#ifdef ESP32_ETHERNET_SUPPORT
        // Ethernet
        case NetworkInterfaceType_Ethernet:
            return NF_ESP32_Ethernet_Close();
#endif
    }

    return false;
}

bool Network_Interface_Start_Scan(int index)
{
    HAL_Configuration_NetworkInterface networkConfiguration;

    // load network interface configuration from storage
    if (!ConfigurationManager_GetConfigurationBlock(
            (void *)&networkConfiguration,
            DeviceConfigurationOption_Network,
            index))
    {
        // failed to load configuration
        // FIXME output error?
        return SOCK_SOCKET_ERROR;
    }

    // can only do this is this is STA
    if (networkConfiguration.InterfaceType == NetworkInterfaceType_Wireless80211)
    {
        return (NF_ESP32_Wireless_Scan() == 0);
    }

    return false;
}

bool GetWirelessConfig(int index, HAL_Configuration_Wireless80211 **wirelessConfig)
{
    // Check index in range
    if (g_TargetConfiguration.NetworkInterfaceConfigs->Count <= index)
    {
        return false;
    }

    HAL_Configuration_NetworkInterface *networkConfiguration =
        (HAL_Configuration_NetworkInterface *)platform_malloc(sizeof(HAL_Configuration_NetworkInterface));

    // check allocation
    if (networkConfiguration)
    {
        // load network interface configuration from storage
        if (ConfigurationManager_GetConfigurationBlock(networkConfiguration, DeviceConfigurationOption_Network, index))
        {
            if (networkConfiguration->InterfaceType == NetworkInterfaceType_Wireless80211)
            {
                *wirelessConfig =
                    (HAL_Configuration_Wireless80211 *)platform_malloc(sizeof(HAL_Configuration_Wireless80211));

                // check allocation
                if (wirelessConfig)
                {
                    if (ConfigurationManager_GetConfigurationBlock(
                            *wirelessConfig,
                            DeviceConfigurationOption_Wireless80211Network,
                            networkConfiguration->SpecificConfigId))
                    {
                        // found and loaded
                        return true;
                    }
                }
            }
        }

        platform_free(networkConfiguration);
    }

    return false;
}

//
//  Connect to wireless network SSID using passphrase
//
int Network_Interface_Start_Connect(int index, const char *ssid, const char *passphase, int options)
{
    // need to free memory, if allocated in successful GetWirelessConfig
    HAL_Configuration_Wireless80211 *wirelessConfig = NULL;

    if (GetWirelessConfig(index, &wirelessConfig) == false)
    {
        if (wirelessConfig != NULL)
        {
            platform_free(wirelessConfig);
        }

        return SOCK_SOCKET_ERROR;
    }

    wirelessConfig->Options = Wireless80211Configuration_ConfigurationOptions_Enable;

    if (options & NETWORK_CONNECT_RECONNECT)
    {
        // need this stupid cast because the current gcc version with ESP32 IDF is not happy with the simple syntax '|='
        wirelessConfig->Options =
            (Wireless80211Configuration_ConfigurationOptions)(wirelessConfig->Options | Wireless80211Configuration_ConfigurationOptions_AutoConnect);
    }
    else
    {
        // need this stupid cast because the current gcc version with ESP32 IDF is not happy with the simple syntax '^='
        wirelessConfig->Options =
            (Wireless80211Configuration_ConfigurationOptions)(wirelessConfig->Options ^ Wireless80211Configuration_ConfigurationOptions_AutoConnect);

        // Make sure we are still enabled because AutoConnect includes Enable
        wirelessConfig->Options =
            (Wireless80211Configuration_ConfigurationOptions)(wirelessConfig->Options | Wireless80211Configuration_ConfigurationOptions_Enable);
    }

    // Update Wireless structure with new SSID and passphase
    hal_strcpy_s((char *)wirelessConfig->Ssid, sizeof(wirelessConfig->Ssid), ssid);
    hal_strcpy_s((char *)wirelessConfig->Password, sizeof(wirelessConfig->Password), passphase);

    // Option to Save new config
    if (options & NETWORK_CONNECT_SAVE_CONFIG)
    {
        StoreConfigBlock(
            DeviceConfigurationOption_Wireless80211Network,
            index,
            wirelessConfig,
            sizeof(HAL_Configuration_Wireless80211));
    }

    NF_ESP32_ConnectInProgress = true;
    esp_err_t err = NF_ESP32_Wireless_Start_Connect(wirelessConfig);

    platform_free(wirelessConfig);

    return (int)err;
}

int Network_Interface_Connect_Result(int index)
{
    HAL_Configuration_NetworkInterface networkConfiguration;

    // load network interface configuration from storage
    if (!ConfigurationManager_GetConfigurationBlock(
            (void *)&networkConfiguration,
            DeviceConfigurationOption_Network,
            index))
    {
        // failed to load configuration
        // FIXME output error?
        return SOCK_SOCKET_ERROR;
    }

    if (networkConfiguration.InterfaceType == NetworkInterfaceType_Wireless80211)
    {
        return NF_ESP32_ConnectInProgress ? -1 : NF_ESP32_ConnectResult;
    }

    return SOCK_SOCKET_ERROR;
}

int Network_Interface_Disconnect(int index)
{
    HAL_Configuration_NetworkInterface networkConfiguration;

    // load network interface configuration from storage
    if (!ConfigurationManager_GetConfigurationBlock(
            (void *)&networkConfiguration,
            DeviceConfigurationOption_Network,
            index))
    {
        // failed to load configuration
        // FIXME output error?
        return SOCK_SOCKET_ERROR;
    }

    if (networkConfiguration.InterfaceType == NetworkInterfaceType_Wireless80211)
    {
        esp_err_t err = NF_ESP32_Wireless_Disconnect();

        return (err == ESP_OK);
    }

    return false;
}

wifi_sta_info_t wireless_sta[ESP_WIFI_MAX_CONN_NUM] = {0};

//
//	Update save stations with rssi
//
void Network_Interface_update_Stations()
{
    esp_err_t ec;
    wifi_sta_list_t stations;

    ec = esp_wifi_ap_get_sta_list(&stations);

    if (ec == ESP_OK)
    {
        // Find save station and update
        for (int x = 0; x < stations.num; x++)
        {
            for (int y = 0; y < ESP_WIFI_MAX_CONN_NUM; y++)
            {
                if (wireless_sta[y].reserved)
                {
                    if (memcmp(wireless_sta[y].mac, stations.sta[x].mac, 6) == 0)
                    {
                        memcpy(&wireless_sta[y], &stations.sta[x], sizeof(wifi_sta_info_t));
                        wireless_sta[y].reserved = 1;
                        break;
                    }
                }
            }
        }
    }
}

//
//	Add a station to our list of Stations
//
void Network_Interface_Add_Station(uint16_t index, uint8_t *macAddress)
{
    if (index < ESP_WIFI_MAX_CONN_NUM)
    {
        memcpy(wireless_sta[index].mac, macAddress, 6);
        wireless_sta[index].reserved = 1;
        Network_Interface_update_Stations();
    }
}

//
// Remove a station from our list of stations
void Network_Interface_Remove_Station(uint16_t index)
{
    if (index < ESP_WIFI_MAX_CONN_NUM)
    {
        wireless_sta[index].reserved = 0;
        Network_Interface_update_Stations();
    }
}

// Return the maximum number of stations
int Network_Interface_Max_Stations()
{
    return ESP_WIFI_MAX_CONN_NUM;
}

//
//
//
bool Network_Interface_Get_Station(uint16_t index, uint8_t *macAddress, uint8_t *rssi, uint32_t *phyModes)
{
    if (wireless_sta[index].reserved)
    {
        memcpy(macAddress, wireless_sta[index].mac, 6);
        *rssi = wireless_sta[index].rssi;
        *phyModes = wireless_sta[index].phy_11b | (wireless_sta[index].phy_11g << 1) |
                    (wireless_sta[index].phy_11n << 2) | (wireless_sta[index].phy_lr << 3);

        return true;
    }

    return false;
}

//
// De-Auth connected stations
//
void Network_Interface_Deauth_Station(uint16_t stationIndex)
{
    stationIndex++;

    if (stationIndex >= 1 && stationIndex <= ESP_WIFI_MAX_CONN_NUM)
    {
        esp_wifi_deauth_sta(stationIndex);
    }
}
