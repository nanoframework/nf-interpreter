//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// This file includes the board specific Ethernet Intialisation

#include <nanoHAL.h>
#include <esp32_ethernet_options.h>

extern struct netif *Esp32_find_netif(esp_interface_t esp_if);

esp_eth_handle_t eth_handle = NULL;


//
// Olimex ESP32-EVB Rev B, OLimex ESP32-Gateway, Generic Lan8270
//
#define CONFIG_PHY_LAN8720      1
#define CONFIG_PHY_ADDRESS      0
#define CONFIG_PHY_SMI_MDC_PIN  23
#define CONFIG_PHY_SMI_MDIO_PIN 18

// Uncomment one of these following lines to support switching of a power gpio used on some boards
//#define ESP32_CONFIG_PIN_PHY_POWER		12     // Olimex_POE
//#define ESP32_CONFIG_PIN_PHY_POWER		5      // Olimex_gateway revs newer than C

// Uncomment one of these lines to select alternate clock modes
//#define ESP32_CONFIG_PHY_CLOCK_MODE       ETH_CLOCK_GPIO0_IN      // Default
//#define ESP32_CONFIG_PHY_CLOCK_MODE       ETH_CLOCK_GPIO17_OUT    // Olimex_POE, Olimex_POE-ISO
//#define ESP32_CONFIG_PHY_CLOCK_MODE		ETH_CLOCK_GPIO0_OUT     //
//#define ESP32_CONFIG_PHY_CLOCK_MODE		ETH_CLOCK_GPIO16_OUT    //

#ifdef CONFIG_PHY_LAN8720
// #include "eth_phy/phy_lan8720.h"
#define DEFAULT_ETHERNET_PHY_CONFIG phy_lan8720_default_ethernet_config
#endif

#ifdef CONFIG_PHY_TLK110
#include "eth_phy/phy_tlk110.h"
#define DEFAULT_ETHERNET_PHY_CONFIG phy_tlk110_default_ethernet_config
#endif

#define PIN_SMI_MDC  CONFIG_PHY_SMI_MDC_PIN
#define PIN_SMI_MDIO CONFIG_PHY_SMI_MDIO_PIN

#ifdef ESP32_CONFIG_PIN_PHY_POWER
static void phy_device_power_enable_via_gpio(bool enable)
{
    if (!enable)
        phy_lan8720_default_ethernet_config.phy_power_enable(false);

    gpio_pad_select_gpio((gpio_num_t)ESP32_CONFIG_PIN_PHY_POWER);
    gpio_set_direction((gpio_num_t)ESP32_CONFIG_PIN_PHY_POWER, GPIO_MODE_OUTPUT);
    gpio_set_level((gpio_num_t)ESP32_CONFIG_PIN_PHY_POWER, (int)enable);

    // Allow the power up/down to take effect, min 300us
    vTaskDelay(1);

    if (enable)
        phy_lan8720_default_ethernet_config.phy_power_enable(true);
}
#endif

esp_err_t Esp32_InitialiseEthernet(uint8_t *pMacAdr)
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
    phy_config.phy_addr = CONFIG_EXAMPLE_ETH_PHY_ADDR;
    phy_config.reset_gpio_num = CONFIG_EXAMPLE_ETH_PHY_RST_GPIO;

#ifdef ESP32_CONFIG_PIN_PHY_POWER
    phy_config.phy_power_enable = phy_device_power_enable_via_gpio;
#endif

    mac_config.smi_mdc_gpio_num = CONFIG_EXAMPLE_ETH_MDC_GPIO;
    mac_config.smi_mdio_gpio_num = CONFIG_EXAMPLE_ETH_MDIO_GPIO;
    
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
int Esp32_Ethernet_Open(int index, HAL_Configuration_NetworkInterface *config)
{
    (void)index;

    if (Esp32_InitialiseEthernet(config->MacAddress) == ESP_OK)
    {
        // Return NetIf number for Esp32 wireless station

        // FIXME find a better way to get the netif ptr
        struct netif *pNetIf;
        while (true)
        {
            vTaskDelay(100 / portTICK_PERIOD_MS);

            // Return NetIf number for Esp32 wireless station
            pNetIf = Esp32_find_netif(ESP_IF_ETH);
            if (pNetIf != NULL)
                break;
        }

        if (pNetIf != NULL)
        {
            // TODO
            // return pNetIf->num;
            return 0;
        }
    }

    return SOCK_SOCKET_ERROR;
}

bool Esp32_Ethernet_Close(int index)
{
    (void)index;
    return esp_eth_stop(eth_handle) == ESP_OK;
}
