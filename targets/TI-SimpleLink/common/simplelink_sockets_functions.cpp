//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Texas Instruments Incorporated.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoHAL_ConfigurationManager.h>
#include "simplelink_sockets.h"

#include <ti/drivers/net/wifi/simplelink.h>

#define NATIVE_PROFILE_PAL_NETWORK()

bool HAL_SOCK_Initialize()
{
    NATIVE_PROFILE_PAL_NETWORK();
    return SimpleLink_SOCKETS_Initialize();
}

bool HAL_SOCK_Uninitialize()
{
    NATIVE_PROFILE_PAL_NETWORK();
    return SimpleLink_SOCKETS_Uninitialize();
}

HRESULT HAL_SOCK_CONFIGURATION_LoadAdapterConfiguration(HAL_Configuration_NetworkInterface* config, uint32_t interfaceIndex)
{
    NATIVE_PROFILE_PAL_NETWORK();
    
    uint16_t configOptions;
    uint16_t ipLen = sizeof(SlNetCfgIpV4Args_t);
    SlNetCfgIpV4Args_t ipV4 = {0};

    if(config->StartupAddressMode == AddressMode_DHCP)
    {
        // if this is on DHCP we need to load whatever we have on top of the stored configuration
#if LWIP_IPV6
        {
            // FIXME IPV6
            // config->IPv6Address     = networkInterface->ip_addr.u_addr.ip6.addr;
            // config->IPv6NetMask = networkInterface->netmask.u_addr.ip6.addr;
            // config->IPv6GatewayAddress    = networkInterface->gw.u_addr.ip6.addr;
        }
#else
        if (sl_NetCfgGet(SL_NETCFG_IPV4_STA_ADDR_MODE, &configOptions, &ipLen, (uint8_t *)&ipV4) >= 0)
        {
            config->IPv4Address         = sl_Ntohl(ipV4.Ip);
            config->IPv4NetMask         = sl_Ntohl(ipV4.IpMask);
            config->IPv4GatewayAddress  = sl_Ntohl(ipV4.IpGateway);
            config->IPv4DNSAddress1     = sl_Ntohl(ipV4.IpDnsServer);
            config->IPv4DNSAddress2     = 0;
        }
        else
        {
            config->IPv4Address         = 0;
            config->IPv4NetMask         = 0;
            config->IPv4GatewayAddress  = 0;
            config->IPv4DNSAddress1     = 0;
            config->IPv4DNSAddress2     = 0;
        }        
#endif
    }
    
    return S_OK;
}

HRESULT HAL_SOCK_CONFIGURATION_UpdateAdapterConfiguration(HAL_Configuration_NetworkInterface* config,  uint32_t interfaceIndex, uint32_t updateFlags)
{
    NATIVE_PROFILE_PAL_NETWORK();

    uint16_t configOptions;
    uint16_t ipLen = sizeof(SlNetCfgIpV4Args_t);
    SlNetCfgIpV4Args_t ipV4 = {0};

    bool enableDHCP = (config->StartupAddressMode == AddressMode_DHCP);

    if (sl_NetCfgGet(SL_NETCFG_IPV4_STA_ADDR_MODE, &configOptions, &ipLen, (uint8_t *)&ipV4) >= 0)
    {
        // when using DHCP do not use the static settings
        if(0 != (updateFlags & NetworkInterface_UpdateOperation_Dns))
        {
            // FIXME IPV6
            if(config->AutomaticDNS == 0)
            {
                // user defined DNS addresses
                if(config->IPv4DNSAddress1 != 0)
                {
                    ipV4.IpDnsServer = sl_Htonl(config->IPv4DNSAddress1);
                }

                // this interface doesn't have a secondary DNS

                // update configuration
                if(sl_NetCfgSet(SL_NETCFG_IPV4_STA_ADDR_MODE, SL_NETCFG_ADDR_STATIC, sizeof(SlNetCfgIpV4Args_t), (uint8_t*)&ipV4) < 0)
                {
                    return CLR_E_FAIL;
                }            
            }
        }

        if(0 != (updateFlags & NetworkInterface_UpdateOperation_Dhcp))
        {
            if(enableDHCP)
            {
                // need to start DHCP
                if(sl_NetCfgSet(SL_NETCFG_IPV4_STA_ADDR_MODE, SL_NETCFG_ADDR_DHCP ,0 ,0 ) < 0)
                {
                    return CLR_E_FAIL;
                }
            }
            else
            {
                // we should be polite and let the DHCP server that we are now using a static IP
                sl_NetCfgSet(SL_NETCFG_IPV4_STA_ADDR_MODE, SL_NETCFG_ADDR_RELEASE_IP_SET, 0, 0); 

                ipV4.Ip          = sl_Htonl(config->IPv4Address);
                ipV4.IpMask      = sl_Htonl(config->IPv4NetMask);
                ipV4.IpGateway   = sl_Htonl(config->IPv4GatewayAddress);
                ipV4.IpDnsServer = sl_Htonl(config->IPv4DNSAddress1);

                if(sl_NetCfgSet(SL_NETCFG_IPV4_STA_ADDR_MODE, SL_NETCFG_ADDR_STATIC, sizeof(SlNetCfgIpV4Args_t), (uint8_t*)&ipV4) < 0)
                {
                    return CLR_E_FAIL;
                }

            }
        }

        if(enableDHCP)
        {
            if(0 != (updateFlags & NetworkInterface_UpdateOperation_DhcpRelease))
            {
                sl_NetCfgSet(SL_NETCFG_IPV4_STA_ADDR_MODE, SL_NETCFG_ADDR_RELEASE_IP_SET, 0, 0); 
            }
            else if(0 != (updateFlags & NetworkInterface_UpdateOperation_DhcpRenew))
            {
                //dhcp_renew(networkInterface);
            }
            else if(0 != (updateFlags & (NetworkInterface_UpdateOperation_DhcpRelease | NetworkInterface_UpdateOperation_DhcpRenew)) )
            {
                return CLR_E_INVALID_PARAMETER;
            }
        }

        if(0 != (updateFlags & NetworkInterface_UpdateOperation_Mac))
        {
            uint8_t macAddress[SL_MAC_ADDR_LEN];

            memcpy(macAddress, config->MacAddress, SL_MAC_ADDR_LEN);

            if(sl_NetCfgSet(SL_NETCFG_MAC_ADDRESS_SET, 1, SL_MAC_ADDR_LEN, (uint8_t*)macAddress) < 0)
            {
                return CLR_E_FAIL;
            }
        }     
    }

    // all changes require SL reinit
    sl_Stop(0);
    sl_Start(NULL,NULL,NULL);

    return S_OK;
}

void HAL_SOCK_EventsSet( uint32_t events ) 
{
    NATIVE_PROFILE_PAL_NETWORK();
    ASSERT( (events == SOCKET_EVENT_FLAG_SOCKET) || (events == SOCKET_EVENT_FLAG_SOCKETS_READY));

    Events_Set( SYSTEM_EVENT_FLAG_SOCKET );
}

void * HAL_SOCK_GlobalLockContext() 
{
    NATIVE_PROFILE_PAL_NETWORK();
    return NULL;
}
