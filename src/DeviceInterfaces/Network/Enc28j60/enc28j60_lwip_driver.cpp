//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanohal.h>
//#include "net_decl_lwip.h"
#include "enc28j60_lwip.h"

extern "C"
{
#include "netif\etharp.h"
#include "lwip\dhcp.h"
#include "lwip\tcpip.h"
#include "lwip\dns.h"
#include "lwip\ip_addr.h"
}

static ENC28J60_LWIP_Driver g_ENC28J60_LWIP_Driver;
static struct netif g_ENC28J60_NetIF;

HAL_CONTINUATION InterruptTaskContinuation;
HAL_COMPLETION LwipUpTimeCompletion;
static bool LwipNetworkStatus = FALSE;
static uint32_t LwipLastIpAddress = 0;

extern ENC28J60_LWIP_DEVICE_CONFIG g_ENC28J60_LWIP_Config;
extern HAL_TARGET_CONFIGURATION g_TargetConfiguration;

extern bool enc28j60_get_link_status(uint32_t spiHandle);

void enc28j60_status_callback(struct netif *netif)
{
    //    if (LwipLastIpAddress != netif->ip_addr.u_addr.ip4.addr)
    if (LwipLastIpAddress != netif->ip_addr.addr)
    {
        Network_PostEvent(
            NetworkChange_NetworkEventType_AvailabilityChanged,
            NetworkChange_NetworkEvents_NetworkNOTAvailable,
            0);

        //        LwipLastIpAddress = netif->ip_addr.u_addr.ip4.addr;
        LwipLastIpAddress = netif->ip_addr.addr;
    }
    /*
    #if !defined(BUILD_RTM)
        lcd_printf("\f\n\n\n\n\n\nLink Update: %s\n", (netif_is_up(netif) ? "UP  " : "DOWN") );
        lcd_printf("         IP: %d.%d.%d.%d\n", (netif->ip_addr.addr >>  0) & 0xFF,
                                                 (netif->ip_addr.addr >>  8) & 0xFF,
                                                 (netif->ip_addr.addr >> 16) & 0xFF,
                                                 (netif->ip_addr.addr >> 24) & 0xFF);
        lcd_printf("         SM: %d.%d.%d.%d\n", (netif->netmask.addr >>  0) & 0xFF,
                                                 (netif->netmask.addr >>  8) & 0xFF,
                                                 (netif->netmask.addr >> 16) & 0xFF,
                                                 (netif->netmask.addr >> 24) & 0xFF);
        lcd_printf("         GW: %d.%d.%d.%d\n", (netif->gw.addr >>  0) & 0xFF,
                                                 (netif->gw.addr >>  8) & 0xFF,
                                                 (netif->gw.addr >> 16) & 0xFF,
                                                 (netif->gw.addr >> 24) & 0xFF);
        debug_printf("IP Address: %d.%d.%d.%d\n", (netif->ip_addr.addr >>  0) & 0xFF,
                                                 (netif->ip_addr.addr >>  8) & 0xFF,
                                                 (netif->ip_addr.addr >> 16) & 0xFF,
                                                 (netif->ip_addr.addr >> 24) & 0xFF);
    #if LWIP_DNS
        if(netif->flags & NETIF_FLAG_DHCP)
        {
            struct ip_addr dns1 = dns_getserver(0);
            struct ip_addr dns2 = dns_getserver(1);

            lcd_printf("         dns1: %d.%d.%d.%d\n",(dns1.addr >>  0) & 0xFF,
                                                      (dns1.addr >>  8) & 0xFF,
                                                      (dns1.addr >> 16) & 0xFF,
                                                      (dns1.addr >> 24) & 0xFF);

            lcd_printf("         dns2: %d.%d.%d.%d\n",(dns2.addr >>  0) & 0xFF,
                                                      (dns2.addr >>  8) & 0xFF,
                                                      (dns2.addr >> 16) & 0xFF,
                                                      (dns2.addr >> 24) & 0xFF);
        }
    #endif
    #endif
    */
}

err_t enc28j60_ethhw_init(netif *myNetIf)
{
    myNetIf->mtu = ETHERSIZE;

    myNetIf->flags = NETIF_FLAG_IGMP | NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET;

    /* ethhw_init() is user-defined */
    /* use ip_input instead of ethernet_input for non-ethernet hardware */
    /* (this function is assigned to netif.input and should be called by the hardware driver) */

    /* Assign the xmit routine to the stack's netif and call the driver's Open */
    myNetIf->output = etharp_output;
    myNetIf->linkoutput = enc28j60_lwip_xmit;

    // FIXME needs LWIP_NETIF_STATUS_CALLBACK option
    // this is just used for Adress status change events
    // myNetIf->status_callback = enc28j60_status_callback;

    enc28j60_lwip_open(myNetIf);

    return ERR_OK;
}

void lwip_interrupt_continuation()
{
    NATIVE_PROFILE_PAL_NETWORK();
    GLOBAL_LOCK();

    if (!InterruptTaskContinuation.IsLinked())
    {
        InterruptTaskContinuation.Enqueue();
    }

    GLOBAL_UNLOCK();
}

uint64_t Delta64(uint64_t milliSecFromNow)
{
    NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();

    // grab time first to be closest to now as possible from when this function was called
    uint64_t Now = HAL_Time_CurrentSysTicks();
    uint64_t EventTimeTicks = CPU_MillisecondsToTicks(milliSecFromNow);

    return Now + EventTimeTicks;
}

//--//
void lwip_network_uptime_completion(void *arg)
{
    NATIVE_PROFILE_PAL_NETWORK();

    struct netif *pNetIf = (struct netif *)arg;

    bool status = enc28j60_get_link_status(g_ENC28J60_LWIP_Config.DeviceConfigs[0].spiHandle);

    if (status != LwipNetworkStatus)
    {
        if (status)
        {
            tcpip_callback((sys_timeout_handler)netif_set_link_up, (void *)pNetIf);
            tcpip_callback((sys_timeout_handler)netif_set_up, (void *)pNetIf);

            Network_PostEvent(
                NetworkChange_NetworkEventType_AvailabilityChanged,
                NetworkChange_NetworkEvents_NetworkAvailable,
                0);
        }
        else
        {
            tcpip_callback((sys_timeout_handler)netif_set_link_down, (void *)pNetIf);
            tcpip_callback((sys_timeout_handler)netif_set_down, (void *)pNetIf);

            Network_PostEvent(
                NetworkChange_NetworkEventType_AvailabilityChanged,
                NetworkChange_NetworkEvents_NetworkNOTAvailable,
                0);
        }

        Events_Set(SYSTEM_EVENT_FLAG_SOCKET);
        Events_Set(SYSTEM_EVENT_FLAG_NETWORK);

        LwipNetworkStatus = status;
    }

    LwipUpTimeCompletion.EnqueueTicks(Delta64(2000)); // 2 secs
}

void InitContinuations(struct netif *pNetIf)
{
    InterruptTaskContinuation.InitializeCallback((HAL_CALLBACK_FPN)enc28j60_lwip_interrupt, &g_ENC28J60_NetIF);

    LwipUpTimeCompletion.InitializeForUserMode((HAL_CALLBACK_FPN)lwip_network_uptime_completion, pNetIf);

    LwipUpTimeCompletion.EnqueueTicks(Delta64(500));
}

bool enc28j60_Network_Interface_Bind(int index, HAL_Configuration_NetworkInterface *pConfig)
{
    NATIVE_PROFILE_HAL_DRIVERS_ETHERNET();
    if (pConfig->SpecificConfigId >= ARRAYSIZE(g_ENC28J60_LWIP_Config.DeviceConfigs))
        return false;

    return g_ENC28J60_LWIP_Driver.Bind(&g_ENC28J60_LWIP_Config.DeviceConfigs[pConfig->SpecificConfigId], index);
}

int enc28j60_Network_Interface_Open(int index, HAL_Configuration_NetworkInterface *pConfig)
{
    NATIVE_PROFILE_HAL_DRIVERS_ETHERNET();

    if (pConfig->SpecificConfigId >= ARRAYSIZE(g_ENC28J60_LWIP_Config.DeviceConfigs))
        return -1;

    // FIXME
    // HAL_CONFIG_BLOCK::ApplyConfig( ENC28J60_LWIP_DEVICE_CONFIG::GetDriverName(), &g_ENC28J60_LWIP_Config,
    // sizeof(g_ENC28J60_LWIP_Config) );

    return g_ENC28J60_LWIP_Driver.Open(&g_ENC28J60_LWIP_Config.DeviceConfigs[pConfig->SpecificConfigId], index);
}
bool enc28j60_Network_Interface_Close(int index, HAL_Configuration_NetworkInterface *pConfig)
{
    NATIVE_PROFILE_HAL_DRIVERS_ETHERNET();
    if (pConfig->SpecificConfigId >= ARRAYSIZE(g_ENC28J60_LWIP_Config.DeviceConfigs))
        return FALSE;

    return g_ENC28J60_LWIP_Driver.Close(&g_ENC28J60_LWIP_Config.DeviceConfigs[pConfig->SpecificConfigId], index);
}

int ENC28J60_LWIP_Driver::Open(ENC28J60_LWIP_DRIVER_CONFIG *config, int index)
{
    NATIVE_PROFILE_HAL_DRIVERS_ETHERNET();

    /* Network interface variables */
    ip4_addr_t ipaddr, netmask, gw;
    struct netif *pNetIF;
    int len;
    HAL_Configuration_NetworkInterface *iface;

    if (config == NULL)
        return -1;
    iface = g_TargetConfiguration.NetworkInterfaceConfigs->Configs[index];
    ;

    // Open SPI device
    HRESULT hr = nanoSPI_OpenDevice(config->SPI_Config, config->spiHandle);
    if (hr < 0)
        return -1;

    if (0 == (iface->StartupAddressMode & AddressMode_DHCP))
    {
        // Static address
        ipaddr.addr = iface->IPv4Address;
        gw.addr = iface->IPv4GatewayAddress;
        netmask.addr = iface->IPv4NetMask;
    }
    else
    {
        /* Set network address variables - this will be set by either DHCP or when the configuration is applied */
        IP4_ADDR(&gw, 0, 0, 0, 0);
        IP4_ADDR(&ipaddr, 0, 0, 0, 0);
        IP4_ADDR(&netmask, 255, 255, 255, 0);
    }

    // len = g_ENC28J60_NetIF.hwaddr_len;
    len = sizeof(iface->MacAddress);

    // FIXME , we don't have a length field
    // if(len == 0 || iface->macAddressLen < len)
    // {
    //     len = iface->macAddressLen;
    //     g_ENC28J60_NetIF.hwaddr_len = len;
    // }
    g_ENC28J60_NetIF.hwaddr_len = len;
    memcpy(g_ENC28J60_NetIF.hwaddr, iface->MacAddress, len);

    pNetIF = netif_add(
        &g_ENC28J60_NetIF,
        (const ip4_addr_t *)&ipaddr,
        &netmask,
        &gw,
        NULL,
        enc28j60_ethhw_init,
        ethernet_input);

    netif_set_default(pNetIF);

    /* Initialize the continuation routine for the driver interrupt and receive */
    InitContinuations(pNetIF);

    /* Enable the INTERRUPT pin */
    if (CPU_GPIO_EnableInputPin(
            config->INT_Pin,
            0,                                      // no de-bounce
            &enc28j60_lwip_pre_interrupt,           // ISR
            &g_ENC28J60_NetIF,                      // ISR parameter
            GPIO_INT_EDGE_LOW,                      // Interrupt edge
            GpioPinDriveMode_InputPullUp) == false) // Resistor Pull up mode
    {
        return -1;
    }

    return g_ENC28J60_NetIF.num;
}

bool ENC28J60_LWIP_Driver::Close(ENC28J60_LWIP_DRIVER_CONFIG *config, int index)
{
    (void)index;

    NATIVE_PROFILE_HAL_DRIVERS_ETHERNET();

    if (config == NULL)
        return false;

    LwipUpTimeCompletion.Abort();

    netif_set_link_down(&g_ENC28J60_NetIF);
    netif_set_down(&g_ENC28J60_NetIF);
    netif_remove(&g_ENC28J60_NetIF);

    /* Disable the INTERRUPT pin */
    CPU_GPIO_EnableInputPin(
        config->INT_Pin,
        0,                             /* Glitch filter enable */
        NULL,                          /* ISR                  */
        0,                             /* minor number         */
        GPIO_INT_NONE,                 /* Interrupt edge       */
        GpioPinDriveMode_InputPullUp); /* Resistor State / mode*/

    InterruptTaskContinuation.Abort();

    LwipNetworkStatus = false;

    enc28j60_lwip_close(&g_ENC28J60_NetIF);

    memset(&g_ENC28J60_NetIF, 0, sizeof(g_ENC28J60_NetIF));

    nanoSPI_CloseDevice(config->spiHandle);

    return true;
}

bool ENC28J60_LWIP_Driver::Bind(ENC28J60_LWIP_DRIVER_CONFIG *config, int index)
{
    (void)config;
    (void)index;

    NATIVE_PROFILE_HAL_DRIVERS_ETHERNET();

    return true;
}
