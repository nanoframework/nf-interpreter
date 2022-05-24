//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2021 STMicroelectronics. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nf_netxduo.h>

#define NULL_IP_ADDRESS     IP_ADDRESS(0, 0, 0, 0)
#define DEFAULT_PRIORITY    5
#define DEFAULT_MEMORY_SIZE 1024
#define PACKET_PAYLOAD_SIZE 1536
#define NX_PACKET_POOL_SIZE ((1536 + sizeof(NX_PACKET)) * 3)
#define ARP_CACHE_SIZE      1024
#define BSD_STACK_SIZE      1024

void _nx_ram_network_driver(NX_IP_DRIVER *driver_req_ptr);
VOID  nx_driver_stm32l4(NX_IP_DRIVER *driver_req_ptr);
static uint8_t wifi_init();

// byte pool for NetX Duo
#define NX_APP_MEM_POOL_SIZE 1024 * 8
#if defined(__GNUC__)
//__attribute__((section(".NetXPoolSection")))
#endif
#if defined(__GNUC__)
__attribute__((aligned(4)))
#endif
uint8_t __attribute__((section(".ram4"))) nx_byte_pool_buffer[NX_APP_MEM_POOL_SIZE];
static TX_BYTE_POOL nx_app_byte_pool;

/* App memory pointer. */
uint8_t *pointer;
NX_PACKET_POOL pool_0;
NX_IP IpInstance;
NX_DNS DnsInstance;

#if !defined(WIFI_DRIVER_ISM43362)
// ISM43362 handles DHCP
NX_DHCP DhcpInstance;
#endif

uint32_t NF_NetXDuo_Init(HAL_Configuration_NetworkInterface *networkConfig)
{
    (void)networkConfig;

    if (tx_byte_pool_create(&nx_app_byte_pool, "NetX Duo mem pool", nx_byte_pool_buffer, NX_APP_MEM_POOL_SIZE) !=
        TX_SUCCESS)
    {
        HAL_AssertEx();
        return NX_NOT_ENABLED;
    }

    // init NetXDuo
    nx_system_initialize();

    tx_thread_sleep(TX_TICKS_PER_MILLISEC(10));

    // Initialize Wifi
    if (wifi_init() != NX_SUCCESS)
    {
        //HAL_AssertEx();
        return NX_NOT_ENABLED;
    }

    tx_thread_sleep(TX_TICKS_PER_MILLISEC(10));

    // allocate the network packet pool
    if (tx_byte_allocate(&nx_app_byte_pool, (void **)&pointer, NX_PACKET_POOL_SIZE, TX_NO_WAIT) != NX_SUCCESS)
    {
        return NX_NOT_ENABLED;
    }

    // Create a BSD packet pool
    if (nx_packet_pool_create(&pool_0, "NetX Main Packet Pool", PACKET_PAYLOAD_SIZE, pointer, NX_PACKET_POOL_SIZE) !=
        NX_SUCCESS)
    {
        return NX_NOT_ENABLED;
    }

    // Allocate the NetX IP Instance memory
    if (tx_byte_allocate(&nx_app_byte_pool, (void **)&pointer, 1 * DEFAULT_MEMORY_SIZE, TX_NO_WAIT) != NX_SUCCESS)
    {
        return NX_NOT_ENABLED;
    }

    // static or dinamic address
    ULONG ipAddress = NULL_IP_ADDRESS;
    ULONG networkMask = NULL_IP_ADDRESS;

    if(networkConfig->StartupAddressMode == AddressMode_Static)
    {
        // IPv4 configs
        ipAddress = networkConfig->IPv4Address;
        networkMask = networkConfig->IPv4NetMask;
    }
    else
    {
        // IP 0.0.0.0
        ipAddress = IP_ADDRESS(0, 0, 0, 0);
        // mask 255.255.255.0
        networkMask = 0xFFFFFF00UL;        
    }

    // Create an IP instance for BSD by linking the nx_driver_St32f7xx driver
    if (nx_ip_create(
            &IpInstance,
            "nanodevice",
            ipAddress,
            networkMask,
            &pool_0,
            nx_driver_stm32l4,
            pointer,
            1 * DEFAULT_MEMORY_SIZE,
            DEFAULT_PRIORITY) != NX_SUCCESS)
    {
        return NX_NOT_ENABLED;
    }

    // Allocate the ARP cache
    if (tx_byte_allocate(&nx_app_byte_pool, (void **)&pointer, ARP_CACHE_SIZE, TX_NO_WAIT) != NX_SUCCESS)
    {
        return NX_NOT_ENABLED;
    }

    // Enable ARP and supply ARP cache memory for IP Instance 0.
    if (nx_arp_enable(&IpInstance, (void *)pointer, ARP_CACHE_SIZE) != NX_SUCCESS)
    {
        return NX_NOT_ENABLED;
    }

    // Enable the ICMP support
    if (nx_icmp_enable(&IpInstance) != NX_SUCCESS)
    {
        return NX_NOT_ENABLED;
    }

    // Enable UDP traffic
    if (nx_udp_enable(&IpInstance) != NX_SUCCESS)
    {
        return NX_NOT_ENABLED;
    }

    // Enable TCP traffic
    if (nx_tcp_enable(&IpInstance) != NX_SUCCESS)
    {
        return NX_NOT_ENABLED;
    }

    // initialize BSD Socket Wrapper
    if (bsd_initialize(&IpInstance, &pool_0, (CHAR *)pointer, BSD_STACK_SIZE, 5) != NX_SUCCESS)
    {
        return NX_NOT_ENABLED;
    }

    // Create a DNS instance for the Client.
    if (nx_dns_create(&DnsInstance, &IpInstance, (UCHAR *)"DNS Client") != NX_SUCCESS)
    {
        return NX_NOT_ENABLED;
    }

    if(networkConfig->AutomaticDNS == 0)
    {
        // add DNS server(s)
        if(networkConfig->IPv4DNSAddress1 != 0)
        {
            nx_dns_server_add(&DnsInstance, networkConfig->IPv4DNSAddress1);
        }

        if(networkConfig->IPv4DNSAddress2 != 0)
        {
            nx_dns_server_add(&DnsInstance, networkConfig->IPv4DNSAddress2);
        }
    }

#if !defined(WIFI_DRIVER_ISM43362)

    // Create a DHCP instance
    if (nx_dhcp_create(&DhcpInstance, &IpInstance, "My DHCP") != NX_SUCCESS)
    {
        return NX_NOT_ENABLED;
    }

#endif

    if(networkConfig->StartupAddressMode == AddressMode_DHCP ||
       networkConfig->AutomaticDNS == 1)
    {

#if !defined(WIFI_DRIVER_ISM43362)

        // Start the DHCP Client
        if (nx_dhcp_start(&DhcpInstance) != NX_SUCCESS)
        {
            return NX_NOT_ENABLED;
        }

#endif

    }


    return NX_SUCCESS;
}

static uint8_t wifi_init()
{

#if defined(WIFI_DRIVER_ISM43362)

    if (WIFI_Init() != WIFI_STATUS_OK)
    {
        return NX_NOT_SUCCESSFUL;
    }

#endif

    return NX_SUCCESS;
}

void NF_NetXDuo_UnInit(void)
{
    tx_byte_pool_delete(&nx_app_byte_pool);
}
