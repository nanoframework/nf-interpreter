//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Minimal synchronous SNTP client for the Inventek ISM43362 (ES-WIFI) module.
//
// lwIP's sntp_* API (normally used by nf_networking_sntp_nanoFramework_Networking_Sntp.cpp) needs
// a real lwIP netif to send/receive UDP over - this board doesn't have one, since the ES-WIFI
// module runs its own onboard TCP/IP stack, exposed only via an AT-command socket proxy (see
// sockets_ism43362.cpp). So lwIP's sntp_init() never succeeds here.
//
// This file implements a minimal synchronous NTP client instead, on top of the existing SOCK_*
// socket-proxy functions, reusing SetSystemTimeFromUnixEpoch() to update the system clock.
//
// Managed code (nanoFramework.Networking.WifiNetworkHelper.ConnectDhcp(..., requiresDateTime:
// true, ...)) never calls Sntp.Start()/UpdateNow() itself - it just polls DateTime.UtcNow,
// expecting the firmware to auto-sync the clock once the network comes up (like ESP32 and
// lwIP-netif ChibiOS boards already do). So this board needs the same kind of automatic trigger -
// see Ism43362_Sntp_TriggerAutoSync() below, called from Network_Interface_Start_Connect() in
// Target_Network.cpp right after a successful WIFI_Connect().


#include <nanoHAL.h>
#include <nanoCLR_Types.h>
#include <nanoPAL_Sockets.h>
#include <targetHAL_Watchdog.h>
#include <ch.h>
#include <cstdint>
#include <cstring>

extern "C"
{
#include <wifi.h>
}

extern "C" void SetSystemTimeFromUnixEpoch(uint32_t seconds);

// NTP (RFC 5905) timestamps are seconds since 1900-01-01; Unix time is seconds since 1970-01-01 -
// this is the difference between those two epochs, in seconds
#define NTP_TO_UNIX_EPOCH_OFFSET_SECONDS 2208988800U

#define NTP_PACKET_SIZE 48
#define NTP_PORT        123

// Deliberately short compared to ISM43362_SOCKET_TIMEOUT (10000ms, used by the general-purpose
// SOCK_recv()) - see the comment at the WIFI_ReceiveData() call site below for why.
#define NTP_RECEIVE_TIMEOUT_MS 3000

// Performs one synchronous NTP request/response round trip against the given server and, on
// success, updates the system clock. Returns true on success.
bool Ism43362_Sntp_Sync(const char *serverName)
{
    ISM43362_DebugPrintf("[ISM43362] SNTP: resolving '%s'...\r\n", serverName);

    struct SOCK_addrinfo *res = NULL;
    if (SOCK_getaddrinfo(serverName, NULL, NULL, &res) != 0 || res == NULL || res->ai_addr == NULL)
    {
        ISM43362_DebugPrintf("[ISM43362] SNTP: DNS lookup for '%s' failed\r\n", serverName);
        return false;
    }

    SOCK_SOCKET s = SOCK_socket(SOCK_AF_INET, SOCK_SOCK_DGRAM, 0);
    if (s == SOCK_SOCKET_ERROR)
    {
        ISM43362_DebugPrintf("[ISM43362] SNTP: socket() failed\r\n");
        return false;
    }

    // copy out of SOCK_getaddrinfo()'s shared static storage before mutating anything - it can be
    // overwritten by another resolution as soon as this call returns
    SOCK_sockaddr_in addr;
    memcpy(&addr, res->ai_addr, sizeof(addr));

    // getaddrinfo() doesn't know about the service port - set it explicitly (network byte order)
    addr.sin_port = (uint16_t)(((NTP_PORT & 0xFF) << 8) | ((NTP_PORT >> 8) & 0xFF));

    if (SOCK_connect(s, (struct SOCK_sockaddr *)&addr, sizeof(addr)) != 0)
    {
        ISM43362_DebugPrintf("[ISM43362] SNTP: connect() failed\r\n");
        SOCK_close(s);
        return false;
    }

    uint8_t packet[NTP_PACKET_SIZE];
    memset(packet, 0, sizeof(packet));
    // LI = 0 (no warning), VN = 4 (NTPv4), Mode = 3 (client)
    packet[0] = 0x23;

    // this round trip typically completes in well under a second, but feed the watchdog anyway
    // since it's still a blocking network operation with a multi-second worst-case timeout
    Watchdog_Reset();

    if (SOCK_send(s, (const char *)packet, sizeof(packet), 0) != (int)sizeof(packet))
    {
        ISM43362_DebugPrintf("[ISM43362] SNTP: send() failed\r\n");
        SOCK_close(s);
        return false;
    }

    // not using SOCK_recv() - it hardcodes a 10s wait while holding WiFiMutex the whole time,
    // which can block a concurrent WiFi (re)connect's AT commands. Use a shorter timeout instead.
    // `s` is usable directly as the ES-WIFI socket slot index (see AllocateSocketSlot()).
    uint16_t receivedLen = 0;
    int received =
        (WIFI_ReceiveData((uint8_t)s, packet, sizeof(packet), &receivedLen, NTP_RECEIVE_TIMEOUT_MS) == WIFI_STATUS_OK)
            ? (int)receivedLen
            : -1;

    SOCK_close(s);

    Watchdog_Reset();

    if (received != NTP_PACKET_SIZE)
    {
        ISM43362_DebugPrintf("[ISM43362] SNTP: recv() failed/timed out (returned %d)\r\n", received);
        return false;
    }

    // reject responses the server itself flags as not usable (RFC 5905): LI == 3 ("alarm",
    // server clock not synchronized) or Stratum == 0 (kiss-o'-death, e.g. rate-limiting/deny)
    uint8_t leapIndicator = (packet[0] >> 6) & 0x03;
    uint8_t stratum = packet[1];
    if (leapIndicator == 3 || stratum == 0)
    {
        ISM43362_DebugPrintf(
            "[ISM43362] SNTP: server reports unsynchronized (LI=%d, stratum=%d), ignoring\r\n",
            (int)leapIndicator,
            (int)stratum);
        return false;
    }

    // Transmit Timestamp seconds field is the 4 bytes at offset 40, big-endian
    uint32_t ntpSeconds = ((uint32_t)packet[40] << 24) | ((uint32_t)packet[41] << 16) | ((uint32_t)packet[42] << 8) |
                          (uint32_t)packet[43];

    if (ntpSeconds < NTP_TO_UNIX_EPOCH_OFFSET_SECONDS)
    {
        ISM43362_DebugPrintf("[ISM43362] SNTP: implausible timestamp in response, ignoring\r\n");
        return false;
    }

    uint32_t unixSeconds = ntpSeconds - NTP_TO_UNIX_EPOCH_OFFSET_SECONDS;

    SetSystemTimeFromUnixEpoch(unixSeconds);

    ISM43362_DebugPrintf("[ISM43362] SNTP: time synced OK (unixEpochSeconds=%u)\r\n", (unsigned int)unixSeconds);

    return true;
}

// Kicks off a one-shot, best-effort automatic NTP sync - call this right after a WiFi connection
// succeeds (see the big comment at the top of this file for why this is needed).
//
// Runs SYNCHRONOUSLY on the calling thread rather than a background one. A background-thread
// approach (both a static working area and a heap-based thread) was tried and reliably crashed
// (silent reboot) the instant a thread was created from this calling context, regardless of
// stack size/alignment - root cause not identified. Running synchronously here just adds a few
// seconds to a native call that already blocks for several seconds during WiFi join anyway.
//
// This is polled from within the managed connect's 20-second timeout window, so the two sync
// attempts below are capped to a combined budget well under that - a slow/unreachable NTP server
// must not be able to turn an otherwise-successful connect into a reported timeout.
#define SNTP_AUTOSYNC_BUDGET_MS 4000

extern "C" void Ism43362_Sntp_TriggerAutoSync()
{
    ISM43362_DebugPrintf("[ISM43362] SNTP: TriggerAutoSync entered (synchronous)\r\n");

    systime_t start = chVTGetSystemTimeX();

    if (!Ism43362_Sntp_Sync("pool.ntp.org") &&
        TIME_I2MS(chTimeDiffX(start, chVTGetSystemTimeX())) < SNTP_AUTOSYNC_BUDGET_MS)
    {
        Ism43362_Sntp_Sync("time.nist.gov");
    }
}

