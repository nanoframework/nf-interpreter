
//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>

#if defined(AZURE_RTOS_NETXDUO)
#include <nx_user.h>
#include <nx_api.h>
#include <nxd_bsd.h>
#endif

#include <network_options.h>
#include <nf_wireless.h>

bool NF_ConnectInProgress = false;
int NF_ConnectResult = 0;

// extern "C" struct netif *nf_getNetif();
//
// Works with the Target_NetworkConfig to map the Network_Interface_XXXXX calls to the correct driver

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

#if (TARGET_HAS_WIFI_SUPPORT == 1)
        // Wi-Fi (STA)
        case NetworkInterfaceType_Wireless80211:
            return NF_Wireless_Open(&networkConfiguration);
#endif

            // // Wi-Fi (Soft AP)
            // case NetworkInterfaceType_WirelessAP:
            //     return NF_WirelessAP_Open(&networkConfiguration);

#if (TARGET_HAS_ETHERNET_SUPPORT == 1)
        // Ethernet
        case NetworkInterfaceType_Ethernet:
            return NF_ESP32_Ethernet_Open(&networkConfiguration);
#endif
        default:
            break;
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

#if (TARGET_HAS_WIFI_SUPPORT == 1)
        // Wireless
        case NetworkInterfaceType_Wireless80211:
            return NF_Wireless_Close();

            // // Soft AP
            // case NetworkInterfaceType_WirelessAP:
            //     return NF_ESP32_WirelessAP_Close();
#endif

#if (TARGET_HAS_ETHERNET_SUPPORT == 1)
        // Ethernet
        case NetworkInterfaceType_Ethernet:
            return NF_ESP32_Ethernet_Close();
#endif

        default:
            break;
    }

    return false;
}

int Network_Interface_Start_Scan(int index)
{
    HAL_Configuration_NetworkInterface networkConfiguration;

    // load network interface configuration from storage
    if (!ConfigurationManager_GetConfigurationBlock(
            (void *)&networkConfiguration,
            DeviceConfigurationOption_Network,
            index))
    {
        // failed to get configuration
        // TODO include error code enum
        return 7777; // StartScanOutcome_FailedToGetConfiguration;
    }

    // can only do this is this is STA
    if (networkConfiguration.InterfaceType != NetworkInterfaceType_Wireless80211)
    {
        // TODO include error code enum
        return 8888; // StartScanOutcome_WrongInterfaceType;
    }

    // TODO return NF_ESP32_Wireless_Scan();
    // TODO include error code enum
    return 9999;
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

int Network_Interface_Start_Connect(int index, const char *ssid, const char *passphase, int options)
{
    int err = TX_SUCCESS;
    uint32_t wifiRequest;

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
        ConfigurationManager_UpdateConfigurationBlock(
            wirelessConfig,
            DeviceConfigurationOption_Wireless80211Network,
            index);
    }

    NF_ConnectInProgress = true;
    // queue request to connect WiFi
    wifiRequest = (WIFI_REQUEST_CONNECT_STATION | index);
    tx_queue_send(&WiFiConnectQueue, &wifiRequest, TX_NO_WAIT);

    platform_free(wirelessConfig);

    return (int)err;
}

int Network_Interface_Connect_Result(int index)
{
    if (g_TargetConfiguration.NetworkInterfaceConfigs->Configs[index]->InterfaceType ==
        NetworkInterfaceType_Wireless80211)
    {
        return NF_ConnectInProgress ? -1 : NF_ConnectResult;
    }

    return SOCK_SOCKET_ERROR;
}

int Network_Interface_Disconnect(int index)
{
    if (g_TargetConfiguration.NetworkInterfaceConfigs->Configs[index]->InterfaceType ==
            NetworkInterfaceType_Wireless80211 &&
        NF_ConnectResult == 1)
    {
        // queue request to disconnect WiFi
        uint32_t wifiRequest = WIFI_REQUEST_STOP_STATION;
        tx_queue_send(&WiFiConnectQueue, &wifiRequest, TX_NO_WAIT);

        return true;
    }

    return false;
}
