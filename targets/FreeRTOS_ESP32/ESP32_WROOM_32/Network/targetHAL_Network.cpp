//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

// This file includes the board specific Network Intialisation

#include <nanoHAL.h>
#include "Esp32_os.h"
#include "LWIP_Sockets.h"
#include "apps/sntp.h"
#include <target_lwip_sntp_opts.h>

extern "C" void set_signal_sock_function( void (*funcPtr)() );

#define WIFI_EVENT_TYPE_SCAN_COMPLETE 1

//#define 	NetEventPrint 	1


//
// Callback from LWIP on event
//
void sys_signal_sock_event()
{
     Events_Set(SYSTEM_EVENT_FLAG_SOCKET);
}

//
// Dummy Ethernet Initialisation, board specific Initialisation in separate file
__nfweak void InitialiseEthernet()
{
}

static void PostAddressChanged(uint netIndex)
{
	Network_PostEvent(NetworkChange_NetworkEventType_AddressChanged, 0, netIndex);
}

static void PostAvailabilityOn(uint netIndex)
{
	Network_PostEvent(NetworkChange_NetworkEventType_AvailabilityChanged, 1, netIndex);
}

static void PostAvailabilityOff(uint netIndex)
{
	Network_PostEvent(NetworkChange_NetworkEventType_AvailabilityChanged, 0, netIndex);
}

static void PostScanComplete(uint netIndex)
{
    PostManagedEvent( EVENT_WIFI, WiFiEventType_ScanComplete, 0, netIndex );
}

static void PostAPStationChanged(uint connect, uint netInfo)
{
	Network_PostEvent(NetworkChange_NetworkEventType_APStationChanged, connect, netInfo);
}

static void initialize_sntp()
{
	sntp_stop();
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, SNTP_SERVER_DEFAULT_ADDRESS);
    sntp_init();
}

//
// Network event loop handler
//
static  esp_err_t event_handler(void *ctx, system_event_t *event)
{
	(void)ctx;
	int stationIndex;

  #ifdef NetEventPrint
	ets_printf("Network event %d\n", event->event_id);
  #endif

    switch(event->event_id)
	{
		// Wifi station events
		case SYSTEM_EVENT_STA_START:
			// Smart config commented out as giving exception when running
			// xTaskCreate(smartconfig_task, "smartconfig_example_task", 4096, NULL, 3, NULL);
	  	  #ifdef NetEventPrint
			ets_printf("SYSTEM_EVENT_STA_START\n");
	       #endif
			break;

		case SYSTEM_EVENT_STA_GOT_IP:
  	  	  #ifdef NetEventPrint
			ets_printf("SYSTEM_EVENT_STA_GOT_IP\n");
		  #endif
			PostAddressChanged(TCPIP_ADAPTER_IF_STA);
			initialize_sntp();
			//xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
			break;

		case SYSTEM_EVENT_STA_LOST_IP:
			PostAddressChanged(TCPIP_ADAPTER_IF_STA);
	  	  #ifdef NetEventPrint
			ets_printf("SYSTEM_EVENT_STA_LOST_IP\n");
		  #endif
			break;

		case SYSTEM_EVENT_STA_CONNECTED:
		  #ifdef NetEventPrint
			ets_printf("SYSTEM_EVENT_STA_CONNECTED\n");
		  #endif
			PostAvailabilityOn(TCPIP_ADAPTER_IF_STA);
			break;

		case SYSTEM_EVENT_STA_DISCONNECTED:
	 	  #ifdef NetEventPrint
			ets_printf("SYSTEM_EVENT_STA_DISCONNECTED\n");
		  #endif
			PostAvailabilityOff(TCPIP_ADAPTER_IF_STA);
			esp_wifi_connect();
			//xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
			break;
		
		// Scan of available Wifi networks complete
		case SYSTEM_EVENT_SCAN_DONE:
	      #ifdef NetEventPrint
			ets_printf("SYSTEM_EVENT_SCAN_DONE\n");
		  #endif
			PostScanComplete(TCPIP_ADAPTER_IF_STA);
			break;

		case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
			// system_event_sta_authmode_change_t *auth_change = &event->event_info.auth_change;
		  #ifdef NetEventPrint
			ets_printf("SYSTEM_EVENT_STA_AUTHMODE_CHANGE");
	      #endif
			break;

		case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
	      #ifdef NetEventPrint
			ets_printf("SYSTEM_EVENT_STA_WPS_ER_SUCCESS\n");
		  #endif
			break;

		case SYSTEM_EVENT_STA_WPS_ER_FAILED:
		  #ifdef NetEventPrint
			ets_printf("SYSTEM_EVENT_STA_WPS_ER_FAILED\n");
		  #endif
			break;

		case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
		  #ifdef NetEventPrint
			ets_printf("SYSTEM_EVENT_STA_WPS_ER_TIMEOUT\n");
		  #endif
			break;

		case SYSTEM_EVENT_STA_WPS_ER_PIN:
		  #ifdef NetEventPrint
			ets_printf("SYSTEM_EVENT_STA_WPS_ER_PIN\n");
		  #endif
			break;

		// Wireless AP events
		case SYSTEM_EVENT_AP_START:
			PostAvailabilityOn(TCPIP_ADAPTER_IF_AP);
		  #ifdef NetEventPrint
			ets_printf("SYSTEM_EVENT_AP_START\n");
		  #endif
			break;

		case SYSTEM_EVENT_AP_STOP:
			PostAvailabilityOff(TCPIP_ADAPTER_IF_AP);
		  #ifdef NetEventPrint
			ets_printf("SYSTEM_EVENT_AP_STOP\n");
		  #endif
			break;

		case SYSTEM_EVENT_AP_STACONNECTED:
			stationIndex = event->event_info.sta_connected.aid - 1;
			Network_Interface_Add_Station(stationIndex, event->event_info.sta_connected.mac);

			// Post the Network interface + Client ID in top 8 bits 
			PostAPStationChanged(1, TCPIP_ADAPTER_IF_AP + (stationIndex << 8));
		  #ifdef NetEventPrint
			ets_printf("SYSTEM_EVENT_AP_STACONNECTED %d\n", event->event_info.sta_connected.aid);
		  #endif
			break;

		case SYSTEM_EVENT_AP_STADISCONNECTED:
			stationIndex = event->event_info.sta_connected.aid - 1;
			Network_Interface_Remove_Station(stationIndex);
			PostAPStationChanged(0, TCPIP_ADAPTER_IF_AP + (stationIndex << 8));
		  #ifdef NetEventPrint
			ets_printf("SYSTEM_EVENT_AP_STADISCONNECTED %d\n", event->event_info.sta_disconnected.aid);
			PrintStations();
		  #endif
			break;

		case SYSTEM_EVENT_AP_STA_GOT_IP6:
		  #ifdef NetEventPrint
			ets_printf("SYSTEM_EVENT_AP_STA_GOT_IP6\n");
		  #endif
			break;

		case SYSTEM_EVENT_AP_PROBEREQRECVED:
		  #ifdef NetEventPrint
			ets_printf("SYSTEM_EVENT_AP_PROBEREQRECVEDxz\n");
		  #endif
			break;

		// Ethernet events
		case SYSTEM_EVENT_ETH_START:
		  #ifdef NetEventPrint
			ets_printf("SYSTEM_EVENT_ETH_START\n");
		  #endif
			break;

		case SYSTEM_EVENT_ETH_STOP:
		  #ifdef NetEventPrint
			ets_printf("SYSTEM_EVENT_ETH_STOP\n");
		  #endif
			break;

		case SYSTEM_EVENT_ETH_CONNECTED:
		  #ifdef NetEventPrint
			ets_printf("SYSTEM_EVENT_ETH_CONNECTED\n");
		  #endif
			PostAvailabilityOn(TCPIP_ADAPTER_IF_ETH);
			break;

		case SYSTEM_EVENT_ETH_GOT_IP:
		  #ifdef NetEventPrint
			ets_printf("SYSTEM_EVENT_ETH_GOT_IP\n");
	      #endif
			PostAddressChanged(TCPIP_ADAPTER_IF_ETH);
			initialize_sntp();
			break;

		case SYSTEM_EVENT_ETH_DISCONNECTED:
		  #ifdef NetEventPrint
			ets_printf("SYSTEM_EVENT_ETH_DISCONNECTED\n");
		  #endif
			PostAvailabilityOff(TCPIP_ADAPTER_IF_ETH);
			break;

		default:
			break;
	}

    return ESP_OK;
}

void nanoHAL_Network_Initialize()
{
 	// Initialise the Lwip CLR signal callback
	set_signal_sock_function( &sys_signal_sock_event );

	// Initialise Tcp adpater
    tcpip_adapter_init();
   
    esp_event_loop_init(event_handler, NULL);
}
