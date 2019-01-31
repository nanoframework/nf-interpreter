
//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include "Esp32_os.h"


//
// Works with the Target_NetworkConfig to map the Network_Interface_XXXXX calls to the correct driver
	
int  Esp32_Wireless_Open(int index, HAL_Configuration_NetworkInterface * config);
bool Esp32_Wireless_Close(int index);
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

    // Check valid config
//    _ASSERTE(pConfig->StartupAddressMode > 0);

    switch((tcpip_adapter_if_t)configIndex)
    {
        // Wireless 
        case TCPIP_ADAPTER_IF_STA:
            return Esp32_Wireless_Open(configIndex, pConfig);

       // Soft AP
        case TCPIP_ADAPTER_IF_AP:
            break;

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
            break;

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
    
    // TODO update Reconnect option ( ) - first we need a space for it in config block
    //pWireless->reconnect = options & NETWORK_CONNECT_RECONNECT;

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