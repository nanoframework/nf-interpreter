//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

// This file includes the board specific Ethernet Intialisation

#include <nanoHAL.h>
#include "Esp32_os.h"

extern "C"
{
#include "lwip\netif.h"
}


struct netif * Esp32_find_netif(esp_interface_t esp_if);

static EventGroupHandle_t wifi_event_group;

static bool WifiInitialised = false;

extern WIRELESS_CONFIG g_WirelessConfig;

esp_err_t Esp32_InitaliseWifi()
{
	esp_err_t ec = ESP_OK;

	if (!WifiInitialised)
	{
		// Init WiFi Alloc resource for WiFi driver, such as WiFi control structure, 
		// RX/TX buffer, WiFi NVS structure etc, this WiFi also start WiFi task. 
		wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
		ec = esp_wifi_init(&cfg);
		if ( ec != ESP_OK) return ec;

		// We don't want to save config to NVS, we will do it ourselves
		esp_wifi_set_storage(WIFI_STORAGE_RAM); 

		esp_wifi_set_mode(WIFI_MODE_STA);

		ec = esp_wifi_start();
		if ( ec != ESP_OK) return ec;

		WifiInitialised = true;
	}

	return ec;
}

esp_err_t Esp32_Wireless_Connect(SOCK_WirelessConfiguration * pWireless)
{
	esp_err_t ec;

	// Connect directly
	wifi_config_t sta_config;
	hal_strncpy_s( (char *)sta_config.sta.ssid, sizeof(sta_config.sta.ssid), pWireless->ssid, hal_strlen_s(pWireless->ssid) );
	hal_strncpy_s( (char*)sta_config.sta.password, sizeof(sta_config.sta.password), pWireless->passPhrase, hal_strlen_s(pWireless->passPhrase) );
	sta_config.sta.bssid_set = false;
	
	ec = esp_wifi_set_config(WIFI_IF_STA, &sta_config);
	if(ec != ESP_OK) return ec;
	
	ec = esp_wifi_connect();
	if(ec != ESP_OK) return ec;

	return ESP_OK;
}

int  Esp32_Wireless_Open(int index, HAL_Configuration_NetworkInterface * config) 
{ 
	esp_err_t ec;

    ec = Esp32_InitaliseWifi();
    if(ec != ESP_OK) return SOCK_SOCKET_ERROR;
 
	// FIXME
    //int wirelessIndex = SOCK_NETWORKCONFIGURATION_FLAGS_SUBINDEX__value(config->flags);
	//SOCK_WirelessConfiguration * pWireless = &g_WirelessConfig.WirelessInterfaces[wirelessIndex];

	//FIXME Configure Wireless Interface

	// Connect if we have an ssid
	// if ( hal_strlen_s(pWireless->ssid) > 0 )
	// {
	// 	Esp32_Wireless_Connect(pWireless); 
	// }
	
	// FIXME find a better way to get the netif ptr
	struct netif *pNetIf;
	while(true)
	{
 		vTaskDelay(100 / portTICK_PERIOD_MS);

    	// Return NetIf number for Esp32 wireless station
		pNetIf = Esp32_find_netif(ESP_IF_WIFI_STA);
		if (pNetIf != NULL) break; 
	}

    return pNetIf->num;
}

bool Esp32_Wireless_Close(int index)
{ 
	if ( WifiInitialised )
	{
		esp_wifi_deinit();
		WifiInitialised = false;
	}
    return false; 
}

//
//	Find the netif * for the passed esp_interface_t
//
extern struct netif * Esp32_find_netif(esp_interface_t esp_if)
{
    struct netif *pNetIf;

    for (pNetIf = netif_list; pNetIf != NULL; pNetIf = pNetIf->next)
    {
        if (esp_if == tcpip_adapter_get_esp_if((void *)pNetIf))
        {
            return pNetIf;
        }
    }
    return NULL;
}