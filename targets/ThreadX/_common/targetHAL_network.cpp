//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// This file includes the platform and board specific Network Intialisation

#include <nanoHAL.h>
#include <nf_netxduo.h>
#if defined(WIFI_DRIVER_ISM43362) && defined(I_AM_NANOCLR)
#include <wifi.h>
#endif

// this is the declaration for the callback implement in nf_sys_arch.c
// extern "C" void set_signal_sock_function( void (*funcPtr)() );

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
    // TODO NETWORK
    // set_signal_sock_function( &sys_signal_sock_event );

    // get network configuration, if available
    if (g_TargetConfiguration.NetworkInterfaceConfigs->Count == 0)
    {
        // there is no networking configuration block, can't proceed
        return;
    }

    HAL_Configuration_NetworkInterface networkConfig;

    if (ConfigurationManager_GetConfigurationBlock((void *)&networkConfig, DeviceConfigurationOption_Network, 0) ==
        true)
    {
        NF_NetXDuo_Init(&networkConfig);
        // TODO NETWORK
        // // build lwIP configuration
        // lwipthread_opts lwipOptions;

        // // init config
        // memset(&lwipOptions, 0, sizeof(lwipOptions));

        // // grab MAC address
        // lwipOptions.macaddress = (uint8_t *)networkConfig.MacAddress;

        // // static or dinamic address
        // if(networkConfig.StartupAddressMode == AddressMode_Static)
        // {
        //     // IPv4 configs
        //     lwipOptions.address = networkConfig.IPv4Address;
        //     lwipOptions.netmask = networkConfig.IPv4NetMask;
        //     lwipOptions.gateway = networkConfig.IPv4GatewayAddress;
        // }
        // else if(networkConfig.StartupAddressMode == AddressMode_DHCP)
        // {
        //     // clear  IPv4 configs
        //     lwipOptions.address = 0;
        //     lwipOptions.netmask = 0;
        //     lwipOptions.gateway = 0;
        // }

        // // set address mode option
        // // our enum follows lwIP defines for address mode
        // lwipOptions.addrMode = (net_addr_mode_t)networkConfig.StartupAddressMode;

        // // we could consider making the device name configurable (or maybe not)
        // lwipOptions.ourHostName = "nanodevice";

        // Start lwIP thread in ChibiOS bindings using the above options
        // lwipInit(&lwipOptions);

#if defined(WIFI_DRIVER_ISM43362) && defined(I_AM_NANOCLR)

        // // check if we have the MAC address stored in the configuration block
        // if (networkConfig.MacAddress[0] == 0xFF)
        // {
        //     // OK to ignore the return value, no harm done if it fails
        //     if (WIFI_GetMAC_Address(&networkConfig.MacAddress[0]) == WIFI_STATUS_OK)
        //     {
        //         // store the MAC address in the configuration block
        //         ConfigurationManager_UpdateConfigurationBlock(
        //             &networkConfig,
        //             DeviceConfigurationOption_Network,
        //             0);
        //     }
        // }

#endif

    }
}

void nanoHAL_Network_Uninitialize()
{
    NF_NetXDuo_UnInit();
}