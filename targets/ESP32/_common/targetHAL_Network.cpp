//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// This file includes the board specific Network Initialisation

#include "NF_ESP32_Network.h"
#include "lwIP_Sockets.h"
#include "apps/sntp.h"
#include <target_lwip_sntp_opts.h>
#include "target_platform.h"

extern "C" void set_signal_sock_function(void (*funcPtr)());

#define WIFI_EVENT_TYPE_SCAN_COMPLETE 1

//#define 	PRINT_NET_EVENT 	1

// buffer with host name
char hostName[18] = "nanodevice_";

//
// Call-back from LWIP on event
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
    NF_ESP32_ConnectResult = result;
    NF_ESP32_ConnectInProgress = false;

#ifdef PRINT_NET_EVENT
    ets_printf("PostConnectResult  reason : %d\n", result);
#endif

    // fire event for Wi-Fi station job complete
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

static void compose_esp32_hostname()
{
    // compose host name with nanodevice and last 3 bytes of MAC address
    // nanodevice_XXXXXX
    uint8_t mac[6];
    char *macPosition = hostName + 11;

    // get MAC address
    esp_efuse_mac_get_default(mac);

    // copy over last 3 bytes of MAC address
    for (int index = 3; index < 6; index++)
    {
        sprintf(macPosition, "%02X", (int)mac[index]);
        macPosition += 2;
    }
}

//
// Network event loop handler
//
static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    esp_err_t result;
    int stationIndex;
    esp_netif_t *espNetif;
    wifi_event_ap_staconnected_t *apConnectedEvent;
    wifi_event_ap_stadisconnected_t *apDisconnectedEvent;
    wifi_event_sta_disconnected_t *staDisconnectedEvent;

#ifdef PRINT_NET_EVENT
    ets_printf("Event %d, ID: %d\n", event_base, event_id);
#endif

    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
            // Wi-Fi station events
            case WIFI_EVENT_STA_START:
#ifdef PRINT_NET_EVENT
                ets_printf("WIFI_EVENT_STA_START\n");
#endif

                // get Netif for STA
                espNetif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");

                // set host name for STA
                esp_netif_set_hostname(espNetif, hostName);

                if (NF_ESP32_IsToConnect)
                {
                    // start connect
                    result = esp_wifi_connect();
                    ets_printf("connect... %d \n", result);
                }

                break;

            case WIFI_EVENT_STA_CONNECTED:
#ifdef PRINT_NET_EVENT
                ets_printf("WIFI_EVENT_STA_CONNECTED\n");
#endif
                if (NF_ESP32_ConnectInProgress)
                {
                    PostConnectResult(0);
                }

                PostAvailabilityOn(IDF_WIFI_STA_DEF);

                break;

            case WIFI_EVENT_STA_DISCONNECTED:
                // get disconnected reason
                staDisconnectedEvent = (wifi_event_sta_disconnected_t *)event_data;

#ifdef PRINT_NET_EVENT
                ets_printf("WIFI_EVENT_STA_DISCONNECTED  reason : %d\n", staDisconnectedEvent->reason);
#endif

                if (NF_ESP32_ConnectInProgress)
                {
                    PostConnectResult(staDisconnectedEvent->reason);
                }

                PostAvailabilityOff(IDF_WIFI_STA_DEF);

                if (NF_ESP32_IsToConnect)
                {
                    // better re-connect
                    result = esp_wifi_connect();
                    ets_printf("connect... %d \n", result);
                }

                break;

            // Scan of available Wi-Fi networks complete
            case WIFI_EVENT_SCAN_DONE:
#ifdef PRINT_NET_EVENT
                ets_printf("WIFI_EVENT_SCAN_DONE\n");
#endif
                PostScanComplete(IDF_WIFI_STA_DEF);
                break;

            case WIFI_EVENT_STA_AUTHMODE_CHANGE:
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
                PostAvailabilityOn(IDF_WIFI_AP_DEF);
#ifdef PRINT_NET_EVENT
                ets_printf("WIFI_EVENT_AP_START\n");
#endif
                break;

            case WIFI_EVENT_AP_STOP:
                PostAvailabilityOff(IDF_WIFI_AP_DEF);
#ifdef PRINT_NET_EVENT
                ets_printf("WIFI_EVENT_AP_STOP\n");
#endif
                break;

            case WIFI_EVENT_AP_STACONNECTED:
                // access STA connected event
                apConnectedEvent = (wifi_event_ap_staconnected_t *)event_data;
                stationIndex = apConnectedEvent->aid - 1;
                Network_Interface_Add_Station(stationIndex, apConnectedEvent->mac);

                // Post the Network interface + Client ID in top 8 bits
                PostAPStationChanged(1, IDF_WIFI_AP_DEF + (stationIndex << 8));
#ifdef PRINT_NET_EVENT
                ets_printf("WIFI_EVENT_AP_STACONNECTED %d\n", apConnectedEvent->aid);
#endif
                break;

            case WIFI_EVENT_AP_STADISCONNECTED:
                // access STA disconnected event
                apDisconnectedEvent = (wifi_event_ap_stadisconnected_t *)event_data;
                stationIndex = apDisconnectedEvent->aid - 1;

                Network_Interface_Remove_Station(stationIndex);
                PostAPStationChanged(0, IDF_WIFI_AP_DEF + (stationIndex << 8));

#ifdef PRINT_NET_EVENT
                ets_printf("WIFI_EVENT_AP_STADISCONNECTED %d\n", apDisconnectedEvent->aid);
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
                PostAddressChanged(IDF_WIFI_STA_DEF);
                initialize_sntp();
                break;

            case IP_EVENT_STA_LOST_IP:
                PostAddressChanged(IDF_WIFI_STA_DEF);
#ifdef PRINT_NET_EVENT
                ets_printf("IP_EVENT_STA_LOST_IP\n");
#endif
                break;

            case IP_EVENT_ETH_GOT_IP:
#ifdef PRINT_NET_EVENT
                ets_printf("IP_EVENT_ETH_GOT_IP\n");
#endif
                PostAddressChanged(IDF_ETH_DEF);
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
                // get Netif for ETH
                espNetif = esp_netif_get_handle_from_ifkey("ETH_DEF");

                // set host name for ETH
                esp_netif_set_hostname(espNetif, hostName);

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
                PostAvailabilityOn(IDF_ETH_DEF);
                break;

            case ETHERNET_EVENT_DISCONNECTED:
#ifdef PRINT_NET_EVENT
                ets_printf("ETHERNET_EVENT_DISCONNECTED\n");
#endif
                PostAvailabilityOff(IDF_ETH_DEF);
                break;

            default:
                break;
        }
    }
}

void nanoHAL_Network_Initialize()
{
    // Initialise the lwIP CLR signal call-back
    set_signal_sock_function(&sys_signal_sock_event);

    // initialize network interface
    ESP_ERROR_CHECK(esp_netif_init());

    // set hostname
    compose_esp32_hostname();

    // create the default event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // register the handler for WIFI events
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));

    // register the event handler for IP events
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));

#ifdef ESP32_ETHERNET_SUPPORT
    // register the event handler for Ethernet events
    ESP_ERROR_CHECK(esp_event_handler_instance_register(ETH_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));
#endif
}
