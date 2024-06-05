//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "lwIP_Sockets.h"

bool HAL_SOCK_Initialize()
{
    NATIVE_PROFILE_PAL_NETWORK();
    return LWIP_SOCKETS_Driver::Initialize();
}

bool HAL_SOCK_Uninitialize()
{
    NATIVE_PROFILE_PAL_NETWORK();
    return LWIP_SOCKETS_Driver::Uninitialize();
}

SOCK_SOCKET HAL_SOCK_socket(int family, int type, int protocol)
{
    NATIVE_PROFILE_PAL_NETWORK();
    return LWIP_SOCKETS_Driver::Socket(family, type, protocol);
}
int HAL_SOCK_bind(SOCK_SOCKET socket, const struct SOCK_sockaddr *address, int addressLen)
{
    NATIVE_PROFILE_PAL_NETWORK();
    return LWIP_SOCKETS_Driver::Bind(socket, address, addressLen);
}
int HAL_SOCK_connect(SOCK_SOCKET socket, const struct SOCK_sockaddr *address, int addressLen)
{
    NATIVE_PROFILE_PAL_NETWORK();
    return LWIP_SOCKETS_Driver::Connect(socket, address, addressLen);
}
int HAL_SOCK_send(SOCK_SOCKET socket, const char *buf, int len, int flags)
{
    NATIVE_PROFILE_PAL_NETWORK();
    return LWIP_SOCKETS_Driver::Send(socket, buf, len, flags);
}
int HAL_SOCK_recv(SOCK_SOCKET socket, char *buf, int len, int flags)
{
    NATIVE_PROFILE_PAL_NETWORK();
    return LWIP_SOCKETS_Driver::Recv(socket, buf, len, flags);
}
int HAL_SOCK_close(SOCK_SOCKET socket)
{
    NATIVE_PROFILE_PAL_NETWORK();
    return LWIP_SOCKETS_Driver::Close(socket);
}
int HAL_SOCK_listen(SOCK_SOCKET socket, int backlog)
{
    NATIVE_PROFILE_PAL_NETWORK();
    return LWIP_SOCKETS_Driver::Listen(socket, backlog);
}
SOCK_SOCKET HAL_SOCK_accept(SOCK_SOCKET socket, struct SOCK_sockaddr *address, int *addressLen)
{
    NATIVE_PROFILE_PAL_NETWORK();
    return LWIP_SOCKETS_Driver::Accept(socket, address, addressLen);
}
int HAL_SOCK_shutdown(SOCK_SOCKET socket, int how)
{
    NATIVE_PROFILE_PAL_NETWORK();
    return LWIP_SOCKETS_Driver::Shutdown(socket, how);
}
int HAL_SOCK_getaddrinfo(
    const char *nodename,
    char *servname,
    const struct SOCK_addrinfo *hints,
    struct SOCK_addrinfo **res)
{
    NATIVE_PROFILE_PAL_NETWORK();
    return LWIP_SOCKETS_Driver::GetAddrInfo(nodename, servname, hints, res);
}
void HAL_SOCK_freeaddrinfo(struct SOCK_addrinfo *ai)
{
    NATIVE_PROFILE_PAL_NETWORK();
    LWIP_SOCKETS_Driver::FreeAddrInfo(ai);
}
int HAL_SOCK_ioctl(SOCK_SOCKET socket, int cmd, int *data)
{
    NATIVE_PROFILE_PAL_NETWORK();
    return LWIP_SOCKETS_Driver::Ioctl(socket, cmd, data);
}
int HAL_SOCK_getlasterror()
{
    NATIVE_PROFILE_PAL_NETWORK();
    return LWIP_SOCKETS_Driver::GetLastError();
}
int HAL_SOCK_getsocklasterror(SOCK_SOCKET socket)
{
    NATIVE_PROFILE_PAL_NETWORK();
    return LWIP_SOCKETS_Driver::GetSockLastError(socket);
}
int HAL_SOCK_select(
    int nfds,
    SOCK_fd_set *readfds,
    SOCK_fd_set *writefds,
    SOCK_fd_set *except,
    const struct SOCK_timeval *timeout)
{
    NATIVE_PROFILE_PAL_NETWORK();
    return LWIP_SOCKETS_Driver::Select(nfds, readfds, writefds, except, timeout);
}
int HAL_SOCK_setsockopt(SOCK_SOCKET socket, int level, int optname, const char *optval, int optlen)
{
    NATIVE_PROFILE_PAL_NETWORK();
    return LWIP_SOCKETS_Driver::SetSockOpt(socket, level, optname, optval, optlen);
}
int HAL_SOCK_getsockopt(SOCK_SOCKET socket, int level, int optname, char *optval, int *optlen)
{
    NATIVE_PROFILE_PAL_NETWORK();
    return LWIP_SOCKETS_Driver::GetSockOpt(socket, level, optname, optval, optlen);
}
int HAL_SOCK_getpeername(SOCK_SOCKET socket, struct SOCK_sockaddr *name, int *namelen)
{
    NATIVE_PROFILE_PAL_NETWORK();
    return LWIP_SOCKETS_Driver::GetPeerName(socket, name, namelen);
}
int HAL_SOCK_getsockname(SOCK_SOCKET socket, struct SOCK_sockaddr *name, int *namelen)
{
    NATIVE_PROFILE_PAL_NETWORK();
    return LWIP_SOCKETS_Driver::GetSockName(socket, name, namelen);
}
int HAL_SOCK_recvfrom(SOCK_SOCKET s, char *buf, int len, int flags, struct SOCK_sockaddr *from, int *fromlen)
{
    NATIVE_PROFILE_PAL_NETWORK();
    return LWIP_SOCKETS_Driver::RecvFrom(s, buf, len, flags, from, fromlen);
}
int HAL_SOCK_sendto(SOCK_SOCKET s, const char *buf, int len, int flags, const struct SOCK_sockaddr *to, int tolen)
{
    NATIVE_PROFILE_PAL_NETWORK();
    return LWIP_SOCKETS_Driver::SendTo(s, buf, len, flags, to, tolen);
}

HRESULT HAL_SOCK_CONFIGURATION_LoadAdapterConfiguration(
    HAL_Configuration_NetworkInterface *config,
    uint32_t interfaceIndex)
{
    NATIVE_PROFILE_PAL_NETWORK();
    return LWIP_SOCKETS_Driver::LoadAdapterConfiguration(config, interfaceIndex);
}

HRESULT HAL_SOCK_CONFIGURATION_UpdateAdapterConfiguration(
    HAL_Configuration_NetworkInterface *config,
    uint32_t interfaceIndex,
    uint32_t updateFlags)
{
    NATIVE_PROFILE_PAL_NETWORK();
    return LWIP_SOCKETS_Driver::UpdateAdapterConfiguration(interfaceIndex, updateFlags, config);
}

HRESULT HAL_SOCK_CONFIGURATION_Link_status(uint32_t interfaceIndex, bool *status)
{
    NATIVE_PROFILE_PAL_NETWORK();
    return LWIP_SOCKETS_Driver::Link_status(interfaceIndex, status);
}

HRESULT HAL_SOCK_IPV4AddressFromString(const char *ipString, uint64_t *address)
{
    NATIVE_PROFILE_PAL_NETWORK();
    return LWIP_SOCKETS_Driver::IPV4AddressFromString(ipString, address);
}

const char *HAL_SOCK_IPV4AddressToString(uint32_t address)
{
    NATIVE_PROFILE_PAL_NETWORK();
    return LWIP_SOCKETS_Driver::IP4AddressToString(address);
}

#if LWIP_IPV6
HRESULT HAL_SOCK_IPV6AddressFromString(const char *ipString, uint16_t *address)
{
    NATIVE_PROFILE_PAL_NETWORK();
    return LWIP_SOCKETS_Driver::IPV6AddressFromString(ipString, address);
}

const char *HAL_SOCK_IPV6AddressToString(uint16_t *addr)
{
    NATIVE_PROFILE_PAL_NETWORK();
    return LWIP_SOCKETS_Driver::IP6AddressToString(addr);
}
#endif

void HAL_SOCK_EventsSet(uint32_t events)
{
#ifdef BUILD_RTM
    (void)events;
#endif

    NATIVE_PROFILE_PAL_NETWORK();
    ASSERT((events == SOCKET_EVENT_FLAG_SOCKET) || (events == SOCKET_EVENT_FLAG_SOCKETS_READY));

    Events_Set(SYSTEM_EVENT_FLAG_SOCKET);
}

void *HAL_SOCK_GlobalLockContext()
{
    NATIVE_PROFILE_PAL_NETWORK();
    return NULL;
}

void HAL_SOCK_SetInterfaceNumber(uint32_t interfaceIndex, uint32_t interfaceNumber)
{
    LWIP_SOCKETS_Driver::SetSocketDriverInterface(interfaceIndex, interfaceNumber);
}
