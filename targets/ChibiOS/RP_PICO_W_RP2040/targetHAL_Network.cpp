//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// WiFi-aware targetHAL_Network for ChibiOS + CYW43 (Pico W).
// This replaces the Ethernet-based _common/targetHAL_Network.cpp.

#include <nanoHAL.h>

extern "C" {
#include <nf_lwipthread_wifi.h>
}

// callback from nf_sys_arch.c
extern "C" void set_signal_sock_function(void (*funcPtr)());

// buffer with host name
static char hostName[18] = "nanodevice_";

// Callback from lwIP on event
void sys_signal_sock_event()
{
    Events_Set(SYSTEM_EVENT_FLAG_SOCKET);
}

void nanoHAL_Network_Initialize()
{
    // Initialize the lwIP CLR signal callback
    set_signal_sock_function(&sys_signal_sock_event);

    // get network configuration, if available
    if (g_TargetConfiguration.NetworkInterfaceConfigs->Count == 0)
    {
        // no networking configuration block — can't proceed
        return;
    }

    HAL_Configuration_NetworkInterface networkConfig;

    if (ConfigurationManager_GetConfigurationBlock((void *)&networkConfig, DeviceConfigurationOption_Network, 0) ==
        true)
    {
        // build lwIP configuration
        lwipthread_opts lwipOptions;
        memset(&lwipOptions, 0, sizeof(lwipOptions));

        // For WiFi, get MAC address from the CYW43 chip
        uint8_t wifiMac[6];
        cyw43_wifi_get_mac_addr(wifiMac);

        // Use CYW43 MAC if available, otherwise fall back to stored config
        if (wifiMac[0] != 0 || wifiMac[1] != 0 || wifiMac[2] != 0)
        {
            lwipOptions.macaddress = wifiMac;
        }
        else
        {
            lwipOptions.macaddress = (uint8_t *)networkConfig.MacAddress;
        }

        // static or dynamic address
        if (networkConfig.StartupAddressMode == AddressMode_Static)
        {
            lwipOptions.address = networkConfig.IPv4Address;
            lwipOptions.netmask = networkConfig.IPv4NetMask;
            lwipOptions.gateway = networkConfig.IPv4GatewayAddress;
        }
        else if (networkConfig.StartupAddressMode == AddressMode_DHCP)
        {
            lwipOptions.address = 0;
            lwipOptions.netmask = 0;
            lwipOptions.gateway = 0;
        }

        // set address mode option
        lwipOptions.addrMode = (net_addr_mode_t)networkConfig.StartupAddressMode;

        // compose host name: nanodevice_XXXXXX (last 3 bytes of MAC)
        char *macPosition = hostName + 11;
        for (int index = 3; index < 6; index++)
        {
            sprintf(macPosition, "%02X", (int)lwipOptions.macaddress[index]);
            macPosition += 2;
        }

        lwipOptions.ourHostName = hostName;

        // Start WiFi lwIP thread
        lwIPInit(&lwipOptions);
    }
}

void nanoHAL_Network_Uninitialize()
{
    cyw43_wifi_power_off();
}
