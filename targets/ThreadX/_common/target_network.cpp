
//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>

#if defined(THREADX_RTOS_NETXDUO)
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
    bool found = false;

    *wirelessConfig = NULL;

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
        if (ConfigurationManager_GetConfigurationBlock(
                networkConfiguration,
                DeviceConfigurationOption_Network,
                index) &&
            networkConfiguration->InterfaceType == NetworkInterfaceType_Wireless80211)
        {
            // the network interface refers to its station profile by Id
            *wirelessConfig =
                ConfigurationManager_GetWirelessConfigurationFromId(networkConfiguration->SpecificConfigId);

            if (*wirelessConfig == NULL &&
                ConfigurationManager_GetBlockCount(DeviceConfigurationOption_Wireless80211Network) == 0)
            {
                // a device can have no station profiles, start from the defaults (with the Id of the interface)
                *wirelessConfig =
                    (HAL_Configuration_Wireless80211 *)platform_malloc(sizeof(HAL_Configuration_Wireless80211));

                if (*wirelessConfig != NULL)
                {
                    InitialiseWirelessDefaultConfig(*wirelessConfig, networkConfiguration->SpecificConfigId);
                }
            }

            found = (*wirelessConfig != NULL);
        }

        platform_free(networkConfiguration);
    }

    return found;
}

// gets the index of the station profile with this Id, -1 if there is none, -2 if a profile couldn't be read
static int32_t FindWirelessProfileIndex(uint32_t configurationId)
{
    int32_t profileIndex = -1;
    uint32_t count = ConfigurationManager_GetBlockCount(DeviceConfigurationOption_Wireless80211Network);

    HAL_Configuration_Wireless80211 *profile =
        (HAL_Configuration_Wireless80211 *)platform_malloc(sizeof(HAL_Configuration_Wireless80211));

    if (profile != NULL)
    {
        for (uint32_t i = 0; i < count && profileIndex == -1; i++)
        {
            if (!ConfigurationManager_GetConfigurationBlock(profile, DeviceConfigurationOption_Wireless80211Network, i))
            {
                // can't tell if the profile exists
                profileIndex = -2;
            }
            else if (profile->Id == configurationId)
            {
                profileIndex = (int32_t)i;
            }
        }

        memset(profile, 0, sizeof(HAL_Configuration_Wireless80211));
        platform_free(profile);
    }

    return profileIndex;
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
            (Wireless80211Configuration_ConfigurationOptions)(wirelessConfig->Options |
                                                              Wireless80211Configuration_ConfigurationOptions_AutoConnect);
    }
    else
    {
        // need this stupid cast because the current gcc version with ESP32 IDF is not happy with the simple syntax '^='
        wirelessConfig->Options =
            (Wireless80211Configuration_ConfigurationOptions)(wirelessConfig->Options ^
                                                              Wireless80211Configuration_ConfigurationOptions_AutoConnect);

        // Make sure we are still enabled because AutoConnect includes Enable
        wirelessConfig->Options =
            (Wireless80211Configuration_ConfigurationOptions)(wirelessConfig->Options |
                                                              Wireless80211Configuration_ConfigurationOptions_Enable);
    }

    // Update Wireless structure with new SSID and passphase
    hal_strcpy_s((char *)wirelessConfig->Ssid, sizeof(wirelessConfig->Ssid), ssid);
    hal_strcpy_s((char *)wirelessConfig->Password, sizeof(wirelessConfig->Password), passphase);

    // Option to Save new config
    if (options & NETWORK_CONNECT_SAVE_CONFIG)
    {
        // replace the station profile used by this network interface, or add it if there is none
        // (not if the profiles couldn't be read: that could add a duplicate)
        int32_t profileIndex = FindWirelessProfileIndex(wirelessConfig->Id);
        ConfigurationOperationError saveResult = ConfigurationOperationError_StorageError;

        if (profileIndex != -2)
        {
            saveResult = ConfigurationManager_ExecuteConfigurationOperation(
                profileIndex >= 0 ? ConfigurationOperation_Replace : ConfigurationOperation_Add,
                DeviceConfigurationOption_Wireless80211Network,
                profileIndex >= 0 ? (uint32_t)profileIndex : 0,
                0,
                wirelessConfig,
                sizeof(HAL_Configuration_Wireless80211),
                NULL,
                NULL,
                NULL);
        }

        if (saveResult != ConfigurationOperationError_Success && saveResult != ConfigurationOperationError_NoChanges)
        {
            // the profile has the password, clear it before releasing the memory
            memset(wirelessConfig, 0, sizeof(HAL_Configuration_Wireless80211));
            platform_free(wirelessConfig);

            return SOCK_SOCKET_ERROR;
        }
    }

    // the worker connects with these credentials, stored or not
    if (!NF_Wireless_SetRequestedProfile(wirelessConfig))
    {
        // profile has the password, clear it before releasing the memory
        memset(wirelessConfig, 0, sizeof(HAL_Configuration_Wireless80211));
        platform_free(wirelessConfig);

        return SOCK_SOCKET_ERROR;
    }

    NF_ConnectInProgress = true;
    // queue request to connect WiFi, with the Id of the station profile
    wifiRequest = (WIFI_REQUEST_CONNECT_STATION | (wirelessConfig->Id & ~WIFI_REQUEST_MASK));
    tx_queue_send(&WiFiConnectQueue, &wifiRequest, TX_NO_WAIT);

    // the profile has the password, clear it before releasing the memory
    memset(wirelessConfig, 0, sizeof(HAL_Configuration_Wireless80211));
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
