
//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <lwip/netifapi.h>

extern "C" struct netif * nf_getNetif();
//
// Works with the Target_NetworkConfig to map the Network_Interface_XXXXX calls to the correct driver


bool Network_Interface_Bind(int index)
{
    return true;
}

int  Network_Interface_Open(int index)
{
    HAL_Configuration_NetworkInterface * config = g_TargetConfiguration.NetworkInterfaceConfigs->Configs[index];

    switch(index)
    {
        case 0:
        {
            // Open the network interface and set its config
            // TODO / FIXME

            // Return index to NetIF in its linked list, return 0 (probably right if only interface)
            // This used by Network stack to hook in to status/address changes for events to users

            // For now get the Netif number form original Chibios binding code
            struct netif * nptr = nf_getNetif();
            return nptr->num; 
        }
        break;
    }
	return SOCK_SOCKET_ERROR;
}

bool Network_Interface_Close(int index)
{
     switch(index)
    {
        case 0:
            return true;
    }
	return false; 
}