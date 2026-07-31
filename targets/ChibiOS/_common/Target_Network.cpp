
//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoCLR_Types.h>
#include <ch.h>
#include <lwip/netifapi.h>
#include <lwip/dns.h>

#if defined(RP2040) || defined(RP2350)
extern "C" {
#include <nf_lwipthread_wifi.h>
}
#elif defined(TARGET_HAS_WIFI_ISM43362)
extern "C" {
#include <wifi.h>
}

// set to 1 to re-enable the "[ISM43362] ..." trace prints below (see the same flag in
// sockets_ism43362.cpp for the bulk of the AT-command-level tracing this quiets down)
#ifndef ISM43362_ENABLE_DEBUG_TRACE
#define ISM43362_ENABLE_DEBUG_TRACE 0
#endif
#if ISM43362_ENABLE_DEBUG_TRACE
#define ISM43362_TRACE(...) CLR_Debug::Printf(__VA_ARGS__)
#else
#define ISM43362_TRACE(...) ((void)0)
#endif
#else
extern "C" struct netif *nf_getNetif();
#endif

//
// Works with the Target_NetworkConfig to map the Network_Interface_XXXXX calls to the correct driver

bool Network_Interface_Bind(int index)
{
    (void)index;

    return true;
}

int Network_Interface_Open(int index)
{
    HAL_Configuration_NetworkInterface networkConfiguration;

    // load network interface configuration from storage
    if (!ConfigurationManager_GetConfigurationBlock(
            (void *)&networkConfiguration,
            DeviceConfigurationOption_Network,
            index))
    {
        // failed to load configuration
        // FIXME output error?
        return SOCK_SOCKET_ERROR;
    }
    _ASSERTE(networkConfiguration.StartupAddressMode > 0);

    switch (index)
    {
        case 0:
        {
#if defined(TARGET_HAS_WIFI_ISM43362)
            // the ES-WIFI module runs its own onboard TCP/IP stack and doesn't expose a raw
            // MAC/link-layer interface, so there's no lwIP netif to hand back here - this is
            // the only network interface on this board, so its index is always 0
            return 0;
#else
            // Open the network interface and set its config
            // TODO / FIXME

            // Return index to NetIF in its linked list, return 0 (probably right if only interface)
            // This used by Network stack to hook in to status/address changes for events to users

            // For now get the Netif number form original Chibios binding code
            struct netif *nptr = nf_getNetif();
            return nptr->num;
#endif
        }
        break;
    }
    return SOCK_SOCKET_ERROR;
}

bool Network_Interface_Close(int index)
{
    switch (index)
    {
        case 0:
#if defined(RP2040) || defined(RP2350)
            cyw43_wifi_disconnect();
#elif defined(TARGET_HAS_WIFI_ISM43362)
            WIFI_Disconnect();
#else
            macStop(&ETHD1);
#endif
            return true;
    }
    return false;
}

#if defined(RP2040) || defined(RP2350)

int Network_Interface_Disconnect(int index)
{
    (void)index;
    return cyw43_wifi_disconnect();
}

int Network_Interface_Start_Connect(int index, const char *ssid, const char *passphrase, int options)
{
    (void)index;
    (void)options;

    uint32_t auth_type = 0;
    if (passphrase != NULL && passphrase[0] != '\0')
    {
        auth_type = 0x00400004; // WPA2_AES_PSK
    }

    return cyw43_wifi_connect(ssid, passphrase, auth_type);
}

int Network_Interface_Connect_Result(int configIndex)
{
    (void)configIndex;

    if (!cyw43_wifi_is_connected())
        return -1;

    if (cyw43_wifi_get_ip4_address() == 0)
        return -1;

    // DHCP has completed — sync the live netif values (IP, gateway, netmask,
    // DNS) into the in-memory config block so that managed code reads the
    // correct addresses via ConfigurationManager_GetConfigurationBlock.
    if (g_TargetConfiguration.NetworkInterfaceConfigs != NULL &&
        g_TargetConfiguration.NetworkInterfaceConfigs->Count > 0)
    {
        HAL_Configuration_NetworkInterface *cfg =
            g_TargetConfiguration.NetworkInterfaceConfigs->Configs[0];

        struct netif *nif = nf_getNetif();
        if (nif != NULL)
        {
            cfg->IPv4Address = nif->ip_addr.addr;
            cfg->IPv4NetMask = nif->netmask.addr;
            cfg->IPv4GatewayAddress = nif->gw.addr;

#if LWIP_DNS
            cfg->IPv4DNSAddress1 = dns_getserver(0)->addr;
            cfg->IPv4DNSAddress2 = dns_getserver(1)->addr;
#endif
        }
    }

    return 0;
}

int Network_Interface_Start_Scan(int index)
{
    (void)index;

    cyw43_wifi_scan_start();
    return 0;
}

#elif defined(TARGET_HAS_WIFI_ISM43362)

// The ES-WIFI module has its own onboard TCP/IP stack and is only ever driven through its
// socket-oriented AT command set (see targets/ChibiOS/_WiFi/inventek), so there's no lwIP netif
// backing this interface - IP configuration is read directly from the module and copied into the
// in-memory config block, and connection lifecycle maps straight onto the WIFI_* driver API.

// implemented in targets/ChibiOS/_WiFi/inventek/sntp_ism43362.cpp
extern "C" void Ism43362_Sntp_TriggerAutoSync();

// Guards against spawning more than one auto-sync thread for the same connection - set once
// Network_Interface_Connect_Result() has confirmed a real IP address, cleared again on disconnect
// so a future reconnect can trigger a fresh sync.
static bool s_sntpAutoSyncTriggered = false;

int Network_Interface_Disconnect(int index)
{
    (void)index;

    s_sntpAutoSyncTriggered = false;

    return WIFI_Disconnect() == WIFI_STATUS_OK ? 0 : -1;
}

int Network_Interface_Start_Connect(int index, const char *ssid, const char *passphrase, int options)
{
    (void)index;
    (void)options;

    WIFI_Ecn_t ecn = (passphrase != NULL && passphrase[0] != '\0') ? WIFI_ECN_WPA2_PSK : WIFI_ECN_OPEN;

    ISM43362_TRACE("[ISM43362] Start_Connect: ssid='%s', ecn=%d - calling WIFI_Connect()...\r\n", ssid, (int)ecn);

    WIFI_Status_t status = WIFI_Connect(ssid, passphrase, ecn);

    ISM43362_TRACE("[ISM43362] Start_Connect: WIFI_Connect() returned %d\r\n", (int)status);

    // NOTE: the automatic NTP sync is NOT triggered here anymore, even though WIFI_Connect()
    // returning OK usually means the module's own onboard DHCP client already has an IP address.
    // Kicking off the SNTP background thread (which drives the SAME ES-WIFI module over the SAME
    // AT command channel/mutex for DNS lookup + UDP socket I/O) this early was found to race with
    // the still-settling post-join status-polling window, so it's deferred to
    // Network_Interface_Connect_Result() instead, once a real IP address has been confirmed - see
    // the comment there.

    return status == WIFI_STATUS_OK ? 0 : -1;
}

int Network_Interface_Connect_Result(int configIndex)
{
    (void)configIndex;

    if (WIFI_IsConnected() != WIFI_STATUS_OK)
    {
        return -1;
    }

    uint8_t ipAddr[4];
    uint8_t ipMask[4];
    uint8_t gatewayAddr[4];

    if (WIFI_GetIP_Address(ipAddr) != WIFI_STATUS_OK)
    {
        return -1;
    }

    // DHCP has completed (the module runs its own DHCP client internally) - sync the reported
    // IP configuration into the in-memory config block so that managed code reads the correct
    // addresses via ConfigurationManager_GetConfigurationBlock.
    if (g_TargetConfiguration.NetworkInterfaceConfigs != NULL &&
        g_TargetConfiguration.NetworkInterfaceConfigs->Count > 0)
    {
        HAL_Configuration_NetworkInterface *cfg = g_TargetConfiguration.NetworkInterfaceConfigs->Configs[0];

        cfg->IPv4Address = LWIP_MAKEU32(ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);

        if (WIFI_GetIP_Mask(ipMask) == WIFI_STATUS_OK)
        {
            cfg->IPv4NetMask = LWIP_MAKEU32(ipMask[0], ipMask[1], ipMask[2], ipMask[3]);
        }

        if (WIFI_GetGateway_Address(gatewayAddr) == WIFI_STATUS_OK)
        {
            cfg->IPv4GatewayAddress = LWIP_MAKEU32(gatewayAddr[0], gatewayAddr[1], gatewayAddr[2], gatewayAddr[3]);
        }
    }

    if (!s_sntpAutoSyncTriggered)
    {
        s_sntpAutoSyncTriggered = true;

        ISM43362_TRACE("[ISM43362] Connect_Result: about to call Ism43362_Sntp_TriggerAutoSync()\r\n");

        // only kick off the best-effort automatic NTP sync now that a real IP address has been
        // confirmed (matching how other targets auto-sync the clock once the network comes up -
        // see the comment at the top of sntp_ism43362.cpp for why this is needed on this board
        // specifically) - deferring it to here (instead of right after WIFI_Connect() returns)
        // avoids racing the background SNTP thread against the connection-establishment window.
        Ism43362_Sntp_TriggerAutoSync();

        ISM43362_TRACE("[ISM43362] Connect_Result: Ism43362_Sntp_TriggerAutoSync() returned\r\n");
    }

    return 0;
}

int Network_Interface_Start_Scan(int index)
{
    (void)index;

    // handled synchronously by WIFI_ListAccessPoints(), called directly from the managed API layer
    return 0;
}

#endif
