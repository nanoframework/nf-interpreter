//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2001-2004 Swedish Institute of Computer Science.  All rights reserved.
// Portions Copyright (c) 2006..2015 Giovanni Di Sirio. All rights reserved.
// See LICENSE file in the project root for full license information.
//

// WiFi-aware lwIP thread for ChibiOS + CYW43.
// This replaces nf_lwipthread.c, using the CYW43 WiFi chip instead of
// ChibiOS ETHD1 (Ethernet MAC). The lwIP stack is reused as-is.

#include "hal.h"
#include "evtimer.h"
#include "nf_lwipthread_wifi.h"
#include "cyw43_configport.h"
#include "cyw43.h"
#include "cyw43_ll.h"
#include "cyw43_internal.h"
#include <nanoHAL_ConfigurationManager.h>

// CLR event system — fire SYSTEM_EVENT_FLAG_WIFI_STATION on link change
extern void Events_Set(uint32_t events);
#define SYSTEM_EVENT_FLAG_WIFI_STATION 0x01000000

#include <lwip/opt.h>
#include <lwip/def.h>
#include <lwip/mem.h>
#include <lwip/pbuf.h>
#include <lwip/sys.h>
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include <lwip/tcpip.h>
#include <netif/etharp.h>
#include <lwip/netifapi.h>
#include <lwip/apps/sntp.h>

// Default SNTP server addresses (matches lwip_sntp_default_options.h)
#ifndef SNTP_SERVER0_DEFAULT_ADDRESS
#define SNTP_SERVER0_DEFAULT_ADDRESS "0.pool.ntp.org"
#endif
#ifndef SNTP_SERVER1_DEFAULT_ADDRESS
#define SNTP_SERVER1_DEFAULT_ADDRESS "1.pool.ntp.org"
#endif

#if LWIP_DHCP
#include <lwip/dhcp.h>
#endif

#if LWIP_AUTOIP
#include <lwip/autoip.h>
#endif

#define PERIODIC_TIMER_ID 1
#define CYW43_POLL_ID     2

// Forward declarations for CYW43 driver functions (from cyw43_ll / cyw43_lwip)
extern void cyw43_arch_init(void);
extern void cyw43_arch_deinit(void);
extern int cyw43_is_wifi_connected(void);

// Suspension point for initialization
thread_reference_t lwip_trp = NULL;

// Stack area for the WiFi lwIP thread
static THD_WORKING_AREA(wa_lwip_thread, LWIP_THREAD_STACK_SIZE + 512);

// Single global netif for WiFi
static struct netif thisif = {0};
static net_addr_mode_t addressMode;
static ip4_addr_t ip, gateway, netmask;

// ---------------------------------------------------------------------------
// Scan results storage
// ---------------------------------------------------------------------------
#define MAX_SCAN_RESULTS 32

static wifi_scan_record_t scan_results[MAX_SCAN_RESULTS];
static volatile int scan_result_count = 0;
static volatile int scan_complete = 0;

// Callback invoked by cyw43_wifi_scan for each AP found
static int wifi_scan_result_cb(void *env, const cyw43_ev_scan_result_t *result)
{
    (void)env;

    if (result == NULL)
    {
        return 0;
    }
    // Deduplicate: if an AP with the same SSID and BSSID already exists,
    // update it only if the new result has a stronger (less negative) RSSI.
    uint8_t ssid_len = result->ssid_len < 32 ? result->ssid_len : 32;
    for (int i = 0; i < scan_result_count; i++)
    {
        if (memcmp(scan_results[i].bssid, result->bssid, 6) == 0 &&
            memcmp(scan_results[i].ssid, result->ssid, ssid_len) == 0 &&
            scan_results[i].ssid[ssid_len] == 0)
        {
            // Duplicate found — keep the stronger signal
            if (result->rssi > scan_results[i].rssi)
            {
                scan_results[i].rssi = result->rssi;
                scan_results[i].auth_mode = result->auth_mode;
                scan_results[i].channel = result->channel;
            }
            return 0;
        }
    }

    if (scan_result_count >= MAX_SCAN_RESULTS)
    {
        return 0; // buffer full, ignore
    }

    wifi_scan_record_t *rec = &scan_results[scan_result_count];
    memcpy(rec->bssid, result->bssid, 6);
    memset(rec->ssid, 0, sizeof(rec->ssid));

    memcpy(rec->ssid, result->ssid, ssid_len);
    rec->rssi = result->rssi;
    rec->auth_mode = result->auth_mode;
    rec->channel = result->channel;
    scan_result_count++;

    return 0;
}

struct netif *nf_getNetif(void)
{
    return &thisif;
}

// ---------------------------------------------------------------------------
// WiFi netif low-level functions
// ---------------------------------------------------------------------------

// Called to initialize the WiFi network interface
// Matches SDK cyw43_netif_init — sets flags, MAC, MTU.
static void wifi_low_level_init(struct netif *netif)
{
    // Set MAC hardware address from the CYW43 chip (read during firmware init).
    // Must be done here (inside netif_add callback) because netif_add may
    // zero the struct before calling the init function.
    cyw43_wifi_get_mac(&cyw43_state, CYW43_ITF_STA, netif->hwaddr);
    netif->hwaddr_len = ETHARP_HWADDR_LEN;

    // Maximum transfer unit for WiFi
    netif->mtu = 1500;

    // Flags match the SDK exactly: BROADCAST | ETHARP | ETHERNET | IGMP
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP;
}

static err_t wifi_low_level_output(struct netif *netif, struct pbuf *p)
{
    (void)netif;

    // Linearize the pbuf chain into a contiguous buffer for CYW43
    uint8_t buf[1536]; // Max Ethernet frame
    uint16_t offset = 0;
    struct pbuf *q;

    for (q = p; q != NULL; q = q->next)
    {
        if (offset + q->len > sizeof(buf))
        {
            return ERR_BUF;
        }
        memcpy(buf + offset, q->payload, q->len);
        offset += q->len;
    }

    // Send the Ethernet frame to CYW43 for transmission
    // CYW43 driver will handle 802.11 encapsulation
    CYW43_THREAD_ENTER;

    // Use the CYW43 bus to send the frame
    cyw43_t *state = &cyw43_state;
    int ret = cyw43_send_ethernet(state, CYW43_ITF_STA, offset, buf, false);
    CYW43_THREAD_EXIT;
    if (ret != 0)
    {
        return ERR_IF;
    }

    MIB2_STATS_NETIF_ADD(netif, ifoutoctets, p->tot_len);
    LINK_STATS_INC(link.xmit);

    return ERR_OK;
}

// Called by the CYW43 driver when an Ethernet frame is received from WiFi.
// This is a callback invoked from cyw43_cb_process_ethernet().
void cyw43_cb_process_ethernet(void *self, int itf, size_t len, const uint8_t *buf)
{
    (void)self;
    (void)itf;

    if (len == 0 || buf == NULL)
        return;

    // Allocate a pbuf and copy the frame data
    struct pbuf *p = pbuf_alloc(PBUF_RAW, (u16_t)len, PBUF_POOL);
    if (p == NULL)
    {
        LINK_STATS_INC(link.memerr);
        LINK_STATS_INC(link.drop);
        return;
    }

    // Copy frame data into pbuf chain
    struct pbuf *q;
    uint16_t offset = 0;
    for (q = p; q != NULL; q = q->next)
    {
        uint16_t copy_len = q->len;
        if (offset + copy_len > len)
        {
            copy_len = (uint16_t)(len - offset);
        }
        memcpy(q->payload, buf + offset, copy_len);
        offset += copy_len;
    }

    MIB2_STATS_NETIF_ADD(&thisif, ifinoctets, p->tot_len);
    LINK_STATS_INC(link.recv);

    // Pass to tcpip_input for processing in the tcpip thread
    if (thisif.input(p, &thisif) != ERR_OK)
    {
        pbuf_free(p);
    }
}

// ---------------------------------------------------------------------------
// Network interface init (passed to netifapi_netif_add)
// ---------------------------------------------------------------------------

static err_t wifiif_init(struct netif *netif)
{
    osalDbgAssert((netif != NULL), "netif != NULL");

    MIB2_INIT_NETIF(netif, snmp_ifType_ieee80211, 54000000); // 54 Mbps WiFi

    netif->state = NULL;
    netif->name[0] = 'w';
    netif->name[1] = 'l';
    netif->output = etharp_output;
    netif->linkoutput = wifi_low_level_output;

    wifi_low_level_init(netif);

    return ERR_OK;
}

// ---------------------------------------------------------------------------
// lwIP callbacks for CYW43
// ---------------------------------------------------------------------------

// Netif status callback — fires when IP address changes (e.g., DHCP completes).
// Signal the CLR so NativeConnect's WaitEvents loop wakes up, and start SNTP.
static void wifi_netif_status_cb(struct netif *nif)
{
    Events_Set(SYSTEM_EVENT_FLAG_WIFI_STATION);

#if SNTP_SERVER_DNS
    // Start SNTP once we have an IP address
    if (nif->ip_addr.addr != 0)
    {
        sntp_stop();
        sntp_setoperatingmode(SNTP_OPMODE_POLL);
        sntp_setservername(0, SNTP_SERVER0_DEFAULT_ADDRESS);
        sntp_setservername(1, SNTP_SERVER1_DEFAULT_ADDRESS);
        sntp_init();
    }
#endif
}

// Callback scheduled on tcpip thread from cyw43_cb_tcpip_init.
// At this point the CYW43 firmware is loaded and cyw43_state.mac is valid.
static void wifi_tcpip_init_done(void *arg)
{
    struct netif *ifc = (struct netif *)arg;

    // Copy the real OTP MAC from the CYW43 chip to the netif
    cyw43_wifi_get_mac(&cyw43_state, CYW43_ITF_STA, ifc->hwaddr);

    // Update the in-memory network config block so managed code reads
    // the real hardware MAC (the stored default is all zeros).
    if (g_TargetConfiguration.NetworkInterfaceConfigs != NULL &&
        g_TargetConfiguration.NetworkInterfaceConfigs->Count > 0)
    {
        memcpy(
            g_TargetConfiguration.NetworkInterfaceConfigs->Configs[0]->MacAddress,
            ifc->hwaddr,
            NETIF_MAX_HWADDR_LEN);
    }

#if SNTP_SERVER_DNS
    // Pre-configure SNTP server names early, before the CLR's
    // LWIP_SOCKETS_Driver replaces our netif status callback.
    // Server names persist across sntp_stop()/sntp_init() cycles,
    // so when the CLR callback later calls sntp_init(), it will
    // use these servers.
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, SNTP_SERVER0_DEFAULT_ADDRESS);
    sntp_setservername(1, SNTP_SERVER1_DEFAULT_ADDRESS);
#endif

#if LWIP_DHCP
    // Start DHCP — matches SDK flow (dhcp_start in cyw43_cb_tcpip_init).
    // DHCP DISCOVER will be sent immediately, but won't reach the AP until
    // the WPA handshake completes.  When netif_set_link_up fires,
    // dhcp_network_changed() restarts DHCP discovery automatically.
    dhcp_start(ifc);
#endif
}

void cyw43_cb_tcpip_init(cyw43_t *self, int itf)
{
    (void)self;
    (void)itf;
    // CYW43 firmware is loaded — schedule MAC copy + DHCP start on tcpip thread.
    // Can't call lwIP APIs directly here (CYW43 mutex is held).
    tcpip_callback_with_block(wifi_tcpip_init_done, &thisif, 0);
}

void cyw43_cb_tcpip_deinit(cyw43_t *self, int itf)
{
    (void)self;
    (void)itf;
}

// Forward declaration — defined below
void lwipDefaultLinkUpCB(void *p);

// Callback queued to tcpip thread for non-blocking link-up.
// Matches SDK: cyw43_cb_tcpip_set_link_up just calls netif_set_link_up.
// lwIP internally fires dhcp_network_changed() which restarts DHCP
// discovery if it is already running.
// Only called from cyw43_cb_tcpip_set_link_up (after WIFI_JOIN_STATE_ALL).
static void wifi_link_up_and_dhcp(void *arg)
{
    struct netif *ifc = (struct netif *)arg;

    // Refresh the chip's real MAC on the netif (should already be set from
    // wifiif_init, but re-copy in case of reconnect after MAC change).
    cyw43_wifi_get_mac(&cyw43_state, CYW43_ITF_STA, ifc->hwaddr);

    // Set link up flag — lwIP fires dhcp_network_changed() automatically
    // which restarts DHCP discovery.  This matches the Pico SDK exactly.
    netif_set_link_up(ifc);
}

void cyw43_cb_tcpip_set_link_up(cyw43_t *self, int itf)
{
    (void)self;
    (void)itf;

    // Queue link-up to the tcpip thread (NON-BLOCKING).
    // Matches SDK: link-up fires when WIFI_JOIN_STATE_ALL
    // (auth + link + keyed) is reached.  DHCP restart is handled
    // by lwIP internally via dhcp_network_changed().
    tcpip_callback_with_block(wifi_link_up_and_dhcp, &thisif, 0);

    // Signal the CLR that WiFi station state changed (link up / connect complete)
    Events_Set(SYSTEM_EVENT_FLAG_WIFI_STATION);
}

// Callback queued to tcpip thread for non-blocking link-down + DHCP stop
static void wifi_link_down_and_dhcp_stop(void *arg)
{
    struct netif *ifc = (struct netif *)arg;

    netif_set_link_down(ifc);

#if LWIP_DHCP
    // Stop DHCP, then immediately re-start so that dhcp_network_changed()
    // (fired by the next netif_set_link_up) has an active DHCP session
    // to restart discovery on.  This matches the Pico SDK where DHCP is
    // started once during init and left running forever.
    dhcp_release_and_stop(ifc);
    dhcp_start(ifc);
#endif
}

void cyw43_cb_tcpip_set_link_down(cyw43_t *self, int itf)
{
    (void)self;
    (void)itf;

    // Non-blocking — same reasoning as cyw43_cb_tcpip_set_link_up
    tcpip_callback_with_block(wifi_link_down_and_dhcp_stop, &thisif, 0);

    // Signal the CLR that WiFi station state changed (link down / disconnect)
    Events_Set(SYSTEM_EVENT_FLAG_WIFI_STATION);
}

// ---------------------------------------------------------------------------
// Link up/down callbacks (same as nf_lwipthread.c)
// ---------------------------------------------------------------------------

void lwipDefaultLinkUpCB(void *p)
{
    struct netif *ifc = (struct netif *)p;

#if LWIP_AUTOIP
    if (addressMode == NET_ADDRESS_AUTO)
    {
        autoip_start(ifc);
    }
#endif

#if LWIP_DHCP
    if (addressMode == NET_ADDRESS_DHCP)
    {
        dhcp_start(ifc);
    }
#endif

#if SNTP_SERVER_DNS
    if (addressMode != NET_ADDRESS_DHCP)
    {
        sntp_init();
    }
#endif
}

void lwipDefaultLinkDownCB(void *p)
{
    struct netif *ifc = (struct netif *)p;

#if LWIP_AUTOIP
    if (addressMode == NET_ADDRESS_AUTO)
    {
        autoip_stop(ifc);
    }
#endif

#if LWIP_DHCP
    if (addressMode == NET_ADDRESS_DHCP)
    {
        dhcp_release_and_stop(ifc);
    }
#endif

#if SNTP_SERVER_DNS
    sntp_stop();
#endif
}

// ---------------------------------------------------------------------------
// WiFi lwIP thread
// ---------------------------------------------------------------------------

static THD_FUNCTION(lwip_thread, p)
{
    event_timer_t evt;
    event_listener_t el0;
    err_t result;
    tcpip_callback_fn link_up_cb = NULL;
    tcpip_callback_fn link_down_cb = NULL;
    bool prev_link_status = false;

    chRegSetThreadName("lwip_wifi");

    // TCP/IP parameters from configuration
    if (p)
    {
        lwipthread_opts_t *opts = p;
        unsigned i;

        for (i = 0; i < 6; i++)
        {
            thisif.hwaddr[i] = opts->macaddress[i];
        }

        ip.addr = opts->address;
        gateway.addr = opts->gateway;
        netmask.addr = opts->netmask;
        addressMode = opts->addrMode;

#if LWIP_NETIF_HOSTNAME
        thisif.hostname = opts->ourHostName;
#endif

        link_up_cb = opts->link_up_cb;
        link_down_cb = opts->link_down_cb;
    }
    else
    {
        thisif.hwaddr[0] = LWIP_ETHADDR_0;
        thisif.hwaddr[1] = LWIP_ETHADDR_1;
        thisif.hwaddr[2] = LWIP_ETHADDR_2;
        thisif.hwaddr[3] = LWIP_ETHADDR_3;
        thisif.hwaddr[4] = LWIP_ETHADDR_4;
        thisif.hwaddr[5] = LWIP_ETHADDR_5;

        LWIP_IPADDR(&ip);
        LWIP_GATEWAY(&gateway);
        LWIP_NETMASK(&netmask);
    }

#if LWIP_NETIF_HOSTNAME
    if (thisif.hostname == NULL)
    {
        thisif.hostname = LWIP_NETIF_HOSTNAME_STRING;
    }
#endif

    if (!link_up_cb)
    {
        link_up_cb = lwipDefaultLinkUpCB;
    }

    if (!link_down_cb)
    {
        link_down_cb = lwipDefaultLinkDownCB;
    }

    // Initialize CYW43 architecture layer (powers on the chip, loads firmware)
    cyw43_arch_init();

    // Add the WiFi network interface
    result = netifapi_netif_add(&thisif, &ip, &netmask, &gateway, NULL, wifiif_init, tcpip_input);
    if (result != ERR_OK)
    {
        chThdSleepMilliseconds(1000);
        osalSysHalt("wifiif_add error");
    }

    netifapi_netif_set_default(&thisif);
    netifapi_netif_set_up(&thisif);

    // NOTE: DHCP is NOT started here — the netif MAC is still a placeholder
    // because the CYW43 firmware hasn't been loaded yet (lazy init).
    // DHCP is started later from cyw43_cb_tcpip_init, which fires during
    // cyw43_wifi_set_up (first ensure_wifi_up call), when the real OTP MAC
    // is available.  This matches the Pico SDK flow.

    // Register status callback so that DHCP IP assignment fires Event_Wifi_Station,
    // waking NativeConnect's WaitEvents loop.
    netif_set_status_callback(&thisif, wifi_netif_status_cb);

    // Set up periodic timer for link status polling
    evtObjectInit(&evt, LWIP_LINK_POLL_INTERVAL);
    evtStart(&evt);
    chEvtRegisterMask(&evt.et_es, &el0, PERIODIC_TIMER_ID);
    chEvtAddEvents(PERIODIC_TIMER_ID);

    // Resume the caller (lwIPInit is waiting)
    chThdResume(&lwip_trp, MSG_OK);
    chThdSetPriority(LWIP_THREAD_PRIORITY);

    // Setup SNTP — only after WiFi is connected and DHCP has IP.
    // SNTP is started from wifi_netif_status_cb when IP becomes non-zero.
    // Starting it here at boot causes ARP/DNS probes on a non-connected interface.

    // Main event loop
    while (true)
    {
        eventmask_t mask = chEvtWaitAny(ALL_EVENTS);

        if (mask & PERIODIC_TIMER_ID)
        {
            // Check WiFi link status (CYW43 connection state).
            // IMPORTANT: Only handle link-DOWN here.  Link-UP is handled
            // exclusively by the driver's cyw43_cb_tcpip_set_link_up callback,
            // which fires when WIFI_JOIN_STATE_ALL (auth+link+keyed) is reached.
            // The Pico SDK does NOT have a periodic link-up check.  Starting
            // DHCP before the WPA 4-way handshake completes sends unencrypted
            // frames that the AP drops, poisoning the DHCP server state.
            bool current_link_status = (cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA) == CYW43_LINK_JOIN);

            if (prev_link_status && !current_link_status)
            {
                prev_link_status = false;
                tcpip_callback_with_block(wifi_link_down_and_dhcp_stop, &thisif, 0);
            }
            else if (current_link_status)
            {
                prev_link_status = true;
            }
        }
    }
}

// ---------------------------------------------------------------------------
// tcpip init completion callback
// ---------------------------------------------------------------------------

static void tcpIpInitDone(void *arg)
{
    semaphore_t *initDone = arg;
    chSemSignal(initDone);
}

// ---------------------------------------------------------------------------
// lwIPInit — same API as nf_lwipthread.c
// ---------------------------------------------------------------------------

void lwIPInit(const lwipthread_opts_t *opts)
{
    // Create the WiFi lwIP thread
    chThdCreateStatic(wa_lwip_thread, sizeof(wa_lwip_thread), chThdGetPriorityX() - 1, lwip_thread, (void *)opts);

    // Create semaphore for TCP/IP init completion
    semaphore_t initDone;
    chSemObjectInit(&initDone, 0);

    // Initialize TCP/IP stack
    tcpip_init(tcpIpInitDone, &initDone);

    // Wait for lwIP thread initialization
    chSysLock();
    chThdSuspendS(&lwip_trp);
    chSysUnlock();

    // Wait for TCP/IP stack init
    chSemWait(&initDone);
}

// ---------------------------------------------------------------------------
// lwipReconfigure — same API as nf_lwipthread.c
// ---------------------------------------------------------------------------

typedef struct lwip_reconf_params
{
    const lwipreconf_opts_t *opts;
    semaphore_t completion;
} lwip_reconf_params_t;

static void do_reconfigure(void *p)
{
    lwip_reconf_params_t *reconf = (lwip_reconf_params_t *)p;

    switch (addressMode)
    {
#if LWIP_DHCP
        case NET_ADDRESS_DHCP:
            if (netif_is_up(&thisif))
            {
                dhcp_release_and_stop(&thisif);
            }
            break;
#endif
        case NET_ADDRESS_STATIC:
        {
            ip4_addr_t zero = {0};
            netif_set_ipaddr(&thisif, &zero);
            netif_set_netmask(&thisif, &zero);
            netif_set_gw(&thisif, &zero);
            break;
        }
#if LWIP_AUTOIP
        case NET_ADDRESS_AUTO:
            if (netif_is_up(&thisif))
                autoip_stop(&thisif);
            break;
#endif
    }

    ip.addr = reconf->opts->address;
    gateway.addr = reconf->opts->gateway;
    netmask.addr = reconf->opts->netmask;
    addressMode = reconf->opts->addrMode;

    switch (addressMode)
    {
#if LWIP_DHCP
        case NET_ADDRESS_DHCP:
            dhcp_start(&thisif);
            break;
#endif
        case NET_ADDRESS_STATIC:
            netif_set_ipaddr(&thisif, &ip);
            netif_set_netmask(&thisif, &netmask);
            netif_set_gw(&thisif, &gateway);
            break;
#if LWIP_AUTOIP
        case NET_ADDRESS_AUTO:
            if (netif_is_up(&thisif))
                autoip_start(&thisif);
            break;
#endif
    }

    chSemSignal(&reconf->completion);
}

void lwipReconfigure(const lwipreconf_opts_t *opts)
{
    lwip_reconf_params_t params;
    params.opts = opts;
    chSemObjectInit(&params.completion, 0);
    tcpip_callback_with_block(do_reconfigure, &params, 0);
    chSemWait(&params.completion);
}

// ---------------------------------------------------------------------------
// WiFi-specific helper functions
// ---------------------------------------------------------------------------

static volatile bool wifi_powered = false;

void cyw43_wifi_power_on(void)
{
    if (!wifi_powered)
    {
        // CYW43 is initialized in the lwip_thread via cyw43_arch_init()
        wifi_powered = true;
    }
}

void cyw43_wifi_power_off(void)
{
    if (wifi_powered)
    {
        cyw43_arch_deinit();
        wifi_powered = false;
    }
}

// WiFi connect — called from Target_Network.cpp
int cyw43_wifi_connect(const char *ssid, const char *password, uint32_t auth_type)
{
    (void)auth_type;

    if (ssid == NULL)
        return -1;

    // Ensure WiFi hardware is initialized
    if (cyw43_ensure_wifi_up_impl() != 0)
    {
        return -1;
    }

    cyw43_t *state = &cyw43_state;

    size_t ssid_len = strlen(ssid);
    size_t key_len = password ? strlen(password) : 0;

    // WPA2_AES_PSK = 0x00400004
    uint32_t auth = key_len > 0 ? 0x00400004 : 0; // Open or WPA2

    // Clean disconnect first — ensures AP doesn't see stale association state
    // and prevents BADAUTH on rapid re-join.  EV_DISASSOC will fire link_down
    // callback to stop any running DHCP.
    CYW43_THREAD_ENTER;
    cyw43_wifi_leave(state, CYW43_ITF_STA);
    CYW43_THREAD_EXIT;
    chThdSleepMilliseconds(100);

    // Disable power management during connect — PM2 may cause the radio
    // to sleep and miss the DHCP OFFER response from the AP.
    cyw43_wifi_pm(state, CYW43_NONE_PM);

    CYW43_THREAD_ENTER;
    int result = cyw43_wifi_join(state, ssid_len, (const uint8_t *)ssid, key_len, (const uint8_t *)password, auth, NULL,
                                 0);
    CYW43_THREAD_EXIT;

    // After successful join, issue an ioctl to kick inline packet processing.
    // The ioctl's inline polling may process pending DATA_HEADER packets
    // (e.g., DHCP OFFER) that arrived during the join process.
    if (result == 0)
    {
        CYW43_THREAD_ENTER;
        uint8_t rssi_buf[4] = {0};
        cyw43_ll_ioctl(&state->cyw43_ll, 127 << 1, 4, rssi_buf, 0);
        CYW43_THREAD_EXIT;
    }

    return result;
}

int cyw43_wifi_disconnect(void)
{
    cyw43_t *state = &cyw43_state;

    CYW43_THREAD_ENTER;
    int result = cyw43_wifi_leave(state, CYW43_ITF_STA);
    CYW43_THREAD_EXIT;

    return result;
}

// ---------------------------------------------------------------------------
// WiFi scan
// ---------------------------------------------------------------------------

int cyw43_wifi_scan_start(void)
{
    // Ensure WiFi hardware is initialized
    if (cyw43_ensure_wifi_up_impl() != 0)
    {
        return -1;
    }

    cyw43_t *state = &cyw43_state;

    // Reset scan results
    scan_result_count = 0;
    scan_complete = 0;

    cyw43_wifi_scan_options_t scan_opts = {0};
    CYW43_THREAD_ENTER;
    int result = cyw43_wifi_scan(state, &scan_opts, NULL, wifi_scan_result_cb);
    CYW43_THREAD_EXIT;

    if (result != 0)
    {
        return result;
    }

    // Wait for scan to complete (wifi_scan_state goes from 1 to 2)
    // Timeout after 10 seconds
    int timeout_ms = 10000;
    int elapsed = 0;

    while (elapsed < timeout_ms)
    {
        CYW43_THREAD_ENTER;
        int scanning = cyw43_wifi_scan_active(state);
        CYW43_THREAD_EXIT;

        if (!scanning)
        {
            scan_complete = 1;
            break;
        }

        chThdSleepMilliseconds(100);
        elapsed += 100;
    }

    scan_complete = 1;
    return 0;
}

int cyw43_wifi_scan_get_count(void)
{
    return scan_result_count;
}

const wifi_scan_record_t *cyw43_wifi_scan_get_results(void)
{
    return scan_results;
}

int cyw43_wifi_is_connected(void)
{
    return cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA) == CYW43_LINK_JOIN;
}

int cyw43_wifi_get_link_status(void)
{
    return cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA);
}

int cyw43_wifi_get_join_state(void)
{
    return (int)cyw43_state.wifi_join_state;
}

void cyw43_wifi_get_mac_addr(uint8_t *mac)
{
    // Copy from the netif (set during initialization)
    memcpy(mac, thisif.hwaddr, 6);
}

void cyw43_wifi_set_netif_mac(const uint8_t *mac)
{
    memcpy(thisif.hwaddr, mac, 6);
}

uint32_t cyw43_wifi_get_ip4_address(void)
{
    return thisif.ip_addr.addr;
}

// Direct poll from NativeConnect — calls cyw43_poll to drain SPI packets.
uint32_t cyw43_wifi_direct_poll(void)
{
    CYW43_THREAD_ENTER;

    if (cyw43_poll)
        cyw43_poll();

    CYW43_THREAD_EXIT;

    return 0;
}
