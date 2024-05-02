//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef ISM43362_SOCKETS_H
#define ISM43362_SOCKETS_H

#include <nanoHAL.h>
#include <nanoHAL_ConfigurationManager.h>
#include <nanoHAL_StorageOperation.h>
#include "wifi.h"

#define NATIVE_PROFILE_PAL_NETWORK()

//////////////////////////////////////////////////////////////////////////////
// ISM43362 Socket structure (heavily inspired by the NX Duo socket structure)

typedef struct ISM43362_SOCKET
{
    uint16_t protocol;
    uint32_t localIP;
    uint32_t remoteIP;
    uint16_t localPort;
    uint16_t remotePort;
    uint8_t tcpConnected;
    uint8_t isClient;
    uint8_t isSecure;
} ISM43362_Socket;

//////////////////////////////////////////////////////////////////////////////
// SOCKET driver
//

struct ISM43362_DRIVER_INTERFACE_DATA
{
    int m_interfaceNumber;
};

#define SOCK_SOCKADDR_TO_IP_DATA(ssa)                                                                                  \
    ipPort = __builtin_bswap16(((SOCK_sockaddr_in *)ssa)->sin_port);                                                   \
    ipAddress[0] = ((SOCK_sockaddr_in *)ssa)->sin_addr.S_un.S_un_b.s_b4;                                               \
    ipAddress[1] = ((SOCK_sockaddr_in *)ssa)->sin_addr.S_un.S_un_b.s_b3;                                               \
    ipAddress[2] = ((SOCK_sockaddr_in *)ssa)->sin_addr.S_un.S_un_b.s_b2;                                               \
    ipAddress[3] = ((SOCK_sockaddr_in *)ssa)->sin_addr.S_un.S_un_b.s_b1;

/*
#define SOCKADDR_TO_SOCK_SOCKADDR(ssa, sa, addrLen)                                                                    \
    ((SOCK_sockaddr_in *)ssa)->sin_port = sa.sin_port;                                                                 \
    ((SOCK_sockaddr_in *)ssa)->sin_addr.S_un.S_addr = sa.sin_addr.s_addr;                                              \
    ((SOCK_sockaddr_in *)ssa)->sin_family = sa.sin_family;                                                             \
    memcpy(((SOCK_sockaddr_in *)ssa)->sin_zero, sa.sin_zero, sizeof(((SOCK_sockaddr_in *)ssa)->sin_zero));             \
    *addrLen = sizeof(SOCK_sockaddr_in)
*/
struct ISM43362_SOCKETS_Driver
{
    static bool Initialize();

    static bool Uninitialize();

    static SOCK_SOCKET Socket(int family, int type, int protocol);

    static int Bind(SOCK_SOCKET socket, const SOCK_sockaddr *address, int addressLen);

    static int Connect(SOCK_SOCKET socket, const SOCK_sockaddr *address, int addressLen);

    static int Send(SOCK_SOCKET socket, const char *buf, int len, int flags);

    static int Recv(SOCK_SOCKET socket, char *buf, int len, int flags);

    static int Close(SOCK_SOCKET socket);

    static int Listen(SOCK_SOCKET socket, int backlog);

    static SOCK_SOCKET Accept(SOCK_SOCKET socket, SOCK_sockaddr *address, int *addressLen);

    static int Shutdown(SOCK_SOCKET socket, int how);

    static int GetAddrInfo(const char *nodename, char *servname, const SOCK_addrinfo *hints, SOCK_addrinfo **res);

    static void FreeAddrInfo(SOCK_addrinfo *ai);

    static int Ioctl(SOCK_SOCKET socket, int cmd, int *data);

    static int GetLastError();

    static int GetSockLastError(SOCK_SOCKET socket);

    static int GetNativeTcpOption(int optname);

    static int GetNativeSockOption(int optname);

    static int GetNativeError(int error);

    static int GetNativeIPOption(int optname);

    static int Select(
        int nfds,
        SOCK_fd_set *readfds,
        SOCK_fd_set *writefds,
        SOCK_fd_set *exceptfds,
        const SOCK_timeval *timeout);

    static int SetSockOpt(SOCK_SOCKET socket, int level, int optname, const char *optval, int optlen);

    static int GetSockOpt(SOCK_SOCKET socket, int level, int optname, char *optval, int *optlen);

    static int GetPeerName(SOCK_SOCKET socket, SOCK_sockaddr *name, int *namelen);

    static int GetSockName(SOCK_SOCKET socket, SOCK_sockaddr *name, int *namelen);

    static int RecvFrom(SOCK_SOCKET s, char *buf, int len, int flags, SOCK_sockaddr *from, int *fromlen);

    static int SendTo(SOCK_SOCKET s, const char *buf, int len, int flags, const SOCK_sockaddr *to, int tolen);

    static HRESULT Link_status(uint32_t interfaceIndex, bool *status);

    static HRESULT IPAddressFromString(const char *ipString, uint64_t *address);

    static const char *IPAddressToString(uint32_t address);

    static HRESULT LoadAdapterConfiguration(HAL_Configuration_NetworkInterface *config, uint32_t interfaceIndex);

    static HRESULT UpdateAdapterConfiguration(
        uint32_t interfaceIndex,
        uint32_t updateFlags,
        HAL_Configuration_NetworkInterface *config);

    static int UpgradeToSsl(SOCK_SOCKET s);

  private:
    // TODO
    // static void Status_callback(NX_IP *ipInstance);
    // TODO
    // static void Link_callback(NX_IP *ipInstance);

    static void PostAddressChanged(void *arg);

    static void PostAvailabilityOn(void *arg);

    static void PostAvailabilityOff(void *arg);

    ISM43362_DRIVER_INTERFACE_DATA *m_interfaces;
};

//
// SOCKET driver
/////////////////////////////////////////////////////////////////////////////////////////////////

#endif // ISM43362_SOCKETS_H
