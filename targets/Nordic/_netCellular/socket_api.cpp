//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

// Need this until 9160 supports getpeeraddr
#define PERRADDR_WORKAROUND 1

#include "sockets_nrf9160.h"

//--//
// TODO: Check stack size.
static K_THREAD_STACK_DEFINE(sock_thread_stack, 1024);
static struct k_thread sock_thread_data;

Sockets_nrf9160_Driver g_Sockets_nrf9160_Driver;

SOCK_SOCKET SOCK_socket(int family, int type, int protocol)
{
    NATIVE_PROFILE_PAL_COM();
    return Sockets_nrf9160_Driver::Socket(family, type, protocol, FALSE);
}
int SOCK_bind(SOCK_SOCKET socket, const struct SOCK_sockaddr *address, int addressLen)
{
    NATIVE_PROFILE_PAL_COM();
    return HAL_SOCK_bind(socket, address, addressLen);
}
int SOCK_connect(SOCK_SOCKET socket, const struct SOCK_sockaddr *address, int addressLen)
{
    NATIVE_PROFILE_PAL_COM();
    int rc;
    rc = Sockets_nrf9160_Driver::Connect(socket, address, addressLen);
    return rc;
}
int SOCK_send(SOCK_SOCKET socket, const char *buf, int len, int flags)
{
    NATIVE_PROFILE_PAL_COM();
    return Sockets_nrf9160_Driver::Send(socket, buf, len, flags);
}
int SOCK_recv(SOCK_SOCKET socket, char *buf, int len, int flags)
{
    NATIVE_PROFILE_PAL_COM();
    return Sockets_nrf9160_Driver::Recv(socket, buf, len, flags);
}
int SOCK_close(SOCK_SOCKET socket)
{
    NATIVE_PROFILE_PAL_COM();
    return Sockets_nrf9160_Driver::Close(socket);
}
int SOCK_listen(SOCK_SOCKET socket, int backlog)
{
    NATIVE_PROFILE_PAL_COM();
    return Sockets_nrf9160_Driver::Listen(socket, backlog);
}
SOCK_SOCKET SOCK_accept(SOCK_SOCKET socket, struct SOCK_sockaddr *address, int *addressLen)
{
    NATIVE_PROFILE_PAL_COM();
    return Sockets_nrf9160_Driver::Accept(socket, address, addressLen, FALSE);
}
int SOCK_shutdown(SOCK_SOCKET socket, int how)
{
    NATIVE_PROFILE_PAL_COM();
    return HAL_SOCK_shutdown(socket, how);
}
int SOCK_getaddrinfo(
    const char *nodename,
    char *servname,
    const struct SOCK_addrinfo *hints,
    struct SOCK_addrinfo **res)
{
    NATIVE_PROFILE_PAL_COM();
    return HAL_SOCK_getaddrinfo(nodename, servname, hints, res);
}
void SOCK_freeaddrinfo(struct SOCK_addrinfo *ai)
{
    NATIVE_PROFILE_PAL_COM();
    HAL_SOCK_freeaddrinfo(ai);
}
int SOCK_ioctl(SOCK_SOCKET socket, int cmd, int *data)
{
    NATIVE_PROFILE_PAL_COM();
    return HAL_SOCK_ioctl(socket, cmd, data);
}
int SOCK_getlasterror()
{
    NATIVE_PROFILE_PAL_COM();
    return HAL_SOCK_getlasterror();
}
int SOCK_getsocklasterror(SOCK_SOCKET socket)
{
    NATIVE_PROFILE_PAL_COM();
    return HAL_SOCK_getsocklasterror(socket);
}
int SOCK_select(
    int nfds,
    SOCK_fd_set *readfds,
    SOCK_fd_set *writefds,
    SOCK_fd_set *except,
    const struct SOCK_timeval *timeout)
{
    NATIVE_PROFILE_PAL_COM();
    return Sockets_nrf9160_Driver::Select(nfds, readfds, writefds, except, timeout);
}
int SOCK_setsockopt(SOCK_SOCKET socket, int level, int optname, const char *optval, int optlen)
{
    NATIVE_PROFILE_PAL_COM();
    return HAL_SOCK_setsockopt(socket, level, optname, optval, optlen);
}
int SOCK_getsockopt(SOCK_SOCKET socket, int level, int optname, char *optval, int *optlen)
{
    NATIVE_PROFILE_PAL_COM();
    return HAL_SOCK_getsockopt(socket, level, optname, optval, optlen);
}
int SOCK_getpeername(SOCK_SOCKET socket, struct SOCK_sockaddr *name, int *namelen)
{
    NATIVE_PROFILE_PAL_COM();
    int rc;
#ifdef PERRADDR_WORKAROUND
    //printk("getpeername workaround\n");
    rc = g_Sockets_nrf9160_Driver.GetSocketAddrData(socket, name, namelen);
    if (rc)
        printk("###  GetSocketAddrData ret %d\n", rc);
#else
    rc = HAL_SOCK_getpeername(socket, name, namelen);
#endif
    return rc;
}
int SOCK_getsockname(SOCK_SOCKET socket, struct SOCK_sockaddr *name, int *namelen)
{
    NATIVE_PROFILE_PAL_COM();
    return HAL_SOCK_getsockname(socket, name, namelen);
}
int SOCK_recvfrom(SOCK_SOCKET s, char *buf, int len, int flags, struct SOCK_sockaddr *from, int *fromlen)
{
    NATIVE_PROFILE_PAL_COM();
    return Sockets_nrf9160_Driver::RecvFrom(s, buf, len, flags, from, fromlen);
}
int SOCK_sendto(SOCK_SOCKET s, const char *buf, int len, int flags, const struct SOCK_sockaddr *to, int tolen)
{
    NATIVE_PROFILE_PAL_COM();
    return Sockets_nrf9160_Driver::SendTo(s, buf, len, flags, to, tolen);
}

bool Network_Initialize()
{
    NATIVE_PROFILE_PAL_COM();
    return Sockets_nrf9160_Driver::Initialize();
}

bool Network_Uninitialize()
{
    NATIVE_PROFILE_PAL_COM();

    return Sockets_nrf9160_Driver::Uninitialize();
}

void SOCKETS_CloseConnections()
{
    NATIVE_PROFILE_PAL_COM();
    Sockets_nrf9160_Driver::CloseConnections(FALSE);
}

HRESULT SOCK_CONFIGURATION_LoadAdapterConfiguration(HAL_Configuration_NetworkInterface *config, uint32_t interfaceIndex)
{
    NATIVE_PROFILE_PAL_COM();
    // printk ("SOCK_CONFIGURATION_LoadAdapterConfiguration++ %d\n", interfaceIndex);

    return HAL_SOCK_CONFIGURATION_LoadAdapterConfiguration(config, interfaceIndex);
}

HRESULT SOCK_CONFIGURATION_UpdateAdapterConfiguration(
    HAL_Configuration_NetworkInterface *config,
    uint32_t interfaceIndex,
    uint32_t updateFlags)
{
    NATIVE_PROFILE_PAL_COM();
    HRESULT hr = S_OK;
    bool success = FALSE;

    const uint32_t c_reInitFlag = NetworkInterface_UpdateOperation_Dhcp | NetworkInterface_UpdateOperation_DhcpRenew |
                                  NetworkInterface_UpdateOperation_Mac;

    const uint32_t c_uninitFlag = c_reInitFlag | NetworkInterface_UpdateOperation_DhcpRelease;

    if (0 != (updateFlags & c_uninitFlag))
    {
        success = SOCKETS_DbgUninitialize(COM_SOCKET_DBG);
    }

    hr = HAL_SOCK_CONFIGURATION_UpdateAdapterConfiguration(config, interfaceIndex, updateFlags);

    if (!SUCCEEDED(hr))
    {
        return S_FALSE;
    }

    if (0 != (updateFlags & c_reInitFlag))
    {
        if (success)
            SOCKETS_DbgInitialize(COM_SOCKET_DBG);
    }

    return hr;
}

HRESULT SOCK_CONFIGURATION_LoadConfiguration(HAL_Configuration_NetworkInterface *config, uint32_t interfaceIndex)
{
    NATIVE_PROFILE_PAL_COM();
    HRESULT hr = S_OK;

    // printk ("SOCK_CONFIGURATION_LoadConfiguration++\n");
    //  load current DCHP settings
    hr = SOCK_CONFIGURATION_LoadAdapterConfiguration(config, interfaceIndex);

    return hr;
}

HRESULT SOCK_CONFIGURATION_LinkStatus(uint32_t interfaceIndex, bool *status)
{
    NATIVE_PROFILE_PAL_COM();

    return HAL_SOCK_CONFIGURATION_Link_status(interfaceIndex, status);
}

HRESULT SOCK_IPAddressFromString(const char *ipString, uint64_t *address)
{
    NATIVE_PROFILE_PAL_COM();

    return HAL_SOCK_IPAddressFromString(ipString, address);
}

const char *SOCK_IPAddressToString(uint32_t address)
{
    NATIVE_PROFILE_PAL_COM();

    return HAL_SOCK_IPAddressToString(address);
}

#define SOCKET_SHUTDOWN_READ       0
#define SOCKET_SHUTDOWN_WRITE      1
#define SOCKET_SHUTDOWN_READ_WRITE 2

#define ISSET_SOCKET_FLAG(x, y) ((y) == ((y) & (x).m_flags))
#define SET_SOCKET_FLAG(x, y)   (x).m_flags |= (y)
#define CLEAR_SOCKET_FLAG(x, y) (x).m_flags &= ~(y)

//-----------------------------------------------------------------------------
//
//  CloseConnections - close all connections with the option of leaving the debugger sockets open.
//  debugger sockets are left open on CLR reboot so that communication with the debugger is not
//  lost.
//
//-----------------------------------------------------------------------------
void Sockets_nrf9160_Driver::CloseConnections(bool fCloseDbg)
{
    NATIVE_PROFILE_PAL_COM();

    int32_t cnt = g_Sockets_nrf9160_Driver.m_cntSockets;
    int32_t idx = 0;

    // round one - close all SSL sockets
    for (int32_t i = cnt - 1; i >= 0; i--)
    {
        struct SocketRegisterMap &entry = g_Sockets_nrf9160_Driver.m_socketHandles[i];

        if (entry.m_socket != SOCK_SOCKET_ERROR && entry.m_sslData != NULL)
        {
            SSL_CloseSocket(entry.m_socket);
        }
    }

    // round two - close all non-SSL sockets
    cnt = g_Sockets_nrf9160_Driver.m_cntSockets;

    for (int32_t i = 0; i < cnt; i++)
    {
        struct SocketRegisterMap &entry = g_Sockets_nrf9160_Driver.m_socketHandles[i];

        if (entry.m_socket != SOCK_SOCKET_ERROR)
        {
            if (fCloseDbg || !ISSET_SOCKET_FLAG(entry, SocketRegisterMap::c_DebugSocket))
            {
                // use the HAL method so we don't unregister the socket since we handle that here
                HAL_SOCK_close(entry.m_socket);

                g_Sockets_nrf9160_Driver.m_socketHandles[i].m_socket = SOCK_SOCKET_ERROR;
                g_Sockets_nrf9160_Driver.m_socketHandles[i].m_flags = 0;
                g_Sockets_nrf9160_Driver.m_socketHandles[i].m_sslData = NULL;
                g_Sockets_nrf9160_Driver.m_cntSockets--;
            }
            else if (i > 0)
            {
                memcpy(
                    &g_Sockets_nrf9160_Driver.m_socketHandles[i],
                    &g_Sockets_nrf9160_Driver.m_socketHandles[idx++],
                    sizeof(g_Sockets_nrf9160_Driver.m_socketHandles[i]));
            }
        }
    }
}

//--//

SOCK_SOCKET Sockets_nrf9160_Driver::Socket(int32_t family, int32_t type, int32_t protocol, bool fDebug)
{
    NATIVE_PROFILE_PAL_COM();

    int ret = HAL_SOCK_socket(family, type, protocol);

    if (ret != SOCK_SOCKET_ERROR)
    {
        RegisterSocket(ret, (protocol != SOCK_IPPROTO_TCP), fDebug);
    }
    return ret;
}
int Sockets_nrf9160_Driver::Connect(SOCK_SOCKET socket, const struct SOCK_sockaddr *address, int32_t addressLen)
{
    NATIVE_PROFILE_PAL_COM();
    int rc;
    rc = HAL_SOCK_connect(socket, address, addressLen);
#ifdef PERRADDR_WORKAROUND
    if (rc == 0)
    {
        printk("calling SetSocketAddrData %d %p %d\n", socket, address, addressLen);
        g_Sockets_nrf9160_Driver.SetSocketAddrData(socket, address, addressLen);
    }
#endif
    return rc;
}
int Sockets_nrf9160_Driver::Send(SOCK_SOCKET socket, const char *buf, int32_t len, int32_t flags)
{
    NATIVE_PROFILE_PAL_COM();

    return HAL_SOCK_send(socket, buf, len, flags);
}
int Sockets_nrf9160_Driver::Recv(SOCK_SOCKET socket, char *buf, int32_t len, int32_t flags)
{
    NATIVE_PROFILE_PAL_COM();

    return HAL_SOCK_recv(socket, buf, len, flags);
}

int Sockets_nrf9160_Driver::Shutdown(SOCK_SOCKET sock, int32_t how)
{
    NATIVE_PROFILE_PAL_COM();

    return HAL_SOCK_shutdown(sock, how);
}

//-----------------------------------------------------------------------------
//
// Close - The close method marks a socket to be closed by the select thread.  Close is handled
// in this way because (at least for one implementation) the select method can not be set for a
// closed socket.  Therfore in the select thread the socket is closed.
//
//-----------------------------------------------------------------------------
int Sockets_nrf9160_Driver::Close(SOCK_SOCKET sock)
{
    NATIVE_PROFILE_PAL_COM();
    UnregisterSocket(sock);
    return HAL_SOCK_close(sock);
}
int Sockets_nrf9160_Driver::Listen(SOCK_SOCKET socket, int32_t backlog)
{
    NATIVE_PROFILE_PAL_COM();

    return HAL_SOCK_listen(socket, backlog);
}
SOCK_SOCKET Sockets_nrf9160_Driver::Accept(
    SOCK_SOCKET socket,
    struct SOCK_sockaddr *address,
    int *addressLen,
    bool fDebug)
{
    NATIVE_PROFILE_PAL_COM();

    int ret = HAL_SOCK_accept(socket, address, addressLen);

    if (ret != SOCK_SOCKET_ERROR)
    {
        RegisterSocket(ret, TRUE, fDebug);
#ifdef PERRADDR_WORKAROUND
        g_Sockets_nrf9160_Driver.SetSocketAddrData(socket, address, *addressLen);
#endif
    }

    return ret;
}

int Sockets_nrf9160_Driver::Select(
    int32_t nfds,
    SOCK_fd_set *readfds,
    SOCK_fd_set *writefds,
    SOCK_fd_set *exceptfds,
    const SOCK_timeval *timeout)
{
    NATIVE_PROFILE_PAL_COM();
    int rc;
    uint32_t i;
    uint32_t *sock_flgs;

    // Clean the select flags
    for (i = 0; i < (uint32_t)g_Sockets_nrf9160_Driver.m_cntSockets; i++)
    {
        g_Sockets_nrf9160_Driver.m_socketHandles[i].m_flags &= ~SocketRegisterMap::c_SelSocketMask;
    }
    if (readfds)
    {
        for (i = 0; i < readfds->fd_count; i++)
        {
            sock_flgs = g_Sockets_nrf9160_Driver.GetSockFlags(readfds->fd_array[i]);
            if (sock_flgs)
                *sock_flgs |= SocketRegisterMap::c_SelRdSocket;
        }
    }
    if (writefds)
    {
        for (i = 0; i < writefds->fd_count; i++)
        {
            sock_flgs = g_Sockets_nrf9160_Driver.GetSockFlags(writefds->fd_array[i]);
            if (sock_flgs)
                *sock_flgs |= SocketRegisterMap::c_SelWrSocket;
        }
    }
    if (exceptfds)
    {
        for (i = 0; i < exceptfds->fd_count; i++)
        {
            sock_flgs = g_Sockets_nrf9160_Driver.GetSockFlags(exceptfds->fd_array[i]);
            if (sock_flgs)
                *sock_flgs |= SocketRegisterMap::c_SelErSocket;
        }
    }
    k_sem_give(&g_Sockets_nrf9160_Driver.sem_sockmon);

    rc = HAL_SOCK_select(nfds, readfds, writefds, exceptfds, timeout);
    return rc;
}

int Sockets_nrf9160_Driver::RecvFrom(
    SOCK_SOCKET s,
    char *buf,
    int32_t len,
    int32_t flags,
    struct SOCK_sockaddr *from,
    int *fromlen)
{
    NATIVE_PROFILE_PAL_COM();

    return HAL_SOCK_recvfrom(s, buf, len, flags, from, fromlen);
}

int Sockets_nrf9160_Driver::SendTo(
    SOCK_SOCKET s,
    const char *buf,
    int32_t len,
    int32_t flags,
    const struct SOCK_sockaddr *to,
    int32_t tolen)
{
    NATIVE_PROFILE_PAL_COM();

    return HAL_SOCK_sendto(s, buf, len, flags, to, tolen);
}

bool Sockets_nrf9160_Driver::Initialize()
{
    NATIVE_PROFILE_PAL_COM();
    SOCKET_CHECK_ENTER();

printk ("Sockets_nrf9160_Driver::Initialize++ %d\n", s_initialized);
    if (!s_initialized)
    {

printk ("Sockets_nrf9160_Driver::Initialize  1\n");

        g_Sockets_nrf9160_Driver.m_fShuttingDown = FALSE;
        g_Sockets_nrf9160_Driver.m_cntSockets = 0;

        for (int32_t i = 0; i < SOCKETS_MAX_COUNT; i++)
        {
            g_Sockets_nrf9160_Driver.m_socketHandles[i].m_socket = SOCK_SOCKET_ERROR;
            g_Sockets_nrf9160_Driver.m_socketHandles[i].m_flags = 0;
            g_Sockets_nrf9160_Driver.m_socketHandles[i].m_sslData = NULL;
        }
        k_sem_init(&g_Sockets_nrf9160_Driver.sem_sockmon, 0, 1);
printk ("Sockets_nrf9160_Driver::Initialize  2\n");
        k_thread_create(
            &sock_thread_data,
            sock_thread_stack,
            K_THREAD_STACK_SIZEOF(sock_thread_stack),
            g_Sockets_nrf9160_Driver.SockMonitorThread,
            NULL,
            NULL,
            NULL,
            K_PRIO_COOP(5),
            0,
            K_NO_WAIT);

        SOCKET_CHECK_bool(HAL_SOCK_Initialize());

        SSL_Initialize();

        s_initialized = TRUE;
    }

    SOCKET_CHECK_EXIT_bool();
}

// Dummy methods in case Debbuger is not installed
__nfweak void Sockets_nrf9160_Driver::Debugger_Abort()
{
}
__nfweak void Sockets_nrf9160_Driver::Debugger_Uninitialize()
{
}

bool Sockets_nrf9160_Driver::Uninitialize()
{
    NATIVE_PROFILE_PAL_COM();
    bool ret = TRUE;

    if (s_initialized)
    {
        Debugger_Abort();

        // close all connections (including debugger sockets)
        CloseConnections(TRUE);

        SSL_Uninitialize();

        Debugger_Uninitialize();

        ret = HAL_SOCK_Uninitialize();

        s_initialized = FALSE;
        s_wirelessInitialized = FALSE;
        s_discoveryInitialized = FALSE;
    }

    return ret;
}

//-----------------------------------------------------------------------------
//
//  RegisterSocket - socket tracking.  This method is used to track sockets
//    opened by this driver.  It does not include the fake socket.  Register
//    should be called after a socket is opened (via socket or accept methods).
//    The selectable parameter should only be true if the socket is in the
//    listening or connected state
//
//-----------------------------------------------------------------------------
void Sockets_nrf9160_Driver::RegisterSocket(SOCK_SOCKET sock, bool selectable, bool fDebug)
{
    (void)selectable;

    NATIVE_PROFILE_PAL_COM();
    if (sock == SOCK_SOCKET_ERROR)
    {
        ASSERT(FALSE);
        return;
    }

    if (g_Sockets_nrf9160_Driver.m_cntSockets >= SOCKETS_MAX_COUNT)
        return;

    GLOBAL_LOCK_SOCKETS(lock);

    g_Sockets_nrf9160_Driver.m_socketHandles[g_Sockets_nrf9160_Driver.m_cntSockets].m_socket = sock;
    g_Sockets_nrf9160_Driver.m_socketHandles[g_Sockets_nrf9160_Driver.m_cntSockets].m_flags = 0;
    g_Sockets_nrf9160_Driver.m_socketHandles[g_Sockets_nrf9160_Driver.m_cntSockets].m_sslData = NULL;

    if (fDebug)
        SET_SOCKET_FLAG(
            g_Sockets_nrf9160_Driver.m_socketHandles[g_Sockets_nrf9160_Driver.m_cntSockets],
            SocketRegisterMap::c_DebugSocket);

    g_Sockets_nrf9160_Driver.m_cntSockets++;

    // Signal the monitor thread about the new socket.
    k_sem_give(&g_Sockets_nrf9160_Driver.sem_sockmon);
}

//-----------------------------------------------------------------------------
//
//  UnregisterSocket - No longer track a given socket for clean up and selection.
//    This method should only be called immediately before closing down a socket.
//
//-----------------------------------------------------------------------------
void Sockets_nrf9160_Driver::UnregisterSocket(SOCK_SOCKET sock)
{
    int32_t index = -1;

    NATIVE_PROFILE_PAL_COM();
    GLOBAL_LOCK_SOCKETS(lock);

    g_Sockets_nrf9160_Driver.GetSocketSslData(sock, index);

    if (index == -1)
        return;

    g_Sockets_nrf9160_Driver.m_cntSockets--;

    if (index != g_Sockets_nrf9160_Driver.m_cntSockets)
    {
        memcpy(
            &g_Sockets_nrf9160_Driver.m_socketHandles[index],
            &g_Sockets_nrf9160_Driver.m_socketHandles[g_Sockets_nrf9160_Driver.m_cntSockets],
            sizeof(g_Sockets_nrf9160_Driver.m_socketHandles[index]));
    }

    g_Sockets_nrf9160_Driver.m_socketHandles[g_Sockets_nrf9160_Driver.m_cntSockets].m_socket = SOCK_SOCKET_ERROR;
    g_Sockets_nrf9160_Driver.m_socketHandles[g_Sockets_nrf9160_Driver.m_cntSockets].m_flags = 0;
}

//-----------------------------------------------------------------------------
//
//  SockMonitorThread - Thread to monitor open sockets with non-blocking
//    select/poll calls.
//
//-----------------------------------------------------------------------------
void Sockets_nrf9160_Driver::SockMonitorThread(void *arg1, void *arg2, void *arg3)
{
    int rc, i, sock_cnt = 0;
    struct pollfd fd[SOCKETS_MAX_COUNT];
    int r, w, e, c, b;

    printk ("**** SockMonitorThread++\n");

    NATIVE_PROFILE_PAL_COM();
    while (true)
    {
        // Set poll for each socket. For now, watch everything.
        r = 0, w = 0, e = 0;
        sock_cnt = 0;
        for (i = 0; i < g_Sockets_nrf9160_Driver.m_cntSockets; i++)
        {
            fd[sock_cnt].fd = g_Sockets_nrf9160_Driver.m_socketHandles[i].m_socket;
            fd[sock_cnt].events = 0;
            if (g_Sockets_nrf9160_Driver.m_socketHandles[i].m_flags & SocketRegisterMap::c_SelRdSocket)
            {
                fd[sock_cnt].events |= POLLIN;
                g_Sockets_nrf9160_Driver.m_socketHandles[i].m_flags &= ~SocketRegisterMap::c_SelRdSocket;
                r++;
            }
            if (g_Sockets_nrf9160_Driver.m_socketHandles[i].m_flags & SocketRegisterMap::c_SelWrSocket)
            {
                fd[sock_cnt].events |= POLLOUT;
                g_Sockets_nrf9160_Driver.m_socketHandles[i].m_flags &= ~SocketRegisterMap::c_SelWrSocket;
                w++;
            }

            if (g_Sockets_nrf9160_Driver.m_socketHandles[i].m_flags & SocketRegisterMap::c_SelErSocket)
            {
                fd[sock_cnt].events |= POLLERR;
                g_Sockets_nrf9160_Driver.m_socketHandles[i].m_flags &= ~SocketRegisterMap::c_SelErSocket;
                e++;
            }

            // fd[sock_cnt].events = POLLIN | POLLOUT | POLLERR;
            fd[sock_cnt].revents = 0;

            if (fd[sock_cnt].events != 0)
            {
                sock_cnt++;
            }
        }
        // If no socks have flags, just wait.
        if (sock_cnt == 0)
        {
            printk ("**** SockMonitorThread  zero socket wait\n");
            k_sem_reset(&g_Sockets_nrf9160_Driver.sem_sockmon);

            // Wait for a new socket
            k_sem_take(&g_Sockets_nrf9160_Driver.sem_sockmon, K_FOREVER);
            printk ("**** SockMonitorThread  triggered\n");
            continue;
        }

        // Now monitor the open sockets.
        printk ("**** SockMonitorThread  poll cnt:%d  %d %d %d\n", g_Sockets_nrf9160_Driver.m_cntSockets, r, w, e);
        rc = poll(fd, g_Sockets_nrf9160_Driver.m_cntSockets, 1000);
        printk ("**** SockMonitorThread  poll return %d %x   ", rc, fd[0].revents);
        if (rc < 0)
        {
            printk("poll error %d  %d\n", rc, errno);
            k_sleep(K_MSEC(500));
            continue;
        }
        // If timeout, try again.
        if (rc == 0)
        {
            printk("\n");
            continue;
        }
        // Set poll for each socket. For now, watch everything.
        r = w = e = c = b = 0;
        for (i = 0; i < g_Sockets_nrf9160_Driver.m_cntSockets; i++)
        {
            if (fd[i].revents & POLLIN)
                r++;
            if (fd[i].revents & POLLOUT)
                w++;
            if (fd[i].revents & POLLERR)
                e++;
            if (fd[i].revents & POLLHUP)
                c++;
            if (fd[i].revents & POLLNVAL)
                b++;
        }
        printk("cnt %d r %d w %d err %d close %d Inv %d\n", 
               g_Sockets_nrf9160_Driver.m_cntSockets, r, w, e, c, b);
        // signal the nf stack.
        Events_Set(SYSTEM_EVENT_FLAG_SOCKET);

        // printk ("Sockets_nrf9160_Driver::SockMonitorThread Ping stack!\n");
        // k_sleep(K_MSEC(100));

        // k_sleep(K_MSEC(1000));
    }
    printk("Sockets_nrf9160_Driver::SockMonitorThread EXIT\n");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// weak implementations of the functions (to be replaced with _strong_ implementations if and when required) //
// C++ version
// __nfweak void debug_printf( const char* format, ... )
// {
//     return;
// }

bool Sockets_nrf9160_Driver::s_initialized = FALSE;
bool Sockets_nrf9160_Driver::s_wirelessInitialized = FALSE;
bool Sockets_nrf9160_Driver::s_discoveryInitialized = FALSE;
