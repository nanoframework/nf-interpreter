
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


bool Network_Interface_Bind(int index)
{
    return true;
}

int  Network_Interface_Open(int index)
{
    ets_printf( "Network_Interface_Open %d\n", index);
    HAL_Configuration_NetworkInterface config;

    // load network interface configuration from storage
    if(!ConfigurationManager_GetConfigurationBlock((void*)&config, DeviceConfigurationOption_Network, index))
    {
        // failed to load configuration
        // FIXME output error?
        return SOCK_SOCKET_ERROR;
    }
    _ASSERTE(config.StartupAddressMode > 0);

    switch((tcpip_adapter_if_t)index)
    {
        // Wireless 
        case TCPIP_ADAPTER_IF_STA:
            return Esp32_Wireless_Open(index, &config);

       // Soft AP
        case TCPIP_ADAPTER_IF_AP:
            break;

#if ESP32_ETHERNET_SUPPORT
        // Ethernet
        case TCPIP_ADAPTER_IF_ETH:
            return Esp32_Ethernet_Open(index, &config);
#endif
    }
	return SOCK_SOCKET_ERROR;
}

bool Network_Interface_Close(int index)
{
     switch((tcpip_adapter_if_t)index)
    {
        // Wireless 
        case TCPIP_ADAPTER_IF_STA:
            return Esp32_Wireless_Close(index);

       // Soft AP
       case TCPIP_ADAPTER_IF_AP:
            break;

#if ESP32_ETHERNET_SUPPORT
        // Ethernet
        case TCPIP_ADAPTER_IF_ETH:
            return Esp32_Ethernet_Close(index);
#endif

    }
	return false; 
}
