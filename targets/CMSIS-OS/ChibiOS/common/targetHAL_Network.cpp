//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

// This file includes the platform and board specific Network Intialisation

#include <nanoHAL.h>
#include <lwipthread.h>
#include <Target_BlockStorage_STM32FlashDriver.h>


// this is the declaration for the callback implement in nf_sys_arch.c 
extern "C" void set_signal_sock_function( void (*funcPtr)() );

//
// Callback from lwIP on event
//
void sys_signal_sock_event()
{
     Events_Set(SYSTEM_EVENT_FLAG_SOCKET);
}

void nanoHAL_Network_Initialize()
{
    // Initialise the lwIP CLR signal callback
    set_signal_sock_function( &sys_signal_sock_event );

    // get network configuration
    Configuration_Network* networkConfig = (Configuration_Network*)platform_malloc(sizeof(Configuration_Network));
    
    if(GetConfigurationNetwork(networkConfig) == true)
    {
        // build lwIP configuration 
        struct lwipthread_opts lwipOptions;

        // grab MAC address
        lwipOptions.macaddress = (uint8_t *)networkConfig->MacAddress;

        // static or dinamic address
        if(networkConfig->StartupAddressMode == AddressMode_Static)
        {
            // IPv4 configs
            lwipOptions.address = networkConfig->IPv4Address;
            lwipOptions.netmask = networkConfig->IPv4NetMask;
            lwipOptions.gateway = networkConfig->IPv4GatewayAddress;
        }
        else if(networkConfig->StartupAddressMode == AddressMode_DHCP)
        {
            // clear  IPv4 configs
            lwipOptions.address = 0;
            lwipOptions.netmask = 0;
            lwipOptions.gateway = 0;
        }

        // set address mode option
        // our enum follows lwIP defines for address mode
        lwipOptions.addrMode = (net_addr_mode_t)networkConfig->StartupAddressMode;

        // we could consider making the device name configurable (or maybe not)
        lwipOptions.ourHostName = "nanodevice";

        // Start lwIP thread in ChibiOS bindings using the above options
        lwipInit(&lwipOptions);

        // free memory
        platform_free(networkConfig);
    }
}

// Gets the network configuration block from the configuration block stored in the flash sector, 
// it's implemented with 'weak' attribute so it can be replaced at target level if a different persistance mechanism is used
__nfweak bool GetConfigurationNetwork(Configuration_Network* configurationNetwork)
{
    // copy the config block content to the pointer in the argument
    memcpy(configurationNetwork, (Configuration_Network*)&__nanoConfig_start__, sizeof(Configuration_Network));

    return TRUE;
}

// Stored the network configuration block to the configuration block stored in the flash sector, 
// it's implemented with 'weak' attribute so it can be replaced at target level if a different persistance mechanism is used
__nfweak bool StoreConfigurationNetwork(Configuration_Network* configurationNetwork)
{
    // copy the config block content to the config block storage
    return STM32FlashDriver_Write(NULL, (ByteAddress)&__nanoConfig_start__, sizeof(Configuration_Network), (unsigned char*)configurationNetwork, true);
}
