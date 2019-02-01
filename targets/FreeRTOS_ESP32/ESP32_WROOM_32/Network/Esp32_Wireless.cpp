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

static const char *TAG = "wifi";
static bool WifiInitialised = false;

// Forward / external references
struct netif * Esp32_find_netif(esp_interface_t esp_if);
int Esp32_Wait_NetNumber(esp_interface_t esp_if);
void Start_wifi_smart_config();



esp_err_t Esp32_InitaliseWifi()
{
	esp_err_t ec = ESP_OK;

	if (!WifiInitialised)
	{
		// Init WiFi Alloc resource for WiFi driver, such as WiFi control structure, 
		// RX/TX buffer, WiFi NVS structure etc, this WiFi also start WiFi task. 
		wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
		
		// Don't store Wireless params in NVS
		cfg.nvs_enable = 0;

		ec = esp_wifi_init(&cfg);
		if ( ec != ESP_OK) return ec;
		
		esp_wifi_set_mode(WIFI_MODE_STA);

		ec = esp_wifi_start();
		if ( ec != ESP_OK) return ec;

		WifiInitialised = true;
	}

	return ec;
}

esp_err_t Esp32_Wireless_Connect(HAL_Configuration_Wireless80211 * pWireless)
{
	esp_err_t ec;

	// Connect directly
	wifi_config_t sta_config = {};
	hal_strncpy_s( (char *)sta_config.sta.ssid, sizeof(sta_config.sta.ssid), (char *)pWireless->Ssid, hal_strlen_s((char *)pWireless->Ssid) );
	hal_strncpy_s( (char*)sta_config.sta.password, sizeof(sta_config.sta.password), (char *)pWireless->Password, hal_strlen_s((char *)pWireless->Password) );
	sta_config.sta.bssid_set = false;
	
	ec = esp_wifi_set_config(WIFI_IF_STA, &sta_config);
 	if(ec != ESP_OK) return ec;
	
	ec = esp_wifi_connect();
    ESP_LOGI(TAG, "WiFi Connect to %s result %d", sta_config.sta.ssid, ec);
 	if(ec != ESP_OK) return ec;

	return ESP_OK;
}

esp_err_t Esp32_Wireless_Disconnect()
{
	esp_err_t ec;

	ec = esp_wifi_disconnect();
	if(ec != ESP_OK) return ec;

	return ESP_OK;
}

int  Esp32_Wireless_Open(int index, HAL_Configuration_NetworkInterface * pConfig) 
{ 
	(void)index;
	
	esp_err_t ec;

    ec = Esp32_InitaliseWifi();
    if(ec != ESP_OK) return SOCK_SOCKET_ERROR;
 
	// Get Wireless config
	HAL_Configuration_Wireless80211 * pWireless = ConfigurationManager_GetWirelessConfigurationFromId(pConfig->SpecificConfigId);

	// Connect if we have an ssid
	if ( hal_strlen_s((const char *)pWireless->Ssid) > 0 )
	{
	 	Esp32_Wireless_Connect(pWireless); 
	}
	else
	{
		// Start Samrt config (if enabled (TODO) )
		// probably best to have a config flag for this, but for now just start if no Wireless config set up
		Start_wifi_smart_config();
	}	

	return Esp32_Wait_NetNumber(ESP_IF_WIFI_STA);
}

bool Esp32_Wireless_Close(int index)
{ 
	(void)index;

	if ( WifiInitialised )
	{
		esp_wifi_deinit();
		WifiInitialised = false;
	}
    return false; 
}

// Start a scan
int Esp32_Wireless_Scan()
{
	wifi_scan_config_t config = {};

	config.scan_type = WIFI_SCAN_TYPE_PASSIVE;
	config.scan_time.passive = 500;                         // 500 ms

	// Start a WIFI scan
	// When complete a Scan Complete event will be fired
	esp_err_t res = esp_wifi_scan_start( &config, false );
	return (int)res;
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

// Esp32_Wait_NetNumber
//
// 	Find the NetiF number used by esp_interface_t
//  If not availbale then loop and wait for interface to start
//
int Esp32_Wait_NetNumber(esp_interface_t esp_if)
{
	int number = 0;

	// FIXME find a better way to get the netif ptr
	struct netif *pNetIf;
	while(true)
	{
    	// Return NetIf number for Esp32 wireless station
		pNetIf = Esp32_find_netif(esp_if);
		if (pNetIf != NULL) break; 

 		vTaskDelay(20 / portTICK_PERIOD_MS);
	}

	return pNetIf->num;
}