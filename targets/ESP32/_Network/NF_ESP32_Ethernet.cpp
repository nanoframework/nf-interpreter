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
// values taken from IDF CONFIG files

#ifndef ESP32_ETHERNET_INTERNAL
// Use internal Ethernet
#define ESP32_ETHERNET_INTERNAL
#endif

#ifndef ETH_PHY_ADDR
// PHY address
#define ETH_PHY_ADDR 0
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

#ifdef ESP32_ETHERNET_INTERNAL
    // Internal Ethernet
    mac_config.smi_mdc_gpio_num = ETH_MDC_GPIO;
    mac_config.smi_mdio_gpio_num = ETH_MDIO_GPIO;
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&mac_config);

    // Define PHY to use with internal Ethernet
#ifdef ESP32_ETHERNET_PHY_IP101
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config);
#elif defined(ESP32_ETHERNET_PHY_RTL8201)
    esp_eth_phy_t *phy = esp_eth_phy_new_rtl8201(&phy_config);
#elif defined(ESP32_ETHERNET_PHY_LAN8720)
    ESP_LOGI("ETH", "Ethernet Lan8720 phy, phy adr %d power %d\n", ETH_PHY_ADDR, ETH_PHY_RST_GPIO);
    esp_eth_phy_t *phy = esp_eth_phy_new_lan8720(&phy_config);
#elif defined(ESP32_ETHERNET_PHY_DP83848)
    esp_eth_phy_t *phy = esp_eth_phy_new_dp83848(&phy_config);
#elif defined(ESP32_ETHERNET_PHY_KSZ8041)
    esp_eth_phy_t *phy = esp_eth_phy_new_ksz8041(&phy_config);
#endif

#elif ESP32_ETHERNET_SPI
    // Or Use SPI ethernet module

    // Initialise SPI bus
    gpio_install_isr_service(0);
    spi_device_handle_t spi_handle = NULL;
    spi_bus_config_t buscfg = {
        .miso_io_num = ESP32_ETHERNET_SPI_MISO_GPIO,
        .mosi_io_num = ESP32_ETHERNET_SPI_MOSI_GPIO,
        .sclk_io_num = ESP32_ETHERNET_SPI_SCLK_GPIO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(CONFIG_EXAMPLE_ETH_SPI_HOST, &buscfg, 1));

    // Define SPI interface to use
#if (ESP32_ETHERNET_SPI_MODULE == DM9051)
    spi_device_interface_config_t devcfg = {
        .command_bits = 1,
        .address_bits = 7,
        .mode = 0,
        .clock_speed_hz = ESP32_ETHERNET_SPI_CLOCK_MHZ * 1000 * 1000,
        .spics_io_num = ESP32_ETHERNET_SPI_CS,
        .queue_size = 20};
    ESP_ERROR_CHECK(spi_bus_add_device(CONFIG_EXAMPLE_ETH_SPI_HOST, &devcfg, &spi_handle));
    /* dm9051 ethernet driver is based on spi driver */
    eth_dm9051_config_t dm9051_config = ETH_DM9051_DEFAULT_CONFIG(spi_handle);
    dm9051_config.int_gpio_num = ESP32_ETHERNET_SPI_INT_GPIO;
    esp_eth_mac_t *mac = esp_eth_mac_new_dm9051(&dm9051_config, &mac_config);

#elif (ESP32_ETHERNET_SPI_MODULE == W5500)
    // Use W5500 in mac mode
    spi_device_interface_config_t devcfg = {
        .command_bits = 16, // Actually it's the address phase in W5500 SPI frame
        .address_bits = 8,  // Actually it's the control phase in W5500 SPI frame
        .mode = 0,
        .clock_speed_hz = ESP32_ETHERNET_SPI_CLOCK_MHZ * 1000 * 1000,
        .spics_io_num = ESP32_ETHERNET_SPI_CS,
        .queue_size = 20};
    ESP_ERROR_CHECK(spi_bus_add_device(CONFIG_EXAMPLE_ETH_SPI_HOST, &devcfg, &spi_handle));
    // w5500 ethernet driver is based on spi driver
    eth_w5500_config_t w5500_config = ETH_W5500_DEFAULT_CONFIG(spi_handle);
    w5500_config.int_gpio_num = ESP32_ETHERNET_SPI_INT_GPIO;
    esp_eth_mac_t *mac = esp_eth_mac_new_w5500(&w5500_config, &mac_config);
    esp_eth_phy_t *phy = esp_eth_phy_new_w5500(&phy_config);
#else
    // No SPI module defined

#endif
#endif // CONFIG_ETH_USE_SPI_ETHERNET

    // now the do the Ethernet config
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);
    ESP_ERROR_CHECK(esp_eth_driver_install(&config, &eth_handle));

#ifndef ESP32_ETHERNET_INTERNAL
    // The SPI Ethernet module might doesn't have a burned factory MAC address, we have to set it manually.
    // Supplied in the config
    ESP_ERROR_CHECK(esp_eth_ioctl(eth_handle, ETH_CMD_S_MAC_ADDR, pMacAdr));
#endif

    // attach Ethernet driver to TCP/IP stack
    ESP_ERROR_CHECK(esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handle)));

    // start Ethernet driver state machine
    ESP_ERROR_CHECK(esp_eth_start(eth_handle));

#endif // ESP32_ETHERNET_SUPPORT
    return ESP_OK;
}

//
//  Open Ethernet Network driver
//
int NF_ESP32_Ethernet_Open(HAL_Configuration_NetworkInterface *config)
{
    if (NF_ESP32_InitialiseEthernet(config->MacAddress) == ESP_OK)
    {
        // Return NetIf number for Esp32 Ethernet
        return NF_ESP32_Wait_NetNumber(IDF_ETH_DEF);
    }

    return SOCK_SOCKET_ERROR;
}

bool NF_ESP32_Ethernet_Close()
{
    return esp_eth_stop(eth_handle) == ESP_OK;
}
