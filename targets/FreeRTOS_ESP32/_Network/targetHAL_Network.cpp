//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// This file includes the board specific Network Intialisation

#include <nanoHAL.h>
#include "lwIP_Sockets.h"
#include "apps/sntp.h"
#include <target_lwip_sntp_opts.h>
#include <target_lwip_sntp_opts.h>

extern "C" void set_signal_sock_function(void (*funcPtr)());
extern bool Esp32_ConnectInProgress;
extern int Esp32_ConnectResult;

#define WIFI_EVENT_TYPE_SCAN_COMPLETE 1

//#define 	PRINT_NET_EVENT 	1

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
    PostManagedEvent(EVENT_WIFI, WiFiEventType_ScanComplete, 0, netIndex);
}

static void PostAPStationChanged(uint connect, uint netInfo)
{
    Network_PostEvent(NetworkChange_NetworkEventType_APStationChanged, connect, netInfo);
}

static void PostConnectResult(int result)
{
    Esp32_ConnectResult = result;
    Esp32_ConnectInProgress = false;

#ifdef PRINT_NET_EVENT
    ets_printf("PostConnectResult  reason : %d\n", result);
#endif

    // fire event for Wifi station job complete
    Events_Set(SYSTEM_EVENT_FLAG_WIFI_STATION);
}

static void initialize_sntp()
{
    sntp_stop();
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, SNTP_SERVER0_DEFAULT_ADDRESS);
    sntp_setservername(1, SNTP_SERVER1_DEFAULT_ADDRESS);
    sntp_init();
}

//
// Network event loop handler
//
static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    int stationIndex;
    wifi_event_ap_staconnected_t *connectedEvent;
    wifi_event_ap_stadisconnected_t *disconnectedEvent;

#ifdef PRINT_NET_EVENT
    ets_printf("Network event %d\n", event->event_id);
#endif

    esp_netif_t *staEspNetif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    esp_netif_t *apEspNetif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");
    esp_netif_t *ethnertEspNetif = esp_netif_get_handle_from_ifkey("ETH_DEF");

    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
            // Wifi station events
            case WIFI_EVENT_STA_START:
#ifdef PRINT_NET_EVENT
                ets_printf("WIFI_EVENT_STA_START\n");
#endif
                break;

            case WIFI_EVENT_STA_CONNECTED:
#ifdef PRINT_NET_EVENT
                ets_printf("WIFI_EVENT_STA_CONNECTED\n");
#endif
                if (Esp32_ConnectInProgress)
                {
                    PostConnectResult(0);
                }

                // TODO PostAvailabilityOn(TCPIP_ADAPTER_IF_STA);
                break;

            case WIFI_EVENT_STA_DISCONNECTED:
#ifdef PRINT_NET_EVENT
                ets_printf("WIFI_EVENT_STA_DISCONNECTED  reason : %d\n", event->event_info.disconnected.reason);
#endif

                if (Esp32_ConnectInProgress)
                {
                    // get disconnected reason
                    wifi_event_sta_disconnected_t *disconnectedEvent = (wifi_event_sta_disconnected_t *)event_data;
                    PostConnectResult(disconnectedEvent->reason);
                }
                else
                {
                    // TODO PostAvailabilityOff(TCPIP_ADAPTER_IF_STA);
                    esp_wifi_connect();
                }
                break;

            // Scan of available Wifi networks complete
            case WIFI_EVENT_SCAN_DONE:
#ifdef PRINT_NET_EVENT
                ets_printf("WIFI_EVENT_SCAN_DONE\n");
#endif
                // TODO PostScanComplete(TCPIP_ADAPTER_IF_STA);
                break;

            case WIFI_EVENT_STA_AUTHMODE_CHANGE:
                // system_event_sta_authmode_change_t *auth_change = &event->event_info.auth_change;
#ifdef PRINT_NET_EVENT
                ets_printf("WIFI_EVENT_STA_AUTHMODE_CHANGE");
#endif
                break;

            case WIFI_EVENT_STA_WPS_ER_SUCCESS:
#ifdef PRINT_NET_EVENT
                ets_printf("WIFI_EVENT_STA_WPS_ER_SUCCESS\n");
#endif
                break;

            case WIFI_EVENT_STA_WPS_ER_FAILED:
#ifdef PRINT_NET_EVENT
                ets_printf("WIFI_EVENT_STA_WPS_ER_FAILED\n");
#endif
                break;

            case WIFI_EVENT_STA_WPS_ER_TIMEOUT:
#ifdef PRINT_NET_EVENT
                ets_printf("WIFI_EVENT_STA_WPS_ER_TIMEOUT\n");
#endif
                break;

            case WIFI_EVENT_STA_WPS_ER_PIN:
#ifdef PRINT_NET_EVENT
                ets_printf("WIFI_EVENT_STA_WPS_ER_PIN\n");
#endif
                break;

            // Wireless AP events
            case WIFI_EVENT_AP_START:
                // TODO PostAvailabilityOn(TCPIP_ADAPTER_IF_AP);
#ifdef PRINT_NET_EVENT
                ets_printf("WIFI_EVENT_AP_START\n");
#endif
                break;

            case WIFI_EVENT_AP_STOP:
                // TODO PostAvailabilityOff(TCPIP_ADAPTER_IF_AP);
#ifdef PRINT_NET_EVENT
                ets_printf("WIFI_EVENT_AP_STOP\n");
#endif
                break;

            case WIFI_EVENT_AP_STACONNECTED:
                // access STA connected event
                connectedEvent = (wifi_event_ap_staconnected_t *)event_data;
                stationIndex = connectedEvent->aid - 1;
                Network_Interface_Add_Station(stationIndex, connectedEvent->mac);

                // Post the Network interface + Client ID in top 8 bits
                // TODO PostAPStationChanged(1, TCPIP_ADAPTER_IF_AP + (stationIndex << 8));
#ifdef PRINT_NET_EVENT
                ets_printf("WIFI_EVENT_AP_STACONNECTED %d\n", event->event_info.sta_connected.aid);
#endif
                break;

            case WIFI_EVENT_AP_STADISCONNECTED:
                // access STA disconnected event
                disconnectedEvent = (wifi_event_ap_stadisconnected_t *)event_data;
                stationIndex = disconnectedEvent->aid - 1;

                Network_Interface_Remove_Station(stationIndex);
                // TODO PostAPStationChanged(0, TCPIP_ADAPTER_IF_AP + (stationIndex << 8));

#ifdef PRINT_NET_EVENT
                ets_printf("WIFI_EVENT_AP_STADISCONNECTED %d\n", event->event_info.sta_disconnected.aid);
#endif
                break;

            case WIFI_EVENT_AP_PROBEREQRECVED:
#ifdef PRINT_NET_EVENT
                ets_printf("WIFI_EVENT_AP_PROBEREQRECVED\n");
#endif
                break;
        }
    }
    else if (event_base == IP_EVENT)
    {
        switch (event_id)
        {

            case IP_EVENT_STA_GOT_IP:
#ifdef PRINT_NET_EVENT
                ets_printf("IP_EVENT_STA_GOT_IP\n");
#endif
                // TODO PostAddressChanged(TCPIP_ADAPTER_IF_STA);
                initialize_sntp();
                break;

            case IP_EVENT_STA_LOST_IP:
                // TODO PostAddressChanged(TCPIP_ADAPTER_IF_STA);
#ifdef PRINT_NET_EVENT
                ets_printf("IP_EVENT_STA_LOST_IP\n");
#endif
                break;

            case IP_EVENT_ETH_GOT_IP:
#ifdef PRINT_NET_EVENT
                ets_printf("IP_EVENT_ETH_GOT_IP\n");
#endif
                // TODO PostAddressChanged(TCPIP_ADAPTER_IF_ETH);
                initialize_sntp();
                break;
        }
    }
    else if (event_base == ETH_EVENT)
    {
        switch (event_id)
        {
            case ETHERNET_EVENT_START:
#ifdef PRINT_NET_EVENT
                ets_printf("ETHERNET_EVENT_START\n");
#endif
                break;

            case ETHERNET_EVENT_STOP:
#ifdef PRINT_NET_EVENT
                ets_printf("ETHERNET_EVENT_STOP\n");
#endif
                break;

            case ETHERNET_EVENT_CONNECTED:
#ifdef PRINT_NET_EVENT
                ets_printf("ETHERNET_EVENT_CONNECTED\n");
#endif
                // TODO PostAvailabilityOn(TCPIP_ADAPTER_IF_ETH);
                break;

            case ETHERNET_EVENT_DISCONNECTED:
#ifdef PRINT_NET_EVENT
                ets_printf("ETHERNET_EVENT_DISCONNECTED\n");
#endif
                // TODO PostAvailabilityOff(TCPIP_ADAPTER_IF_ETH);
                break;

            default:
                break;
        }
    }
}

void nanoHAL_Network_Initialize()
{
    // Initialise the Lwip CLR signal callback
    //set_signal_sock_function(&sys_signal_sock_event);

    // initialize netif
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));
}
