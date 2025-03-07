//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanohal.h>
#include "wiznet5500_lwip.h"

extern "C"
{
#include "netif\etharp.h"
#include "lwip\dhcp.h"
#include "lwip\tcpip.h"
#include "lwip\dns.h"
#include "lwip\ip_addr.h"
}

static WIZNET5500_LWIP_Driver g_WIZNET5500_LWIP_Driver;
static struct netif g_WIZNET5500_NetIF;

HAL_CONTINUATION InterruptTaskContinuation;
HAL_COMPLETION LwipUpTimeCompletion;
static bool LwipNetworkStatus = FALSE;
static uint32_t LwipLastIpAddress = 0;

extern WIZNET5500_LWIP_DEVICE_CONFIG g_WIZNET5500_LWIP_Config;
extern HAL_TARGET_CONFIGURATION g_TargetConfiguration;

extern bool wiznet5500_get_link_status(uint32_t spiHandle);

void wiznet5500_status_callback(struct netif *netif)
{
    if (LwipLastIpAddress != netif->ip_addr.addr)
    {
        Network_PostEvent(
            NetworkChange_NetworkEventType_AvailabilityChanged,
            NetworkChange_NetworkEvents_NetworkNOTAvailable,
            0);

        LwipLastIpAddress = netif->ip_addr.addr;
    }

}

err_t wiznet5500_ethhw_init(netif *myNetIf)
{
    myNetIf->mtu = ETHERSIZE;

    myNetIf->flags = NETIF_FLAG_IGMP | NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET;

    /* ethhw_init() is user-defined */
    /* use ip_input instead of ethernet_input for non-ethernet hardware */
    /* (this function is assigned to netif.input and should be called by the hardware driver) */

    /* Assign the xmit routine to the stack's netif and call the driver's Open */
    myNetIf->output = etharp_output;
    myNetIf->linkoutput = wiznet5500_lwip_xmit;

    wiznet5500_lwip_open(myNetIf);

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

    bool status = wiznet5500_get_link_status(g_WIZNET5500_LWIP_Config.DeviceConfigs[0].spiHandle);

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
    InterruptTaskContinuation.InitializeCallback((HAL_CALLBACK_FPN)wiznet5500_lwip_interrupt, &g_ENC28J60_NetIF);

    LwipUpTimeCompletion.InitializeForUserMode((HAL_CALLBACK_FPN)lwip_network_uptime_completion, pNetIf);

    LwipUpTimeCompletion.EnqueueTicks(Delta64(500));
}

bool wiznet5500_Network_Interface_Bind(int index, HAL_Configuration_NetworkInterface *pConfig)
{
    NATIVE_PROFILE_HAL_DRIVERS_ETHERNET();
    if (pConfig->SpecificConfigId >= ARRAYSIZE(g_WIZNET5500_LWIP_Config.DeviceConfigs))
        return false;

    return g_WIZNET5500_LWIP_Driver.Bind(&g_WIZNET5500_LWIP_Config.DeviceConfigs[pConfig->SpecificConfigId], index);
}

int wiznet5500_Network_Interface_Open(int index, HAL_Configuration_NetworkInterface *pConfig)
{
    NATIVE_PROFILE_HAL_DRIVERS_ETHERNET();

    if (pConfig->SpecificConfigId >= ARRAYSIZE(g_WIZNET5500_LWIP_Config.DeviceConfigs))
        return -1;

    return g_WIZNET5500_LWIP_Driver.Open(&g_WIZNET5500_LWIP_Config.DeviceConfigs[pConfig->SpecificConfigId], index);
}
bool wiznet5500_Network_Interface_Close(int index, HAL_Configuration_NetworkInterface *pConfig)
{
    NATIVE_PROFILE_HAL_DRIVERS_ETHERNET();
    if (pConfig->SpecificConfigId >= ARRAYSIZE(g_WIZNET5500_LWIP_Config.DeviceConfigs))
        return FALSE;

    return g_WIZNET5500_LWIP_Driver.Close(&g_WIZNET5500_LWIP_Config.DeviceConfigs[pConfig->SpecificConfigId], index);
}

int WIZNET5500_LWIP_Driver::Open(WIZNET5500_LWIP_DRIVER_CONFIG *config, int index)
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

    len = sizeof(iface->MacAddress);

    g_WIZNET5500_NetIF.hwaddr_len = len;
    memcpy(g_WIZNET5500_NetIF.hwaddr, iface->MacAddress, len);

    pNetIF = netif_add(
        &g_WIZNET5500_NetIF,
        (const ip4_addr_t *)&ipaddr,
        &netmask,
        &gw,
        NULL,
        wiznet5500_ethhw_init,
        ethernet_input);

    netif_set_default(pNetIF);

    /* Initialize the continuation routine for the driver interrupt and receive */
    InitContinuations(pNetIF);

    /* Enable the INTERRUPT pin */
    if (CPU_GPIO_EnableInputPin(
            config->INT_Pin,
            0,                             // no de-bounce
            &wiznet5500_lwip_pre_interrupt,  // ISR
            &g_WIZNET5500_NetIF,             // ISR parameter
            GPIO_INT_EDGE_LOW,             // Interrupt edge
            PinMode_InputPullUp) == false) // Resistor Pull up mode
    {
        return -1;
    }

    return g_WIZNET5500_NetIF.num;
}

bool WIZNET5500_LWIP_Driver::Close(WIZNET5500_LWIP_DRIVER_CONFIG *config, int index)
{
    (void)index;

    NATIVE_PROFILE_HAL_DRIVERS_ETHERNET();

    if (config == NULL)
        return false;

    LwipUpTimeCompletion.Abort();

    netif_set_link_down(&g_WIZNET5500_NetIF);
    netif_set_down(&g_WIZNET5500_NetIF);
    netif_remove(&g_WIZNET5500_NetIF);

    /* Disable the INTERRUPT pin */
    CPU_GPIO_EnableInputPin(
        config->INT_Pin,
        0,                    /* Glitch filter enable */
        NULL,                 /* ISR                  */
        0,                    /* minor number         */
        GPIO_INT_NONE,        /* Interrupt edge       */
        PinMode_InputPullUp); /* Resistor State / mode*/

    InterruptTaskContinuation.Abort();

    LwipNetworkStatus = false;

    wiznet5500_lwip_close(&g_WIZNET5500_NetIF);

    memset(&g_WIZNET5500_NetIF, 0, sizeof(g_WIZNET5500_NetIF));

    nanoSPI_CloseDevice(config->spiHandle);

    return true;
}

bool WIZNET5500_LWIP_Driver::Bind(WIZNET5500_LWIP_DRIVER_CONFIG *config, int index)
{
    (void)config;
    (void)index;

    NATIVE_PROFILE_HAL_DRIVERS_ETHERNET();

    return true;
}
