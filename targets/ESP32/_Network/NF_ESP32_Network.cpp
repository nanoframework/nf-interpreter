//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// This file includes common method for networking code

#include "NF_ESP32_Network.h"
#include "esp_netif_net_stack.h"
#include "lwIP_Sockets.h"

// This function retrives the esp_netif ptr from the lwip structure for use else where
// This is a copy of internal funtion of ESP_NETIF
esp_netif_t *NF_ESP32_GetEspNetif(struct netif *netif)
{
#if LWIP_ESP_NETIF_DATA
    return (esp_netif *)netif_get_client_data(netif);
#else
    return (esp_netif_t *)netif->state;
#endif
}

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

    // Add a delay before picking up "lwip_netif->num" as not been filled in yet on slower processors
    vTaskDelay(50 / portTICK_PERIOD_MS);

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

esp_err_t NF_ESP32_ConfigureNetworkByConfigIndex(int index)
{
    esp_err_t ec = ESP_OK;

    HAL_Configuration_NetworkInterface *networkConfig = NF_ESP32_GetNetworkConfigBlock(index);
    if (networkConfig == NULL)
    {
        return ESP_FAIL;
    }

    // Configure network interface
    LWIP_SOCKETS_Driver::UpdateAdapterConfiguration(
        index,
        NetworkInterface_UpdateOperation_Dns | NetworkInterface_UpdateOperation_Dhcp,
        networkConfig);

    platform_free(networkConfig);

    return ec;
}
