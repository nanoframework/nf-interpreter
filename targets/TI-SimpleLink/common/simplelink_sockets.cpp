//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright (c) Texas Instruments Incorporated.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


#include "simplelink_sockets.h"

// extern "C"
// {
// #include "lwip\init.h"
// #include "lwip\tcpip.h"
// #include "lwip\dns.h"
// #include "lwip\netifapi.h"
// #include "lwip\Netdb.h"
// #include "lwip\tcp.h"
// #include "lwip\Sockets.h"
// #include "lwip\dhcp.h"
// #include "lwip\netif.h"
// }


//--// 

#if defined(DEBUG)
#define DEBUG_HANDLE_SOCKET_ERROR(t,a) 
// assume there is something to add in later??
#else
#define DEBUG_HANDLE_SOCKET_ERROR(t,a) 
#endif

//--//

SimpleLink_SOCKETS_Driver g_SimpleLink_SOCKETS_Driver;

//--//
static HAL_CONTINUATION PostAddressChangedContinuation;
static HAL_CONTINUATION PostAvailabilityOnContinuation;
static HAL_CONTINUATION PostAvailabilityOffContinuation;

void SimpleLink_SOCKETS_Driver::PostAddressChanged(void* arg)
{
    (void)arg;

	Network_PostEvent(NetworkEventType_AddressChanged, 0);
}

void SimpleLink_SOCKETS_Driver::PostAvailabilityOn(void* arg)
{
    (void)arg;

	Network_PostEvent(NetworkEventType_AvailabilityChanged, NetworkEventFlags_NetworkAvailable);
}

void SimpleLink_SOCKETS_Driver::PostAvailabilityOff(void* arg)
{
    (void)arg;

	Network_PostEvent(NetworkEventType_AvailabilityChanged, NetworkEventFlags_NetworkNOTAvailable);
}

void SimpleLink_SOCKETS_Driver::Link_callback()
{
	// if (netif_is_link_up(netif))
	// {
	// 	if (!PostAvailabilityOnContinuation.IsLinked())
	// 		PostAvailabilityOnContinuation.Enqueue();
	// }
	// else
	// {
	// 	if (!PostAvailabilityOffContinuation.IsLinked())
	// 		PostAvailabilityOffContinuation.Enqueue();
	// }
    Events_Set(SYSTEM_EVENT_FLAG_SOCKET);
    Events_Set(SYSTEM_EVENT_FLAG_NETWORK);
}

void SimpleLink_SOCKETS_Driver::Status_callback()
{
	if (!PostAddressChangedContinuation.IsLinked())
		PostAddressChangedContinuation.Enqueue();

#if !defined(BUILD_RTM)
	// lcd_printf("\f\n\n\n\n\n\nLink Update: %s\n", (netif_is_up(netif) ? "UP  " : "DOWN"));
	// lcd_printf("         IP: %d.%d.%d.%d\n", (netif->ip_addr.addr >> 0) & 0xFF,
	// 	(netif->ip_addr.addr >> 8) & 0xFF,
	// 	(netif->ip_addr.addr >> 16) & 0xFF,
	// 	(netif->ip_addr.addr >> 24) & 0xFF);
	// lcd_printf("         SM: %d.%d.%d.%d\n", (netif->netmask.addr >> 0) & 0xFF,
	// 	(netif->netmask.addr >> 8) & 0xFF,
	// 	(netif->netmask.addr >> 16) & 0xFF,
	// 	(netif->netmask.addr >> 24) & 0xFF);
	// lcd_printf("         GW: %d.%d.%d.%d\n", (netif->gw.addr >> 0) & 0xFF,
	// 	(netif->gw.addr >> 8) & 0xFF,
	// 	(netif->gw.addr >> 16) & 0xFF,
	// 	(netif->gw.addr >> 24) & 0xFF);

//FIXME debug_printf("IP Address: %d.%d.%d.%d\n", (netif->ip_addr.u_addr.ip4.addr >> 0) & 0xFF,
// 	(netif->ip_addr.u_addr.ip4.addr >> 8) & 0xFF,
// 	(netif->ip_addr.u_addr.ip4.addr >> 16) & 0xFF,
// 	(netif->ip_addr.u_addr.ip4.addr >> 24) & 0xFF);
#if LWIP_DNS
	if (netif->flags & NETIF_FLAG_ETHARP)
	{
		//ip_addr_t * dns1 = dns_getserver(0);
		//ip_addr_t * dns2 = dns_getserver(1);

		// lcd_printf("         dns1: %d.%d.%d.%d\n", (dns1.addr >> 0) & 0xFF,
		// 	(dns1.addr >> 8) & 0xFF,
		// 	(dns1.addr >> 16) & 0xFF,
		// 	(dns1.addr >> 24) & 0xFF);

		// lcd_printf("         dns2: %d.%d.%d.%d\n", (dns2.addr >> 0) & 0xFF,
		// 	(dns2.addr >> 8) & 0xFF,
		// 	(dns2.addr >> 16) & 0xFF,
		// 	(dns2.addr >> 24) & 0xFF);
	}
#endif
    Events_Set(SYSTEM_EVENT_FLAG_SOCKET);
    Events_Set(SYSTEM_EVENT_FLAG_NETWORK);
}
#endif

bool SimpleLink_SOCKETS_Driver::Initialize()
{   
    NATIVE_PROFILE_PAL_NETWORK();

	struct netif *networkInterface;
    HAL_Configuration_NetworkInterface networkConfiguration;
    int interfaceNumber;

    PostAddressChangedContinuation.InitializeCallback(PostAddressChanged, NULL);
    PostAvailabilityOnContinuation.InitializeCallback(PostAvailabilityOn, NULL);
    PostAvailabilityOffContinuation.InitializeCallback(PostAvailabilityOff, NULL);

    // create m_interfaceNumber array
    int interfaceCount = g_TargetConfiguration.NetworkInterfaceConfigs->Count;
    // FIXME
    // g_SimpleLink_SOCKETS_Driver.m_interfaces = (LWIP_DRIVER_INTERFACE_DATA*)platform_malloc(interfaceCount * sizeof(LWIP_DRIVER_INTERFACE_DATA)); 

    // FIXME 
    // already done?
    /* Initialize the target board lwIP stack */
    //nanoHAL_Network_Initialize();

	for (int i = 0; i < g_TargetConfiguration.NetworkInterfaceConfigs->Count; i++)
	{
        // load network interface configuration from storage
        if(!ConfigurationManager_GetConfigurationBlock((void*)&networkConfiguration, DeviceConfigurationOption_Network, i))
        {
            // failed to load configuration
            // FIXME output error?
            // move to the next, if any
            continue;
        }
        _ASSERTE(networkConfiguration.StartupAddressMode > 0);

 		/* Bind and Open the Ethernet driver */
 		Network_Interface_Bind(i);
 		interfaceNumber = Network_Interface_Open(i);

		if (interfaceNumber == SOCK_SOCKET_ERROR)
		{
			DEBUG_HANDLE_SOCKET_ERROR("Network init", FALSE);
//FIXME			debug_printf("SocketError: %d\n", errorCode);
			continue;
		}

 		g_SimpleLink_SOCKETS_Driver.m_interfaces[i].m_interfaceNumber = interfaceNumber;

//  		UpdateAdapterConfiguration(i, (UpdateOperation_Dhcp | UpdateOperation_Dns), &networkConfiguration);

//  		networkInterface = netif_find_interface(interfaceNumber);

//  		if (networkInterface)
//  		{	
//  			netif_set_link_callback(networkInterface, Link_callback);

// 			if (netif_is_link_up(networkInterface))
//             {
// 			    Link_callback(networkInterface);
//             }

// 			netif_set_status_callback(networkInterface, Status_callback);

// 			if (netif_is_up(networkInterface))
//             {
// 			    Status_callback(networkInterface);
//             }

// 			// default debugger interface
//             if (0 == i)
//             {
// #if LWIP_IPV6
//                 //uint8_t* addr = (uint8_t*)&networkInterface->ip_addr.u_addr.ip4.addr;
// #else
//                 //uint8_t* addr = (uint8_t*)&networkInterface->ip_addr.addr;
// #endif                
// //                lcd_printf("\f\n\n\n\n\n\n\nip address: %d.%d.%d.%d\r\n", addr[0], addr[1], addr[2], addr[3]);
// // FIXME               debug_printf("ip address from interface info: %d.%d.%d.%d\r\n", addr[0], addr[1], addr[2], addr[3]);
//             }
// 		}
	}

    return TRUE;
}

bool SimpleLink_SOCKETS_Driver::Uninitialize()
{
    NATIVE_PROFILE_PAL_NETWORK();      

    PostAddressChangedContinuation.Abort();
    PostAvailabilityOnContinuation.Abort();
    PostAvailabilityOffContinuation.Abort();

    for(int i=0; i<g_TargetConfiguration.NetworkInterfaceConfigs->Count; i++)
    {
        Network_Interface_Close(i);
    }

//FIXME    tcpip_shutdown();
// tcpip_shutdown is MS method added to lwip tcpip.c

    return TRUE;
}

SOCK_SOCKET SimpleLink_SOCKETS_Driver::Accept(SOCK_SOCKET socket, SOCK_sockaddr* address, int* addressLen)
{ 
    NATIVE_PROFILE_PAL_NETWORK();
    SOCK_SOCKET ret;

    sockaddr_in addr;

    if (address)
    {
        SOCK_SOCKADDR_TO_SOCKADDR(address, addr, addressLen);
    }
    
    ret = lwip_accept(socket, address?(sockaddr*)&addr:NULL, (u32_t*)addressLen);
    
    if(address)
    {
        SOCKADDR_TO_SOCK_SOCKADDR(address, addr, addressLen);
    }
    
    return ret;
}

int SimpleLink_SOCKETS_Driver::SetSockOpt( SOCK_SOCKET socket, int level, int optname, const char* optval, int  optlen )
{ 
    NATIVE_PROFILE_PAL_NETWORK();
    int nativeLevel;
    int nativeOptionName;
    int nativeIntValue;
    char *pNativeOptionValue = (char*)optval;
    struct linger lopt = {0,0};

    switch(level)
    {
        case SOCK_IPPROTO_IP:
            nativeLevel = IPPROTO_IP;
            nativeOptionName = GetNativeIPOption(optname);
            break;
        case SOCK_IPPROTO_TCP:    
            nativeLevel = IPPROTO_TCP;
            nativeOptionName = GetNativeTcpOption(optname);
            break;
        case SOCK_IPPROTO_UDP: 
        case SOCK_IPPROTO_ICMP:
        case SOCK_IPPROTO_IGMP:
        case SOCK_IPPROTO_IPV4:
        case SOCK_SOL_SOCKET:
            nativeLevel      = SOL_SOCKET;
            nativeOptionName = GetNativeSockOption(optname);            

            switch(optname)
            {        
                // If linger value negative then linger off
                // otherwise enabled and linger value is number of seconds
                case SOCK_SOCKO_LINGER:
                    {
                        int lingerValue = *(int*)optval;
                        if ( lingerValue >= 0  )
                        {
                            lopt.l_onoff  =  1;
                            lopt.l_linger =  abs(lingerValue);
                        }
                        pNativeOptionValue = (char*)&lopt;
                        optlen = sizeof(lopt);
                    }
                    break;

                case SOCK_SOCKO_DONTLINGER:
                case SOCK_SOCKO_EXCLUSIVEADDRESSUSE:
                    nativeIntValue     = !*(int*)optval;
                    pNativeOptionValue = (char*)&nativeIntValue;
                    break;
                default:
                    break;
            }
            break;
        default:
            nativeLevel         = 0;
            nativeOptionName    = 0;
            break;
    }

    return lwip_setsockopt(socket, nativeLevel, nativeOptionName, pNativeOptionValue, optlen);
}

int SimpleLink_SOCKETS_Driver::GetSockOpt( SOCK_SOCKET socket, int level, int optname, char* optval, int* optlen )
{ 
    NATIVE_PROFILE_PAL_NETWORK();
    int         nativeLevel;
    int         nativeOptionName;
    char*       pNativeOptval = optval;
    int         ret;
    
    switch(level)
    {
        case SOCK_IPPROTO_IP:
            nativeLevel         = IPPROTO_IP;
            nativeOptionName    = GetNativeIPOption(optname);
            break;
        case SOCK_IPPROTO_TCP:    
            nativeLevel         = IPPROTO_TCP;
            nativeOptionName    = GetNativeTcpOption(optname);
            break;
        case SOCK_IPPROTO_UDP: 
        case SOCK_IPPROTO_ICMP:
        case SOCK_IPPROTO_IGMP:
        case SOCK_IPPROTO_IPV4:
        case SOCK_SOL_SOCKET:
            nativeLevel         = SOL_SOCKET;
            nativeOptionName    = GetNativeSockOption(optname);
            break;
        default:
            nativeLevel         = level;
            nativeOptionName    = optname;
            break;
    }

    ret = lwip_getsockopt(socket, nativeLevel, nativeOptionName, pNativeOptval, (u32_t*)optlen);

    if(ret == 0)
    {
        switch(level)
        {
            case SOCK_SOL_SOCKET:
                switch(optname)
                {       
                    case SOCK_SOCKO_EXCLUSIVEADDRESSUSE:
                    case SOCK_SOCKO_DONTLINGER:
                        *optval = !(*(int*)optval != 0);
                        break;
                        
                    case SOCK_SOCKO_ACCEPTCONNECTION:
                    case SOCK_SOCKO_BROADCAST:
                    case SOCK_SOCKO_KEEPALIVE:
                        *optval = (*(int*)optval != 0);
                        break;
                }
                break;
        }
    }

    return ret;    
}

int SimpleLink_SOCKETS_Driver::GetSockName( SOCK_SOCKET socket, SOCK_sockaddr* name, int* namelen )
{ 
    NATIVE_PROFILE_PAL_NETWORK();
    int ret;

    sockaddr_in addr;

    SOCK_SOCKADDR_TO_SOCKADDR(name, addr, namelen);

    ret = lwip_getsockname(socket, (sockaddr*)&addr, (u32_t*)namelen);

    SOCKADDR_TO_SOCK_SOCKADDR(name, addr, namelen);

    return ret;
}

int SimpleLink_SOCKETS_Driver::RecvFrom( SOCK_SOCKET socket, char* buf, int len, int flags, SOCK_sockaddr* from, int* fromlen )
{ 
    NATIVE_PROFILE_PAL_NETWORK();
    sockaddr_in addr;
    sockaddr *pFrom = NULL;
    int ret;

    if(from)
    {
        SOCK_SOCKADDR_TO_SOCKADDR(from, addr, fromlen);
        pFrom = (sockaddr*)&addr;
    }
        
    ret = lwip_recvfrom(socket, buf, len, flags, pFrom, (u32_t*)fromlen);

    if(from && ret != SOCK_SOCKET_ERROR)
    {
        SOCKADDR_TO_SOCK_SOCKADDR(from, addr, fromlen);
    }

    return ret;
}

int SimpleLink_SOCKETS_Driver::SendTo( SOCK_SOCKET socket, const char* buf, int len, int flags, const SOCK_sockaddr* to, int tolen )
{ 
    NATIVE_PROFILE_PAL_NETWORK();

    sockaddr_in addr;

    SOCK_SOCKADDR_TO_SOCKADDR(to, addr, &tolen);

    return lwip_sendto(socket, buf, len, flags, (sockaddr*)&addr, (u32_t)tolen);
}

struct dhcp_client_id
{
    uint8_t code;
    uint8_t length;
    uint8_t type;
    uint8_t clientId[6];
};

int SimpleLink_SOCKETS_Driver::GetNativeTcpOption (int optname)
{
    NATIVE_PROFILE_PAL_NETWORK();
    int nativeOptionName = 0;

    switch(optname)
    {
        case SOCK_TCP_NODELAY:
            nativeOptionName = TCP_NODELAY;
            break;

        case SOCK_SOCKO_KEEPALIVE:
            nativeOptionName = TCP_KEEPALIVE;
            break;

        // allow the C# user to specify LWIP options that our managed enum
        // doesn't support
        default:
            nativeOptionName = optname;
            break;
    }
    return nativeOptionName;
}

int SimpleLink_SOCKETS_Driver::GetNativeSockOption (int optname)
{
    NATIVE_PROFILE_PAL_NETWORK();
    int nativeOptionName = 0;

    switch(optname)
    {
        case SOCK_SOCKO_DONTLINGER:
        case SOCK_SOCKO_LINGER:    
            nativeOptionName = SO_LINGER;
            break;
        case SOCK_SOCKO_SENDTIMEOUT:          
            nativeOptionName = SO_SNDTIMEO;
            break;
        case SOCK_SOCKO_RECEIVETIMEOUT:       
            nativeOptionName = SO_RCVTIMEO;
            break;
        case SOCK_SOCKO_EXCLUSIVEADDRESSUSE: 
        case SOCK_SOCKO_REUSEADDRESS:         
            nativeOptionName = SO_REUSEADDR;
            break;
        case SOCK_SOCKO_KEEPALIVE:  
            nativeOptionName = SO_KEEPALIVE;
            break;
        case SOCK_SOCKO_ERROR:                  
            nativeOptionName = SO_ERROR;
            break;
        case SOCK_SOCKO_BROADCAST:              
            nativeOptionName = SO_BROADCAST;
            break;
        case SOCK_SOCKO_RECEIVEBUFFER:
            nativeOptionName =  SO_RCVBUF;
            break;
        case SOCK_SOCKO_SENDBUFFER:
            nativeOptionName = SO_SNDBUF;
            break;
        case SOCK_SOCKO_ACCEPTCONNECTION:
            nativeOptionName = SO_ACCEPTCONN;
            break;
        case SOCK_SOCKO_TYPE:
            nativeOptionName = SO_TYPE;
            break;
            
        case SOCK_SOCKO_USELOOPBACK:
            nativeOptionName = SO_USELOOPBACK;
            break;
        case SOCK_SOCKO_DONTROUTE:  
            nativeOptionName = SO_DONTROUTE;
            break;
        case SOCK_SOCKO_OUTOFBANDINLINE:
            nativeOptionName = SO_OOBINLINE;
            break;

        case SOCK_SOCKO_DEBUG:
            nativeOptionName = SO_DEBUG;
            break;
            
        case SOCK_SOCKO_SENDLOWWATER:
            nativeOptionName = SO_SNDLOWAT;
            break;
            
        case SOCK_SOCKO_RECEIVELOWWATER:
            nativeOptionName = SO_RCVLOWAT;
            break;
            
//        case SOCK_SOCKO_MAXCONNECTIONS:         //don't support
        case SOCK_SOCKO_UPDATE_ACCEPT_CTX:
        case SOCK_SOCKO_UPDATE_CONNECT_CTX:
            nativeOptionName = 0;
            break;
            
        // allow the C# user to specify LWIP options that our managed enum
        // doesn't support
        default:
            nativeOptionName = optname;
            break;
            
    }

    return nativeOptionName;
}

int SimpleLink_SOCKETS_Driver::GetNativeIPOption (int optname)
{
    NATIVE_PROFILE_PAL_NETWORK();
    int nativeOptionName = 0;

    switch(optname)
    {
        case SOCK_IPO_TTL:           
            nativeOptionName = IP_TTL;
            break;
        case SOCK_IPO_TOS:    
            nativeOptionName = IP_TOS;
            break;
#if LWIP_IGMP
        case SOCK_IPO_MULTICAST_IF:
            nativeOptionName = IP_MULTICAST_IF;
            break;
        case SOCK_IPO_MULTICAST_TTL:  
            nativeOptionName = IP_MULTICAST_TTL;
            break;
        case SOCK_IPO_MULTICAST_LOOP: 
            nativeOptionName = IP_MULTICAST_LOOP;
            break;
        case SOCK_IPO_ADD_MEMBERSHIP:
            nativeOptionName = IP_ADD_MEMBERSHIP;
            break;
        case SOCK_IPO_DROP_MEMBERSHIP:
            nativeOptionName = IP_DROP_MEMBERSHIP;
            break;
#else
        case SOCK_IPO_MULTICAST_IF:
        case SOCK_IPO_MULTICAST_TTL:  
        case SOCK_IPO_MULTICAST_LOOP: 
        case SOCK_IPO_ADD_MEMBERSHIP:
        case SOCK_IPO_DROP_MEMBERSHIP:
#endif
        case SOCK_IPO_ADD_SOURCE_MEMBERSHIP:
        case SOCK_IPO_DROP_SOURCE_MEMBERSHIP:
        case SOCK_IPO_OPTIONS:
        case SOCK_IPO_HDRINCL:
        case SOCK_IPO_IP_DONTFRAGMENT:
        case SOCK_IPO_BLOCK_SOURCE:
        case SOCK_IPO_UBLOCK_SOURCE:
        case SOCK_IPO_PACKET_INFO: 
            nativeOptionName = 0;
            break;

        // allow the C# user to specify LWIP options that our managed enum
        // doesn't support
        default:
            nativeOptionName = optname;
            break;
    }
    
    return nativeOptionName;
}   

int SimpleLink_SOCKETS_Driver::GetNativeError ( int error )
{
    NATIVE_PROFILE_PAL_NETWORK();
    int ret;

    switch(error)
    {
        case EINTR:
            ret = SOCK_EINTR;
            break;

        case EACCES:
            ret = SOCK_EACCES;
            break;

        case EFAULT:
            ret = SOCK_EFAULT;
            break;

        case EINVAL:
            ret = SOCK_EINVAL;
            break;

        case EMFILE:
            ret = SOCK_EMFILE;
            break;

        case EAGAIN:
        case EBUSY:
        /* case EWOULDBLOCK: same as EINPROGRESS */ 
        case EINPROGRESS:
            ret = SOCK_EWOULDBLOCK;
            break;

        case EALREADY:
            ret = SOCK_EALREADY;
            break;

        case ENOTSOCK:
            ret = SOCK_ENOTSOCK;
            break;

        case EDESTADDRREQ:
            ret = SOCK_EDESTADDRREQ;
            break;

        case EMSGSIZE:
            ret = SOCK_EMSGSIZE;
            break;

        case EPROTOTYPE:
            ret = SOCK_EPROTOTYPE;
            break;

        case ENOPROTOOPT:
            ret = SOCK_ENOPROTOOPT;
            break;

        case EPROTONOSUPPORT:
            ret = SOCK_EPROTONOSUPPORT;
            break;
// TODO nanoframework check why missing
        // case ESOCKTNOSUPPORT:
        //     ret = SOCK_ESOCKTNOSUPPORT;
        //     break;

        case EPFNOSUPPORT:
            ret = SOCK_EPFNOSUPPORT;
            break;

        case EAFNOSUPPORT:
            ret = SOCK_EAFNOSUPPORT;
            break;

        case EADDRINUSE:
            ret = SOCK_EADDRINUSE;
            break;

        case EADDRNOTAVAIL:
            ret = SOCK_EADDRNOTAVAIL;
            break;

        case ENETDOWN:
            ret = SOCK_ENETDOWN;
            break;

        case ENETUNREACH:
            ret = SOCK_ENETUNREACH;
            break;

        case ENETRESET:
            ret = SOCK_ENETRESET;
            break;

        case ECONNABORTED:
            ret = SOCK_ECONNABORTED;
            break;

        case ECONNRESET:
            ret = SOCK_ECONNRESET;
            break;

        case ENOBUFS:
        case ENOMEM:
            ret = SOCK_ENOBUFS;
            break;

        case EISCONN:
            ret = SOCK_EISCONN;
            break;

        case ENOTCONN:
            ret = SOCK_EISCONN;
            break;

#if !defined(__GNUC__) // same as ENOTSOCK for GCC
        case ESHUTDOWN:
            ret = SOCK_ESHUTDOWN;
            break;
#endif

        case ETIMEDOUT:
            ret = SOCK_ETIMEDOUT;
            break;

        case ECONNREFUSED:
            ret = SOCK_ECONNREFUSED;
            break;

        case EHOSTDOWN:
            ret = SOCK_EHOSTDOWN;
            break;

        case EHOSTUNREACH:
            ret = SOCK_EHOSTUNREACH;
            break;

        case ENODATA:
            ret = SOCK_NO_DATA;
            break;

        default:
            ret = error;
            break;
    } 
    
    return (ret);   
}
