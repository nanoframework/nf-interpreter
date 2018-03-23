//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

// This file includes the board specific Network Intialisation

#include <nanoHAL.h>
#include "Esp32_os.h"

extern "C" void set_signal_sock_function( void (*funcPtr)() );

// NVS parameters for Ethernet config
#define NVS_NAMESPACE	    "nanoF"
#define NVS_NETWORK_CONFIG1	"Net1"
//
// Callback from LWIP on event
//
void sys_signal_sock_event()
{
     Events_Set(SYSTEM_EVENT_FLAG_SOCKET);
}


//
// Dummy Ethernet Initialisation, board specific Initialisation in separate file
__nfweak void InitialiseEthernet()
{
}

static bool WifiInitialised = false;


void InitaliseWifiConfig()
{
	esp_err_t ec;

	// Load Wifi Config and se if we need to initialise it 

	// if (?????)
	{
		if (!WifiInitialised)
		{
			// Init WiFi Alloc resource for WiFi driver, such as WiFi control structure, 
			// RX/TX buffer, WiFi NVS structure etc, this WiFi also start WiFi task. 
			wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
			ec = esp_wifi_init(&cfg);

			// We don't want to save config to NVS, we will do it ourselves
			esp_wifi_set_storage(WIFI_STORAGE_RAM); 

			ec = esp_wifi_start();
			WifiInitialised = true;
		}

		// What about auto reconnect ? 
		// would need to monitor wifi events

		// Connect directly
		wifi_config_t sta_config;
		memcpy( (char *)sta_config.sta.ssid, "ssid", sizeof(sta_config.sta.ssid));
		memcpy( (char*)sta_config.sta.password,"password",sizeof(sta_config.sta.password));
		sta_config.sta.bssid_set = false;
		
		ec = esp_wifi_set_config(WIFI_IF_STA, &sta_config);
		
		ec = esp_wifi_connect();
	}
}


void IRAM_ATTR nanoHAL_Network_Initialize(tcpip_init_done_fn initfunc)
{
	esp_err_t ret;

 	// Initialise the Lwip CLR signal callback
	set_signal_sock_function( &sys_signal_sock_event );

	// Initialise Tcp adpater
    tcpip_adapter_init();
   
    esp_event_loop_init(NULL, NULL);

	// Load Wifi saved config and connect if required 
	InitaliseWifiConfig();

	// Initialise Ethernet interface if available
	// Gets in a reset loop if no Phy available
//	InitialiseEthernet();   

	// Callback to Network stack when init complete 
	initfunc(NULL);
}

// Gets the network configuration block from the configuration block stored in the NVS block, 
// maybe better to store each config item under a separate key which would work better if config block changes
bool GetConfigurationNetwork(Configuration_Network* configurationNetwork)
{
	bool result = FALSE;
	nvs_handle out_handle;
	size_t     blobSize = sizeof(Configuration_Network);

	// Open NVS storage using NanoFramework namespace
	esp_err_t ret = nvs_open( NVS_NAMESPACE, NVS_READONLY, &out_handle);
	if ( ret == ESP_OK )
	{
	   // copy the config block content to the pointer in the argument
		ret = nvs_get_blob(out_handle, NVS_NETWORK_CONFIG1, (void *)configurationNetwork, &blobSize);
		if (ret == ESP_OK) result = TRUE;
		nvs_close(out_handle);
	}

    return result;
}

// Stores the network configuration block to the EPS32 storage 
bool StoreConfigurationNetwork(Configuration_Network* configurationNetwork)
{
	bool result = FALSE;
	nvs_handle out_handle;

    // copy the config block content to the pointer in the argument
	esp_err_t ret = nvs_open( NVS_NAMESPACE, NVS_READWRITE, &out_handle);
	if ( ret == ESP_OK )
	{
		ret = nvs_set_blob(out_handle, NVS_NETWORK_CONFIG1, (void *)configurationNetwork, sizeof(Configuration_Network));
		if ( ret == ESP_OK ) 
		{
			ret = nvs_commit(out_handle);
			if (ret == ESP_OK) result = TRUE;
		}
		nvs_close(out_handle);
	}

    return result;
}
