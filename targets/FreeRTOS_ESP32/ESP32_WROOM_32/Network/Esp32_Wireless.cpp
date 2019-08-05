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
static wifi_mode_t wifiMode;

// Forward / external references
struct netif * Esp32_find_netif(esp_interface_t esp_if);
int Esp32_Wait_NetNumber(esp_interface_t esp_if);
void Start_wifi_smart_config();
esp_err_t Esp32_WirelessAP_Configure(HAL_Configuration_NetworkInterface * pConfig );

//
//  Check what is the required Wifi mode
//  Station only or AP only or Station and AP
//
//  See what network interfaces are enabled
//
wifi_mode_t Esp32_CheckWifiMode()
{
	wifi_mode_t mode = WIFI_MODE_NULL;

	// Check Wifi station available
	if ( g_TargetConfiguration.NetworkInterfaceConfigs->Count >= 1 )
	{
		// Check if Station config available
		HAL_Configuration_NetworkInterface * pCfgSta = g_TargetConfiguration.NetworkInterfaceConfigs->Configs[0];

		// Wireless Config with SSID setup
		if ( pCfgSta->InterfaceType == NetworkInterfaceType::NetworkInterfaceType_Wireless80211 )
		{
			HAL_Configuration_Wireless80211 * pWirelessSta = ConfigurationManager_GetWirelessConfigurationFromId(pCfgSta->SpecificConfigId);
			if (pWirelessSta != 0 ) 
			{
				if ( pWirelessSta->Flags & WirelessFlags_Enable )
				{
					mode = WIFI_MODE_STA;
				}
			}
		}
	}

	// Check if AP config available
	if ( g_TargetConfiguration.NetworkInterfaceConfigs->Count >= 2 )
	{
		HAL_Configuration_NetworkInterface * pCfgAP = g_TargetConfiguration.NetworkInterfaceConfigs->Configs[1];

		// Wireless Config with SSID setup
		if ( pCfgAP->InterfaceType == NetworkInterfaceType::NetworkInterfaceType_WirelessAP )
		{
			HAL_Configuration_WirelessAP * pWirelessAp = ConfigurationManager_GetWirelessAPConfigurationFromId(pCfgAP->SpecificConfigId);
			if (pWirelessAp != 0 ) 
			{
				if ( pWirelessAp->Flags & WirelessAPFlags_Enable )
				{
					// Use STATION + AP or just AP
					mode = ( mode == WIFI_MODE_STA)? WIFI_MODE_APSTA : WIFI_MODE_AP;
					//mode = WIFI_MODE_APSTA;
				}
			}
		}
	}
	
	ets_printf("Esp32_CheckWifiMode %d\n", (int)mode);

	return mode;
}

wifi_mode_t Esp32_GetWifiMode()
{
	wifi_mode_t current_wifi_mode;
	esp_wifi_get_mode(&current_wifi_mode);
	return current_wifi_mode;
}

void Esp32_DeinitWifi()
{
	WifiInitialised = false;
	esp_wifi_stop();
	esp_wifi_deinit();
}

esp_err_t Esp32_InitaliseWifi()
{
	esp_err_t ec = ESP_OK;

	wifi_mode_t wifi_mode = Esp32_CheckWifiMode();

	if ( WifiInitialised )
	{
		// Check if we are running correct mode
		// if not force a new init
		if ( Esp32_GetWifiMode() != wifi_mode )
		{
			Esp32_DeinitWifi();
		}
	}

	if (!WifiInitialised)
	{
		// Init WiFi Alloc resource for WiFi driver, such as WiFi control structure, 
		// RX/TX buffer, WiFi NVS structure etc, this WiFi also start WiFi task. 
		wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
		ec = esp_wifi_init(&cfg);
		if ( ec != ESP_OK) return ec;
		
		esp_wifi_set_mode(wifi_mode);

		// We need to start the WIFI stack before the station can Connect
		// Also we can only get the NetIf number used by ESP Idf after it has been started.
		// Starting will also start the Soft- AP (if we have enabled it).
		// So make sure it configured as per wireless config otherwise we will get the default SSID   
		if (wifi_mode & WIFI_MODE_AP )
		{
			HAL_Configuration_NetworkInterface * pConfig = g_TargetConfiguration.NetworkInterfaceConfigs->Configs[1];
			ec = Esp32_WirelessAP_Configure(pConfig);
		} 

		ec = esp_wifi_start();
		if ( ec != ESP_OK) return ec;

		WifiInitialised = true;
	}

	return ec;
}


// esp_err_t Esp32_InitaliseWifi()
// {
// 	esp_err_t ec = ESP_OK;

// 	if (!WifiInitialised)
// 	{
// 		// Init WiFi Alloc resource for WiFi driver, such as WiFi control structure, 
// 		// RX/TX buffer, WiFi NVS structure etc, this WiFi also start WiFi task. 
// 		wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
		
// 		// Don't store Wireless params in NVS
// 		cfg.nvs_enable = 0;

// 		ec = esp_wifi_init(&cfg);
// 		if ( ec != ESP_OK) return ec;
		
// 		esp_wifi_set_mode(WIFI_MODE_STA);

// 		ec = esp_wifi_start();
// 		if ( ec != ESP_OK) return ec;

// 		WifiInitialised = true;
// 	}

// 	return ec;
// }

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
    if (pWireless == 0 ) return SOCK_SOCKET_ERROR;

	// Wireless station not enabled
	if ( !(Esp32_GetWifiMode() & WIFI_MODE_STA) ) return SOCK_SOCKET_ERROR;

	if ( !(pWireless->Flags & WirelessFlags_Enable))  return SOCK_SOCKET_ERROR;

	// Connect if Auto connect and we have an ssid
	if ( (pWireless->Flags & WirelessFlags_Auto) && (hal_strlen_s((const char *)pWireless->Ssid) > 0) )
	{
	 	Esp32_Wireless_Connect(pWireless);

		// Maybe remove SmartConfig flag if conected ok
	}

	if ( pWireless->Flags & WirelessFlags_SmartConfig)
	{
		// FIXME
		// Disable for now, When the smart_config starts it scans for wireless AP
		// If it doesn't find any AP it ends up with a exception which causes the device to restart

		// Start Smart config (if enabled)
		//Start_wifi_smart_config();
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

wifi_auth_mode_t MapAuthenication( AuthenticationType type)
{
     wifi_auth_mode_t mapAuth[] = { 
		WIFI_AUTH_OPEN, 		// 0 None
		WIFI_AUTH_OPEN, 		// 1 EAP
		WIFI_AUTH_OPEN, 		// 2 PEAP
		WIFI_AUTH_OPEN,			// 3 WCN
		WIFI_AUTH_OPEN,			// 4 Open
		WIFI_AUTH_OPEN,			// 5 Shared
		WIFI_AUTH_WEP,			// 6 WEP
		WIFI_AUTH_WPA_PSK,		// 7 WPA
		WIFI_AUTH_WPA_WPA2_PSK  // 8 WPA2
	};

	return mapAuth[type];
}

esp_err_t Esp32_WirelessAP_Configure(HAL_Configuration_NetworkInterface * pConfig )
{
	esp_err_t ec;

ets_printf( "Esp32_WirelessAP_Configure id:%d \n", pConfig->SpecificConfigId  );
	tcpip_adapter_ip_info_t tcpip_info;

	ec = tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_AP, &tcpip_info);
ets_printf( "Get AP tcpip %d %X", ec, tcpip_info.ip.addr);

	if ( pConfig->IPv4Address != 0 )
	{
		tcpip_info.ip.addr = pConfig->IPv4Address;
		tcpip_info.netmask.addr = pConfig->IPv4NetMask;
		tcpip_info.gw.addr = pConfig->IPv4GatewayAddress;
		ec = tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &tcpip_info );
ets_printf( "Set AP tcpip %d %X", ec, tcpip_info.ip.addr);
	}
	else
	{
		pConfig->IPv4Address = tcpip_info.ip.addr;
		pConfig->IPv4NetMask = tcpip_info.netmask.addr;
		pConfig->IPv4GatewayAddress = tcpip_info.gw.addr;
ets_printf( "Update config with current address");

	}

	HAL_Configuration_WirelessAP * pWireless = ConfigurationManager_GetWirelessAPConfigurationFromId(pConfig->SpecificConfigId);
    if (pWireless == 0 ) return ESP_FAIL;

    wifi_config_t ap_config = {};
	hal_strncpy_s( (char *)ap_config.ap.ssid, sizeof(ap_config.ap.ssid), (char *)pWireless->Ssid, hal_strlen_s((char *)pWireless->Ssid) );
	hal_strncpy_s( (char*)ap_config.ap.password, sizeof(ap_config.ap.password), (char *)pWireless->Password, hal_strlen_s((char *)pWireless->Password) );

    ap_config.ap.channel = pWireless->Channel;
    ap_config.ap.ssid_hidden = (pWireless->Flags & WirelessAPFlags_Hidden_SSID)? 1 : 0;
    ap_config.ap.authmode = MapAuthenication(pWireless->Authentication);
    ap_config.ap.max_connection = pWireless->MaxConnections;
    ap_config.ap.beacon_interval = 100;

  	ec = esp_wifi_set_config(ESP_IF_WIFI_AP, &ap_config);

ets_printf( "Esp32_WirelessAP_Configure esp_wifi_set_config %d \n", ec );
	 
	 return ec;
}


//
//	Open Wireless Soft AP
//
//  If AP is enabled it will have been configured and started running when the WiFI stack is initialised
//  All we need to do here is return the NetIf number used by ESP IDF
//  Also make sure WIfi in initialised correctly if config is changed
int  Esp32_WirelessAP_Open(int index, HAL_Configuration_NetworkInterface * pConfig) 
{ 
    (void)index;
	esp_err_t ec;

ets_printf( "Esp32_WirelessAP_Open id:%d \n", pConfig->SpecificConfigId  );
   
    // Initialise Wifi stack if required
    ec = Esp32_InitaliseWifi();
    if(ec != ESP_OK) return SOCK_SOCKET_ERROR;

	// AP mode enabled ?
	if ( !(Esp32_GetWifiMode() & WIFI_MODE_AP) )
		return SOCK_SOCKET_ERROR;

	// Return NetIf number
   	// FIXME find a better way to get the netif ptr
	// This becomes available on the event AP STARTED
	// for the moment we just wait for it
	return Esp32_Wait_NetNumber(ESP_IF_WIFI_AP);
}

//
//  Closing down AP
//
//  	Either config being updated  or shutdown
//  	Closing AP will also stop Station
//
bool Esp32_WirelessAP_Close(int index)
{ 
	(void)index;

    Esp32_DeinitWifi();

ets_printf( "Esp32_WirelessAP_close Esp32_DeinitWifi\n");
 	return true;
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