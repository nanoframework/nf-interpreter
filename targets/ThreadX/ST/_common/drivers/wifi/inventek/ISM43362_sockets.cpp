//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "ISM43362_sockets.h"
#include <nanoCLR_Types.h>

int errorCode;

//--//

#if defined(DEBUG)
#define DEBUG_HANDLE_SOCKET_ERROR(t, a)
// assume there is something to add in later??
#else
#define DEBUG_HANDLE_SOCKET_ERROR(t, a)
#endif

#define IP4ADDR_STRLEN_MAX 16

#define ISM43362_AVAILABLE_SOCKETS 4
#define LOCAL_PORT_START           0xC000
#define LOCAL_PORT_MAX             0xFFFF

//--//

ISM43362_SOCKETS_Driver g_ISM43362_SOCKETS_Driver;
static char str[IP4ADDR_STRLEN_MAX];

static ISM43362_Socket _sockets[ISM43362_AVAILABLE_SOCKETS];

//--//
static HAL_CONTINUATION PostAddressChangedContinuation;
static HAL_CONTINUATION PostAvailabilityOnContinuation;
static HAL_CONTINUATION PostAvailabilityOffContinuation;

// generate ramdon port number
uint16_t GetRandomPortNumber(void)
{
    uint16_t port = 0;

    while (port == 0)
    {
        port = LOCAL_PORT_START + (rand() % ((LOCAL_PORT_MAX + 1) - LOCAL_PORT_START));

        // check if this port is in use
        for (int i = 0; i < ISM43362_AVAILABLE_SOCKETS; i++)
        {
            if (_sockets[i].protocol != 0xFF && _sockets[i].localPort == port)
            {
                // try again
                port = 0;
                break;
            }
        }
    }

    return port;
}

void ISM43362_SOCKETS_Driver::PostAddressChanged(void *arg)
{
    (void)arg;

    Network_PostEvent(NetworkChange_NetworkEventType_AddressChanged, 0, 0);
}

void ISM43362_SOCKETS_Driver::PostAvailabilityOn(void *arg)
{
    (void)arg;

    Network_PostEvent(
        NetworkChange_NetworkEventType_AvailabilityChanged,
        NetworkChange_NetworkEvents_NetworkAvailable,
        0);
}

void ISM43362_SOCKETS_Driver::PostAvailabilityOff(void *arg)
{
    (void)arg;

    Network_PostEvent(
        NetworkChange_NetworkEventType_AvailabilityChanged,
        NetworkChange_NetworkEvents_NetworkNOTAvailable,
        0);
}

HRESULT ISM43362_SOCKETS_Driver::Link_status(uint32_t interfaceIndex, bool *status)
{
    (void)interfaceIndex;
    (void)status;

    // TODO
    // uint32_t actual_status;
    // if (nx_ip_status_check(&IpInstance, NX_IP_LINK_ENABLED, &actual_status, NX_NO_WAIT) != NX_SUCCESS)
    // {
    //     return CLR_E_FAIL;
    // }

    // *status = (actual_status & NX_IP_LINK_ENABLED);

    return S_OK;
}

HRESULT ISM43362_SOCKETS_Driver::IPAddressFromString(const char *ipString, uint64_t *address)
{
    (void)ipString;
    (void)address;
    // TODO
    // struct in_addr ipv4_addr;

    // if (inet_aton(ipString, &ipv4_addr))
    // {
    //     *address = ipv4_addr.s_addr;
    // }
    // else
    // {
    //     return CLR_E_INVALID_PARAMETER;
    // }

    return S_OK;
}

const char *ISM43362_SOCKETS_Driver::IPAddressToString(uint32_t address)
{
    char *szBuffer = str;

    size_t iBuffer = IP4ADDR_STRLEN_MAX;
    uint8_t ipAddress[4];

    // get IP v4 address in numeric format
    ipAddress[0] = (uint8_t)((address & 0xFF000000) >> 24);
    ipAddress[1] = (uint8_t)((address & 0x00FF0000) >> 16);
    ipAddress[2] = (uint8_t)((address & 0x0000FF00) >> 8);
    ipAddress[3] = (uint8_t)((address & 0x000000FF));

    // FIXME IPV6
    // struct in_addr ip4Address = {address};

    memset(str, 0, IP4ADDR_STRLEN_MAX);

    CLR_SafeSprintf(szBuffer, iBuffer, "%d.%d.%d.%d", ipAddress[0], ipAddress[1], ipAddress[2], ipAddress[3]);

    return str;
}

// TODO
// void ISM43362_SOCKETS_Driver::Link_callback(NX_IP *ipInstance)
// {
//     uint32_t actual_status;

//     if (nx_ip_status_check(ipInstance, NX_IP_LINK_ENABLED, &actual_status, NX_NO_WAIT) == NX_SUCCESS)
//     {

//         if ((actual_status & NX_IP_LINK_ENABLED))
//         {
//             if (!PostAvailabilityOnContinuation.IsLinked())
//                 PostAvailabilityOnContinuation.Enqueue();
//         }
//         else
//         {
//             if (!PostAvailabilityOffContinuation.IsLinked())
//                 PostAvailabilityOffContinuation.Enqueue();
//         }

//         Events_Set(SYSTEM_EVENT_FLAG_SOCKET);
//         Events_Set(SYSTEM_EVENT_FLAG_NETWORK);
//     }
// }

// TODO
// void ISM43362_SOCKETS_Driver::Status_callback(NX_IP *ipInstance)
// {
//     (void)ipInstance;

//     if (!PostAddressChangedContinuation.IsLinked())
//     {
//         PostAddressChangedContinuation.Enqueue();
//     }

// #if !defined(BUILD_RTM)
//     // lcd_printf("\f\n\n\n\n\n\nLink Update: %s\n", (netif_is_up(netif) ? "UP  " : "DOWN"));
//     // lcd_printf("         IP: %d.%d.%d.%d\n", (netif->ip_addr.addr >> 0) & 0xFF,
//     // 	(netif->ip_addr.addr >> 8) & 0xFF,
//     // 	(netif->ip_addr.addr >> 16) & 0xFF,
//     // 	(netif->ip_addr.addr >> 24) & 0xFF);
//     // lcd_printf("         SM: %d.%d.%d.%d\n", (netif->netmask.addr >> 0) & 0xFF,
//     // 	(netif->netmask.addr >> 8) & 0xFF,
//     // 	(netif->netmask.addr >> 16) & 0xFF,
//     // 	(netif->netmask.addr >> 24) & 0xFF);
//     // lcd_printf("         GW: %d.%d.%d.%d\n", (netif->gw.addr >> 0) & 0xFF,
//     // 	(netif->gw.addr >> 8) & 0xFF,
//     // 	(netif->gw.addr >> 16) & 0xFF,
//     // 	(netif->gw.addr >> 24) & 0xFF);

//     // FIXME debug_printf("IP Address: %d.%d.%d.%d\n", (netif->ip_addr.u_addr.ip4.addr >> 0) & 0xFF,
//     // 	(netif->ip_addr.u_addr.ip4.addr >> 8) & 0xFF,
//     // 	(netif->ip_addr.u_addr.ip4.addr >> 16) & 0xFF,
//     // 	(netif->ip_addr.u_addr.ip4.addr >> 24) & 0xFF);

//     // if (netif->flags & NETIF_FLAG_ETHARP)
//     // {
//     // ip_addr_t * dns1 = dns_getserver(0);
//     // ip_addr_t * dns2 = dns_getserver(1);

//     // lcd_printf("         dns1: %d.%d.%d.%d\n", (dns1.addr >> 0) & 0xFF,
//     // 	(dns1.addr >> 8) & 0xFF,
//     // 	(dns1.addr >> 16) & 0xFF,
//     // 	(dns1.addr >> 24) & 0xFF);

//     // lcd_printf("         dns2: %d.%d.%d.%d\n", (dns2.addr >> 0) & 0xFF,
//     // 	(dns2.addr >> 8) & 0xFF,
//     // 	(dns2.addr >> 16) & 0xFF,
//     // 	(dns2.addr >> 24) & 0xFF);
//     //}
// #endif

//     Events_Set(SYSTEM_EVENT_FLAG_SOCKET);
//     Events_Set(SYSTEM_EVENT_FLAG_NETWORK);
// }

bool ISM43362_SOCKETS_Driver::Initialize()
{
    NATIVE_PROFILE_PAL_NETWORK();

    HAL_Configuration_NetworkInterface networkConfiguration;
    int interfaceNumber;

    PostAddressChangedContinuation.InitializeCallback(PostAddressChanged, NULL);
    PostAvailabilityOnContinuation.InitializeCallback(PostAvailabilityOn, NULL);
    PostAvailabilityOffContinuation.InitializeCallback(PostAvailabilityOff, NULL);

    // initialize sockets array
    for (int i = 0; i < ISM43362_AVAILABLE_SOCKETS; i++)
    {
        // clear socket struct
        memset(&_sockets[i], 0, sizeof(ISM43362_Socket));
        // this signals that the socket is not in use
        _sockets[i].protocol = 0xFF;
    }

    // create m_interfaceNumber array
    int interfaceCount = g_TargetConfiguration.NetworkInterfaceConfigs->Count;

    // sanity check for any interfaces
    if (interfaceCount > 0)
    {
        g_ISM43362_SOCKETS_Driver.m_interfaces =
            (ISM43362_DRIVER_INTERFACE_DATA *)platform_malloc(interfaceCount * sizeof(ISM43362_DRIVER_INTERFACE_DATA));

        // Initialize the target board network
        // TODO: consider removing this
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

            g_ISM43362_SOCKETS_Driver.m_interfaces[i].m_interfaceNumber = interfaceNumber;

            if (UpdateAdapterConfiguration(
                    i,
                    (NetworkInterface_UpdateOperation_Dhcp | NetworkInterface_UpdateOperation_Dns),
                    &networkConfiguration) != S_OK)
            {
                return FALSE;
            }

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

bool ISM43362_SOCKETS_Driver::Uninitialize()
{
    NATIVE_PROFILE_PAL_NETWORK();

    PostAddressChangedContinuation.Abort();
    PostAvailabilityOnContinuation.Abort();
    PostAvailabilityOffContinuation.Abort();

    for (int i = 0; i < g_TargetConfiguration.NetworkInterfaceConfigs->Count; i++)
    {
        Network_Interface_Close(i);
    }

    // FIXME    tcpip_shutdown();
    // tcpip_shutdown is MS method added to lwip tcpip.c

    return TRUE;
}

extern void debug_printf(const char *format, ...);

SOCK_SOCKET ISM43362_SOCKETS_Driver::Socket(int family, int type, int protocol)
{
    (void)family;
    (void)type;

    NATIVE_PROFILE_PAL_NETWORK();

    int8_t socketIndex = -1;

    if (protocol != SOCK_IPPROTO_TCP && protocol != SOCK_IPPROTO_UDP)
    {
        return SOCK_SOCKET_ERROR;
    }

    // find a free socket
    for (int i = 0; i < ISM43362_AVAILABLE_SOCKETS; i++)
    {
        if (_sockets[i].protocol == 0xFF)
        {
            // found a free socket
            socketIndex = i;

            // done here
            break;
        }
    }

    if (socketIndex == -1)
    {
        return SOCK_SOCKET_ERROR;
    }
    else
    {
        // set protocol
        _sockets[socketIndex].protocol = protocol == SOCK_IPPROTO_TCP ? WIFI_TCP_PROTOCOL : WIFI_UDP_PROTOCOL;

        // clear error
        errorCode = 0;

        return socketIndex;
    }
}

int ISM43362_SOCKETS_Driver::Bind(SOCK_SOCKET socket, const SOCK_sockaddr *address, int addressLen)
{
    (void)socket;
    (void)address;
    (void)addressLen;

    NATIVE_PROFILE_PAL_NETWORK();

    // TODO
    //     sockaddr_in addr;

    //     SOCK_SOCKADDR_TO_SOCKADDR(address, addr, &addressLen);

    // errorCode = bind(socket, (sockaddr *)&addr, addressLen);

    return errorCode;
}

int ISM43362_SOCKETS_Driver::Connect(SOCK_SOCKET socket, const SOCK_sockaddr *address, int addressLen)
{
    (void)addressLen;

    NATIVE_PROFILE_PAL_NETWORK();

    uint8_t ipAddress[4];
    uint16_t ipPort;

    SOCK_SOCKADDR_TO_IP_DATA(address);

    // fill socket details
    _sockets[socket].isClient = true;
    _sockets[socket].remotePort = ipPort;
    _sockets[socket].remoteIP = ((SOCK_sockaddr_in *)address)->sin_addr.S_un.S_addr;
    _sockets[socket].localPort = GetRandomPortNumber();
    _sockets[socket].isSecure = false;

    if (WIFI_OpenClientConnection(
            socket,
            (WIFI_Protocol_t)_sockets[socket].protocol,
            _sockets[socket].isSecure,
            "",
            ipAddress,
            ipPort,
            _sockets[socket].localPort) != WIFI_STATUS_OK)
    {
        errorCode = SOCK_SOCKET_ERROR;
    }
    else
    {
        errorCode = 0;
    }

    return errorCode;
}

int ISM43362_SOCKETS_Driver::Send(SOCK_SOCKET socket, const char *buf, int len, int flags)
{
    (void)flags;

    NATIVE_PROFILE_PAL_NETWORK();
    uint16_t bytesSent = 0;

    if (WIFI_SendData((uint8_t)socket, (uint8_t *)buf, len, &bytesSent, 100) != WIFI_STATUS_OK)
    {
        errorCode = SOCK_SOCKET_ERROR;
        return -1;
    }

    return bytesSent;
}

int ISM43362_SOCKETS_Driver::Recv(SOCK_SOCKET socket, char *buf, int len, int flags)
{
    (void)flags;

    NATIVE_PROFILE_PAL_NETWORK();
    uint16_t bytesReceived = 0;

    if (WIFI_ReceiveData((uint8_t)socket, (uint8_t *)buf, len, &bytesReceived, 1000) != WIFI_STATUS_OK)
    {
        errorCode = SOCK_SOCKET_ERROR;
        return -1;
    }

    return bytesReceived;
}

int ISM43362_SOCKETS_Driver::Close(SOCK_SOCKET socket)
{
    NATIVE_PROFILE_PAL_NETWORK();

    WIFI_Status_t status;

    if (_sockets[socket].isClient)
    {
        status = WIFI_CloseClientConnection((uint8_t)socket);
    }
    else
    {
        status = WIFI_CloseServerConnection((uint8_t)socket);
    }

    errorCode = status == WIFI_STATUS_OK ? 0 : SOCK_SOCKET_ERROR;

    return errorCode;
}

int ISM43362_SOCKETS_Driver::Listen(SOCK_SOCKET socket, int backlog)
{
    (void)socket;
    (void)backlog;

    NATIVE_PROFILE_PAL_NETWORK();

    // TODO
    // errorCode =  listen(socket, backlog);

    return errorCode;
}

SOCK_SOCKET ISM43362_SOCKETS_Driver::Accept(SOCK_SOCKET socket, SOCK_sockaddr *address, int *addressLen)
{
    (void)socket;
    (void)address;
    (void)addressLen;

    NATIVE_PROFILE_PAL_NETWORK();
    // TODO
    // sockaddr_in addr;

    // if (address)
    // {
    //     SOCK_SOCKADDR_TO_SOCKADDR(address, addr, addressLen);
    // }

    // errorCode = accept(socket, address ? (sockaddr *)&addr : NULL, addressLen);

    // if (address)
    // {
    //     SOCKADDR_TO_SOCK_SOCKADDR(address, addr, addressLen);
    // }

    return errorCode;
}

int ISM43362_SOCKETS_Driver::Shutdown(SOCK_SOCKET socket, int how)
{
    (void)socket;
    (void)how;

    NATIVE_PROFILE_PAL_NETWORK();

    // TODO NETWORK
    // return shutdown(socket, how);
    return -1;
}

int ISM43362_SOCKETS_Driver::GetAddrInfo(
    const char *nodename,
    char *servname,
    const SOCK_addrinfo *hints,
    SOCK_addrinfo **res)
{
    (void)servname;

    NATIVE_PROFILE_PAL_NETWORK();

    uint8_t ipAddress[4];
    SOCK_addrinfo *ai;
    void *dummyPtr;
    SOCK_sockaddr_in *sa = NULL;
    int total_size = sizeof(SOCK_addrinfo) + sizeof(SOCK_sockaddr_in);

    if (res == NULL)
    {
        return SOCK_SOCKET_ERROR;
    }

    *res = NULL;

    // if the nodename == "" then return the IP address of this device
    if (nodename[0] == 0 && servname == NULL)
    {
        if (WIFI_GetIP_Address(ipAddress) != WIFI_STATUS_OK)
        {
            return S_FALSE;
        }

        ai = (SOCK_addrinfo *)platform_malloc(total_size);
        if (ai == NULL)
        {
            return SOCK_SOCKET_ERROR;
        }

        memset(ai, 0, total_size);
        sa = (SOCK_sockaddr_in *)((uint8_t *)ai + sizeof(SOCK_addrinfo));

        // set up sockaddr
        sa->sin_addr.S_un.S_addr = IP_ADDRESS(ipAddress[0], ipAddress[1], ipAddress[2], ipAddress[3]);

        sa->sin_family = SOCK_AF_INET;
        sa->sin_port = 0;

        // set up addrinfo
        ai->ai_family = SOCK_AF_INET;
        if (hints != NULL)
        {
            // copy socktype & protocol from hints if specified
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
    else
    {
        if (WIFI_GetHostAddress(nodename, ipAddress) != WIFI_STATUS_OK)
        {
            errorCode = SOCK_EINVAL;

            return SOCK_SOCKET_ERROR;
        }

        ai = (SOCK_addrinfo *)platform_malloc(total_size);
        if (ai == NULL)
        {
            return SOCK_SOCKET_ERROR;
        }

        memset(ai, 0, total_size);

        sa = (SOCK_sockaddr_in *)((uint8_t *)ai + sizeof(SOCK_addrinfo));

        // set up sockaddr
        sa->sin_addr.S_un.S_addr = (u_long)IP_ADDRESS(ipAddress[0], ipAddress[1], ipAddress[2], ipAddress[3]);
        sa->sin_family = SOCK_AF_INET;
        sa->sin_port = 0;

        // set up addrinfo
        ai->ai_family = SOCK_AF_INET;
        if (hints != NULL)
        {
            // copy socktype & protocol from hints if specified
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

    return SOCK_SOCKET_ERROR;
}

void ISM43362_SOCKETS_Driver::FreeAddrInfo(SOCK_addrinfo *ai)
{
    (void)ai;

    NATIVE_PROFILE_PAL_NETWORK();
}

int ISM43362_SOCKETS_Driver::Ioctl(SOCK_SOCKET socket, int cmd, int *data)
{
    (void)socket;
    (void)cmd;
    (void)data;

    NATIVE_PROFILE_PAL_NETWORK();

    // TODO

    //    return ioctl(socket, cmd, data);
    return 0;
}

int ISM43362_SOCKETS_Driver::GetLastError()
{
    NATIVE_PROFILE_PAL_NETWORK();

    return GetNativeError(errorCode);
}

int ISM43362_SOCKETS_Driver::GetSockLastError(SOCK_SOCKET socket)
{
    (void)socket;

    NATIVE_PROFILE_PAL_NETWORK();

    // get last error number from socket
    return errorCode;
}

// static int MARSHAL_SOCK_FDSET_TO_FDSET(SOCK_fd_set *sf, fd_set *f)
// {
//     (void)sf;
// (void)f;

//     // TODO
//     // if (f != NULL && sf != NULL)
//     // {
//     //     FD_ZERO(f);

//     //     for (unsigned int i = 0; i < sf->fd_count; i++)
//     //     {
//     //         FD_SET(sf->fd_array[i], f);
//     //     }
//     //     return sf->fd_count;
//     // }

//     return 0;
// }

// static void MARSHAL_FDSET_TO_SOCK_FDSET(SOCK_fd_set *sf, fd_set *f)
// {
//     (void)sf;
// (void)f;

//     // TODO
//     // if (sf != NULL && f != NULL)
//     // {
//     //     int cnt = sf->fd_count;
//     //     sf->fd_count = 0;

//     //     for (int i = 0; i < cnt; i++)
//     //     {
//     //         if (FD_ISSET(sf->fd_array[i], f))
//     //         {
//     //             sf->fd_array[sf->fd_count] = sf->fd_array[i];
//     //             sf->fd_count++;
//     //         }
//     //     }
//     // }
// }

int ISM43362_SOCKETS_Driver::Select(
    int nfds,
    SOCK_fd_set *readfds,
    SOCK_fd_set *writefds,
    SOCK_fd_set *exceptfds,
    const SOCK_timeval *timeout)
{
    (void)nfds;
    (void)timeout;

    NATIVE_PROFILE_PAL_NETWORK();

    // implement a check for data in the sockets
    int ret = 1;

    // If the network goes down then we should alert any pending socket actions
    if (exceptfds != NULL && exceptfds->fd_count > 0)
    {
        // check if network is down
        if (WIFI_IsConnected() == WIFI_STATUS_ERROR)
        {
            if (readfds != NULL)
            {
                readfds->fd_count = 0;
            }

            if (writefds != NULL)
            {
                writefds->fd_count = 0;
            }

            errorCode = SOCK_ENETDOWN;

            return exceptfds->fd_count;
        }

        // clear exception state
        exceptfds->fd_count = 0;
    }

    return ret;
}

int ISM43362_SOCKETS_Driver::SetSockOpt(SOCK_SOCKET socket, int level, int optname, const char *optval, int optlen)
{
    (void)socket;
    (void)level;
    (void)optname;
    (void)optval;
    (void)optlen;

    NATIVE_PROFILE_PAL_NETWORK();

    // TODO
    // int nativeLevel;
    // int nativeOptionName;
    // int nativeIntValue;
    // char *pNativeOptionValue = (char *)optval;
    // struct linger lopt = {0, 0};

    // switch (level)
    // {
    //     case SOCK_IPPROTO_IP:
    //         nativeLevel = IPPROTO_IP;
    //         nativeOptionName = GetNativeIPOption(optname);
    //         break;
    //     case SOCK_IPPROTO_TCP:
    //         nativeLevel = IPPROTO_TCP;
    //         nativeOptionName = GetNativeTcpOption(optname);
    //         break;
    //     case SOCK_IPPROTO_UDP:
    //     case SOCK_IPPROTO_ICMP:
    //     case SOCK_IPPROTO_IGMP:
    //     case SOCK_IPPROTO_IPV4:
    //     case SOCK_SOL_SOCKET:
    //         nativeLevel = SOL_SOCKET;
    //         nativeOptionName = GetNativeSockOption(optname);

    //         switch (optname)
    //         {
    //             // If linger value negative then linger off
    //             // otherwise enabled and linger value is number of seconds
    //             case SOCK_SOCKO_LINGER:
    //             {
    //                 int lingerValue = *(int *)optval;
    //                 if (lingerValue >= 0)
    //                 {
    //                     lopt.l_onoff = 1;
    //                     lopt.l_linger = abs(lingerValue);
    //                 }
    //                 pNativeOptionValue = (char *)&lopt;
    //                 optlen = sizeof(lopt);
    //             }
    //             break;

    //             case SOCK_SOCKO_DONTLINGER:
    //             case SOCK_SOCKO_EXCLUSIVEADDRESSUSE:
    //                 nativeIntValue = !*(int *)optval;
    //                 pNativeOptionValue = (char *)&nativeIntValue;
    //                 break;
    //             default:
    //                 break;
    //         }
    //         break;
    //     default:
    //         nativeLevel = 0;
    //         nativeOptionName = 0;
    //         break;
    // }

    // return setsockopt(socket, nativeLevel, nativeOptionName, pNativeOptionValue, optlen);
    return 0;
}

int ISM43362_SOCKETS_Driver::GetSockOpt(SOCK_SOCKET socket, int level, int optname, char *optval, int *optlen)
{
    (void)socket;
    (void)level;
    (void)optname;
    (void)optval;
    (void)optlen;

    NATIVE_PROFILE_PAL_NETWORK();

    // TODO
    // int nativeLevel;
    // int nativeOptionName;
    // char *pNativeOptval = optval;
    int ret = 0;

    // switch (level)
    // {
    //     case SOCK_IPPROTO_IP:
    //         nativeLevel = IPPROTO_IP;
    //         nativeOptionName = GetNativeIPOption(optname);
    //         break;
    //     case SOCK_IPPROTO_TCP:
    //         nativeLevel = IPPROTO_TCP;
    //         nativeOptionName = GetNativeTcpOption(optname);
    //         break;
    //     case SOCK_IPPROTO_UDP:
    //     case SOCK_IPPROTO_ICMP:
    //     case SOCK_IPPROTO_IGMP:
    //     case SOCK_IPPROTO_IPV4:
    //     case SOCK_SOL_SOCKET:
    //         nativeLevel = SOL_SOCKET;
    //         nativeOptionName = GetNativeSockOption(optname);
    //         break;
    //     default:
    //         nativeLevel = level;
    //         nativeOptionName = optname;
    //         break;
    // }

    // ret = getsockopt(socket, nativeLevel, nativeOptionName, pNativeOptval, optlen);

    // if (ret == 0)
    // {
    //     switch (level)
    //     {
    //         case SOCK_SOL_SOCKET:
    //             switch (optname)
    //             {
    //                 case SOCK_SOCKO_EXCLUSIVEADDRESSUSE:
    //                 case SOCK_SOCKO_DONTLINGER:
    //                     *optval = !(*(int *)optval != 0);
    //                     break;

    //                 case SOCK_SOCKO_ACCEPTCONNECTION:
    //                 case SOCK_SOCKO_BROADCAST:
    //                 case SOCK_SOCKO_KEEPALIVE:
    //                     *optval = (*(int *)optval != 0);
    //                     break;
    //             }
    //             break;
    //     }
    // }

    return ret;
}

int ISM43362_SOCKETS_Driver::GetPeerName(SOCK_SOCKET socket, SOCK_sockaddr *name, int *namelen)
{
    (void)socket;
    (void)name;
    (void)namelen;

    NATIVE_PROFILE_PAL_NETWORK();
    int ret = 0;

    // TODO
    // sockaddr_in addr;

    // SOCK_SOCKADDR_TO_SOCKADDR(name, addr, namelen);

    // ret = getpeername(socket, (sockaddr *)&addr, namelen);

    // SOCKADDR_TO_SOCK_SOCKADDR(name, addr, namelen);

    return ret;
}

int ISM43362_SOCKETS_Driver::GetSockName(SOCK_SOCKET socket, SOCK_sockaddr *name, int *namelen)
{
    (void)socket;
    (void)name;
    (void)namelen;

    NATIVE_PROFILE_PAL_NETWORK();
    int ret = 0;

    // TODO
    // sockaddr_in addr;

    // SOCK_SOCKADDR_TO_SOCKADDR(name, addr, namelen);

    // ret = getsockname(socket, (sockaddr *)&addr, namelen);

    // SOCKADDR_TO_SOCK_SOCKADDR(name, addr, namelen);

    return ret;
}

int ISM43362_SOCKETS_Driver::RecvFrom(
    SOCK_SOCKET socket,
    char *buf,
    int len,
    int flags,
    SOCK_sockaddr *from,
    int *fromlen)
{
    (void)socket;
    (void)buf;
    (void)len;
    (void)flags;
    (void)from;
    (void)fromlen;

    NATIVE_PROFILE_PAL_NETWORK();

    // TODO
    // sockaddr_in addr;
    // sockaddr *pFrom = NULL;
    int ret = 0;

    // if (from)
    // {
    //     SOCK_SOCKADDR_TO_SOCKADDR(from, addr, fromlen);
    //     pFrom = (sockaddr *)&addr;
    // }

    // ret = recvfrom(socket, buf, len, flags, pFrom, fromlen);

    // if (from && ret != SOCK_SOCKET_ERROR)
    // {
    //     SOCKADDR_TO_SOCK_SOCKADDR(from, addr, fromlen);
    // }

    return ret;
}

int ISM43362_SOCKETS_Driver::SendTo(
    SOCK_SOCKET socket,
    const char *buf,
    int len,
    int flags,
    const SOCK_sockaddr *to,
    int tolen)
{
    (void)socket;
    (void)buf;
    (void)len;
    (void)flags;
    (void)to;
    (void)tolen;

    NATIVE_PROFILE_PAL_NETWORK();

    // TODO
    // sockaddr_in addr;

    // SOCK_SOCKADDR_TO_SOCKADDR(to, addr, &tolen);

    // return sendto(socket, (CHAR *)buf, len, flags, (sockaddr *)&addr, tolen);

    return 0;
}

HRESULT ISM43362_SOCKETS_Driver::LoadAdapterConfiguration(
    HAL_Configuration_NetworkInterface *config,
    uint32_t interfaceIndex)
{
    (void)interfaceIndex;

    uint8_t ipAddress[4];
    uint8_t ipMask[4];
    uint8_t gatewayAddress[4];
    uint8_t dns1addr[4];
    uint8_t dns2addr[4];

    NATIVE_PROFILE_PAL_NETWORK();

    if (config->StartupAddressMode == AddressMode_DHCP)
    {
        if (WIFI_GetIP_Address(ipAddress) != WIFI_STATUS_OK)
        {
            return S_FALSE;
        }
        else if (WIFI_GetIP_Mask(ipMask) != WIFI_STATUS_OK)
        {
            return S_FALSE;
        }
        else if (WIFI_GetGateway_Address(gatewayAddress) != WIFI_STATUS_OK)
        {
            return S_FALSE;
        }
        else if (WIFI_GetDNS_Address(dns1addr, dns2addr) != WIFI_STATUS_OK)
        {
            return S_FALSE;
        }

        config->IPv4Address = IP_ADDRESS(ipAddress[0], ipAddress[1], ipAddress[2], ipAddress[3]);
        config->IPv4NetMask = IP_ADDRESS(ipMask[0], ipMask[1], ipMask[2], ipMask[3]);
        config->IPv4GatewayAddress =
            IP_ADDRESS(gatewayAddress[0], gatewayAddress[1], gatewayAddress[2], gatewayAddress[3]);

        config->IPv4DNSAddress1 = IP_ADDRESS(dns1addr[0], dns1addr[1], dns1addr[2], dns1addr[3]);
        config->IPv4DNSAddress2 = IP_ADDRESS(dns2addr[0], dns2addr[1], dns2addr[2], dns2addr[3]);
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

HRESULT ISM43362_SOCKETS_Driver::UpdateAdapterConfiguration(
    uint32_t interfaceIndex,
    uint32_t updateFlags,
    HAL_Configuration_NetworkInterface *config)
{
    (void)interfaceIndex;
    (void)updateFlags;
    (void)config;

    NATIVE_PROFILE_PAL_NETWORK();

    bool enableDHCP = (config->StartupAddressMode == AddressMode_DHCP);

    // when using DHCP do not use the static settings
    if (0 != (updateFlags & NetworkInterface_UpdateOperation_Dns))
    {
        // FIXME IPV6
        if (config->AutomaticDNS == 0)
        {
            // NOT supported!
            return CLR_E_NOT_SUPPORTED;
        }
    }

    if (0 != (updateFlags & NetworkInterface_UpdateOperation_Dhcp))
    {
        if (enableDHCP)
        {
            // this is the default, nothing else to do
        }
        else
        {
            // stop DHCP
            // NOT supported!
            return CLR_E_NOT_SUPPORTED;
        }
    }

    if (enableDHCP)
    {
        if (0 != (updateFlags & NetworkInterface_UpdateOperation_DhcpRelease))
        {
            // nothing to do here
        }
        else if (0 != (updateFlags & NetworkInterface_UpdateOperation_DhcpRenew))
        {
            // nothing to do here
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
        // TODO
        // changing MAC address is not supported
        return CLR_E_NOT_SUPPORTED;
    }

    return S_OK;
}

int ISM43362_SOCKETS_Driver::GetNativeTcpOption(int optname)
{
    (void)optname;

    NATIVE_PROFILE_PAL_NETWORK();
    int nativeOptionName = 0;

    // TODO
    // switch (optname)
    // {
    //     case SOCK_TCP_NODELAY:
    //         nativeOptionName = TCP_NODELAY;
    //         break;

    //     case SOCK_SOCKO_KEEPALIVE:
    //         nativeOptionName = SO_KEEPALIVE;
    //         break;

    //     // allow the C# user to specify LWIP options that our managed enum
    //     // doesn't support
    //     default:
    //         nativeOptionName = optname;
    //         break;
    // }

    return nativeOptionName;
}

int ISM43362_SOCKETS_Driver::UpgradeToSsl(SOCK_SOCKET socket)
{
    NATIVE_PROFILE_PAL_NETWORK();

    WIFI_Status_t status;
    uint8_t ipAddress[4];

    if (_sockets[socket].protocol != 0xFF && !_sockets[socket].isSecure)
    {
        // disconnect socket
        if (_sockets[socket].isClient)
        {
            status = WIFI_CloseClientConnection((uint8_t)socket);
        }
        else
        {
            status = WIFI_CloseServerConnection((uint8_t)socket);
        }

        if (status != WIFI_STATUS_OK)
        {
            return 0;
        }

        // open socket again this time as secure
        ipAddress[0] = _sockets[socket].remoteIP >> 24;
        ipAddress[1] = _sockets[socket].remoteIP >> 16;
        ipAddress[2] = _sockets[socket].remoteIP >> 8;
        ipAddress[3] = _sockets[socket].remoteIP;

        _sockets[socket].isSecure = true;

        return WIFI_OpenClientConnection(
                   socket,
                   (WIFI_Protocol_t)_sockets[socket].protocol,
                   _sockets[socket].isSecure,
                   "",
                   ipAddress,
                   _sockets[socket].remotePort,
                   _sockets[socket].localPort) == WIFI_STATUS_OK;
    }
    else
    {
        return 0;
    }
}

int ISM43362_SOCKETS_Driver::GetNativeSockOption(int optname)
{
    (void)optname;

    NATIVE_PROFILE_PAL_NETWORK();
    int nativeOptionName = 0;

    // TODO
    // switch (optname)
    // {
    //     case SOCK_SOCKO_DONTLINGER:
    //     case SOCK_SOCKO_LINGER:
    //         nativeOptionName = SO_LINGER;
    //         break;
    //     case SOCK_SOCKO_SENDTIMEOUT:
    //         nativeOptionName = SO_SNDTIMEO;
    //         break;
    //     case SOCK_SOCKO_RECEIVETIMEOUT:
    //         nativeOptionName = SO_RCVTIMEO;
    //         break;
    //     case SOCK_SOCKO_EXCLUSIVEADDRESSUSE:
    //     case SOCK_SOCKO_REUSEADDRESS:
    //         nativeOptionName = SO_REUSEADDR;
    //         break;
    //     case SOCK_SOCKO_KEEPALIVE:
    //         nativeOptionName = SO_KEEPALIVE;
    //         break;
    //     case SOCK_SOCKO_ERROR:
    //         nativeOptionName = SO_ERROR;
    //         break;
    //     case SOCK_SOCKO_BROADCAST:
    //         nativeOptionName = SO_BROADCAST;
    //         break;
    //     case SOCK_SOCKO_RECEIVEBUFFER:
    //         nativeOptionName = SO_RCVBUF;
    //         break;
    //     case SOCK_SOCKO_SENDBUFFER:
    //         nativeOptionName = SO_SNDBUF;
    //         break;
    //     // not supported
    //     //case SOCK_SOCKO_ACCEPTCONNECTION:
    //     // case SOCK_SOCKO_USELOOPBACK:
    //     case SOCK_SOCKO_DONTROUTE:
    //         nativeOptionName = SO_DONTROUTE;
    //         break;
    //     case SOCK_SOCKO_OUTOFBANDINLINE:
    //         nativeOptionName = SO_OOBINLINE;
    //         break;

    //     case SOCK_SOCKO_DEBUG:
    //         nativeOptionName = SO_DEBUG;
    //         break;

    //     case SOCK_SOCKO_SENDLOWWATER:
    //         nativeOptionName = SO_SNDLOWAT;
    //         break;

    //     case SOCK_SOCKO_RECEIVELOWWATER:
    //         nativeOptionName = SO_RCVLOWAT;
    //         break;

    //         //        case SOCK_SOCKO_MAXCONNECTIONS:         //don't support
    //     case SOCK_SOCKO_UPDATE_ACCEPT_CTX:
    //     case SOCK_SOCKO_UPDATE_CONNECT_CTX:
    //         nativeOptionName = 0;
    //         break;

    //     // allow the C# user to specify LWIP options that our managed enum
    //     // doesn't support
    //     default:
    //         nativeOptionName = optname;
    //         break;
    // }

    return nativeOptionName;
}

int ISM43362_SOCKETS_Driver::GetNativeIPOption(int optname)
{
    (void)optname;

    NATIVE_PROFILE_PAL_NETWORK();
    int nativeOptionName = 0;

    // TODO
    // switch (optname)
    // {
    //     // not supported
    //     // case SOCK_IPO_TTL:
    //     // case SOCK_IPO_TOS:

    //     case SOCK_IPO_MULTICAST_IF:
    //         nativeOptionName = IP_MULTICAST_IF;
    //         break;
    //     case SOCK_IPO_MULTICAST_TTL:
    //         nativeOptionName = IP_MULTICAST_TTL;
    //         break;
    //     case SOCK_IPO_MULTICAST_LOOP:
    //         nativeOptionName = IP_MULTICAST_LOOP;
    //         break;
    //     case SOCK_IPO_ADD_MEMBERSHIP:
    //         nativeOptionName = IP_ADD_MEMBERSHIP;
    //         break;
    //     case SOCK_IPO_DROP_MEMBERSHIP:
    //         nativeOptionName = IP_DROP_MEMBERSHIP;
    //         break;

    //     case SOCK_IPO_ADD_SOURCE_MEMBERSHIP:
    //     case SOCK_IPO_DROP_SOURCE_MEMBERSHIP:
    //     case SOCK_IPO_OPTIONS:
    //     case SOCK_IPO_HDRINCL:
    //     case SOCK_IPO_IP_DONTFRAGMENT:
    //     case SOCK_IPO_BLOCK_SOURCE:
    //     case SOCK_IPO_UBLOCK_SOURCE:
    //     case SOCK_IPO_PACKET_INFO:
    //         nativeOptionName = 0;
    //         break;

    //     // allow the C# user to specify LWIP options that our managed enum
    //     // doesn't support
    //     default:
    //         nativeOptionName = optname;
    //         break;
    // }

    return nativeOptionName;
}

int ISM43362_SOCKETS_Driver::GetNativeError(int error)
{
    (void)error;

    NATIVE_PROFILE_PAL_NETWORK();
    int ret = 0;

    // until we have a better mapping, just return the native error
    ret = error;

    // TODO
    //     switch (error)
    //     {
    //         case EINTR:
    //             ret = SOCK_EINTR;
    //             break;

    //         case EACCES:
    //             ret = SOCK_EACCES;
    //             break;

    //         case EFAULT:
    //             ret = SOCK_EFAULT;
    //             break;

    //         case EINVAL:
    //             ret = SOCK_EINVAL;
    //             break;

    //         case EMFILE:
    //             ret = SOCK_EMFILE;
    //             break;

    //         case EAGAIN:
    //         case EBUSY:
    //         /* case EWOULDBLOCK: same as EINPROGRESS */
    //         case EINPROGRESS:
    //             ret = SOCK_EWOULDBLOCK;
    //             break;

    //         case EALREADY:
    //             ret = SOCK_EALREADY;
    //             break;

    //         case ENOTSOCK:
    //             ret = SOCK_ENOTSOCK;
    //             break;

    //         case EDESTADDRREQ:
    //             ret = SOCK_EDESTADDRREQ;
    //             break;

    //         case EMSGSIZE:
    //             ret = SOCK_EMSGSIZE;
    //             break;

    //         case EPROTOTYPE:
    //             ret = SOCK_EPROTOTYPE;
    //             break;

    //         case ENOPROTOOPT:
    //             ret = SOCK_ENOPROTOOPT;
    //             break;

    //         case EPROTONOSUPPORT:
    //             ret = SOCK_EPROTONOSUPPORT;
    //             break;
    //             // TODO nanoframework check why missing
    //             // case ESOCKTNOSUPPORT:
    //             //     ret = SOCK_ESOCKTNOSUPPORT;
    //             //     break;

    //         case EPFNOSUPPORT:
    //             ret = SOCK_EPFNOSUPPORT;
    //             break;

    //         case EAFNOSUPPORT:
    //             ret = SOCK_EAFNOSUPPORT;
    //             break;

    //         case EADDRINUSE:
    //             ret = SOCK_EADDRINUSE;
    //             break;

    //         case EADDRNOTAVAIL:
    //             ret = SOCK_EADDRNOTAVAIL;
    //             break;

    //         case ENETDOWN:
    //             ret = SOCK_ENETDOWN;
    //             break;

    //         case ENETUNREACH:
    //             ret = SOCK_ENETUNREACH;
    //             break;

    //         case ENETRESET:
    //             ret = SOCK_ENETRESET;
    //             break;

    //         case ECONNABORTED:
    //             ret = SOCK_ECONNABORTED;
    //             break;

    //         case ECONNRESET:
    //             ret = SOCK_ECONNRESET;
    //             break;

    //         case ENOBUFS:
    //         case ENOMEM:
    //             ret = SOCK_ENOBUFS;
    //             break;

    //         case EISCONN:
    //             ret = SOCK_EISCONN;
    //             break;

    //         case ENOTCONN:
    //             ret = SOCK_EISCONN;
    //             break;

    // #if !defined(__GNUC__) // same as ENOTSOCK for GCC
    //         case ESHUTDOWN:
    //             ret = SOCK_ESHUTDOWN;
    //             break;
    // #endif

    //         case ETIMEDOUT:
    //             ret = SOCK_ETIMEDOUT;
    //             break;

    //         case ECONNREFUSED:
    //             ret = SOCK_ECONNREFUSED;
    //             break;

    //         case EHOSTDOWN:
    //             ret = SOCK_EHOSTDOWN;
    //             break;

    //         case EHOSTUNREACH:
    //             ret = SOCK_EHOSTUNREACH;
    //             break;

    //         case ENODATA:
    //             ret = SOCK_NO_DATA;
    //             break;

    //         default:
    //             ret = error;
    //             break;
    //     }

    return (ret);
}
