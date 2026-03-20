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

struct netif *nf_getNetif(void)
{
    return &thisif;
}

// ---------------------------------------------------------------------------
// WiFi netif low-level functions
// ---------------------------------------------------------------------------

// Called to initialize the WiFi network interface
static void wifi_low_level_init(struct netif *netif)
{
    // Set MAC hardware address length
    netif->hwaddr_len = ETHARP_HWADDR_LEN;

    // Maximum transfer unit for WiFi
    netif->mtu = 1500;

    // WiFi device capabilities
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP;
}

// Called by lwIP to transmit a packet over WiFi.
// The packet data is in the pbuf chain.
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

void cyw43_cb_tcpip_init(cyw43_t *self, int itf)
{
    (void)self;
    (void)itf;
    // netif already added in lwip_thread
}

void cyw43_cb_tcpip_deinit(cyw43_t *self, int itf)
{
    (void)self;
    (void)itf;
}

void cyw43_cb_tcpip_set_link_up(cyw43_t *self, int itf)
{
    (void)self;
    (void)itf;
    netifapi_netif_set_link_up(&thisif);
}

void cyw43_cb_tcpip_set_link_down(cyw43_t *self, int itf)
{
    (void)self;
    (void)itf;
    netifapi_netif_set_link_down(&thisif);
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

    // Set up periodic timer for link status polling
    evtObjectInit(&evt, LWIP_LINK_POLL_INTERVAL);
    evtStart(&evt);
    chEvtRegisterMask(&evt.et_es, &el0, PERIODIC_TIMER_ID);
    chEvtAddEvents(PERIODIC_TIMER_ID);

    // Resume the caller (lwIPInit is waiting)
    chThdResume(&lwip_trp, MSG_OK);
    chThdSetPriority(LWIP_THREAD_PRIORITY);

    // Setup SNTP
#if SNTP_SERVER_DNS
    sntp_stop();
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, SNTP_SERVER0_DEFAULT_ADDRESS);
    sntp_setservername(1, SNTP_SERVER1_DEFAULT_ADDRESS);
    sntp_init();
#endif

    // Main event loop
    while (true)
    {
        eventmask_t mask = chEvtWaitAny(ALL_EVENTS);

        if (mask & PERIODIC_TIMER_ID)
        {
            // Check WiFi link status (CYW43 connection state)
            bool current_link_status = (cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA) == CYW43_LINK_JOIN);

            if (current_link_status != prev_link_status)
            {
                prev_link_status = current_link_status;
                if (current_link_status)
                {
                    tcpip_callback_with_block((tcpip_callback_fn)netif_set_link_up, &thisif, 0);
                    tcpip_callback_with_block(link_up_cb, &thisif, 0);
                }
                else
                {
                    tcpip_callback_with_block((tcpip_callback_fn)netif_set_link_down, &thisif, 0);
                    tcpip_callback_with_block(link_down_cb, &thisif, 0);
                }
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

    cyw43_t *state = &cyw43_state;

    size_t ssid_len = strlen(ssid);
    size_t key_len = password ? strlen(password) : 0;

    // WPA2_AES_PSK = 0x00400004
    uint32_t auth = key_len > 0 ? 0x00400004 : 0; // Open or WPA2

    CYW43_THREAD_ENTER;
    int result = cyw43_wifi_join(state, ssid_len, (const uint8_t *)ssid, key_len, (const uint8_t *)password, auth, NULL,
                                 0);
    CYW43_THREAD_EXIT;

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

int cyw43_wifi_scan_start(void)
{
    cyw43_t *state = &cyw43_state;

    CYW43_THREAD_ENTER;
    int result = cyw43_wifi_scan(state, NULL, NULL, NULL);
    CYW43_THREAD_EXIT;

    return result;
}

int cyw43_wifi_is_connected(void)
{
    return cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA) == CYW43_LINK_JOIN;
}

void cyw43_wifi_get_mac_addr(uint8_t *mac)
{
    // Copy from the netif (set during initialization)
    memcpy(mac, thisif.hwaddr, 6);
}
