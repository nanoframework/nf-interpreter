//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Minimal synchronous SNTP client for the Inventek ISM43362 (ES-WIFI) module.
//
// nanoFramework.Networking.Sntp's native implementation normally just drives lwIP's own
// sntp_* API (see nf_networking_sntp_nanoFramework_Networking_Sntp.cpp) - but lwIP's SNTP
// client sends/receives its UDP packets through a real lwIP netif, and this board doesn't have
// one: the ES-WIFI module runs its own onboard TCP/IP stack and is only exposed to the host via
// an AT-command socket proxy (see sockets_ism43362.cpp), not raw link-layer frames. lwIP's
// sntp_init() therefore has nothing to actually send/receive over on this board and silently
// never succeeds (confirmed on real hardware: WifiNetworkHelper.ConnectDhcp() got a valid IP
// address but never got a DateTime, eventually timing out).
//
// This file implements a minimal, synchronous NTP client instead, built directly on top of the
// existing SOCK_* socket-proxy functions (DNS lookup via SOCK_getaddrinfo() -> WIFI_GetHostAddress,
// and a UDP "connected" socket for send/recv), reusing the SetSystemTimeFromUnixEpoch() helper
// (declared in nf_networking_sntp_nanoFramework_Networking_Sntp.cpp) to actually update the
// system clock. It's synchronous (blocks the calling thread for the DNS+UDP round trip, typically
// well under the ES-WIFI socket timeout) rather than a periodic background service like lwIP's -
// callers get one-shot behavior matching Start()/UpdateNow() being invoked explicitly.
//
// IMPORTANT: managed code (e.g. nanoFramework.Networking.WifiNetworkHelper.ConnectDhcp(...,
// requiresDateTime: true, ...)) does NOT call Sntp.Start()/UpdateNow() itself - it just polls
// DateTime.UtcNow in a loop (see NetworkHelperInternal.WaitForValidDateTime() in the
// nanoframework/System.Net repo), with a comment saying this relies on "SNTP available and
// enabled on target device" - i.e. it expects the FIRMWARE to sync the clock automatically in
// the background once the network comes up, exactly like ESP32 (esp_netif_sntp_init()) and
// lwIP-netif ChibiOS boards (nf_lwipthread_wifi.c's automatic sntp_init() on DHCP-up) already do.
// Confirmed on real hardware: neither Sntp.Start()/UpdateNow() nor SOCK_select() ever got called
// even after multiple successful connect+DHCP cycles - so this board needs the same kind of
// automatic trigger. See Ism43362_Sntp_TriggerAutoSync() below, called from
// Network_Interface_Start_Connect() in Target_Network.cpp right after a successful WIFI_Connect().

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

extern "C" void ISM43362_DebugPrintf(const char *fmt, ...);
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

    // getaddrinfo() doesn't know about the service port - set it explicitly (network byte order)
    SOCK_sockaddr_in *addr = (SOCK_sockaddr_in *)res->ai_addr;
    addr->sin_port = (uint16_t)(((NTP_PORT & 0xFF) << 8) | ((NTP_PORT >> 8) & 0xFF));

    if (SOCK_connect(s, (struct SOCK_sockaddr *)addr, sizeof(SOCK_sockaddr_in)) != 0)
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

    // NOTE: deliberately NOT using SOCK_recv() here - it hardcodes a 10-second
    // (ISM43362_SOCKET_TIMEOUT) wait on the module while holding the shared WiFiMutex for the
    // WHOLE duration (see ES_WIFI_ReceiveData()'s LOCK_WIFI()/UNLOCK_WIFI() pair), which was found
    // to be long enough to make a concurrent WiFi (re)connect attempt's own AT commands hang/time
    // out waiting for the module to become responsive again. Calling WIFI_ReceiveData() directly
    // with a much shorter timeout keeps this best-effort sync from monopolizing the module for
    // long if the NTP server doesn't respond quickly. `s` is usable directly as the ES-WIFI socket
    // slot index since SOCK_SOCKET handles on this board ARE the raw slot index (see
    // AllocateSocketSlot() in sockets_ism43362.cpp).
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
// NOTE: this runs SYNCHRONOUSLY (blocking the calling native method call for the DNS+UDP round
// trip, typically well under a second, occasionally a couple of seconds) rather than on a
// background thread. A background-thread approach was tried extensively first - both a STATIC
// working area (THD_WORKING_AREA()+chThdCreateStatic(), in a custom ".ram4" linker section with
// an explicit 8-byte alignment attribute, verified correctly aligned via `nm`) and a heap-based
// one (chThdCreateFromHeap(), with the heap deliberately sized generously to guarantee the
// allocation would succeed) - but BOTH reliably crashed (silent reboot, no error message) the
// instant the thread was created/started, at the EXACT same point in the call sequence, regardless
// of the underlying memory mechanism, stack size (tried 8192 and 12288 bytes), or verified-correct
// alignment. Since the crash reproduced identically across two totally different thread-creation
// mechanisms, the problem clearly isn't about memory placement/size at all - it's specific to
// creating/starting ANY new OS thread from this exact calling context (inside
// Network_Interface_Connect_Result(), itself called from the NativeConnect___... nanoCLR native
// method) - a root cause that wasn't identified despite extensive bisection. Rather than continue
// blocking this feature on that unresolved mystery, this now runs synchronously on the SAME thread
// that's already executing the native call - which already blocks synchronously for several
// seconds during the WiFi join sequence anyway, so a few more seconds here for SNTP is a
// quantitative increase, not a qualitatively new kind of blocking.
//
// This is called from inside Network_Interface_Connect_Result(), which is itself polled from the
// managed connect's 20-second timeout window - so the two synchronous attempts below are capped
// to a combined budget well under that, to make sure a slow/unreachable NTP server can't eat
// enough of that window to turn an otherwise-successful WiFi connect into a reported timeout.
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
