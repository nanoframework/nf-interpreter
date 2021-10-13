//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// This file includes the board specific Ethernet Intialisation

#include <NF_ESP32_Network.h>
#include <esp32_ethernet_options.h>

esp_eth_handle_t eth_handle = NULL;

// OLIMEX ESP32-EVB Rev B, OLIMEX ESP32-Gateway, Generic Lan8270
// ETH_PHY_ADDR  0
// ETH_MDC_GPIO  23
// ETH_MDIO_GPIO 18

// OLIMEX POE and LILYGO use ETH_RMII_CLK_OUT_GPIO 17
// OLIMEX POE use ETH_PHY_RST_GPIO 12
// OLIMEX gateway revs newer than C use ETH_PHY_RST_GPIO 5

// default values for ESP32 boards
// values taken from IDF KCONFIG files

#ifndef ETH_PHY_ADDR
// PHY address
#define ETH_PHY_ADDR 1
#endif

#ifndef ETH_MDC_GPIO
// GPIO number used by SMI MDC
#define ETH_MDC_GPIO 23
#endif

#ifndef ETH_MDIO_GPIO
// GPIO number used by SMI MDIO
#define ETH_MDIO_GPIO 18
#endif

#ifndef ETH_PHY_RST_GPIO
// GPIO number used to reset PHY chip.
#define ETH_PHY_RST_GPIO 5
#endif

esp_err_t NF_ESP32_InitialiseEthernet(uint8_t *pMacAdr)
{
    (void)pMacAdr;

#ifdef ESP32_ETHERNET_SUPPORT

    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
    esp_netif_t *eth_netif = esp_netif_new(&cfg);
    // Set default handlers to process TCP/IP stuffs
    ESP_ERROR_CHECK(esp_eth_set_default_handlers(eth_netif));

    // now MAC and PHY configs
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();

    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    phy_config.phy_addr = ETH_PHY_ADDR;
    phy_config.reset_gpio_num = ETH_PHY_RST_GPIO;

    mac_config.smi_mdc_gpio_num = ETH_MDC_GPIO;
    mac_config.smi_mdio_gpio_num = ETH_MDIO_GPIO;

    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&mac_config);

    esp_eth_phy_t *phy = esp_eth_phy_new_lan8720(&phy_config);

    // now the ETH config
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);

    ESP_ERROR_CHECK(esp_eth_driver_install(&config, &eth_handle));

    // attach Ethernet driver to TCP/IP stack
    ESP_ERROR_CHECK(esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handle)));

    // start Ethernet driver state machine
    ESP_ERROR_CHECK(esp_eth_start(eth_handle));

#endif

    return ESP_OK;
}

//
//  Open Ethernet Network driver
//
int NF_ESP32_Ethernet_Open(HAL_Configuration_NetworkInterface *config)
{
    if (NF_ESP32_InitialiseEthernet(config->MacAddress) == ESP_OK)
    {
        // Return NetIf number for Esp32 wireless station
        return NF_ESP32_Wait_NetNumber(IDF_WIFI_STA_DEF);
    }

    return SOCK_SOCKET_ERROR;
}

bool NF_ESP32_Ethernet_Close()
{
    return esp_eth_stop(eth_handle) == ESP_OK;
}
