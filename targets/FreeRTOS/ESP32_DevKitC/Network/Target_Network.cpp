
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

int  Network_Interface_Open(int configIndex)
{
    ets_printf( "Network_Interface_Open %d\n", configIndex);
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

    }
	return false; 
}
