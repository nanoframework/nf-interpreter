
//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <lwip/netifapi.h>
#include <lwip/dns.h>

#if defined(RP2040) || defined(RP2350)
extern "C" {
#include <nf_lwipthread_wifi.h>
}
#else
extern "C" struct netif *nf_getNetif();
#endif

//
// Works with the Target_NetworkConfig to map the Network_Interface_XXXXX calls to the correct driver

bool Network_Interface_Bind(int index)
{
    (void)index;

    return true;
}

int Network_Interface_Open(int index)
{
    HAL_Configuration_NetworkInterface networkConfiguration;

    // load network interface configuration from storage
    if (!ConfigurationManager_GetConfigurationBlock(
            (void *)&networkConfiguration,
            DeviceConfigurationOption_Network,
            index))
    {
        // failed to load configuration
        // FIXME output error?
        return SOCK_SOCKET_ERROR;
    }
    _ASSERTE(networkConfiguration.StartupAddressMode > 0);

    switch (index)
    {
        case 0:
        {
            // Open the network interface and set its config
            // TODO / FIXME

            // Return index to NetIF in its linked list, return 0 (probably right if only interface)
            // This used by Network stack to hook in to status/address changes for events to users

            // For now get the Netif number form original Chibios binding code
            struct netif *nptr = nf_getNetif();
            return nptr->num;
        }
        break;
    }
    return SOCK_SOCKET_ERROR;
}

bool Network_Interface_Close(int index)
{
    switch (index)
    {
        case 0:
#if defined(RP2040) || defined(RP2350)
            cyw43_wifi_disconnect();
#else
            macStop(&ETHD1);
#endif
            return true;
    }
    return false;
}

#if defined(RP2040) || defined(RP2350)

int Network_Interface_Disconnect(int index)
{
    (void)index;
    return cyw43_wifi_disconnect();
}

int Network_Interface_Start_Connect(int index, const char *ssid, const char *passphrase, int options)
{
    (void)index;
    (void)options;

    uint32_t auth_type = 0;
    if (passphrase != NULL && passphrase[0] != '\0')
    {
        auth_type = 0x00400004; // WPA2_AES_PSK
    }

    return cyw43_wifi_connect(ssid, passphrase, auth_type);
}

int Network_Interface_Connect_Result(int configIndex)
{
    (void)configIndex;

    if (!cyw43_wifi_is_connected())
        return -1;

    if (cyw43_wifi_get_ip4_address() == 0)
        return -1;

    // DHCP has completed — sync the live netif values (IP, gateway, netmask,
    // DNS) into the in-memory config block so that managed code reads the
    // correct addresses via ConfigurationManager_GetConfigurationBlock.
    if (g_TargetConfiguration.NetworkInterfaceConfigs != NULL &&
        g_TargetConfiguration.NetworkInterfaceConfigs->Count > 0)
    {
        HAL_Configuration_NetworkInterface *cfg =
            g_TargetConfiguration.NetworkInterfaceConfigs->Configs[0];

        struct netif *nif = nf_getNetif();
        if (nif != NULL)
        {
            cfg->IPv4Address = nif->ip_addr.addr;
            cfg->IPv4NetMask = nif->netmask.addr;
            cfg->IPv4GatewayAddress = nif->gw.addr;

#if LWIP_DNS
            cfg->IPv4DNSAddress1 = dns_getserver(0)->addr;
            cfg->IPv4DNSAddress2 = dns_getserver(1)->addr;
#endif
        }
    }

    return 0;
}

int Network_Interface_Start_Scan(int index)
{
    (void)index;

    cyw43_wifi_scan_start();
    return 0;
}

#endif
