//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "netx_sockets.h"

int errorCode;

//--//

#if defined(DEBUG)
#define DEBUG_HANDLE_SOCKET_ERROR(t, a)
// assume there is something to add in later??
#else
#define DEBUG_HANDLE_SOCKET_ERROR(t, a)
#endif

//

// declaration of function not available in standard lwIP API
extern "C"
{
    extern uint32_t lwip_socket_get_err(int s);
}
//--//

NETX_SOCKETS_Driver g_NETX_SOCKETS_Driver;

//--//
static HAL_CONTINUATION PostAddressChangedContinuation;
static HAL_CONTINUATION PostAvailabilityOnContinuation;
static HAL_CONTINUATION PostAvailabilityOffContinuation;

void NETX_SOCKETS_Driver::PostAddressChanged(void *arg)
{
    (void)arg;

    Network_PostEvent(NetworkChange_NetworkEventType_AddressChanged, 0, 0);
}

void NETX_SOCKETS_Driver::PostAvailabilityOn(void *arg)
{
    (void)arg;

    Network_PostEvent(
        NetworkChange_NetworkEventType_AvailabilityChanged,
        NetworkChange_NetworkEvents_NetworkAvailable,
        0);
}

void NETX_SOCKETS_Driver::PostAvailabilityOff(void *arg)
{
    (void)arg;

    Network_PostEvent(
        NetworkChange_NetworkEventType_AvailabilityChanged,
        NetworkChange_NetworkEvents_NetworkNOTAvailable,
        0);
}

HRESULT NETX_SOCKETS_Driver::Link_status(uint32_t interfaceIndex, bool *status)
{
    (void)interfaceIndex;

    uint32_t actual_status;

    if (nx_ip_status_check(&IpInstance, NX_IP_LINK_ENABLED, &actual_status, NX_NO_WAIT) != NX_SUCCESS)
    {
        return CLR_E_FAIL;
    }

    *status = (actual_status & NX_IP_LINK_ENABLED);

    return S_OK;
}

HRESULT NETX_SOCKETS_Driver::IPV4AddressFromString(const char *ipString, uint64_t *address)
{
    struct in_addr ipv4_addr;

    if (inet_aton(ipString, &ipv4_addr))
    {
        *address = ipv4_addr.s_addr;
    }
    else
    {
        return CLR_E_INVALID_PARAMETER;
    }

    return S_OK;
}

const char *NETX_SOCKETS_Driver::IPV4AddressToString(uint32_t address)
{
    // get IP v4 address in numeric format
    // FIXME IPV6
    struct in_addr ip4Address = {address};

    return inet_ntoa(ip4Address);
}

void NETX_SOCKETS_Driver::Link_callback(NX_IP *ipInstance)
{
    uint32_t actual_status;

    if (nx_ip_status_check(ipInstance, NX_IP_LINK_ENABLED, &actual_status, NX_NO_WAIT) == NX_SUCCESS)
    {

        if ((actual_status & NX_IP_LINK_ENABLED))
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
}

void NETX_SOCKETS_Driver::Status_callback(NX_IP *ipInstance)
{
    (void)ipInstance;

    if (!PostAddressChangedContinuation.IsLinked())
    {
        PostAddressChangedContinuation.Enqueue();
    }

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

    // FIXME debug_printf("IP Address: %d.%d.%d.%d\n", (netif->ip_addr.u_addr.ip4.addr >> 0) & 0xFF,
    // 	(netif->ip_addr.u_addr.ip4.addr >> 8) & 0xFF,
    // 	(netif->ip_addr.u_addr.ip4.addr >> 16) & 0xFF,
    // 	(netif->ip_addr.u_addr.ip4.addr >> 24) & 0xFF);

    // if (netif->flags & NETIF_FLAG_ETHARP)
    // {
    // ip_addr_t * dns1 = dns_getserver(0);
    // ip_addr_t * dns2 = dns_getserver(1);

    // lcd_printf("         dns1: %d.%d.%d.%d\n", (dns1.addr >> 0) & 0xFF,
    // 	(dns1.addr >> 8) & 0xFF,
    // 	(dns1.addr >> 16) & 0xFF,
    // 	(dns1.addr >> 24) & 0xFF);

    // lcd_printf("         dns2: %d.%d.%d.%d\n", (dns2.addr >> 0) & 0xFF,
    // 	(dns2.addr >> 8) & 0xFF,
    // 	(dns2.addr >> 16) & 0xFF,
    // 	(dns2.addr >> 24) & 0xFF);
    //}
#endif

    Events_Set(SYSTEM_EVENT_FLAG_SOCKET);
    Events_Set(SYSTEM_EVENT_FLAG_NETWORK);
}

bool NETX_SOCKETS_Driver::Initialize()
{
    NATIVE_PROFILE_PAL_NETWORK();

    HAL_Configuration_NetworkInterface networkConfiguration;
    int interfaceNumber;

    PostAddressChangedContinuation.InitializeCallback(PostAddressChanged, NULL);
    PostAvailabilityOnContinuation.InitializeCallback(PostAvailabilityOn, NULL);
    PostAvailabilityOffContinuation.InitializeCallback(PostAvailabilityOff, NULL);

    // create m_interfaceNumber array
    int interfaceCount = g_TargetConfiguration.NetworkInterfaceConfigs->Count;

    // sanity check for any interfaces
    if (interfaceCount > 0)
    {
        g_NETX_SOCKETS_Driver.m_interfaces =
            (NETX_DRIVER_INTERFACE_DATA *)platform_malloc(interfaceCount * sizeof(NETX_DRIVER_INTERFACE_DATA));

        // Initialize the target board network
        nanoHAL_Network_Initialize();

        for (int i = 0; i < g_TargetConfiguration.NetworkInterfaceConfigs->Count; i++)
        {
            // load network interface configuration from storage
            if (!ConfigurationManager_GetConfigurationBlock(
                    (void *)&networkConfiguration,
                    DeviceConfigurationOption_Network,
                    i))
            {
                // failed to load configuration
                // FIXME output error?
                // move to the next, if any
                continue;
            }

            // sanity check
            if (networkConfiguration.StartupAddressMode == 0)
            {
                return FALSE;
            }

            // Bind and Open the Ethernet driver
            Network_Interface_Bind(i);
            interfaceNumber = Network_Interface_Open(i);

            if (interfaceNumber == SOCK_SOCKET_ERROR)
            {
                DEBUG_HANDLE_SOCKET_ERROR("Network init", FALSE);
                // FIXME			debug_printf("SocketError: %d\n", errorCode);
                continue;
            }

            g_NETX_SOCKETS_Driver.m_interfaces[i].m_interfaceNumber = interfaceNumber;

            UpdateAdapterConfiguration(
                i,
                (NetworkInterface_UpdateOperation_Dhcp | NetworkInterface_UpdateOperation_Dns),
                &networkConfiguration);

            // TODO NETWORK
            // netif_set_link_callback(networkInterface, Link_callback);

            // if (netif_is_link_up(networkInterface))
            // {
            //     Link_callback(networkInterface);
            // }
            // netif_set_status_callback(networkInterface, Status_callback);

            // if (netif_is_up(networkInterface))
            // {
            //     Status_callback(networkInterface);
            // }

            // default debugger interface
            if (0 == i)
            {
                // uint8_t* addr = (uint8_t*)&networkInterface->ip_addr.addr;
                //                lcd_printf("\f\n\n\n\n\n\n\nip address: %d.%d.%d.%d\r\n", addr[0], addr[1], addr[2],
                //                addr[3]);
                // FIXME               debug_printf("ip address from interface info: %d.%d.%d.%d\r\n", addr[0], addr[1],
                // addr[2], addr[3]);
            }
        }
    }

    return TRUE;
}

bool NETX_SOCKETS_Driver::Uninitialize()
{
    NATIVE_PROFILE_PAL_NETWORK();

    PostAddressChangedContinuation.Abort();
    PostAvailabilityOnContinuation.Abort();
    PostAvailabilityOffContinuation.Abort();

    for (int i = 0; i < g_TargetConfiguration.NetworkInterfaceConfigs->Count; i++)
    {
        Network_Interface_Close(i);
    }

    nanoHAL_Network_Uninitialize();

    // FIXME    tcpip_shutdown();
    // tcpip_shutdown is MS method added to lwip tcpip.c

    return TRUE;
}

extern void debug_printf(const char *format, ...);

SOCK_SOCKET NETX_SOCKETS_Driver::Socket(int family, int type, int protocol)
{
    NATIVE_PROFILE_PAL_NETWORK();

    switch (protocol)
    {
        case SOCK_IPPROTO_TCP:
            protocol = IPPROTO_TCP;
            break;
        case SOCK_IPPROTO_UDP:
            protocol = IPPROTO_UDP;
            break;
        case SOCK_IPPROTO_ICMP:
            protocol = IPPROTO_ICMP;
            break;

        case SOCK_IPPROTO_IGMP:
            protocol = IPPROTO_IGMP;
            break;
    }

    errorCode = socket(family, type, protocol);

    return errorCode;
}

int NETX_SOCKETS_Driver::Bind(SOCK_SOCKET socket, const SOCK_sockaddr *address, int addressLen)
{
    NATIVE_PROFILE_PAL_NETWORK();

    sockaddr_in addr;

    SOCK_SOCKADDR_TO_SOCKADDR(address, addr, &addressLen);

    errorCode = bind(socket, (sockaddr *)&addr, addressLen);

    return errorCode;
}

int NETX_SOCKETS_Driver::Connect(SOCK_SOCKET socket, const SOCK_sockaddr *address, int addressLen)
{
    NATIVE_PROFILE_PAL_NETWORK();

    sockaddr_in addr;

    SOCK_SOCKADDR_TO_SOCKADDR(address, addr, &addressLen);

    errorCode = connect(socket, (sockaddr *)&addr, addressLen);

    return errorCode;
}

int NETX_SOCKETS_Driver::Send(SOCK_SOCKET socket, const char *buf, int len, int flags)
{
    NATIVE_PROFILE_PAL_NETWORK();

    errorCode = send(socket, (const CHAR *)buf, len, flags);

    return errorCode;
}

int NETX_SOCKETS_Driver::Recv(SOCK_SOCKET socket, char *buf, int len, int flags)
{
    NATIVE_PROFILE_PAL_NETWORK();
    int nativeFlag;

    switch (flags)
    {
        case SOCKET_READ_PEEK_OPTION:
            nativeFlag = MSG_PEEK;
            break;
        default:
            nativeFlag = flags;
            break;
    }

    errorCode = recv(socket, (void *)buf, len, nativeFlag);

    return errorCode;
}

int NETX_SOCKETS_Driver::Close(SOCK_SOCKET socket)
{
    NATIVE_PROFILE_PAL_NETWORK();

    errorCode = soc_close(socket);

    return errorCode;
}

int NETX_SOCKETS_Driver::Listen(SOCK_SOCKET socket, int backlog)
{
    NATIVE_PROFILE_PAL_NETWORK();

    errorCode = listen(socket, backlog);

    return errorCode;
}

SOCK_SOCKET NETX_SOCKETS_Driver::Accept(SOCK_SOCKET socket, SOCK_sockaddr *address, int *addressLen)
{
    NATIVE_PROFILE_PAL_NETWORK();
    sockaddr_in addr;

    if (address)
    {
        SOCK_SOCKADDR_TO_SOCKADDR(address, addr, addressLen);
    }

    errorCode = accept(socket, address ? (sockaddr *)&addr : NULL, addressLen);

    if (address)
    {
        SOCKADDR_TO_SOCK_SOCKADDR(address, addr, addressLen);
    }

    return errorCode;
}

int NETX_SOCKETS_Driver::Shutdown(SOCK_SOCKET socket, int how)
{
    (void)socket;
    (void)how;

    NATIVE_PROFILE_PAL_NETWORK();

    // TODO NETWORK
    // return shutdown(socket, how);
    return -1;
}

int NETX_SOCKETS_Driver::GetAddrInfo(
    const char *nodename,
    char *servname,
    const SOCK_addrinfo *hints,
    SOCK_addrinfo **res)
{
    NATIVE_PROFILE_PAL_NETWORK();

    SOCK_addrinfo *ai;
    void *dummyPtr;
    SOCK_sockaddr_in *sa = NULL;
    int total_size = sizeof(SOCK_addrinfo) + sizeof(SOCK_sockaddr_in);
    struct addrinfo *netxAddrinfo = NULL;

    if (res == NULL)
        return SOCK_SOCKET_ERROR;

    *res = NULL;

    // if the nodename == "" then return the IP address of this device
    if (nodename[0] == 0 && servname == NULL)
    {
        uint32_t ip_address;
        uint32_t network_mask;

        if (nx_ip_interface_address_get(&IpInstance, 0, &ip_address, &network_mask) != NX_SUCCESS)
        {
            return -1;
        }

        ai = (SOCK_addrinfo *)platform_malloc(total_size);
        if (ai == NULL)
        {
            return -1;
        }
        memset(ai, 0, total_size);
        sa = (SOCK_sockaddr_in *)((uint8_t *)ai + sizeof(SOCK_addrinfo));

        /* set up sockaddr */
        sa->sin_addr.S_un.S_addr = ip_address;

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

        // need this to keep the compiler happy about the cast to SOCK_sockaddr
        // which is intended and perfectly safe
        dummyPtr = sa;

        ai->ai_addrlen = sizeof(SOCK_sockaddr_in);
        ai->ai_addr = (SOCK_sockaddr *)dummyPtr;

        *res = ai;

        return 0;
    }

    int err = getaddrinfo(nodename, servname, (addrinfo *)hints, &netxAddrinfo);

    if (err == 0)
    {
        ///
        /// Marshal addrinfo data
        ///
        struct sockaddr_in *lwip_sockaddr_in;

        ai = (SOCK_addrinfo *)platform_malloc(total_size);
        if (ai == NULL)
        {
            freeaddrinfo(netxAddrinfo);
            return -1;
        }
        memset(ai, 0, total_size);

        lwip_sockaddr_in = ((struct sockaddr_in *)netxAddrinfo->ai_addr);

        sa = (SOCK_sockaddr_in *)((uint8_t *)ai + sizeof(SOCK_addrinfo));
        /* set up sockaddr */
        sa->sin_addr.S_un.S_addr = lwip_sockaddr_in->sin_addr.s_addr;
        sa->sin_family = lwip_sockaddr_in->sin_family;
        sa->sin_port = lwip_sockaddr_in->sin_port;

        /* set up addrinfo */
        ai->ai_family = netxAddrinfo->ai_family;
        if (hints != NULL)
        {
            /* copy socktype & protocol from hints if specified */
            ai->ai_socktype = hints->ai_socktype;
            ai->ai_protocol = hints->ai_protocol;
        }

        // need this to keep the compiler happy about the cast to SOCK_sockaddr
        // which is intended and perfectly safe
        dummyPtr = sa;

        ai->ai_addrlen = sizeof(SOCK_sockaddr_in);
        ai->ai_addr = (SOCK_sockaddr *)dummyPtr;

        *res = ai;

        // free marshalled addrinfo
        platform_free(netxAddrinfo);

        return 0;
    }
    else
    {
        // map DNS error with socket errors
        switch (err)
        {
                // TODO NETWORK
                // case HOST_NOT_FOUND:
                //     errorCode = SOCK_HOST_NOT_FOUND;
                //     break;

            default:
                errorCode = SOCK_EINVAL;
        }
    }

    return SOCK_SOCKET_ERROR;
}

void NETX_SOCKETS_Driver::FreeAddrInfo(SOCK_addrinfo *ai)
{
    NATIVE_PROFILE_PAL_NETWORK();

    SOCK_addrinfo *next;

    while (ai != NULL)
    {
        next = ai->ai_next;
        platform_free(ai);
        ai = next;
    }
}

int NETX_SOCKETS_Driver::Ioctl(SOCK_SOCKET socket, int cmd, int *data)
{
    NATIVE_PROFILE_PAL_NETWORK();

    return ioctl(socket, cmd, data);
}

int NETX_SOCKETS_Driver::GetLastError()
{
    NATIVE_PROFILE_PAL_NETWORK();

    return GetNativeError(errorCode);
}

int NETX_SOCKETS_Driver::GetSockLastError(SOCK_SOCKET socket)
{
    (void)socket;

    NATIVE_PROFILE_PAL_NETWORK();

    // get last error number from socket
    return errorCode;
}

static int MARSHAL_SOCK_FDSET_TO_FDSET(SOCK_fd_set *sf, fd_set *f)
{
    if (f != NULL && sf != NULL)
    {
        FD_ZERO(f);

        for (unsigned int i = 0; i < sf->fd_count; i++)
        {
            FD_SET(sf->fd_array[i], f);
        }
        return sf->fd_count;
    }

    return 0;
}

static void MARSHAL_FDSET_TO_SOCK_FDSET(SOCK_fd_set *sf, fd_set *f)
{
    if (sf != NULL && f != NULL)
    {
        int cnt = sf->fd_count;
        sf->fd_count = 0;

        for (int i = 0; i < cnt; i++)
        {
            if (FD_ISSET(sf->fd_array[i], f))
            {
                sf->fd_array[sf->fd_count] = sf->fd_array[i];
                sf->fd_count++;
            }
        }
    }
}

int NETX_SOCKETS_Driver::Select(
    int nfds,
    SOCK_fd_set *readfds,
    SOCK_fd_set *writefds,
    SOCK_fd_set *exceptfds,
    const SOCK_timeval *timeout)
{
    (void)nfds;

    NATIVE_PROFILE_PAL_NETWORK();
    int ret = 0;

    fd_set read;
    fd_set write;
    fd_set excpt;

    fd_set *pR = (readfds != NULL) ? &read : NULL;
    fd_set *pW = (writefds != NULL) ? &write : NULL;
    fd_set *pE = (exceptfds != NULL) ? &excpt : NULL;

    // If the network goes down then we should alert any pending socket actions
    if (exceptfds != NULL && exceptfds->fd_count > 0)
    {
        uint32_t actual_status;

        if (nx_ip_status_check(&IpInstance, NX_IP_LINK_ENABLED, &actual_status, NX_NO_WAIT) != NX_SUCCESS)
        {
            return -1;
        }

        if (!(actual_status & NX_IP_LINK_ENABLED))
        {
            if (readfds != NULL)
            {
                readfds->fd_count = 0;
            }

            if (writefds != NULL)
            {
                writefds->fd_count = 0;
            }

            errorCode = ENETDOWN;

            return exceptfds->fd_count;
        }
    }

    MARSHAL_SOCK_FDSET_TO_FDSET(readfds, pR);
    MARSHAL_SOCK_FDSET_TO_FDSET(writefds, pW);
    MARSHAL_SOCK_FDSET_TO_FDSET(exceptfds, pE);

    int max_sd = NX_BSD_MAX_SOCKETS;

    // developer note:
    // our declaration of SOCK_timeval is dependent of "long" type which is platform dependent
    // so it's not safe to cast it to "timeval"
    timeval timeoutCopy;
    timeoutCopy.tv_sec = timeout->tv_sec;
    timeoutCopy.tv_usec = timeout->tv_usec;

    ret = select(max_sd, pR, pW, pE, &timeoutCopy);

    MARSHAL_FDSET_TO_SOCK_FDSET(readfds, pR);
    MARSHAL_FDSET_TO_SOCK_FDSET(writefds, pW);
    MARSHAL_FDSET_TO_SOCK_FDSET(exceptfds, pE);

    return ret;
}

int NETX_SOCKETS_Driver::SetSockOpt(SOCK_SOCKET socket, int level, int optname, const char *optval, int optlen)
{
    NATIVE_PROFILE_PAL_NETWORK();
    int nativeLevel;
    int nativeOptionName;
    int nativeIntValue;
    char *pNativeOptionValue = (char *)optval;
    struct linger lopt = {0, 0};

    switch (level)
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
            nativeLevel = SOL_SOCKET;
            nativeOptionName = GetNativeSockOption(optname);

            switch (optname)
            {
                // If linger value negative then linger off
                // otherwise enabled and linger value is number of seconds
                case SOCK_SOCKO_LINGER:
                {
                    int lingerValue = *(int *)optval;
                    if (lingerValue >= 0)
                    {
                        lopt.l_onoff = 1;
                        lopt.l_linger = abs(lingerValue);
                    }
                    pNativeOptionValue = (char *)&lopt;
                    optlen = sizeof(lopt);
                }
                break;

                case SOCK_SOCKO_DONTLINGER:
                case SOCK_SOCKO_EXCLUSIVEADDRESSUSE:
                    nativeIntValue = !*(int *)optval;
                    pNativeOptionValue = (char *)&nativeIntValue;
                    break;
                default:
                    break;
            }
            break;
        default:
            nativeLevel = 0;
            nativeOptionName = 0;
            break;
    }

    return setsockopt(socket, nativeLevel, nativeOptionName, pNativeOptionValue, optlen);
}

int NETX_SOCKETS_Driver::GetSockOpt(SOCK_SOCKET socket, int level, int optname, char *optval, int *optlen)
{
    NATIVE_PROFILE_PAL_NETWORK();
    int nativeLevel;
    int nativeOptionName;
    char *pNativeOptval = optval;
    int ret;

    switch (level)
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
            nativeLevel = SOL_SOCKET;
            nativeOptionName = GetNativeSockOption(optname);
            break;
        default:
            nativeLevel = level;
            nativeOptionName = optname;
            break;
    }

    ret = getsockopt(socket, nativeLevel, nativeOptionName, pNativeOptval, optlen);

    if (ret == 0)
    {
        switch (level)
        {
            case SOCK_SOL_SOCKET:
                switch (optname)
                {
                    case SOCK_SOCKO_EXCLUSIVEADDRESSUSE:
                    case SOCK_SOCKO_DONTLINGER:
                        *optval = !(*(int *)optval != 0);
                        break;

                    case SOCK_SOCKO_ACCEPTCONNECTION:
                    case SOCK_SOCKO_BROADCAST:
                    case SOCK_SOCKO_KEEPALIVE:
                        *optval = (*(int *)optval != 0);
                        break;
                }
                break;
        }
    }

    return ret;
}

int NETX_SOCKETS_Driver::GetPeerName(SOCK_SOCKET socket, SOCK_sockaddr *name, int *namelen)
{
    NATIVE_PROFILE_PAL_NETWORK();
    int ret;

    sockaddr_in addr;

    SOCK_SOCKADDR_TO_SOCKADDR(name, addr, namelen);

    ret = getpeername(socket, (sockaddr *)&addr, namelen);

    SOCKADDR_TO_SOCK_SOCKADDR(name, addr, namelen);

    return ret;
}

int NETX_SOCKETS_Driver::GetSockName(SOCK_SOCKET socket, SOCK_sockaddr *name, int *namelen)
{
    NATIVE_PROFILE_PAL_NETWORK();
    int ret;

    sockaddr_in addr;

    SOCK_SOCKADDR_TO_SOCKADDR(name, addr, namelen);

    ret = getsockname(socket, (sockaddr *)&addr, namelen);

    SOCKADDR_TO_SOCK_SOCKADDR(name, addr, namelen);

    return ret;
}

int NETX_SOCKETS_Driver::RecvFrom(SOCK_SOCKET socket, char *buf, int len, int flags, SOCK_sockaddr *from, int *fromlen)
{
    NATIVE_PROFILE_PAL_NETWORK();
    sockaddr_in addr;
    sockaddr *pFrom = NULL;
    int ret;

    if (from)
    {
        SOCK_SOCKADDR_TO_SOCKADDR(from, addr, fromlen);
        pFrom = (sockaddr *)&addr;
    }

    ret = recvfrom(socket, buf, len, flags, pFrom, fromlen);

    if (from && ret != SOCK_SOCKET_ERROR)
    {
        SOCKADDR_TO_SOCK_SOCKADDR(from, addr, fromlen);
    }

    return ret;
}

int NETX_SOCKETS_Driver::SendTo(
    SOCK_SOCKET socket,
    const char *buf,
    int len,
    int flags,
    const SOCK_sockaddr *to,
    int tolen)
{
    NATIVE_PROFILE_PAL_NETWORK();

    sockaddr_in addr;

    SOCK_SOCKADDR_TO_SOCKADDR(to, addr, &tolen);

    return sendto(socket, (CHAR *)buf, len, flags, (sockaddr *)&addr, tolen);
}

// this implementation it's weak so it can be replaced at plaftorm level
__nfweak HRESULT
    NETX_SOCKETS_Driver::LoadAdapterConfiguration(HAL_Configuration_NetworkInterface *config, uint32_t interfaceIndex)
{
    (void)interfaceIndex;

    NATIVE_PROFILE_PAL_NETWORK();

    if (config->StartupAddressMode == AddressMode_DHCP)
    {
#ifndef NX_DISABLE_IPV6
        config->IPv4Address = networkInterface->ip_addr.u_addr.ip4.addr;
        config->IPv4NetMask = networkInterface->netmask.u_addr.ip4.addr;
        config->IPv4GatewayAddress = networkInterface->gw.u_addr.ip4.addr;

        // FIXME IPV6
        // config->IPv6Address     = networkInterface->ip_addr.u_addr.ip6.addr;
        // config->IPv6NetMask = networkInterface->netmask.u_addr.ip6.addr;
        // config->IPv6GatewayAddress    = networkInterface->gw.u_addr.ip6.addr;
#else
        uint32_t ip_address;
        uint32_t network_mask;

        if (nx_ip_interface_address_get(&IpInstance, 0, &ip_address, &network_mask) != NX_SUCCESS)
        {
            return S_FALSE;
        }

        config->IPv4Address = ip_address;
        config->IPv4NetMask = network_mask;

        uint32_t gwAddress;

        if (nx_ip_gateway_address_get(&IpInstance, &gwAddress) != NX_SUCCESS)
        {
            return S_FALSE;
        }

        config->IPv4GatewayAddress = ip_address;
#endif

        UCHAR dnsIpString[4];
        UINT dnsSize;

        if (nx_dhcp_interface_user_option_retrieve(&DhcpInstance, 0, NX_DHCP_OPTION_DNS_SVR, dnsIpString, &dnsSize) !=
            NX_SUCCESS)
        {
            return S_FALSE;
        }

        config->IPv4DNSAddress1 = nx_dhcp_user_option_convert(dnsIpString);
        config->IPv4DNSAddress2 = 0;
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

// this implementation it's weak so it can be replaced at plaftorm level
__nfweak HRESULT NETX_SOCKETS_Driver::UpdateAdapterConfiguration(
    uint32_t interfaceIndex,
    uint32_t updateFlags,
    HAL_Configuration_NetworkInterface *config)
{
    (void)interfaceIndex;

    NATIVE_PROFILE_PAL_NETWORK();

    bool enableDHCP = (config->StartupAddressMode == AddressMode_DHCP);

    // when using DHCP do not use the static settings
    if (0 != (updateFlags & NetworkInterface_UpdateOperation_Dns))
    {
        // FIXME IPV6
        if (config->AutomaticDNS == 0)
        {
            // user defined DNS addresses
            if (config->IPv4DNSAddress1 != 0)
            {
                // TODO NETWORK
                // nx_dns_server_remove(&DnsInstance, ULONG server_address);

                // // need to convert this first
                // ip_addr_t dnsServer;
                // ip_addr_set_ip4_u32(&dnsServer, config->IPv4DNSAddress1);

                // dns_setserver(0, &dnsServer);
            }
        }
    }

    if (0 != (updateFlags & NetworkInterface_UpdateOperation_Dhcp))
    {
        if (enableDHCP)
        {
            // need to start DHCP
            UINT dhcpStartResult = nx_dhcp_start(&DhcpInstance);

            if (dhcpStartResult != NX_SUCCESS && dhcpStartResult != NX_DHCP_ALREADY_STARTED)
            {
                return CLR_E_FAIL;
            }
        }
        else
        {
            // stop DHCP
            UINT dhcpStopResult = nx_dhcp_stop(&DhcpInstance);

            if (dhcpStopResult != NX_SUCCESS && dhcpStopResult != NX_DHCP_NOT_STARTED)
            {
                return CLR_E_FAIL;
            }

            nx_ip_interface_address_set(&IpInstance, 0, config->IPv4Address, config->IPv4NetMask);

            // we should be polite and let the DHCP server that we are now using a static IP
            nx_dhcp_release(&DhcpInstance);
        }
    }

    if (enableDHCP)
    {
        if (0 != (updateFlags & NetworkInterface_UpdateOperation_DhcpRelease))
        {
            nx_dhcp_release(&DhcpInstance);
        }
        else if (0 != (updateFlags & NetworkInterface_UpdateOperation_DhcpRenew))
        {
            nx_dhcp_force_renew(&DhcpInstance);
        }
        else if (
            0 !=
            (updateFlags & (NetworkInterface_UpdateOperation_DhcpRelease | NetworkInterface_UpdateOperation_DhcpRenew)))
        {
            return CLR_E_INVALID_PARAMETER;
        }
    }

    if (0 != (updateFlags & NetworkInterface_UpdateOperation_Mac))
    {
        // TODO NETWORK
        // at this time changing MAC address is not supported
        return CLR_E_NOT_SUPPORTED;
    }

    return S_OK;
}

int NETX_SOCKETS_Driver::GetNativeTcpOption(int optname)
{
    NATIVE_PROFILE_PAL_NETWORK();
    int nativeOptionName = 0;

    switch (optname)
    {
        case SOCK_TCP_NODELAY:
            nativeOptionName = TCP_NODELAY;
            break;

        case SOCK_SOCKO_KEEPALIVE:
            nativeOptionName = SO_KEEPALIVE;
            break;

        // allow the C# user to specify LWIP options that our managed enum
        // doesn't support
        default:
            nativeOptionName = optname;
            break;
    }

    return nativeOptionName;
}

int NETX_SOCKETS_Driver::GetNativeSockOption(int optname)
{
    NATIVE_PROFILE_PAL_NETWORK();
    int nativeOptionName = 0;

    switch (optname)
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
            nativeOptionName = SO_RCVBUF;
            break;
        case SOCK_SOCKO_SENDBUFFER:
            nativeOptionName = SO_SNDBUF;
            break;
        // not supported
        // case SOCK_SOCKO_ACCEPTCONNECTION:
        // case SOCK_SOCKO_USELOOPBACK:
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

int NETX_SOCKETS_Driver::GetNativeIPOption(int optname)
{
    NATIVE_PROFILE_PAL_NETWORK();
    int nativeOptionName = 0;

    switch (optname)
    {
            // not supported
            // case SOCK_IPO_TTL:
            // case SOCK_IPO_TOS:

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

int NETX_SOCKETS_Driver::GetNativeError(int error)
{
    NATIVE_PROFILE_PAL_NETWORK();
    int ret;

    switch (error)
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
