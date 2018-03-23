//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

// This file includes the board specific Ethernet Intialisation

#include <nanoHAL.h>
#include "Esp32_os.h"
#include "eth_phy/phy_lan8720.h"



//
// Olimex ESP32-EVB Rev B, OLimex ESP32-Gateway
//
#define CONFIG_PHY_LAN8720          1
#define CONFIG_PHY_ADDRESS          0
#define CONFIG_PHY_SMI_MDC_PIN      23
#define CONFIG_PHY_SMI_MDIO_PIN     18


#ifdef CONFIG_PHY_LAN8720
#include "eth_phy/phy_lan8720.h"
#define DEFAULT_ETHERNET_PHY_CONFIG phy_lan8720_default_ethernet_config
#endif

#ifdef CONFIG_PHY_TLK110
#include "eth_phy/phy_tlk110.h"
#define DEFAULT_ETHERNET_PHY_CONFIG phy_tlk110_default_ethernet_config
#endif

#define PIN_SMI_MDC   CONFIG_PHY_SMI_MDC_PIN
#define PIN_SMI_MDIO  CONFIG_PHY_SMI_MDIO_PIN

static void eth_gpio_config_rmii(void)
{
    // RMII data pins are fixed:
    // TXD0 = GPIO19
    // TXD1 = GPIO22
    // TX_EN = GPIO21
    // RXD0 = GPIO25
    // RXD1 = GPIO26
    // CLK == GPIO0
    phy_rmii_configure_data_interface_pins();
    // MDC is GPIO 23, MDIO is GPIO 18
    phy_rmii_smi_configure_pins(PIN_SMI_MDC, PIN_SMI_MDIO);
}


void InitialiseEthernet()
{
    // get network configuration for Ethernet interface if available
    Configuration_Network* networkConfig = (Configuration_Network*)platform_malloc(sizeof(Configuration_Network));
    
//    if(GetConfigurationNetwork(networkConfig) == true)
    {
        // Config Ethernet interface which will depend on current hardware
        eth_config_t config = DEFAULT_ETHERNET_PHY_CONFIG;
        /* Set the PHY address in the example configuration */
        config.phy_addr = PHY0;
        config.gpio_config = eth_gpio_config_rmii;
        config.tcpip_input = tcpip_adapter_eth_input;

        esp_err_t ret = esp_eth_init(&config);
        if(ret == ESP_OK) 
        {
          //  esp_eth_set_mac( networkConfig->MacAddressdr );  // need later IDF
            esp_eth_enable();
            ets_printf("Ethernet enabled");
        }	
	}

    // free memory
    platform_free(networkConfig);
}
