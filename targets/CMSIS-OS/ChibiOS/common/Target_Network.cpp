
//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <lwip/netifapi.h>

extern NETWORK_CONFIG  g_NetworkConfig;

extern "C" struct netif * nf_getNetif();
//
// Works with the Target_NetworkConfig to map the Network_Interface_XXXXX calls to the correct driver
// This works in conjunction with the g_NetworkConfig in the Target_NetworkConfig.cpp



bool Network_Interface_Bind(int index)
{
    return true;
}

int  Network_Interface_Open(int index)
{
    SOCK_NetworkConfiguration * config = &g_NetworkConfig.NetworkInterfaces[index];

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