
//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include "Esp32_os.h"


//
// Works with the Target_NetworkConfig to map the Network_Interface_XXXXX calls to the correct driver
	
int  Esp32_Wireless_Open(int index, HAL_Configuration_NetworkInterface * config);
bool Esp32_Wireless_Close(int index);
int  Esp32_WirelessAP_Open(int index, HAL_Configuration_NetworkInterface * config);
bool Esp32_WirelessAP_Close(int index);
int  Esp32_Ethernet_Open(int index, HAL_Configuration_NetworkInterface * config);
bool Esp32_Ethernet_Close(int index);
int  Esp32_Wireless_Scan();
int  Esp32_Wireless_Disconnect();
int  Esp32_Wireless_Connect(HAL_Configuration_Wireless80211 * pWireless);

bool StoreConfigBlock(DeviceConfigurationOption configType, uint32_t configurationIndex, void * pConfigBlock, size_t writeSize );


bool Network_Interface_Bind(int index)
{
    (void)index;
    
    return true;
}

int  Network_Interface_Open(int configIndex)
{
    HAL_Configuration_NetworkInterface * pConfig;

    // Check index in range
    if ( g_TargetConfiguration.NetworkInterfaceConfigs->Count <= configIndex ) return SOCK_SOCKET_ERROR;

    // load network interface configuration from storage
    pConfig = g_TargetConfiguration.NetworkInterfaceConfigs->Configs[configIndex];

    switch((tcpip_adapter_if_t)configIndex)
    {
        // Wireless 
        case TCPIP_ADAPTER_IF_STA:
            return Esp32_Wireless_Open(configIndex, pConfig);

       // Soft AP
        case TCPIP_ADAPTER_IF_AP:
            return Esp32_WirelessAP_Open(configIndex, pConfig);

#if ESP32_ETHERNET_SUPPORT
        // Ethernet
        case TCPIP_ADAPTER_IF_ETH:
            return Esp32_Ethernet_Open(configIndex, pConfig);
#endif
        default:
            break;
    }
	return SOCK_SOCKET_ERROR;
}

bool Network_Interface_Close(int configIndex)
{
     switch((tcpip_adapter_if_t)configIndex)
    {
        // Wireless 
        case TCPIP_ADAPTER_IF_STA:
            return Esp32_Wireless_Close(configIndex);

       // Soft AP
       case TCPIP_ADAPTER_IF_AP:
            return Esp32_WirelessAP_Close(configIndex);

#if ESP32_ETHERNET_SUPPORT
        // Ethernet
        case TCPIP_ADAPTER_IF_ETH:
            return Esp32_Ethernet_Close(configIndex);
#endif
        default:
            break;

    }
	return false; 
}


bool Network_Interface_Start_Scan(int configIndex)
{
    switch((tcpip_adapter_if_t)configIndex)
    {
        // Wireless 
        case TCPIP_ADAPTER_IF_STA:
            return (Esp32_Wireless_Scan() == 0);

    }

    return false;
}

bool GetWirelessConfig(int configIndex, HAL_Configuration_Wireless80211 ** pWireless)
{
    HAL_Configuration_NetworkInterface * pConfig;

    // Check index in range
    if ( g_TargetConfiguration.NetworkInterfaceConfigs->Count <= configIndex ) return false;

    // load network interface configuration from storage
    pConfig = g_TargetConfiguration.NetworkInterfaceConfigs->Configs[configIndex];
    if ( pConfig->InterfaceType != NetworkInterfaceType_Wireless80211) return false;

    *pWireless = ConfigurationManager_GetWirelessConfigurationFromId(pConfig->SpecificConfigId);

    return true;
}

//
//  Connect to wireless network SSID using passphase
//
int  Network_Interface_Connect(int configIndex, const char * ssid, const char * passphase, int options)
{    
    HAL_Configuration_Wireless80211 * pWireless;

    if ( GetWirelessConfig(configIndex,  & pWireless) == false )  return SOCK_SOCKET_ERROR;
    
    pWireless->Options = Wireless80211Configuration_ConfigurationOptions_Enable;

    if ( options & NETWORK_CONNECT_RECONNECT)
    {
        // need this stupid cast because the current gcc version with ESP32 IDF is not happy with the simple sintax '|='
        pWireless->Options = 
            (Wireless80211Configuration_ConfigurationOptions)
            (pWireless->Options | Wireless80211Configuration_ConfigurationOptions_AutoConnect);
    }
    else
    {
        // need this stupid cast because the current gcc version with ESP32 IDF is not happy with the simple sintax '^='
        pWireless->Options = 
            (Wireless80211Configuration_ConfigurationOptions)
            (pWireless->Options ^ Wireless80211Configuration_ConfigurationOptions_AutoConnect);
    }

    // Update Wireless structure with new SSID and passphase
    hal_strcpy_s( (char *)pWireless->Ssid, sizeof(pWireless->Ssid), ssid );
    hal_strcpy_s( (char *)pWireless->Password, sizeof(pWireless->Password), passphase );

    // Option to Save new config 
    if ( options & NETWORK_CONNECT_SAVE_CONFIG)
    {
        StoreConfigBlock( DeviceConfigurationOption_Wireless80211Network, configIndex, pWireless, sizeof(HAL_Configuration_Wireless80211) );
    }

    switch((tcpip_adapter_if_t)configIndex)
    {
        // Wireless 
        case TCPIP_ADAPTER_IF_STA:
            esp_err_t err = Esp32_Wireless_Connect( pWireless);

            return (int)err;

    }

    return SOCK_SOCKET_ERROR;
}

int  Network_Interface_Disconnect(int configIndex)
{ 
    switch((tcpip_adapter_if_t)configIndex)
    {
        // Wireless 
        case TCPIP_ADAPTER_IF_STA:
            esp_err_t err = Esp32_Wireless_Disconnect();

            return (err == ESP_OK);

    }

    return false;
}


wifi_sta_info_t wireless_sta[ESP_WIFI_MAX_CONN_NUM] = { 0 };

//
//	Update save stations with rssi
//
void Network_Interface_update_Stations()
{
	esp_err_t ec;
	wifi_sta_list_t stations;

	ec = esp_wifi_ap_get_sta_list(&stations);

	if (ec == ESP_OK)
	{
		// Find save station and update
		for (int x = 0; x < stations.num; x++)
		{
			for (int y = 0; y < ESP_WIFI_MAX_CONN_NUM; y++)
			{
				if (wireless_sta[y].reserved)
				{
					if (memcmp( wireless_sta[y].mac, stations.sta[x].mac, 6) == 0 )
					{
						memcpy(&wireless_sta[y], &stations.sta[x], sizeof(wifi_sta_info_t));
						wireless_sta[y].reserved = 1;
						break;
					}
				}
			}
		}

	}
}

//
//	Add a station to our list of Stations
//
void Network_Interface_Add_Station(uint16_t index, uint8_t * macAddress)
{
	if (index < ESP_WIFI_MAX_CONN_NUM)
	{
		memcpy( wireless_sta[index].mac, macAddress , 6);
		wireless_sta[index].reserved = 1;
		Network_Interface_update_Stations();
	}
}

//
// Remove a station from our list of stations
void Network_Interface_Remove_Station(uint16_t index)
{
	if (index < ESP_WIFI_MAX_CONN_NUM)
	{
		wireless_sta[index].reserved = 0;
		Network_Interface_update_Stations();
	}
}

// Return the maximum number of stations
int Network_Interface_Max_Stations()
{
	return ESP_WIFI_MAX_CONN_NUM;
}

//
//  
//
bool Network_Interface_Get_Station(uint16_t index, uint8_t * macAddress, uint8_t * rssi, uint32_t * phyModes)
{
	if (wireless_sta[index].reserved)
	{
		memcpy(macAddress, wireless_sta[index].mac,  6);
		*rssi = wireless_sta[index].rssi;
		*phyModes =  wireless_sta[index].phy_11b | 
					(wireless_sta[index].phy_11g << 1) | 
					(wireless_sta[index].phy_11n << 2) | 
					(wireless_sta[index].phy_lr << 3);
		return true;
	}

	return false;
}

//
// De-Auth connected stations
//
void Network_Interface_Deauth_Station(uint16_t stationIndex)
{
	stationIndex++;
	if (stationIndex>=1 && stationIndex<= ESP_WIFI_MAX_CONN_NUM)
    {
		esp_wifi_deauth_sta(stationIndex);
    }
}
