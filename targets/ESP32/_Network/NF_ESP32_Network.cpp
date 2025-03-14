//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// This file includes common method for networking code

#include "NF_ESP32_Network.h"
#include "esp_netif_net_stack.h"

// Wait for the network interface to become available
int NF_ESP32_Wait_NetNumber(int num)
{
    int number = 0;
    int timeoutMs = 30000; // 30 seconds timeout
    int elapsedMs = 0;
    esp_netif_t *espNetif;

    while (elapsedMs < timeoutMs)
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

            case IDF_OT_DEF:
                espNetif = esp_netif_get_handle_from_ifkey("OT_DEF");
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

        const int delayMs = 20;
        vTaskDelay(delayMs / portTICK_PERIOD_MS);
        elapsedMs += delayMs;
    }

    if (espNetif == NULL)
    {
        // Return error or default value
        return SOCK_SOCKET_ERROR;
    }

    return espNetif->lwip_netif->num;
}

HAL_Configuration_NetworkInterface *NF_ESP32_GetNetworkConfigBlock(int index)
{
    HAL_Configuration_NetworkInterface *networkConfig =
        (HAL_Configuration_NetworkInterface *)platform_malloc(sizeof(HAL_Configuration_NetworkInterface));

    if (networkConfig != NULL)
    {
        if (ConfigurationManager_GetConfigurationBlock(networkConfig, DeviceConfigurationOption_Network, index))
        {
            return networkConfig;
        }
        platform_free(networkConfig);
    }

    return NULL;
}

//
// Configure network settings for a espressif network interface
//
esp_err_t NF_ESP32_ConfigureNetwork(esp_netif_t *netIf, HAL_Configuration_NetworkInterface *config)
{
    esp_err_t ec;
    esp_netif_ip_info_t ip_info;

    ec = esp_netif_get_ip_info(netIf, &ip_info);
    if (ec != ESP_OK)
    {
        return ec;
    }

    bool enableDHCP = (config->StartupAddressMode == AddressMode_DHCP);

    // Set static addresses
    if (config->IPv4Address != 0)
    {
        ip_info.ip.addr = config->IPv4Address;
        ip_info.netmask.addr = config->IPv4NetMask;
        ip_info.gw.addr = config->IPv4GatewayAddress;

        ec = esp_netif_set_ip_info(netIf, &ip_info);

        // Make sure DHCP client is disabled
        netIf->flags = (esp_netif_flags_t)(netIf->flags & ~ESP_NETIF_DHCP_CLIENT);
    }

    return ec;
}

esp_err_t NF_ESP32_ConfigureNetworkByIndex(int index, esp_netif_t *netIf)
{
    esp_err_t ec;

    HAL_Configuration_NetworkInterface *networkConfig = NF_ESP32_GetNetworkConfigBlock(index);
    if (networkConfig == NULL)
    {
        return ESP_FAIL;
    }

    ec = NF_ESP32_ConfigureNetwork(netIf, networkConfig);

    platform_free(networkConfig);

    return ec;
}
