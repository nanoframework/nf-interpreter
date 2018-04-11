
//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include "Esp32_os.h"

extern NETWORK_CONFIG  g_NetworkConfig;

//
// Works with the Target_NetworkConfig to map the Network_Interface_XXXXX calls to the correct driver
// This works in conjunction with the g_NetworkConfig in the Target_NetworkConfig.cpp
	
int  Esp32_Wireless_Open(int index, SOCK_NetworkConfiguration * config);
bool Esp32_Wireless_Close(int index);
int  Esp32_Ethernet_Open(int index, SOCK_NetworkConfiguration * config);
bool Esp32_Ethernet_Close(int index);


bool Network_Interface_Bind(int index)
{
    return true;
}

int  Network_Interface_Open(int index)
{
    ets_printf( "Network_Interface_Open %d\n", index);
    SOCK_NetworkConfiguration * config = &g_NetworkConfig.NetworkInterfaces[index];

    switch((tcpip_adapter_if_t)index)
    {
        // Wireless 
        case TCPIP_ADAPTER_IF_STA:
            return Esp32_Wireless_Open(index, config);

       // Soft AP
        case TCPIP_ADAPTER_IF_AP:
            break;

#if ESP32_ETHERNET_SUPPORT
        // Ethernet
        case TCPIP_ADAPTER_IF_ETH:
            return Esp32_Ethernet_Open(index, config);
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