//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

// This file includes the board specific Network Intialisation

#include <nanoHAL.h>
#include "Esp32_os.h"
#include "LWIP_Sockets.h"

extern "C" void set_signal_sock_function( void (*funcPtr)() );


// NVS parameters for Ethernet config
#define NVS_NAMESPACE	    "nanoF"
#define NVS_NETWORK_CONFIG1	"Net1"
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


static void PostAddressChanged()
{
	Network_PostEvent(NETWORK_EVENT_TYPE_ADDRESS_CHANGED, 0);
}

static void PostAvailabilityOn()
{
	Network_PostEvent(NETWORK_EVENT_TYPE__AVAILABILITY_CHANGED, 1);
}

static void PostAvailabilityOff()
{
	Network_PostEvent(NETWORK_EVENT_TYPE__AVAILABILITY_CHANGED, 0);
}

//
// Network event loop handler
//
static  esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {

// Wifi station events
    case SYSTEM_EVENT_STA_START:
       // Smart config commented out as giving exception when running
       // xTaskCreate(smartconfig_task, "smartconfig_example_task", 4096, NULL, 3, NULL);
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
		PostAddressChanged();
        //xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
		PostAvailabilityOff();
		break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
		PostAvailabilityOff();
        esp_wifi_connect();
        //xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;

// Ethernet events
	case SYSTEM_EVENT_ETH_START:
		break;
	case SYSTEM_EVENT_ETH_STOP:
		break;
	case SYSTEM_EVENT_ETH_CONNECTED:
		PostAvailabilityOn();
		break;
	case SYSTEM_EVENT_ETH_GOT_IP:
		PostAddressChanged();
		break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
		PostAvailabilityOff();
		break;

    default:
        break;
    }
    return ESP_OK;
}


void IRAM_ATTR nanoHAL_Network_Initialize()
{
	esp_err_t ret;

 	// Initialise the Lwip CLR signal callback
	set_signal_sock_function( &sys_signal_sock_event );

	// Initialise Tcp adpater
    tcpip_adapter_init();
   
    esp_event_loop_init(event_handler, NULL);
}
