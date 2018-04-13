//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

// This file includes the platform and board specific Network Intialisation

#include <nanoHAL.h>
#include <lwipthread.h>


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
    Configuration_NetworkInterface* networkConfig = (Configuration_NetworkInterface*)platform_malloc(sizeof(Configuration_NetworkInterface));
    
    if(ConfigurationManager_GetConfigurationBlock(networkConfig, DeviceConfigurationOption_Network, 0) == true)
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
