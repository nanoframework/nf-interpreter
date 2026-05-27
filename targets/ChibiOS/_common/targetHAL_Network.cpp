//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// This file includes the platform and board specific Network Intialisation

#include <nanoHAL.h>

#if defined(RP2040) || defined(RP2350)
extern "C" {
#include <nf_lwipthread_wifi.h>
}
#else
#include <nf_lwipthread.h>
#endif

// this is the declaration for the callback implement in nf_sys_arch.c
extern "C" void set_signal_sock_function(void (*funcPtr)());

// buffer with host name
static char hostName[18] = "nanodevice_";

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
    set_signal_sock_function(&sys_signal_sock_event);

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
        // build lwIP configuration
        lwipthread_opts lwipOptions;

        // init config
        memset(&lwipOptions, 0, sizeof(lwipOptions));

#if defined(RP2040) || defined(RP2350)
        // WiFi targets: use CYW43 chip MAC if available
        uint8_t wifiMac[6];
        cyw43_wifi_get_mac_addr(wifiMac);

        if (wifiMac[0] != 0 || wifiMac[1] != 0 || wifiMac[2] != 0)
        {
            lwipOptions.macaddress = wifiMac;
        }
        else
        {
            lwipOptions.macaddress = (uint8_t *)networkConfig.MacAddress;
        }
#else
        // grab MAC address
        lwipOptions.macaddress = (uint8_t *)networkConfig.MacAddress;
#endif

        // static or dynamic address
        if (networkConfig.StartupAddressMode == AddressMode_Static)
        {
            // IPv4 configs
            lwipOptions.address = networkConfig.IPv4Address;
            lwipOptions.netmask = networkConfig.IPv4NetMask;
            lwipOptions.gateway = networkConfig.IPv4GatewayAddress;
        }
        else if (networkConfig.StartupAddressMode == AddressMode_DHCP)
        {
            // clear IPv4 configs
            lwipOptions.address = 0;
            lwipOptions.netmask = 0;
            lwipOptions.gateway = 0;
        }

        // set address mode option
        // our enum follows lwIP defines for address mode
        lwipOptions.addrMode = (net_addr_mode_t)networkConfig.StartupAddressMode;

        // compose host name with nanodevice and last 3 bytes of MAC address
        // nanodevice_XXXXXX
        char *macPosition = hostName + 11;

        // copy over last 3 bytes of MAC address
        for (int index = 3; index < 6; index++)
        {
            sprintf(macPosition, "%02X", (int)lwipOptions.macaddress[index]);
            macPosition += 2;
        }

        // set host name
        lwipOptions.ourHostName = hostName;

        // Start lwIP thread in ChibiOS bindings using the above options
        lwIPInit(&lwipOptions);

#if defined(RP2040) || defined(RP2350)
        // Ensure CYW43 WiFi is powered on and set netif MAC from chip
        cyw43_ensure_wifi_up_impl();

        {
            uint8_t chipMac[6];
            cyw43_wifi_get_mac_addr(chipMac);
            if (chipMac[0] != 0 || chipMac[1] != 0 || chipMac[2] != 0)
            {
                cyw43_wifi_set_netif_mac(chipMac);
            }
        }
#endif
    }
}

void nanoHAL_Network_Uninitialize()
{
#if defined(RP2040) || defined(RP2350)
    cyw43_wifi_power_off();
#endif
}
