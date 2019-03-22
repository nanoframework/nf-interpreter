//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//



#include "LWIP_sockets.h"

extern "C"
{
#include "lwip\init.h"
#include "lwip\tcpip.h"
#include "lwip\dns.h"
#include "lwip\netifapi.h"
#include "lwip\Netdb.h"
#include "lwip\tcp.h"
#include "lwip\Sockets.h"
#include "lwip\dhcp.h"
#include "lwip\netif.h"
}


int errorCode;

//--// 

#if defined(DEBUG)
#define DEBUG_HANDLE_SOCKET_ERROR(t,a) 
// assume there is something to add in later??
#else
#define DEBUG_HANDLE_SOCKET_ERROR(t,a) 
#endif

struct netif *netif_find_interface(int num);

//

// declaration of function not available in standard lwIP API
extern "C"
{
extern uint32_t lwip_socket_get_err(int s);
}
//--//

LWIP_SOCKETS_Driver g_LWIP_SOCKETS_Driver;

//--//
#if LWIP_NETIF_STATUS_CALLBACK == 1
static HAL_CONTINUATION PostAddressChangedContinuation;
#endif
#if LWIP_NETIF_LINK_CALLBACK == 1
static HAL_CONTINUATION PostAvailabilityOnContinuation;
static HAL_CONTINUATION PostAvailabilityOffContinuation;
#endif

void LWIP_SOCKETS_Driver::PostAddressChanged(void* arg)
{
    (void)arg;

	Network_PostEvent(NetworkEventType_AddressChanged, 0);
}

void LWIP_SOCKETS_Driver::PostAvailabilityOn(void* arg)
{
    (void)arg;

	Network_PostEvent(NetworkEventType_AvailabilityChanged, NetworkEventFlags_NetworkAvailable);
}

void LWIP_SOCKETS_Driver::PostAvailabilityOff(void* arg)
{
    (void)arg;

	Network_PostEvent(NetworkEventType_AvailabilityChanged, NetworkEventFlags_NetworkNOTAvailable);
}

#if LWIP_NETIF_LINK_CALLBACK == 1
void LWIP_SOCKETS_Driver::Link_callback(struct netif *netif)
{
	if (netif_is_link_up(netif))
	{
		if (!PostAvailabilityOnContinuation.IsLinked())
			PostAvailabilityOnContinuation.Enqueue();
	}
	else
	{
		if (!PostAvailabilityOffContinuation.IsLinked())
			PostAvailabilityOffContinuation.Enqueue();
	}
    Events_Set(SYSTEM_EVENT_FLAG_SOCKET);
    Events_Set(SYSTEM_EVENT_FLAG_NETWORK);
}
#endif

#if LWIP_NETIF_STATUS_CALLBACK == 1
void LWIP_SOCKETS_Driver::Status_callback(struct netif *netif)
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
#endif
    Events_Set(SYSTEM_EVENT_FLAG_SOCKET);
    Events_Set(SYSTEM_EVENT_FLAG_NETWORK);
}
#endif

bool LWIP_SOCKETS_Driver::Initialize()
{   
    NATIVE_PROFILE_PAL_NETWORK();

	struct netif *networkInterface;
    HAL_Configuration_NetworkInterface networkConfiguration;
    int interfaceNumber;

#if LWIP_NETIF_STATUS_CALLBACK == 1
    PostAddressChangedContinuation.InitializeCallback(PostAddressChanged, NULL);
#endif
#if LWIP_NETIF_LINK_CALLBACK == 1
    PostAvailabilityOnContinuation.InitializeCallback(PostAvailabilityOn, NULL);
    PostAvailabilityOffContinuation.InitializeCallback(PostAvailabilityOff, NULL);
#endif

    // create m_interfaceNumber array
    int interfaceCount = g_TargetConfiguration.NetworkInterfaceConfigs->Count;
    g_LWIP_SOCKETS_Driver.m_interfaces = (LWIP_DRIVER_INTERFACE_DATA*)platform_malloc(interfaceCount * sizeof(LWIP_DRIVER_INTERFACE_DATA)); 

    /* Initialize the target board lwIP stack */
    nanoHAL_Network_Initialize();

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

 		g_LWIP_SOCKETS_Driver.m_interfaces[i].m_interfaceNumber = interfaceNumber;

 		UpdateAdapterConfiguration(i, (UpdateOperation_Dhcp | UpdateOperation_Dns), &networkConfiguration);

 		networkInterface = netif_find_interface(interfaceNumber);

 		if (networkInterface)
 		{	
#if LWIP_NETIF_LINK_CALLBACK == 1
 			netif_set_link_callback(networkInterface, Link_callback);

			if (netif_is_link_up(networkInterface))
            {
			    Link_callback(networkInterface);
            }
#endif
#if LWIP_NETIF_STATUS_CALLBACK == 1
			netif_set_status_callback(networkInterface, Status_callback);

			if (netif_is_up(networkInterface))
            {
			    Status_callback(networkInterface);
            }
#endif

			// default debugger interface
            if (0 == i)
            {
#if LWIP_IPV6
                //uint8_t* addr = (uint8_t*)&networkInterface->ip_addr.u_addr.ip4.addr;
#else
                //uint8_t* addr = (uint8_t*)&networkInterface->ip_addr.addr;
#endif                
//                lcd_printf("\f\n\n\n\n\n\n\nip address: %d.%d.%d.%d\r\n", addr[0], addr[1], addr[2], addr[3]);
// FIXME               debug_printf("ip address from interface info: %d.%d.%d.%d\r\n", addr[0], addr[1], addr[2], addr[3]);
            }
		}
	}

    return TRUE;
}

bool LWIP_SOCKETS_Driver::Uninitialize()
{
    NATIVE_PROFILE_PAL_NETWORK();      

#if LWIP_NETIF_STATUS_CALLBACK == 1
    PostAddressChangedContinuation.Abort();
#endif
#if LWIP_NETIF_LINK_CALLBACK == 1
    PostAvailabilityOnContinuation.Abort();
    PostAvailabilityOffContinuation.Abort();
#endif

    for(int i=0; i<g_TargetConfiguration.NetworkInterfaceConfigs->Count; i++)
    {
        Network_Interface_Close(i);
    }

//FIXME    tcpip_shutdown();
// tcpip_shutdown is MS method added to lwip tcpip.c

    return TRUE;
}

extern void debug_printf( const char* format, ... );

SOCK_SOCKET LWIP_SOCKETS_Driver::Socket(int family, int type, int protocol) 
{  
    NATIVE_PROFILE_PAL_NETWORK();

    switch(protocol)
    {
        case SOCK_IPPROTO_TCP:
            protocol = IPPROTO_TCP;
            break;
        case SOCK_IPPROTO_UDP:
            protocol = IPPROTO_UDP;
            break;
        case SOCK_IPPROTO_ICMP:
            protocol = IP_PROTO_ICMP;
            break;

        case SOCK_IPPROTO_IGMP:
            protocol = IP_PROTO_IGMP;
            break;
    }
    
    return lwip_socket(family, type, protocol);
}

int LWIP_SOCKETS_Driver::Bind(SOCK_SOCKET socket, const SOCK_sockaddr* address, int addressLen) 
{ 
    NATIVE_PROFILE_PAL_NETWORK();

    sockaddr_in addr;

    SOCK_SOCKADDR_TO_SOCKADDR(address, addr, &addressLen);
        
    return lwip_bind(socket, (sockaddr*)&addr, addressLen);
}

int LWIP_SOCKETS_Driver::Connect(SOCK_SOCKET socket, const SOCK_sockaddr* address, int addressLen) 
{ 
    NATIVE_PROFILE_PAL_NETWORK();
    
    sockaddr_in addr;

    SOCK_SOCKADDR_TO_SOCKADDR(address, addr, &addressLen);
        
    return lwip_connect(socket, (sockaddr*)&addr, addressLen);
}

int LWIP_SOCKETS_Driver::Send(SOCK_SOCKET socket, const char* buf, int len, int flags) 
{ 
    NATIVE_PROFILE_PAL_NETWORK();
        
    return lwip_send(socket, (const void*)buf, len, flags);
}


int LWIP_SOCKETS_Driver::Recv(SOCK_SOCKET socket, char* buf, int len, int flags)
{ 
    NATIVE_PROFILE_PAL_NETWORK();
    int     nativeFlag;
    
    switch (flags)
    {
        case SOCKET_READ_PEEK_OPTION:
            nativeFlag = MSG_PEEK;
            break;
        default:
            nativeFlag = flags;
            break;
    }
    
    return lwip_recv(socket,(void*)buf, len, nativeFlag);
}

int LWIP_SOCKETS_Driver::Close(SOCK_SOCKET socket)
{ 
    NATIVE_PROFILE_PAL_NETWORK();

#ifdef PLATFORM_ESP32
    // We have to call lwip_close_r() method otherwise the socket doesn't get freed up and we run out of sockets.
    // We could also call the posix closesocket() which should work for all platforms and change all other methods to 
    // call the posix version to be consistent.(TODO)
    return lwip_close_r(socket);
#else
    return lwip_close(socket);
#endif
}

int LWIP_SOCKETS_Driver::Listen(SOCK_SOCKET socket, int backlog)
{    
    NATIVE_PROFILE_PAL_NETWORK();
    
    return lwip_listen(socket, backlog);
}

SOCK_SOCKET LWIP_SOCKETS_Driver::Accept(SOCK_SOCKET socket, SOCK_sockaddr* address, int* addressLen)
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

int LWIP_SOCKETS_Driver::Shutdown( SOCK_SOCKET socket, int how )
{    
    NATIVE_PROFILE_PAL_NETWORK();
    
    return lwip_shutdown (socket, how);
}

int LWIP_SOCKETS_Driver::GetAddrInfo(const char* nodename, char* servname, const SOCK_addrinfo* hints, SOCK_addrinfo** res)
{ 
  #if LWIP_DNS
    NATIVE_PROFILE_PAL_NETWORK();

    SOCK_addrinfo *ai;
    SOCK_sockaddr_in *sa = NULL;
    int total_size = sizeof(SOCK_addrinfo) + sizeof(SOCK_sockaddr_in);
    struct addrinfo *lwipAddrinfo = NULL;

    if(res == NULL) return SOCK_SOCKET_ERROR;

    *res = NULL;

    // if the nodename == "" then return the IP address of this device
    if(nodename[0] == 0 && servname == NULL)
    {
        struct netif *networkInterface = netif_find_interface(g_LWIP_SOCKETS_Driver.m_interfaces[0].m_interfaceNumber);

        if(networkInterface == NULL) return -1;

        ai = (SOCK_addrinfo*)mem_malloc(total_size);
        if (ai == NULL) 
        {
            return -1;
        }
        memset(ai, 0, total_size);
        sa = (SOCK_sockaddr_in*)((u8_t*)ai + sizeof(SOCK_addrinfo));

        /* set up sockaddr */
      #if LWIP_IPV6
        sa->sin_addr.S_un.S_addr = networkInterface->ip_addr.u_addr.ip4.addr;
      #else
        sa->sin_addr.S_un.S_addr = networkInterface->ip_addr.addr;
      #endif

        sa->sin_family = AF_INET;
        sa->sin_port = 0;
        
        /* set up addrinfo */
        ai->ai_family = AF_INET;
        if (hints != NULL) 
        {
            /* copy socktype & protocol from hints if specified */
            ai->ai_socktype = hints->ai_socktype;
            ai->ai_protocol = hints->ai_protocol;
        }

        ai->ai_addrlen = sizeof(SOCK_sockaddr_in);
        ai->ai_addr = (SOCK_sockaddr*)sa;

        *res = ai;

        return 0;
    }

    int err = lwip_getaddrinfo(nodename, servname, (addrinfo*)hints, &lwipAddrinfo);

    if(err == 0)
    {
        ///
        /// Marshal addrinfo data
        ///
        struct sockaddr_in* lwip_sockaddr_in;
        
        ai = (SOCK_addrinfo*)mem_malloc(total_size);
        if (ai == NULL) 
        {
            lwip_freeaddrinfo(lwipAddrinfo);
            return -1;
        }
        memset(ai, 0, total_size);

        lwip_sockaddr_in = ((struct sockaddr_in*)lwipAddrinfo->ai_addr);

        sa = (SOCK_sockaddr_in*)((u8_t*)ai + sizeof(SOCK_addrinfo));
        /* set up sockaddr */
        sa->sin_addr.S_un.S_addr = lwip_sockaddr_in->sin_addr.s_addr;
        sa->sin_family = lwip_sockaddr_in->sin_family;
        sa->sin_port = lwip_sockaddr_in->sin_port;
        
        /* set up addrinfo */
        ai->ai_family = lwipAddrinfo->ai_family;
        if (hints != NULL) 
        {
            /* copy socktype & protocol from hints if specified */
            ai->ai_socktype = hints->ai_socktype;
            ai->ai_protocol = hints->ai_protocol;
        }
        
        ai->ai_addrlen = sizeof(SOCK_sockaddr_in);
        ai->ai_addr = (SOCK_sockaddr*)sa;
        
        *res = ai;

        // free marshalled addrinfo
        lwip_freeaddrinfo(lwipAddrinfo);

        return 0;
    }
    else
    {
        // map DNS error with socket errors
        switch(err)
        {
            case HOST_NOT_FOUND:
                errorCode = SOCK_HOST_NOT_FOUND;
                break;

            default:
                errorCode = SOCK_NO_RECOVERY;
        }

    }
 
    return SOCK_SOCKET_ERROR;

  #else
    return SOCK_SOCKET_ERROR;
  #endif
}

void LWIP_SOCKETS_Driver::FreeAddrInfo( SOCK_addrinfo* ai )
{ 
    NATIVE_PROFILE_PAL_NETWORK();

    SOCK_addrinfo *next;
    
    while (ai != NULL) {
      next = ai->ai_next;
      mem_free(ai);
      ai = next;
    }
}

int LWIP_SOCKETS_Driver::Ioctl( SOCK_SOCKET socket, int cmd, int* data )
{ 
    NATIVE_PROFILE_PAL_NETWORK();

    return lwip_ioctl(socket,cmd,data);
}

int LWIP_SOCKETS_Driver::GetLastError()
{
    NATIVE_PROFILE_PAL_NETWORK();

    return GetNativeError(errorCode);
}

int LWIP_SOCKETS_Driver::GetSockLastError(SOCK_SOCKET socket)
{
    NATIVE_PROFILE_PAL_NETWORK();

    // get last error number from socket
    errorCode = lwip_socket_get_err(socket);

    return GetNativeError(errorCode);
}

static int MARSHAL_SOCK_FDSET_TO_FDSET(SOCK_fd_set *sf, fd_set *f)
{
    if(f != NULL && sf != NULL) 
    { 
        FD_ZERO(f);
        
        for(unsigned int i=0; i < sf->fd_count; i++) 
        { 
            FD_SET(sf->fd_array[i], f); 
        } 
        return sf->fd_count;
    } 

    return 0;
}

static void MARSHAL_FDSET_TO_SOCK_FDSET(SOCK_fd_set *sf, fd_set *f)
{
    if(sf != NULL && f != NULL) 
    { 
        int cnt = sf->fd_count;
        sf->fd_count = 0; 
        for(int i=0; i<cnt; i++) 
        { 
            if(FD_ISSET(sf->fd_array[i],f)) 
            { 
                sf->fd_array[sf->fd_count] = sf->fd_array[i]; 
                sf->fd_count++; 
            } 
        } 
    } 
}
    

int LWIP_SOCKETS_Driver::Select( int nfds, SOCK_fd_set* readfds, SOCK_fd_set* writefds, SOCK_fd_set* exceptfds, const SOCK_timeval* timeout )
{
    (void)nfds;

    NATIVE_PROFILE_PAL_NETWORK();
    int ret = 0;

    fd_set read;
    fd_set write;
    fd_set excpt;

    fd_set* pR = (readfds   != NULL) ? &read  : NULL;
    fd_set* pW = (writefds  != NULL) ? &write : NULL;
    fd_set* pE = (exceptfds != NULL) ? &excpt : NULL;

    // If the network goes down then we should alert any pending socket actions
    if(exceptfds != NULL && exceptfds->fd_count > 0)
    {
        struct netif *networkInterface = netif_find_interface(g_LWIP_SOCKETS_Driver.m_interfaces[0].m_interfaceNumber);

        if(networkInterface != NULL)
        {
            if(!netif_is_up(networkInterface))
            {
                if(readfds  != NULL) readfds->fd_count = 0;
                if(writefds != NULL) writefds->fd_count = 0;

                errorCode = ENETDOWN;

                return exceptfds->fd_count;
            }
        }
    }

    MARSHAL_SOCK_FDSET_TO_FDSET(readfds  , pR);
    MARSHAL_SOCK_FDSET_TO_FDSET(writefds , pW);
    MARSHAL_SOCK_FDSET_TO_FDSET(exceptfds, pE);

    int max_sd = MEMP_NUM_NETCONN;
    #if defined(PLATFORM_ESP32)
    max_sd = LWIP_SOCKET_OFFSET + MEMP_NUM_NETCONN;
    #endif

    // developer note: 
    // our declaration of SOCK_timeval is dependent of "long" type which is platform dependent
    // so it's not safe to cast it to "timeval"
    timeval timeoutCopy;
    timeoutCopy.tv_sec = timeout->tv_sec;
    timeoutCopy.tv_usec = timeout->tv_usec;

    ret = select(max_sd, pR, pW, pE, &timeoutCopy);

    MARSHAL_FDSET_TO_SOCK_FDSET(readfds  , pR);
    MARSHAL_FDSET_TO_SOCK_FDSET(writefds , pW);
    MARSHAL_FDSET_TO_SOCK_FDSET(exceptfds, pE);

    return ret;
}

int LWIP_SOCKETS_Driver::SetSockOpt( SOCK_SOCKET socket, int level, int optname, const char* optval, int  optlen )
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

int LWIP_SOCKETS_Driver::GetSockOpt( SOCK_SOCKET socket, int level, int optname, char* optval, int* optlen )
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

int LWIP_SOCKETS_Driver::GetPeerName( SOCK_SOCKET socket, SOCK_sockaddr* name, int* namelen )
{ 
    NATIVE_PROFILE_PAL_NETWORK();
    int ret;

    sockaddr_in addr;

    SOCK_SOCKADDR_TO_SOCKADDR(name, addr, namelen);

    ret = lwip_getpeername(socket, (sockaddr*)&addr, (u32_t*)namelen);

    SOCKADDR_TO_SOCK_SOCKADDR(name, addr, namelen);

    return ret;
}

int LWIP_SOCKETS_Driver::GetSockName( SOCK_SOCKET socket, SOCK_sockaddr* name, int* namelen )
{ 
    NATIVE_PROFILE_PAL_NETWORK();
    int ret;

    sockaddr_in addr;

    SOCK_SOCKADDR_TO_SOCKADDR(name, addr, namelen);

    ret = lwip_getsockname(socket, (sockaddr*)&addr, (u32_t*)namelen);

    SOCKADDR_TO_SOCK_SOCKADDR(name, addr, namelen);

    return ret;
}

int LWIP_SOCKETS_Driver::RecvFrom( SOCK_SOCKET socket, char* buf, int len, int flags, SOCK_sockaddr* from, int* fromlen )
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

int LWIP_SOCKETS_Driver::SendTo( SOCK_SOCKET socket, const char* buf, int len, int flags, const SOCK_sockaddr* to, int tolen )
{ 
    NATIVE_PROFILE_PAL_NETWORK();

    sockaddr_in addr;

    SOCK_SOCKADDR_TO_SOCKADDR(to, addr, &tolen);

    return lwip_sendto(socket, buf, len, flags, (sockaddr*)&addr, (u32_t)tolen);
}

HRESULT LWIP_SOCKETS_Driver::LoadAdapterConfiguration(HAL_Configuration_NetworkInterface* config, uint32_t interfaceIndex)
{
    NATIVE_PROFILE_PAL_NETWORK();

    if(config->StartupAddressMode == AddressMode_DHCP)
    {
        struct netif *networkInterface;

        if ((networkInterface = netif_find_interface(g_LWIP_SOCKETS_Driver.m_interfaces[interfaceIndex].m_interfaceNumber)))
        {
#if LWIP_IPV6
            config->IPv4Address     = networkInterface->ip_addr.u_addr.ip4.addr;
            config->IPv4NetMask = networkInterface->netmask.u_addr.ip4.addr;
            config->IPv4GatewayAddress    = networkInterface->gw.u_addr.ip4.addr;

            // FIXME IPV6
            // config->IPv6Address     = networkInterface->ip_addr.u_addr.ip6.addr;
            // config->IPv6NetMask = networkInterface->netmask.u_addr.ip6.addr;
            // config->IPv6GatewayAddress    = networkInterface->gw.u_addr.ip6.addr;
#else
            config->IPv4Address     = networkInterface->ip_addr.addr;
            config->IPv4NetMask = networkInterface->netmask.addr;
            config->IPv4GatewayAddress    = networkInterface->gw.addr;
#endif

#if LWIP_DNS
#if LWIP_IPV6
#if LWIP_VERSION_MAJOR == 2 
            config->IPv4DNSAddress1 = dns_getserver(0)->u_addr.ip4.addr;
            config->IPv4DNSAddress2 = dns_getserver(1)->u_addr.ip4.addr;

            // FIXME IPV6
            // config->IPv6DNSAddress1 = dns_getserver(0)->u_addr.ip6.addr;
            // config->IPv6DNSAddress2 = dns_getserver(1)->u_addr.ip6.addr;
#else
            config->IPv4DNSAddress1 = dns_getserver(0).u_addr.ip4.addr;
            config->IPv4DNSAddress2 = dns_getserver(1).u_addr.ip4.addr;

            // FIXME IPV6
            // config->IPv6DNSAddress1 = dns_getserver(0).u_addr.ip6.addr;
            // config->IPv6DNSAddress2 = dns_getserver(1).u_addr.ip6.addr;
#endif
#else
#if LWIP_VERSION_MAJOR == 2 
              config->IPv4DNSAddress1 = dns_getserver(0)->addr;
              config->IPv4DNSAddress2 = dns_getserver(1)->addr;
#else
              config->IPv4DNSAddress1 = dns_getserver(0).addr;
              config->IPv4DNSAddress2 = dns_getserver(1).addr;
#endif

#endif
#endif
        }
        else
        {
            config->IPv4Address     = 0;
            config->IPv4NetMask = 0;
            config->IPv4GatewayAddress    = 0;
        }
    }
    
    return S_OK;
}

struct dhcp_client_id
{
    uint8_t code;
    uint8_t length;
    uint8_t type;
    uint8_t clientId[6];
};

HRESULT LWIP_SOCKETS_Driver::UpdateAdapterConfiguration( uint32_t interfaceIndex, uint32_t updateFlags, HAL_Configuration_NetworkInterface* config )
{
    NATIVE_PROFILE_PAL_NETWORK();
    
    bool enableDHCP = (config->StartupAddressMode == AddressMode_DHCP);

    struct netif *networkInterface = netif_find_interface(g_LWIP_SOCKETS_Driver.m_interfaces[interfaceIndex].m_interfaceNumber);
    if (NULL == networkInterface)
    {
        return CLR_E_FAIL;
    }

#if LWIP_DNS
    // when using DHCP do not use the static settings
    if(0 != (updateFlags & UpdateOperation_Dns))
    {
        // FIXME IPV6
        if(config->AutomaticDNS == 0)
        {
            // user defined DNS addresses
            if(config->IPv4DNSAddress1 != 0)
            {
                u8_t idx = 0;
                
                dns_setserver(idx, (const ip_addr_t *)&config->IPv4DNSAddress1);
            }
            if(config->IPv4DNSAddress2 != 0)
            {
                u8_t idx = 1;

                dns_setserver(idx, (const ip_addr_t *)&config->IPv4DNSAddress2);
            }
        }
    }
#endif

#if LWIP_DHCP
    if(0 != (updateFlags & UpdateOperation_Dhcp))
    {
        if(enableDHCP)
        {
            // need to start DHCP   
            if(ERR_OK != dhcp_start(networkInterface))
            {
                return CLR_E_FAIL;
            }
        }
        else
        {
            // stop DHCP
            dhcp_stop(networkInterface);

            // set interface with our static IP configs
            netif_set_addr(networkInterface, (const ip4_addr_t *) &config->IPv4Address, (const ip4_addr_t *)&config->IPv4NetMask, (const ip4_addr_t *)&config->IPv4GatewayAddress);

            // we should be polite and let the DHCP server that we are now using a static IP
            dhcp_inform(networkInterface);
        }
    }

    if(enableDHCP)
    {
        // developer note: on legacy source there was a hack of trying to renew before release and
        // also setting the release flag in managed call when the intent was to renew only
        // nowadays lwIP seems to be doing what is told, so no need for these hacks anymore
        // also it's NOT possible to renew & release on the same pass, so adding an extra else-if for that
        // just in case it's request from the managed code

        if(0 != (updateFlags & UpdateOperation_DhcpRelease))
        {
            dhcp_release(networkInterface);
        }
        else if(0 != (updateFlags & UpdateOperation_DhcpRenew))
        {
            dhcp_renew(networkInterface);
        }
        else if(0 != (updateFlags & (UpdateOperation_DhcpRelease | UpdateOperation_DhcpRenew)) )
        {
            return CLR_E_INVALID_PARAMETER;
        }
    }
#endif

    if(0 != (updateFlags & UpdateOperation_Mac))
    {
        memcpy(networkInterface->hwaddr, config->MacAddress, NETIF_MAX_HWADDR_LEN);
        networkInterface->hwaddr_len = NETIF_MAX_HWADDR_LEN;

        // mac address requires stack re-init
        Network_Interface_Close(interfaceIndex);
        g_LWIP_SOCKETS_Driver.m_interfaces[interfaceIndex].m_interfaceNumber = Network_Interface_Open(interfaceIndex);
    }

    return S_OK;
}

int LWIP_SOCKETS_Driver::GetNativeTcpOption (int optname)
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

int LWIP_SOCKETS_Driver::GetNativeSockOption (int optname)
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

int LWIP_SOCKETS_Driver::GetNativeIPOption (int optname)
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

int LWIP_SOCKETS_Driver::GetNativeError ( int error )
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

/**
 * Find a network interface by searching for its number
 * Similar to LWIP's netif_find(char *name)
 */
struct netif *netif_find_interface(int num)
{
    struct netif *networkInterface;

    for (networkInterface = netif_list; networkInterface != NULL; networkInterface = networkInterface->next)
    {
        if (num == networkInterface->num)
        {
            return networkInterface;
        }
    }
    return NULL;
}
