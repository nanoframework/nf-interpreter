//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright (c) Texas Instruments Incorporated.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "simplelink_sockets.h"

//--// 

#if defined(DEBUG)
#define DEBUG_HANDLE_SOCKET_ERROR(t,a) 
// assume there is something to add in later??
#else
#define DEBUG_HANDLE_SOCKET_ERROR(t,a) 
#endif

//--//

//--//
static HAL_CONTINUATION PostAddressChangedContinuation;
static HAL_CONTINUATION PostAvailabilityOnContinuation;
static HAL_CONTINUATION PostAvailabilityOffContinuation;

void PostAddressChanged(void* arg)
{
    (void)arg;

	Network_PostEvent(NetworkChange_NetworkEventType_AddressChanged, 0, 0);
}

void PostAvailabilityOn(void* arg)
{
    (void)arg;

	Network_PostEvent(NetworkChange_NetworkEventType_AvailabilityChanged, NetworkChange_NetworkEvents_NetworkAvailable, 0);
}

void PostAvailabilityOff(void* arg)
{
    (void)arg;

	Network_PostEvent(NetworkChange_NetworkEventType_AvailabilityChanged, NetworkChange_NetworkEvents_NetworkNOTAvailable, 0);
}

void Link_callback(bool linkUp)
{
	if (linkUp)
	{
		if (!PostAvailabilityOnContinuation.IsLinked())
			PostAvailabilityOnContinuation.Enqueue();
	}
	else
	{
		if (!PostAvailabilityOffContinuation.IsLinked())
			PostAvailabilityOffContinuation.Enqueue();
	}

    Events_Set(SYSTEM_EVENT_FLAG_SOCKET);
    Events_Set(SYSTEM_EVENT_FLAG_NETWORK);
}

void Status_callback()
{
	if (!PostAddressChangedContinuation.IsLinked())
		PostAddressChangedContinuation.Enqueue();

#if !defined(BUILD_RTM)
	// lcd_printf("\f\n\n\n\n\n\nLink Update: %s\n", (netif_is_up(netif) ? "UP  " : "DOWN"));
	// lcd_printf("         IP: %d.%d.%d.%d\n", (netif->ip_addr.addr >> 0) & 0xFF,
	// 	(netif->ip_addr.addr >> 8) & 0xFF,
	// 	(netif->ip_addr.addr >> 16) & 0xFF,
	// 	(netif->ip_addr.addr >> 24) & 0xFF);
	// lcd_printf("         SM: %d.%d.%d.%d\n", (netif->netmask.addr >> 0) & 0xFF,
	// 	(netif->netmask.addr >> 8) & 0xFF,
	// 	(netif->netmask.addr >> 16) & 0xFF,
	// 	(netif->netmask.addr >> 24) & 0xFF);
	// lcd_printf("         GW: %d.%d.%d.%d\n", (netif->gw.addr >> 0) & 0xFF,
	// 	(netif->gw.addr >> 8) & 0xFF,
	// 	(netif->gw.addr >> 16) & 0xFF,
	// 	(netif->gw.addr >> 24) & 0xFF);

//FIXME debug_printf("IP Address: %d.%d.%d.%d\n", (netif->ip_addr.u_addr.ip4.addr >> 0) & 0xFF,
// 	(netif->ip_addr.u_addr.ip4.addr >> 8) & 0xFF,
// 	(netif->ip_addr.u_addr.ip4.addr >> 16) & 0xFF,
// 	(netif->ip_addr.u_addr.ip4.addr >> 24) & 0xFF);
#if LWIP_DNS
	if (netif->flags & NETIF_FLAG_ETHARP)
	{
		//ip_addr_t * dns1 = dns_getserver(0);
		//ip_addr_t * dns2 = dns_getserver(1);

		// lcd_printf("         dns1: %d.%d.%d.%d\n", (dns1.addr >> 0) & 0xFF,
		// 	(dns1.addr >> 8) & 0xFF,
		// 	(dns1.addr >> 16) & 0xFF,
		// 	(dns1.addr >> 24) & 0xFF);

		// lcd_printf("         dns2: %d.%d.%d.%d\n", (dns2.addr >> 0) & 0xFF,
		// 	(dns2.addr >> 8) & 0xFF,
		// 	(dns2.addr >> 16) & 0xFF,
		// 	(dns2.addr >> 24) & 0xFF);
	}
#endif
    Events_Set(SYSTEM_EVENT_FLAG_SOCKET);
    Events_Set(SYSTEM_EVENT_FLAG_NETWORK);
}
#endif

bool SimpleLink_SOCKETS_Initialize()
{   
    NATIVE_PROFILE_PAL_NETWORK();

    PostAddressChangedContinuation.InitializeCallback(PostAddressChanged, NULL);
    PostAvailabilityOnContinuation.InitializeCallback(PostAvailabilityOn, NULL);
    PostAvailabilityOffContinuation.InitializeCallback(PostAvailabilityOff, NULL);

    return true;
}

bool SimpleLink_SOCKETS_Uninitialize()
{
    NATIVE_PROFILE_PAL_NETWORK();

    PostAddressChangedContinuation.Abort();
    PostAvailabilityOnContinuation.Abort();
    PostAvailabilityOffContinuation.Abort();

    return true;
}
