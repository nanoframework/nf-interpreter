
//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <lwip/netif.h>

extern "C" struct netif * nf_getNetif();

bool Network_Interface_Bind(int index)
{
    (void)index;

    return true;
}

int  Network_Interface_Open(int index)
{
    HAL_Configuration_NetworkInterface networkConfiguration;

    // load network interface configuration from storage
    if(!ConfigurationManager_GetConfigurationBlock((void*)&networkConfiguration, DeviceConfigurationOption_Network, index))
    {
        // failed to load configuration
        // FIXME output error?
        return SOCK_SOCKET_ERROR;
    }
    _ASSERTE(networkConfiguration.StartupAddressMode > 0);

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