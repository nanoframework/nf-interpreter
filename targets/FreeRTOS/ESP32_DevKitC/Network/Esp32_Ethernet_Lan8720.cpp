//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

// This file includes the board specific Ethernet Intialisation

#include <nanoHAL.h>
#include "Esp32_os.h"
#include "eth_phy/phy_lan8720.h"
#include "soc/emac_reg_v2.h"

extern "C"
{
#include "lwip\netif.h"
}

extern struct netif * Esp32_find_netif(esp_interface_t esp_if);


//
// Olimex ESP32-EVB Rev B, OLimex ESP32-Gateway, Generic Lan8270
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

esp_err_t Esp32_InitialiseEthernet( uint8_t * pMacAdr)
{
    (void)pMacAdr;
    
    // Config Ethernet interface which will depend on current hardware
    eth_config_t config = DEFAULT_ETHERNET_PHY_CONFIG;
    /* Set the PHY address in the example configuration */
    config.phy_addr = PHY0;
    config.gpio_config = eth_gpio_config_rmii;
    config.tcpip_input = tcpip_adapter_eth_input;

    esp_err_t ret = esp_eth_init(&config);
    if(ret != ESP_OK) return ret;

    // Test the RMII busy bit to try and see if Ethernet PHY is present/connected
    // ets_printf("Ethernet check status\n");
    // int x = 100, status = 0;
    // while( x > 0 )
    // {
    //     ets_printf("Ethernet get status\n");
    //     status = REG_GET_BIT(EMAC_GMACGMIIADDR_REG, EMAC_GMIIBUSY);
    //     ets_printf("Ethernet status=%d\n", status);
    //     if ( status == 0 ) break;
    //     x--;
    // }
    // ets_printf("Ethernet status %d %d \n",x, status);
    // if ( status == 1) return ESP_FAIL;
    
    //  esp_eth_set_mac( pMacAdr );  // need later IDF
    esp_eth_enable();
 
    return ESP_OK;
}

//
//  Open Ethernet Network driver
//
int  Esp32_Ethernet_Open(int index, HAL_Configuration_NetworkInterface * config) 
{ 
    (void)index;

    if ( Esp32_InitialiseEthernet(config->MacAddress) == ESP_OK )
    {
       // Return NetIf number for Esp32 wireless station

       // FIXME find a better way to get the netif ptr
        struct netif *pNetIf;
        while(true)
        {
            vTaskDelay(100 / portTICK_PERIOD_MS);

            // Return NetIf number for Esp32 wireless station
            pNetIf = Esp32_find_netif(ESP_IF_ETH);
            if (pNetIf != NULL) break; 
        }
	    if (pNetIf != NULL)  return pNetIf->num;
   }

    return SOCK_SOCKET_ERROR; 
}

bool Esp32_Ethernet_Close(int index)
{ 
    (void)index;
    return  esp_eth_disable() == ESP_OK;
}
