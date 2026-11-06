//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Wire Protocol over TCP for lwIP based targets.
//
// The device is a TCP server that accepts a single debugger client:
// - non-blocking sockets, driven by polling from the Wire Protocol receiver thread (see WP_ReceiveBytes()).
// - a second client is rejected while the current one is active. If the current one stays silent for 5 seconds the next
//   connection attempt takes over.
// - TCP_NODELAY and linger 0 (no TIME_WAIT) and TCP keepalive to detect clients that vanished.
//
// Optionally the device announces itself with a UDP broadcast every 5 seconds.
// Message format (ASCII): "+:<port>:<targetName>" when running or "-:<port>" when going away.
//
// The listener and client sockets are registered as debug sockets so they stay open when the CLR reboots.

// developer note: sockets_lwip.h has to be the first include because the Wire Protocol headers define macros (min)
// that conflict with the C++ standard library headers
#include "sockets_lwip.h"

#include <nanoHAL_v2.h>
#include <nanoWeak.h>
#include <nanoPAL_Sockets.h>
#include <WireProtocol_Transport.h>
#include <WireProtocol_Message.h>

#if CONFIG_NF_WP_CHANNEL_NETWORK && !defined(I_AM_NANOBOOTER)

#include <lwip/sockets.h>

#ifndef TARGETNAMESTRING
#define TARGETNAMESTRING "nanoCLR"
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
// configuration

// a new connection while there is a client, arms a window of this duration: if the client doesn't send anything the
// current client is dropped and the next connection is accepted
static const uint32_t c_TakeoverWindowMs = 5000;

// interval of the discovery announcements
static const uint32_t c_DiscoveryIntervalMs = 5000;

// interval to retry creating the listener
static const uint32_t c_ListenerRetryMs = 1000;

// max time to wait for the client to take the data when sending
static const uint32_t c_SendTimeoutMs = 3000;

// time to wait for incoming data from the client on each receive
static const uint32_t c_ReceiveWaitMs = 100;

// keepalive: idle time, interval and count of probes (a dead client is detected after ~15 seconds)
static const int c_KeepAliveIdleSeconds = 5;
static const int c_KeepAliveIntervalSeconds = 2;
static const int c_KeepAliveCount = 5;

#ifndef CONFIG_NF_WP_NETWORK_DISCOVERY_PORT
#define CONFIG_NF_WP_NETWORK_DISCOVERY_PORT 23657
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
// state

// sockets are accessed from the receiver thread (poll, receive) and from other threads (send, stop)
static SOCK_SOCKET s_listener = SOCK_SOCKET_ERROR;
static SOCK_SOCKET s_client = SOCK_SOCKET_ERROR;
static SOCK_SOCKET s_discovery = SOCK_SOCKET_ERROR;

static bool s_started = false;

// incremented for each accepted client
static uint32_t s_clientGeneration = 0;

// all times in milliseconds
static uint32_t s_takeoverDeadline = 0;
static bool s_takeoverArmed = false;
static uint32_t s_holdOffUntil = 0;
static bool s_holdOff = false;
static uint32_t s_nextListenerAttempt = 0;
static uint32_t s_nextAnnounce = 0;
static bool s_announceDue = true;

// fd sets are large, so they are static to not eat the stack of the receiver thread
static SOCK_fd_set s_rxSet;
static SOCK_fd_set s_txSet;

///////////////////////////////////////////////////////////////////////////////////////////////////
// helpers

static uint32_t NowMs()
{
    return (uint32_t)(HAL_Time_SysTicksToTime(HAL_Time_CurrentSysTicks()) / TIME_CONVERSION__TO_MILLISECONDS);
}

static bool TimeReached(uint32_t now, uint32_t deadline)
{
    // handles wrap around
    return (int32_t)(now - deadline) >= 0;
}

static SOCK_SOCKET GetClient()
{
    SOCK_SOCKET sock;

    GLOBAL_LOCK();
    sock = s_client;
    GLOBAL_UNLOCK();

    return sock;
}

static SOCK_SOCKET GetClientAndGeneration(uint32_t *generation)
{
    SOCK_SOCKET sock;

    GLOBAL_LOCK();
    sock = s_client;
    *generation = s_clientGeneration;
    GLOBAL_UNLOCK();

    return sock;
}

static SOCK_SOCKET GetListener()
{
    SOCK_SOCKET sock;

    GLOBAL_LOCK();
    sock = s_listener;
    GLOBAL_UNLOCK();

    return sock;
}

// swaps the client if it's still the expected one, returns true when it did
static bool ReplaceClient(SOCK_SOCKET expected, SOCK_SOCKET replacement)
{
    bool replaced = false;

    GLOBAL_LOCK();
    if (s_client == expected)
    {
        s_client = replacement;
        replaced = true;
    }
    GLOBAL_UNLOCK();

    return replaced;
}

static void CloseSocket(SOCK_SOCKET sock)
{
    if (sock != SOCK_SOCKET_ERROR)
    {
        // linger 0: abort the connection, no TIME_WAIT
        int linger = 0;
        HAL_SOCK_setsockopt(sock, SOCK_SOL_SOCKET, SOCK_SOCKO_LINGER, (const char *)&linger, sizeof(linger));

        Sockets_LWIP_Driver::Close(sock);
    }
}

// closes the client, but only if it is still the given one
static void CloseClient(SOCK_SOCKET client)
{
    if (client != SOCK_SOCKET_ERROR && ReplaceClient(client, SOCK_SOCKET_ERROR))
    {
        CloseSocket(client);

        s_takeoverArmed = false;
    }
}

static void SetKeepAliveInterval(SOCK_SOCKET sock)
{
    // lwIP only (these options are not part of the SOCK abstraction)
#if defined(LWIP_TCP_KEEPALIVE) && LWIP_TCP_KEEPALIVE
    int idle = c_KeepAliveIdleSeconds;
    int interval = c_KeepAliveIntervalSeconds;
    int count = c_KeepAliveCount;

    lwip_setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(idle));
    lwip_setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(interval));
    lwip_setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &count, sizeof(count));
#else
    (void)sock;
    (void)c_KeepAliveIdleSeconds;
    (void)c_KeepAliveIntervalSeconds;
    (void)c_KeepAliveCount;
#endif
}

static void ConfigureClient(SOCK_SOCKET sock)
{
    int nonBlocking = 1;
    int enable = 1;
    int linger = 0;

    SOCK_ioctl(sock, SOCK_FIONBIO, &nonBlocking);
    HAL_SOCK_setsockopt(sock, SOCK_IPPROTO_TCP, SOCK_TCP_NODELAY, (const char *)&enable, sizeof(enable));
    HAL_SOCK_setsockopt(sock, SOCK_SOL_SOCKET, SOCK_SOCKO_LINGER, (const char *)&linger, sizeof(linger));
    HAL_SOCK_setsockopt(sock, SOCK_SOL_SOCKET, SOCK_SOCKO_KEEPALIVE, (const char *)&enable, sizeof(enable));

    SetKeepAliveInterval(sock);
}

// waits for the socket to be readable
// returns > 0 when it is, 0 on timeout and < 0 on error
static int WaitReadable(SOCK_SOCKET sock, uint32_t waitMs)
{
    SOCK_timeval timeout;

    timeout.tv_sec = (long)(waitMs / 1000);
    timeout.tv_usec = (long)((waitMs % 1000) * 1000);

    s_rxSet.fd_count = 0;
    SOCK_FD_SET(sock, &s_rxSet);

    return HAL_SOCK_select(SOCK_FD_SETSIZE, &s_rxSet, nullptr, nullptr, &timeout);
}

// sleeps using select, when there are no sockets to wait for
static void SleepMs(uint32_t waitMs)
{
    if (waitMs == 0)
    {
        return;
    }

    PLATFORM_DELAY(waitMs);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// authentication token

static_assert(
    WPAUTHTOKEN_CONFIG_MAX_TOKEN_LENGTH == WP_AUTH_TOKEN_MAX_LENGTH,
    "token length of the configuration block doesn't match the one of the authentication");

#if defined(CONFIG_NF_WP_NETWORK_AUTH_TOKEN)
static_assert(
    sizeof(CONFIG_NF_WP_NETWORK_AUTH_TOKEN) - 1 <= WP_AUTH_TOKEN_MAX_LENGTH,
    "NF_WP_NETWORK_AUTH_TOKEN can't be longer than 64 bytes");
#endif

// token required to authenticate: the one stored in the configuration block, if there is one
// (empty means no authentication), otherwise the default of the build
static void LoadAuthToken()
{
    const uint8_t *token = (const uint8_t *)"";
    uint32_t length = 0;

#if defined(CONFIG_NF_WP_NETWORK_AUTH_TOKEN)
    token = (const uint8_t *)CONFIG_NF_WP_NETWORK_AUTH_TOKEN;
    length = sizeof(CONFIG_NF_WP_NETWORK_AUTH_TOKEN) - 1;
#endif

#if CONFIG_NF_FEATURE_HAS_CONFIG_BLOCK
    HAL_Configuration_WPAuthToken config;

    memset(&config, 0, sizeof(config));

    if (ConfigurationManager_GetConfigurationBlock(&config, DeviceConfigurationOption_WPAuthToken, 0))
    {
        token = config.Token;
        length = config.TokenLength;
    }

    // the length is limited to the maximum by the authentication
    WP_Auth_SetToken(token, length);

    // don't leave the token around
    memset(&config, 0, sizeof(config));
#else
    WP_Auth_SetToken(token, length);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// listener

static void StopListener()
{
    SOCK_SOCKET listener;

    GLOBAL_LOCK();
    listener = s_listener;
    s_listener = SOCK_SOCKET_ERROR;
    GLOBAL_UNLOCK();

    if (listener != SOCK_SOCKET_ERROR)
    {
        Sockets_LWIP_Driver::Close(listener);
    }
}

static bool StartListener()
{
    SOCK_sockaddr_in sockAddr;
    int nonBlocking = 1;
    int enable = 1;
    SOCK_SOCKET listener;

    // sockets are registered as debug sockets so they are not closed when the CLR reboots
    listener = Sockets_LWIP_Driver::Socket(SOCK_AF_INET, SOCK_SOCK_STREAM, SOCK_IPPROTO_TCP, true);

    if (listener == SOCK_SOCKET_ERROR)
    {
        return false;
    }

    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family = SOCK_AF_INET;
    sockAddr.sin_port = SOCK_htons(CONFIG_NF_WP_NETWORK_PORT);
    sockAddr.sin_addr.S_un.S_addr = SOCK_htonl(SOCK_INADDR_ANY);

    // need this to keep the compiler happy about the cast to SOCK_sockaddr
    void *dummyPtr = &sockAddr;

    if (SOCK_ioctl(listener, SOCK_FIONBIO, &nonBlocking) == SOCK_SOCKET_ERROR ||
        HAL_SOCK_setsockopt(
            listener,
            SOCK_SOL_SOCKET,
            SOCK_SOCKO_REUSEADDRESS,
            (const char *)&enable,
            sizeof(enable)) == SOCK_SOCKET_ERROR ||
        SOCK_bind(listener, (const SOCK_sockaddr *)dummyPtr, sizeof(sockAddr)) == SOCK_SOCKET_ERROR ||
        SOCK_listen(listener, 1) == SOCK_SOCKET_ERROR)
    {
        Sockets_LWIP_Driver::Close(listener);

        return false;
    }

    GLOBAL_LOCK();
    s_listener = listener;
    GLOBAL_UNLOCK();

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// discovery

#if CONFIG_NF_WP_NETWORK_DISCOVERY

// appends the decimal representation of a number, returns the new position
static char *AppendNumber(char *pos, char *end, uint32_t value)
{
    char digits[10];
    int count = 0;

    do
    {
        digits[count++] = (char)('0' + (value % 10));
        value /= 10;
    } while (value != 0);

    while (count > 0 && pos < end)
    {
        *pos++ = digits[--count];
    }

    return pos;
}

static char *AppendString(char *pos, char *end, const char *text)
{
    while (*text != '\0' && pos < end)
    {
        *pos++ = *text++;
    }

    return pos;
}

#endif // CONFIG_NF_WP_NETWORK_DISCOVERY

static void Announce(bool present)
{
#if CONFIG_NF_WP_NETWORK_DISCOVERY

    // "+:<port>:<targetName>" or "-:<port>"
    char message[96];
    char *pos = message;
    char *end = message + sizeof(message) - 1;

    if (s_discovery == SOCK_SOCKET_ERROR)
    {
        int enable = 1;

        s_discovery = Sockets_LWIP_Driver::Socket(SOCK_AF_INET, SOCK_SOCK_DGRAM, SOCK_IPPROTO_UDP, true);

        if (s_discovery == SOCK_SOCKET_ERROR)
        {
            return;
        }

        HAL_SOCK_setsockopt(s_discovery, SOCK_SOL_SOCKET, SOCK_SOCKO_BROADCAST, (const char *)&enable, sizeof(enable));
    }

    *pos++ = present ? '+' : '-';
    *pos++ = ':';
    pos = AppendNumber(pos, end, CONFIG_NF_WP_NETWORK_PORT);

    if (present)
    {
        *pos++ = ':';
        pos = AppendString(pos, end, TARGETNAMESTRING);
    }

    SOCK_sockaddr_in destination;

    memset(&destination, 0, sizeof(destination));
    destination.sin_family = SOCK_AF_INET;
    destination.sin_port = SOCK_htons(CONFIG_NF_WP_NETWORK_DISCOVERY_PORT);
    destination.sin_addr.S_un.S_addr = SOCK_htonl(0xFFFFFFFFUL);

    void *dummyPtr = &destination;

    // errors are ignored on purpose: the network may not be ready
    SOCK_sendto(s_discovery, message, (int)(pos - message), 0, (const SOCK_sockaddr *)dummyPtr, sizeof(destination));

#else
    (void)present;
#endif
}

static void StopDiscovery()
{
    if (s_discovery != SOCK_SOCKET_ERROR)
    {
        Sockets_LWIP_Driver::Close(s_discovery);
        s_discovery = SOCK_SOCKET_ERROR;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// state machine

static void AcceptClients()
{
    SOCK_SOCKET listener = GetListener();
    SOCK_sockaddr addr;
    int addrLen = sizeof(addr);

    if (listener == SOCK_SOCKET_ERROR)
    {
        return;
    }

    // holding off after an authentication failure: leave the connection waiting
    if (s_holdOff)
    {
        if (!TimeReached(NowMs(), s_holdOffUntil))
        {
            return;
        }

        s_holdOff = false;
    }

    SOCK_SOCKET sock = Sockets_LWIP_Driver::Accept(listener, &addr, &addrLen, true);

    if (sock == SOCK_SOCKET_ERROR)
    {
        return;
    }

    if (GetClient() != SOCK_SOCKET_ERROR)
    {
        // there is a client and it's active, so reject this one
        CloseSocket(sock);

        // but it is trying to connect so, if the client doesn't talk in a while, it's replaced by the next attempt
        if (!s_takeoverArmed)
        {
            s_takeoverArmed = true;
            s_takeoverDeadline = NowMs() + c_TakeoverWindowMs;
        }

        return;
    }

    ConfigureClient(sock);

    GLOBAL_LOCK();
    s_client = sock;
    s_clientGeneration++;
    GLOBAL_UNLOCK();

    s_takeoverArmed = false;

    // new session: nothing from previous ones is valid
    WP_Auth_SessionChanged(true);
    WP_Message_ResetReception();
}

void WP_Network_ReloadAuthToken(void)
{
    LoadAuthToken();
}

bool WP_Network_Initialize(void)
{
    if (!s_started)
    {
        s_started = true;
        s_announceDue = true;
        s_holdOff = false;
        s_takeoverArmed = false;

        LoadAuthToken();
    }

    if (GetListener() == SOCK_SOCKET_ERROR)
    {
        StartListener();
    }

    return GetListener() != SOCK_SOCKET_ERROR;
}

void WP_Network_Stop(void)
{
    SOCK_SOCKET client;

    // tell debuggers the device is going away
    if (s_started)
    {
        Announce(false);
    }

    GLOBAL_LOCK();
    client = s_client;
    s_client = SOCK_SOCKET_ERROR;
    GLOBAL_UNLOCK();

    CloseSocket(client);
    StopListener();
    StopDiscovery();

    s_takeoverArmed = false;
    s_announceDue = true;
    s_nextListenerAttempt = 0;
}

void WP_Network_Uninitialize(void)
{
    WP_Network_Stop();

    s_started = false;
}

void WP_Network_Poll(uint32_t waitMs)
{
    uint32_t now = NowMs();

    if (!s_started)
    {
        WP_Network_Initialize();
    }

    // listener creation may have failed because the network wasn't ready, retry
    if (GetListener() == SOCK_SOCKET_ERROR)
    {
        if (TimeReached(now, s_nextListenerAttempt))
        {
            s_nextListenerAttempt = now + c_ListenerRetryMs;

            StartListener();
        }
    }

    // discovery, only if the debugger can connect (it stays due until the listener is up)
    if (GetListener() != SOCK_SOCKET_ERROR && (s_announceDue || TimeReached(now, s_nextAnnounce)))
    {
        s_announceDue = false;
        s_nextAnnounce = now + c_DiscoveryIntervalMs;

        Announce(true);
    }

    // the current client was silent when others tried to connect
    if (s_takeoverArmed && TimeReached(now, s_takeoverDeadline))
    {
        CloseClient(GetClient());
    }

    SOCK_SOCKET listener = GetListener();

    if (listener != SOCK_SOCKET_ERROR)
    {
        // nobody connected: this is the place to wait for a client
        uint32_t wait = (GetClient() == SOCK_SOCKET_ERROR) ? waitMs : 0;

        if (WaitReadable(listener, wait) > 0)
        {
            AcceptClients();
        }
    }
    else if (GetClient() == SOCK_SOCKET_ERROR)
    {
        SleepMs(waitMs);
    }
}

void WP_Network_Receive(uint8_t **ptr, uint32_t *size)
{
    SOCK_SOCKET client = GetClient();

    if (client == SOCK_SOCKET_ERROR || *size == 0)
    {
        return;
    }

    if (WaitReadable(client, c_ReceiveWaitMs) <= 0)
    {
        return;
    }

    int received = SOCK_recv(client, (char *)*ptr, (int)*size, 0);

    if (received > 0)
    {
        *ptr += received;
        *size -= (uint32_t)received;

        // client is active, it's not to be replaced
        s_takeoverArmed = false;
    }
    else if (received == 0 || SOCK_getlasterror() != SOCK_EWOULDBLOCK)
    {
        // client closed the connection or there is an error
        CloseClient(client);
    }
}

// sends to the client, as long as it is still the same one
static bool SendToClient(SOCK_SOCKET client, uint32_t generation, const uint8_t *data, uint32_t size)
{
    uint32_t sent = 0;
    uint32_t deadline = NowMs() + c_SendTimeoutMs;

    while (sent < size)
    {
        uint32_t currentGeneration;

        if (GetClientAndGeneration(&currentGeneration) != client || currentGeneration != generation)
        {
            // client is gone, a new one can't get what was meant for the previous one
            return false;
        }

        int result = SOCK_send(client, (const char *)(data + sent), (int)(size - sent), 0);

        if (result > 0)
        {
            sent += (uint32_t)result;
            continue;
        }

        if (result < 0 && SOCK_getlasterror() != SOCK_EWOULDBLOCK)
        {
            // connection is gone
            CloseClient(client);

            return false;
        }

        // send buffer is full, wait for room
        if (TimeReached(NowMs(), deadline))
        {
            CloseClient(client);

            return false;
        }

        SOCK_timeval timeout;

        timeout.tv_sec = 0;
        timeout.tv_usec = 20000;

        s_txSet.fd_count = 0;
        SOCK_FD_SET(client, &s_txSet);

        HAL_SOCK_select(SOCK_FD_SETSIZE, nullptr, &s_txSet, nullptr, &timeout);
    }

    return true;
}

bool WP_Network_Transmit(const uint8_t *header, uint32_t headerSize, const uint8_t *payload, uint32_t payloadSize)
{
    uint32_t generation;
    SOCK_SOCKET client = GetClientAndGeneration(&generation);

    if (client == SOCK_SOCKET_ERROR)
    {
        return false;
    }

    // header and payload go to the same client
    if (!SendToClient(client, generation, header, headerSize))
    {
        return false;
    }

    return payloadSize == 0 || payload == nullptr || SendToClient(client, generation, payload, payloadSize);
}

bool WP_Network_IsConnected(void)
{
    return GetClient() != SOCK_SOCKET_ERROR;
}

void WP_Network_DropClient(uint32_t holdOffMs)
{
    // let the reply that was just sent go out, closing aborts the connection (linger 0) and discards what is pending
    PLATFORM_DELAY(50);

    CloseClient(GetClient());

    if (holdOffMs > 0)
    {
        s_holdOffUntil = NowMs() + holdOffMs;
        s_holdOff = true;
    }
}

#endif // CONFIG_NF_WP_CHANNEL_NETWORK
