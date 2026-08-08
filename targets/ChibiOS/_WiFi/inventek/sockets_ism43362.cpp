//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Socket-proxy layer for the Inventek ISM43362 (ES-WIFI) module.
//
// The ES-WIFI module runs its own onboard TCP/IP stack and only exposes a socket-oriented AT
// command set (see targets/ChibiOS/_WiFi/inventek/wifi.h) - it cannot send/receive raw
// link-layer frames, so there is no lwIP netif backing this interface (see Target_Network.cpp
// and targetHAL_Network.cpp for the interface-level bookkeeping). Instead, the SOCK_* functions
// that nanoFramework's System.Net.Sockets native layer calls are implemented here by mapping
// directly onto the module's own hardware socket slots (WIFI_OpenClientConnection/SendData/
// ReceiveData/CloseClientConnection).
//
// NOTE: this is a first, minimal implementation covering TCP/UDP CLIENT sockets only (connect,
// send, recv, close) - matching the most common IoT use case (connecting out to a server/API).
// Server-side sockets (listen/accept) and SOCK_select() are not implemented yet and return
// SOCK_EOPNOTSUPP; callers relying on them will need this extended in a follow-up.

#include <nanoHAL.h>
#include <nanoHAL_Network.h>
#include <nanoCLR_Types.h>
#include <wifi.h>
#include <lwip/ip4_addr.h>
#include <cstdarg>
#include <ch.h>

// ES-WIFI only has 4 hardware socket slots (AT command P0=0..3)
#define ISM43362_MAX_SOCKETS 4

// timeout (mS) used for the underlying ES-WIFI AT command exchanges
#define ISM43362_SOCKET_TIMEOUT 10000

// Set to 1 to re-enable the extensive "[ISM43362] ..." trace output added throughout the WiFi
// driver (es_wifi.c/wifi.c/es_wifi_io_chibios.c/sockets_ism43362.cpp/sntp_ism43362.cpp) while
// bringing this port up - useful again if something regresses, but very verbose for normal use
// (floods the debugger output, especially from the periodic Z5/CS status polling), so it
// defaults to off now that scan/connect/disconnect/DHCP are confirmed working.
#ifndef ISM43362_ENABLE_DEBUG_TRACE
#define ISM43362_ENABLE_DEBUG_TRACE 0
#endif

#if ISM43362_ENABLE_DEBUG_TRACE
#define ISM43362_TRACE(...) CLR_Debug::Printf(__VA_ARGS__)
#else
#define ISM43362_TRACE(...) ((void)0)
#endif

// Debug tracing shim for the WiFi driver's C files (es_wifi.c/wifi.c/es_wifi_io_chibios.c) -
// forwards to CLR_Debug::Printf so trace output shows up over the wire protocol in the
// debugger output window. Declared (as ISM43362_DebugPrintf, a macro) in es_wifi_conf.h - only
// compiled in at all when ISM43362_ENABLE_DEBUG_TRACE is set.
#if ISM43362_ENABLE_DEBUG_TRACE
extern "C" void ISM43362_DebugPrintf_Impl(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    CLR_Debug::PrintfV(fmt, args);
    va_end(args);
}
#endif

// Size of the per-socket receive lookahead buffer used by SOCK_ioctl(SOCK_FIONREAD) (see below).
// Chosen to match the internal buffer size InputNetworkStreamWrapper itself uses (256 bytes) - big
// enough to turn a whole HTTP response chunk into a handful of AT command round trips instead of
// one round trip PER BYTE, while still being modest on RAM (this struct is placed in the .ram4/CCM
// region - see s_sockets below - which has very little headroom).
#define ISM43362_RX_LOOKAHEAD_SIZE 256

struct Ism43362SocketState
{
    bool inUse;
    bool connected;
    WIFI_Protocol_t protocol;
    uint8_t remoteIp[4];
    uint16_t remotePort;

    // Lookahead buffer for SOCK_ioctl(SOCK_FIONREAD) (Socket.Available/NetworkStream.DataAvailable).
    // The ES-WIFI AT protocol has no MSG_PEEK equivalent, so checking availability means actually
    // reading (with the module's near-instant non-blocking timeout). Caching a whole buffer's
    // worth here lets SOCK_recv() serve large chunks from RAM instead of one round trip per byte.
    uint8_t peekBuf[ISM43362_RX_LOOKAHEAD_SIZE];
    uint16_t peekBufLen; // number of valid bytes currently cached in peekBuf
    uint16_t peekBufPos; // offset of the next unread byte within peekBuf
};

// Placed in .ram4 (CCM SRAM) rather than the default .bss (SRAM1/ram0) - ram0 is a hard,
// zero-slack 100%-packed region on this board (see repo notes), while ram4 has some headroom.
static Ism43362SocketState s_sockets[ISM43362_MAX_SOCKETS] __attribute__((section(".ram4")));
static int s_lastError = 0;

static int AllocateSocketSlot(WIFI_Protocol_t protocol)
{
    for (int i = 0; i < ISM43362_MAX_SOCKETS; i++)
    {
        if (!s_sockets[i].inUse)
        {
            s_sockets[i].inUse = true;
            s_sockets[i].connected = false;
            s_sockets[i].protocol = protocol;
            memset(s_sockets[i].remoteIp, 0, sizeof(s_sockets[i].remoteIp));
            s_sockets[i].remotePort = 0;
            s_sockets[i].peekBufLen = 0;
            s_sockets[i].peekBufPos = 0;
            return i;
        }
    }

    return SOCK_SOCKET_ERROR;
}

static bool IsValidSocket(SOCK_SOCKET socket)
{
    return socket >= 0 && socket < ISM43362_MAX_SOCKETS && s_sockets[socket].inUse;
}

// extracts IPv4 address (network byte order) and port (host byte order) from a SOCK_sockaddr
static void GetIPv4AddressAndPort(const struct SOCK_sockaddr *address, uint8_t ipAddr[4], uint16_t *port)
{
    const SOCK_sockaddr_in *addressIn = (const SOCK_sockaddr_in *)address;

    ipAddr[0] = addressIn->sin_addr.S_un.S_un_b.s_b1;
    ipAddr[1] = addressIn->sin_addr.S_un.S_un_b.s_b2;
    ipAddr[2] = addressIn->sin_addr.S_un.S_un_b.s_b3;
    ipAddr[3] = addressIn->sin_addr.S_un.S_un_b.s_b4;

    // sin_port is stored in network byte order (big endian)
    *port = (uint16_t)(((addressIn->sin_port & 0xFF) << 8) | ((addressIn->sin_port >> 8) & 0xFF));
}

SOCK_SOCKET SOCK_socket(int family, int type, int protocol)
{
    (void)family;
    (void)protocol;

    WIFI_Protocol_t wifiProtocol = (type == SOCK_SOCK_DGRAM) ? WIFI_UDP_PROTOCOL : WIFI_TCP_PROTOCOL;

    int socket = AllocateSocketSlot(wifiProtocol);
    if (socket == SOCK_SOCKET_ERROR)
    {
        s_lastError = SOCK_EMFILE;
    }

    return socket;
}

int SOCK_bind(SOCK_SOCKET socket, const struct SOCK_sockaddr *address, int addressLen)
{
    (void)address;
    (void)addressLen;

    // client sockets don't need an explicit bind before connect on this module
    if (!IsValidSocket(socket))
    {
        s_lastError = SOCK_ENOTSOCK;
        return SOCK_SOCKET_ERROR;
    }

    return 0;
}

int SOCK_connect(SOCK_SOCKET socket, const struct SOCK_sockaddr *address, int addressLen)
{
    (void)addressLen;

    if (!IsValidSocket(socket))
    {
        s_lastError = SOCK_ENOTSOCK;
        return SOCK_SOCKET_ERROR;
    }

    uint8_t ipAddr[4];
    uint16_t port;
    GetIPv4AddressAndPort(address, ipAddr, &port);

    if (WIFI_OpenClientConnection((uint32_t)socket, s_sockets[socket].protocol, "", ipAddr, port, 0) != WIFI_STATUS_OK)
    {
        s_lastError = SOCK_ECONNREFUSED;
        return SOCK_SOCKET_ERROR;
    }

    memcpy(s_sockets[socket].remoteIp, ipAddr, sizeof(s_sockets[socket].remoteIp));
    s_sockets[socket].remotePort = port;
    s_sockets[socket].connected = true;

    return 0;
}

int SOCK_send(SOCK_SOCKET socket, const char *buf, int len, int flags)
{
    (void)flags;

    if (!IsValidSocket(socket) || !s_sockets[socket].connected)
    {
        s_lastError = SOCK_ENOTCONN;
        return SOCK_SOCKET_ERROR;
    }

    // reject anything that doesn't fit in the uint16_t WIFI_SendData() expects - a negative or
    // oversized len would otherwise wrap into an unrelated request length
    if (len < 0 || len > (int)UINT16_MAX)
    {
        s_lastError = SOCK_EINVAL;
        return SOCK_SOCKET_ERROR;
    }

    uint16_t sentLen = 0;

    if (WIFI_SendData((uint8_t)socket, (uint8_t *)buf, (uint16_t)len, &sentLen, ISM43362_SOCKET_TIMEOUT) !=
        WIFI_STATUS_OK)
    {
        s_lastError = SOCK_ECONNRESET;
        return SOCK_SOCKET_ERROR;
    }

    return (int)sentLen;
}

int SOCK_recv(SOCK_SOCKET socket, char *buf, int len, int flags)
{
    (void)flags;

    if (!IsValidSocket(socket) || !s_sockets[socket].connected)
    {
        s_lastError = SOCK_ENOTCONN;
        return SOCK_SOCKET_ERROR;
    }

    if (len <= 0)
    {
        return 0;
    }

    // if SOCK_ioctl(SOCK_FIONREAD) already cached data from the module to answer a
    // Socket.Available query, drain from that buffer first rather than losing it or going back
    // to the module for data we already have in RAM.
    Ism43362SocketState &state = s_sockets[socket];
    if (state.peekBufPos < state.peekBufLen)
    {
        int available = state.peekBufLen - state.peekBufPos;
        int toCopy = (len < available) ? len : available;

        memcpy(buf, &state.peekBuf[state.peekBufPos], toCopy);
        state.peekBufPos += toCopy;

        return toCopy;
    }

    uint16_t receivedLen = 0;

    // ES_WIFI_ReceiveData() (es_wifi.c) has a hard requirement that the requested length fit in
    // a single AT command chunk (ES_WIFI_PAYLOAD_SIZE, 1200 bytes) - if it doesn't, the whole
    // function body is skipped by an `if (Reqlen <= ES_WIFI_PAYLOAD_SIZE)` guard and it returns
    // ES_WIFI_STATUS_ERROR without even attempting anything (NOT a partial-read, an outright
    // error). Clamp the request down to what the module can actually service in one round trip -
    // returning fewer bytes than asked for is completely normal, valid recv() behavior that every
    // caller here already needs to (and does) handle via a loop, so this is not a functional
    // regression, just working within the module's real per-call limit instead of silently
    // erroring out above it.
    uint16_t requestLen = (len > ES_WIFI_PAYLOAD_SIZE) ? ES_WIFI_PAYLOAD_SIZE : (uint16_t)len;

    WIFI_Status_t status =
        WIFI_ReceiveData((uint8_t)socket, (uint8_t *)buf, requestLen, &receivedLen, ISM43362_SOCKET_TIMEOUT);

    if (status == WIFI_STATUS_SOCKET_CLOSED)
    {
        // remote peer closed the connection - this is a normal, expected end-of-stream
        // condition (e.g. a simple HTTP server without Content-Length signaling EOF via
        // connection close), NOT an error - a standard BSD-style recv() returns 0 in this case,
        // NOT -1, so callers (e.g. NetworkStream.Read()) can tell "clean EOF" apart from a real
        // I/O failure instead of throwing
        s_sockets[socket].connected = false;
        return 0;
    }

    if (status != WIFI_STATUS_OK)
    {
        // WIFI_STATUS_TIMEOUT (no data arrived within ISM43362_SOCKET_TIMEOUT on THIS particular
        // attempt) is NOT necessarily a dead/broken connection - a read spanning multiple TCP
        // segments/module reads can have brief gaps between them, or the peer may just be slow.
        // Report it distinctly from a genuine I/O error (SOCK_ECONNRESET) so callers that can
        // sensibly retry don't treat a plain "nothing arrived yet" as a fatal, non-retryable
        // failure.
        s_lastError = (status == WIFI_STATUS_TIMEOUT) ? SOCK_ETIMEDOUT : SOCK_ECONNRESET;
        return SOCK_SOCKET_ERROR;
    }

    return (int)receivedLen;
}

int SOCK_close(SOCK_SOCKET socket)
{
    if (!IsValidSocket(socket))
    {
        s_lastError = SOCK_ENOTSOCK;
        return SOCK_SOCKET_ERROR;
    }

    if (s_sockets[socket].connected)
    {
        WIFI_CloseClientConnection((uint32_t)socket);
    }

    s_sockets[socket].inUse = false;
    s_sockets[socket].connected = false;

    return 0;
}

int SOCK_listen(SOCK_SOCKET socket, int backlog)
{
    (void)socket;
    (void)backlog;

    // not implemented yet - server sockets need ES-WIFI server-mode support
    // (WIFI_StartServer/WIFI_WaitServerConnection) wired up here
    s_lastError = SOCK_EOPNOTSUPP;
    return SOCK_SOCKET_ERROR;
}

SOCK_SOCKET SOCK_accept(SOCK_SOCKET socket, struct SOCK_sockaddr *address, int *addressLen)
{
    (void)socket;
    (void)address;
    (void)addressLen;

    // not implemented yet, see SOCK_listen()
    s_lastError = SOCK_EOPNOTSUPP;
    return SOCK_SOCKET_ERROR;
}

int SOCK_shutdown(SOCK_SOCKET socket, int how)
{
    (void)how;

    // ES-WIFI doesn't support half-close, so shutdown behaves the same as close
    return SOCK_close(socket);
}

int SOCK_getaddrinfo(
    const char *nodename,
    char *servname,
    const struct SOCK_addrinfo *hints,
    struct SOCK_addrinfo **res)
{
    (void)servname;
    (void)hints;

    // returns a pointer into shared static storage - only one resolution can be in use at a time.
    // WIFI_GetHostAddress() locks/unlocks WiFiMutex internally around just the AT command (can't
    // hold it for this whole function - non-recursive mutex), so only the population below is
    // guarded. Callers should copy out anything they need before calling this again.
    static uint8_t s_ipAddr[4];
    static SOCK_sockaddr_in s_sockAddr;
    static SOCK_addrinfo s_addrInfo;

    if (WIFI_GetHostAddress(nodename, s_ipAddr) != WIFI_STATUS_OK)
    {
        s_lastError = SOCK_HOST_NOT_FOUND;
        return SOCK_SOCKET_ERROR;
    }

    LOCK_WIFI();

    memset(&s_sockAddr, 0, sizeof(s_sockAddr));
    s_sockAddr.sin_family = SOCK_AF_INET;
    s_sockAddr.sin_addr.S_un.S_un_b.s_b1 = s_ipAddr[0];
    s_sockAddr.sin_addr.S_un.S_un_b.s_b2 = s_ipAddr[1];
    s_sockAddr.sin_addr.S_un.S_un_b.s_b3 = s_ipAddr[2];
    s_sockAddr.sin_addr.S_un.S_un_b.s_b4 = s_ipAddr[3];

    memset(&s_addrInfo, 0, sizeof(s_addrInfo));
    s_addrInfo.ai_family = SOCK_AF_INET;
    s_addrInfo.ai_addrlen = sizeof(SOCK_sockaddr_in);
    s_addrInfo.ai_addr = (struct SOCK_sockaddr *)&s_sockAddr;

    *res = &s_addrInfo;

    UNLOCK_WIFI();

    return 0;
}

void SOCK_freeaddrinfo(struct SOCK_addrinfo *ai)
{
    // nothing to free - SOCK_getaddrinfo() above uses static storage
    (void)ai;
}

int SOCK_ioctl(SOCK_SOCKET socket, int cmd, int *data)
{
    if (cmd == SOCK_FIONREAD)
    {
        // Socket.Available/NetworkStream.DataAvailable map to this. Managed code polls it in a
        // loop and expects a real value - previously this never set *data, so Available always
        // read 0 and the retry loop spun until a request timeout disposed the connection.
        if (data == NULL)
        {
            return SOCK_SOCKET_ERROR;
        }

        if (!IsValidSocket(socket) || !s_sockets[socket].connected)
        {
            *data = 0;
            return 0;
        }

        Ism43362SocketState &state = s_sockets[socket];

        if (state.peekBufPos < state.peekBufLen)
        {
            *data = state.peekBufLen - state.peekBufPos;
            return 0;
        }

        // Restricted to TCP: SOCK_recvfrom() (UDP) doesn't drain peekBuf, so caching a datagram
        // here would make it silently disappear from a subsequent ReceiveFrom() call. TCP is a
        // byte stream (no datagram framing to lose), and SOCK_recv() already knows to serve from
        // this buffer first.
        if (state.protocol != WIFI_TCP_PROTOCOL)
        {
            *data = 0;
            return 0;
        }

        // No "peek without consuming" AT command exists, so checking for data means actually
        // reading with a near-instant timeout (0 -> 1ms). Read a whole lookahead buffer's worth
        // (not 1 byte) so SOCK_recv() can serve large chunks from RAM instead of one round trip
        // per byte.
        state.peekBufPos = 0;
        state.peekBufLen = 0;

        uint16_t receivedLen = 0;
        WIFI_Status_t status =
            WIFI_ReceiveData((uint8_t)socket, state.peekBuf, ISM43362_RX_LOOKAHEAD_SIZE, &receivedLen, 0);

        if (status == WIFI_STATUS_SOCKET_CLOSED)
        {
            state.connected = false;
            *data = 0;
        }
        else if (status == WIFI_STATUS_OK && receivedLen > 0)
        {
            state.peekBufLen = receivedLen;
            *data = receivedLen;
        }
        else
        {
            *data = 0;
        }

        return 0;
    }

    // other commands (e.g. SOCK_FIONBIO) are accepted as no-ops - every SOCK_* I/O function on
    // this board is already fully synchronous/blocking internally (see the comment in
    // SOCK_select()), so there is no separate non-blocking mode to toggle.
    (void)socket;
    (void)cmd;
    (void)data;

    return 0;
}

int SOCK_getlasterror()
{
    return s_lastError;
}

int SOCK_getsocklasterror(SOCK_SOCKET socket)
{
    (void)socket;

    return s_lastError;
}

int SOCK_select(
    int nfds,
    SOCK_fd_set *readfds,
    SOCK_fd_set *writefds,
    SOCK_fd_set *except,
    const struct SOCK_timeval *timeout)
{
    (void)nfds;
    (void)timeout;

    // The ES-WIFI AT protocol has no true select()-equivalent (no way to check readiness without
    // either consuming data or issuing a real command), but every SOCK_* I/O function on this
    // board (SOCK_connect/SOCK_send/SOCK_recv) is ALREADY fully synchronous/blocking with its own
    // internal timeout (see ISM43362_SOCKET_TIMEOUT and the ES_WIFI_* AT command sequences) - by
    // the time SOCK_connect() returns success, the TCP handshake has already fully completed, so
    // a connected socket is always immediately "ready" for both read and write in the sense that
    // matters to callers using the common "non-blocking connect + poll for writability" pattern
    // (e.g. System.Net.Sockets.Socket.Connect() -> Socket.Poll(), which previously failed outright
    // here with SOCK_EOPNOTSUPP, breaking basic HTTP requests). Report any connected socket in
    // readfds/writefds as ready; the actual blocking/waiting still happens for real inside the
    // subsequent SOCK_recv()/SOCK_send() call.
    //
    // IMPORTANT EXCEPTION: nanoFramework's HttpWebRequest keeps a pool of persistent connections
    // (m_ConnectedStreams) and, before reusing one, calls Socket.Poll(-1, SelectMode.SelectWrite)
    // specifically to check whether the peer has closed the connection while it sat idle in the
    // pool (see HttpWebRequest.EstablishConnection()). If we always claim "ready" here regardless
    // of the peer's actual state, a stale/dead pooled connection gets reused instead of being
    // dropped and recreated, and the next read against it throws ObjectDesposedException/fails.
    // So for the write-check specifically, do a real (cheap, non-consuming) liveness probe against
    // the module - a single "P0=<socket>" AT command (WIFI_IsSocketOpen()) - rather than trusting
    // our own locally-cached "connected" flag, which only reflects OUR side and never learns about
    // a remote close until the next actual send/recv.
    int readyCount = 0;

    // SOCK_FD_CLR() swaps the last set entry into the cleared slot and shrinks fd_count - doing
    // that while iterating fd_array[i] in order would skip the swapped-in entry. Snapshot the
    // original entries first, then rebuild each set via SOCK_FD_ZERO()+SOCK_FD_SET() instead.
    if (writefds != NULL)
    {
        unsigned int count = writefds->fd_count;
        int snapshot[SOCK_FD_SETSIZE];
        memcpy(snapshot, writefds->fd_array, count * sizeof(int));

        SOCK_FD_ZERO(writefds);

        for (unsigned int i = 0; i < count; i++)
        {
            int socket = snapshot[i];
            bool ready = false;

            if (IsValidSocket(socket) && s_sockets[socket].connected)
            {
                if (WIFI_IsSocketOpen((uint8_t)socket))
                {
                    ready = true;
                }
                else
                {
                    // module has invalidated/closed this socket on its side - reflect that in our
                    // own state so subsequent calls (send/recv/select) see it as disconnected too.
                    s_sockets[socket].connected = false;
                }
            }

            if (ready)
            {
                readyCount++;
                SOCK_FD_SET(socket, writefds);
            }
        }
    }

    if (readfds != NULL)
    {
        unsigned int count = readfds->fd_count;
        int snapshot[SOCK_FD_SETSIZE];
        memcpy(snapshot, readfds->fd_array, count * sizeof(int));

        SOCK_FD_ZERO(readfds);

        for (unsigned int i = 0; i < count; i++)
        {
            int socket = snapshot[i];

            if (IsValidSocket(socket) && s_sockets[socket].connected)
            {
                readyCount++;
                SOCK_FD_SET(socket, readfds);
            }
        }
    }

    if (except != NULL)
    {
        SOCK_FD_ZERO(except);
    }

    return readyCount;
}

int SOCK_setsockopt(SOCK_SOCKET socket, int level, int optname, const char *optval, int optlen)
{
    (void)socket;
    (void)level;
    (void)optname;
    (void)optval;
    (void)optlen;

    // most standard socket options don't have an ES-WIFI AT command equivalent - accept them
    // as no-ops rather than failing so callers that set common options (e.g. timeouts) don't break
    return 0;
}

int SOCK_getsockopt(SOCK_SOCKET socket, int level, int optname, char *optval, int *optlen)
{
    (void)socket;
    (void)level;

    if (optname == SOCK_SOCKO_ERROR && optval != NULL && optlen != NULL && *optlen >= (int)sizeof(int))
    {
        *(int *)optval = s_lastError;
        *optlen = sizeof(int);
    }

    return 0;
}

int SOCK_getpeername(SOCK_SOCKET socket, struct SOCK_sockaddr *name, int *namelen)
{
    if (!IsValidSocket(socket) || !s_sockets[socket].connected)
    {
        s_lastError = SOCK_ENOTCONN;
        return SOCK_SOCKET_ERROR;
    }

    if (name == NULL || namelen == NULL || (size_t)*namelen < sizeof(SOCK_sockaddr_in))
    {
        s_lastError = SOCK_EFAULT;
        return SOCK_SOCKET_ERROR;
    }

    SOCK_sockaddr_in *addrIn = (SOCK_sockaddr_in *)name;
    memset(addrIn, 0, sizeof(SOCK_sockaddr_in));
    addrIn->sin_family = SOCK_AF_INET;
    // sin_port is stored in network byte order (big endian)
    addrIn->sin_port =
        (uint16_t)(((s_sockets[socket].remotePort & 0xFF) << 8) | ((s_sockets[socket].remotePort >> 8) & 0xFF));
    addrIn->sin_addr.S_un.S_un_b.s_b1 = s_sockets[socket].remoteIp[0];
    addrIn->sin_addr.S_un.S_un_b.s_b2 = s_sockets[socket].remoteIp[1];
    addrIn->sin_addr.S_un.S_un_b.s_b3 = s_sockets[socket].remoteIp[2];
    addrIn->sin_addr.S_un.S_un_b.s_b4 = s_sockets[socket].remoteIp[3];

    *namelen = sizeof(SOCK_sockaddr_in);

    return 0;
}

int SOCK_getsockname(SOCK_SOCKET socket, struct SOCK_sockaddr *name, int *namelen)
{
    if (!IsValidSocket(socket))
    {
        s_lastError = SOCK_ENOTSOCK;
        return SOCK_SOCKET_ERROR;
    }

    if (name == NULL || namelen == NULL || (size_t)*namelen < sizeof(SOCK_sockaddr_in))
    {
        s_lastError = SOCK_EFAULT;
        return SOCK_SOCKET_ERROR;
    }

    // the ES-WIFI module doesn't expose the auto-assigned local (ephemeral) port for client
    // sockets, so this reports our own IP address with port 0 - good enough for callers that
    // just need Socket.LocalEndPoint's address (e.g. logging), less so for ones that need the
    // actual local port
    uint8_t localIp[4] = {0, 0, 0, 0};
    WIFI_GetIP_Address(localIp);

    SOCK_sockaddr_in *addrIn = (SOCK_sockaddr_in *)name;
    memset(addrIn, 0, sizeof(SOCK_sockaddr_in));
    addrIn->sin_family = SOCK_AF_INET;
    addrIn->sin_port = 0;
    addrIn->sin_addr.S_un.S_un_b.s_b1 = localIp[0];
    addrIn->sin_addr.S_un.S_un_b.s_b2 = localIp[1];
    addrIn->sin_addr.S_un.S_un_b.s_b3 = localIp[2];
    addrIn->sin_addr.S_un.S_un_b.s_b4 = localIp[3];

    *namelen = sizeof(SOCK_sockaddr_in);

    return 0;
}

int SOCK_recvfrom(SOCK_SOCKET s, char *buf, int len, int flags, struct SOCK_sockaddr *from, int *fromlen)
{
    (void)flags;

    if (!IsValidSocket(s))
    {
        s_lastError = SOCK_ENOTSOCK;
        return SOCK_SOCKET_ERROR;
    }

    uint8_t ipAddr[4];
    uint16_t port;
    uint16_t receivedLen = 0;

    if (WIFI_ReceiveDataFrom(
            (uint8_t)s,
            (uint8_t *)buf,
            (uint16_t)len,
            &receivedLen,
            ISM43362_SOCKET_TIMEOUT,
            ipAddr,
            &port) != WIFI_STATUS_OK)
    {
        s_lastError = SOCK_ECONNRESET;
        return SOCK_SOCKET_ERROR;
    }

    if (from != NULL && fromlen != NULL && *fromlen >= (int)sizeof(SOCK_sockaddr_in))
    {
        SOCK_sockaddr_in *fromIn = (SOCK_sockaddr_in *)from;
        memset(fromIn, 0, sizeof(SOCK_sockaddr_in));
        fromIn->sin_family = SOCK_AF_INET;
        fromIn->sin_addr.S_un.S_un_b.s_b1 = ipAddr[0];
        fromIn->sin_addr.S_un.S_un_b.s_b2 = ipAddr[1];
        fromIn->sin_addr.S_un.S_un_b.s_b3 = ipAddr[2];
        fromIn->sin_addr.S_un.S_un_b.s_b4 = ipAddr[3];
        fromIn->sin_port = (uint16_t)(((port & 0xFF) << 8) | ((port >> 8) & 0xFF));
        *fromlen = sizeof(SOCK_sockaddr_in);
    }

    return (int)receivedLen;
}

int SOCK_sendto(SOCK_SOCKET s, const char *buf, int len, int flags, const struct SOCK_sockaddr *to, int tolen)
{
    (void)flags;
    (void)tolen;

    if (!IsValidSocket(s))
    {
        s_lastError = SOCK_ENOTSOCK;
        return SOCK_SOCKET_ERROR;
    }

    // reject anything that doesn't fit in the uint16_t WIFI_SendDataTo() expects - a negative or
    // oversized len would otherwise wrap into an unrelated request length
    if (len < 0 || len > (int)UINT16_MAX)
    {
        s_lastError = SOCK_EINVAL;
        return SOCK_SOCKET_ERROR;
    }

    uint8_t ipAddr[4];
    uint16_t port;
    GetIPv4AddressAndPort(to, ipAddr, &port);

    uint16_t sentLen = 0;

    if (WIFI_SendDataTo((uint8_t)s, (uint8_t *)buf, (uint16_t)len, &sentLen, ISM43362_SOCKET_TIMEOUT, ipAddr, port) !=
        WIFI_STATUS_OK)
    {
        s_lastError = SOCK_ECONNRESET;
        return SOCK_SOCKET_ERROR;
    }

    return (int)sentLen;
}

bool Network_Initialize()
{
    memset(s_sockets, 0, sizeof(s_sockets));

    ISM43362_TRACE("[ISM43362] Network_Initialize: calling nanoHAL_Network_Initialize()...\r\n");

    // bring up the ES-WIFI module over SPI (WIFI_Init()) - this is the actual entry point
    // called at boot (from nanoHAL_Initialize() in targetHAL.cpp), whereas
    // nanoHAL_Network_Initialize() itself is otherwise only ever invoked from the lwIP socket
    // stack's own init path, which isn't compiled in for this board - without this call the
    // module's I/O function pointers are never registered (ES_WIFIObject_t is zero-initialized),
    // so any later AT command exchange would call through a NULL function pointer.
    nanoHAL_Network_Initialize();

    ISM43362_TRACE("[ISM43362] Network_Initialize: done.\r\n");

    return true;
}

bool Network_Uninitialize()
{
    for (int i = 0; i < ISM43362_MAX_SOCKETS; i++)
    {
        if (s_sockets[i].inUse)
        {
            SOCK_close(i);
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Generic network configuration / IP address helpers.
////////////////////////////////////////////////////////////////////////////////////////////

HRESULT SOCK_CONFIGURATION_LoadAdapterConfiguration(HAL_Configuration_NetworkInterface *config, uint32_t interfaceIndex)
{
    (void)interfaceIndex;

    // always refresh the MAC from the module - the config block pointer may reference
    // read-only flash where runtime updates via memcpy are silently ignored
    uint8_t mac[6];
    if (WIFI_GetMAC_Address(mac) == WIFI_STATUS_OK)
    {
        memcpy(config->MacAddress, mac, 6);
    }

    if (WIFI_IsConnected() == WIFI_STATUS_OK)
    {
        uint8_t ipAddr[4];
        uint8_t ipMask[4];
        uint8_t gatewayAddr[4];
        uint8_t dns1[4];
        uint8_t dns2[4];

        if (WIFI_GetIP_Address(ipAddr) == WIFI_STATUS_OK)
        {
            config->IPv4Address = LWIP_MAKEU32(ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
        }

        if (WIFI_GetIP_Mask(ipMask) == WIFI_STATUS_OK)
        {
            config->IPv4NetMask = LWIP_MAKEU32(ipMask[0], ipMask[1], ipMask[2], ipMask[3]);
        }

        if (WIFI_GetGateway_Address(gatewayAddr) == WIFI_STATUS_OK)
        {
            config->IPv4GatewayAddress = LWIP_MAKEU32(gatewayAddr[0], gatewayAddr[1], gatewayAddr[2], gatewayAddr[3]);
        }

        if (WIFI_GetDNS_Address(dns1, dns2) == WIFI_STATUS_OK)
        {
            config->IPv4DNSAddress1 = LWIP_MAKEU32(dns1[0], dns1[1], dns1[2], dns1[3]);
            config->IPv4DNSAddress2 = LWIP_MAKEU32(dns2[0], dns2[1], dns2[2], dns2[3]);
        }
    }

    return S_OK;
}

HRESULT SOCK_CONFIGURATION_UpdateAdapterConfiguration(
    HAL_Configuration_NetworkInterface *config,
    uint32_t interfaceIndex,
    uint32_t updateFlags)
{
    (void)config;
    (void)interfaceIndex;
    (void)updateFlags;

    // the module manages DHCP/static addressing internally as part of the join process (see
    // Network_Interface_Start_Connect in Target_Network.cpp) - there's no separate adapter
    // reconfiguration step to trigger post-join for this first pass
    return S_OK;
}

HRESULT SOCK_CONFIGURATION_LoadConfiguration(HAL_Configuration_NetworkInterface *config, uint32_t interfaceIndex)
{
    return SOCK_CONFIGURATION_LoadAdapterConfiguration(config, interfaceIndex);
}

HRESULT SOCK_CONFIGURATION_LinkStatus(uint32_t interfaceIndex, bool *status)
{
    (void)interfaceIndex;

    *status = (WIFI_IsConnected() == WIFI_STATUS_OK);

    return S_OK;
}

HRESULT SOCK_IPV4AddressFromString(const char *ipString, uint64_t *address)
{
    ip4_addr_t ipv4Address;

    if (!ip4addr_aton(ipString, &ipv4Address))
    {
        return CLR_E_INVALID_PARAMETER;
    }

    *address = ipv4Address.addr;

    return S_OK;
}

const char *SOCK_IPV4AddressToString(uint32_t address)
{
    const ip4_addr_t ip4Address = {address};

    return ip4addr_ntoa(&ip4Address);
}

////////////////////////////////////////////////////////////////////////////////////////////
// Socket-based debugger transport is not supported in this first pass (it would require
// SOCK_listen/SOCK_accept, which are explicitly out of scope - see SOCK_listen/SOCK_accept
// above). Debugging on this board is expected to happen over a different transport (UART/USB).
////////////////////////////////////////////////////////////////////////////////////////////

bool SOCKETS_DbgInitialize(int comPortNum)
{
    (void)comPortNum;

    return false;
}

bool SOCKETS_DbgUninitialize(int comPortNum)
{
    (void)comPortNum;

    return false;
}
