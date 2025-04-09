//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// This file includes the board specific Network Initialisation

#include "NF_ESP32_Network.h"
#include "lwIP_Sockets.h"
#include "esp_netif_sntp.h"
#include <target_lwip_sntp_opts.h>
#include "target_platform.h"
#include "esp_mac.h"
#include "esp_eth_com.h"

#if HAL_USE_THREAD == TRUE
#include "../_nanoCLR/nanoFramework.Networking.Thread/net_thread_native.h"
#endif

extern "C" void set_signal_sock_function(void (*funcPtr)());
extern esp_netif_t *WifiStationEspNetif;

#define WIFI_EVENT_TYPE_SCAN_COMPLETE 1

// #define 	PRINT_NET_EVENT 	1

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

static void SignalSocketEvent()
{
    Events_Set(SYSTEM_EVENT_FLAG_SOCKET);
    Events_Set(SYSTEM_EVENT_FLAG_NETWORK);
}

static void PostAddressChanged(uint netIndex)
{
    SignalSocketEvent();
    Network_PostEvent(NetworkChange_NetworkEventType_AddressChanged, 0, netIndex);
}

static void PostAvailabilityOn(uint netIndex)
{
    SignalSocketEvent();
    Network_PostEvent(NetworkChange_NetworkEventType_AvailabilityChanged, 1, netIndex);
}

static void PostAvailabilityOff(uint netIndex)
{
    SignalSocketEvent();
    Network_PostEvent(NetworkChange_NetworkEventType_AvailabilityChanged, 0, netIndex);
}

static void PostScanComplete(uint netIndex)
{
    PostManagedEvent(EVENT_WIFI, WiFiEventType_ScanComplete, 0, netIndex);
}

static void PostAPStationChanged(uint connect, uint netInfo)
{
    SignalSocketEvent();
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

#if HAL_USE_THREAD == TRUE
static void PostThreadEvent(OpenThreadEventType event, uint16_t data1, uint32_t data2)
{
    PostManagedEvent(EVENT_OPENTHREAD, event, data1, data2);
}
#endif

static void initialize_sntp()
{
    esp_sntp_config_t config = {
        .smooth_sync = false,
        .server_from_dhcp = false,
        .wait_for_sync = true,
        .start = true,
        .sync_cb = NULL,
        .renew_servers_after_new_IP = false,
        .ip_event_to_renew = (ip_event_t)0,
        .index_of_first_server = 0,
        .num_of_servers = CONFIG_LWIP_SNTP_MAX_SERVERS};

    config.servers[0] = SNTP_SERVER0_DEFAULT_ADDRESS;

#if (CONFIG_LWIP_SNTP_MAX_SERVERS > 1)
    config.servers[1] = SNTP_SERVER1_DEFAULT_ADDRESS;
#endif

    esp_netif_sntp_init(&config);
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

#if defined(CONFIG_SOC_WIFI_SUPPORTED)
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
#ifdef PRINT_NET_EVENT
                    ets_printf("connect... %d \n", result);
#endif
                }
                break;

            case WIFI_EVENT_STA_CONNECTED:
#ifdef PRINT_NET_EVENT
                ets_printf("WIFI_EVENT_STA_CONNECTED\n");
#endif
#if LWIP_IPV6
                {
                    // Note: Did use esp_netif_create_ip6_linklocal originally but this failed because
                    // it tests for Netif to be up and didn't seem to be. Calling netif_create_ip6_linklocal_address
                    // directly seems to work fine.
                    struct netif *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF")->lwip_netif;
                    netif_create_ip6_linklocal_address(netif, 1);
                }
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
#ifdef PRINT_NET_EVENT
                    ets_printf("connect... %d \n", result);
#endif
                }

                break;

            // Scan of available Wi-Fi networks complete
            case WIFI_EVENT_SCAN_DONE:
#ifdef PRINT_NET_EVENT
                ets_printf("WIFI_EVENT_SCAN_DONE\n");
#endif
                PostScanComplete(IDF_WIFI_STA_DEF);
                break;

#ifdef PRINT_NET_EVENT
            case WIFI_EVENT_STA_AUTHMODE_CHANGE:
                ets_printf("WIFI_EVENT_STA_AUTHMODE_CHANGE");
                break;

            case WIFI_EVENT_STA_WPS_ER_SUCCESS:
                ets_printf("WIFI_EVENT_STA_WPS_ER_SUCCESS\n");
                break;

            case WIFI_EVENT_STA_WPS_ER_FAILED:
                ets_printf("WIFI_EVENT_STA_WPS_ER_FAILED\n");
                break;

            case WIFI_EVENT_STA_WPS_ER_TIMEOUT:
                ets_printf("WIFI_EVENT_STA_WPS_ER_TIMEOUT\n");
                break;

            case WIFI_EVENT_STA_WPS_ER_PIN:
                ets_printf("WIFI_EVENT_STA_WPS_ER_PIN\n");
                break;
#endif

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
    else
#endif
        if (event_base == IP_EVENT)
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

#if LWIP_IPV6
            case IP_EVENT_GOT_IP6:
#ifdef PRINT_NET_EVENT
                ip_event_got_ip6_t *event = (ip_event_got_ip6_t *)event_data;
                esp_ip6_addr_type_t ipv6_type = esp_netif_ip6_get_addr_type(&event->ip6_info.ip);
                ets_printf(
                    "IP_EVENT_ETH_GOT_IP6 type %d Adr %X:%X:%X:%X:%X:%X:%X:%X\n",
                    ipv6_type,
                    IPV62STR(event->ip6_info.ip));
#endif
                break;
#endif
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

#if HAL_USE_THREAD == TRUE
static void thread_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    // Possible events
    // OPENTHREAD_EVENT_START,
    // OPENTHREAD_EVENT_STOP,
    // OPENTHREAD_EVENT_DETACHED,
    // OPENTHREAD_EVENT_ATTACHED,
    // OPENTHREAD_EVENT_ROLE_CHANGED,
    // OPENTHREAD_EVENT_IF_UP,
    // OPENTHREAD_EVENT_IF_DOWN,
    // OPENTHREAD_EVENT_GOT_IP6,
    // OPENTHREAD_EVENT_LOST_IP6,
    // OPENTHREAD_EVENT_MULTICAST_GROUP_JOIN,
    // OPENTHREAD_EVENT_MULTICAST_GROUP_LEAVE,
    // OPENTHREAD_EVENT_TREL_ADD_IP6,
    // OPENTHREAD_EVENT_TREL_REMOVE_IP6,
    // OPENTHREAD_EVENT_TREL_MULTICAST_GROUP_JOIN,
    // OPENTHREAD_EVENT_SET_DNS_SERVER,

    switch (event_id)
    {
        case OPENTHREAD_EVENT_START:
#ifdef PRINT_NET_EVENT
            ets_printf("OPENTHREAD_EVENT_START\n");
#endif
            PostThreadEvent(OpenThreadEventType_StateChanged, 0, event_id);
            break;

        case OPENTHREAD_EVENT_STOP:
#ifdef PRINT_NET_EVENT
            ets_printf("OPENTHREAD_EVENT_STOP\n");
#endif
            PostThreadEvent(OpenThreadEventType_StateChanged, 0, event_id);
            break;

        case OPENTHREAD_EVENT_IF_UP:
#ifdef PRINT_NET_EVENT
            ets_printf("OPENTHREAD_EVENT_IF_UP\n");
#endif
            PostThreadEvent(OpenThreadEventType_StateChanged, 0, event_id);
            break;

        case OPENTHREAD_EVENT_IF_DOWN:
#ifdef PRINT_NET_EVENT
            ets_printf("OPENTHREAD_EVENT_IF_DOWN\n");
#endif
            PostThreadEvent(OpenThreadEventType_StateChanged, 0, event_id);
            break;

        case OPENTHREAD_EVENT_GOT_IP6:
#ifdef PRINT_NET_EVENT
            ets_printf("OPENTHREAD_EVENT_GOT_IP6\n");
#endif
            PostAddressChanged(IDF_OT_DEF);
            PostThreadEvent(OpenThreadEventType_StateChanged, 0, event_id);
            break;

        case OPENTHREAD_EVENT_LOST_IP6:
#ifdef PRINT_NET_EVENT
            ets_printf("OPENTHREAD_EVENT_LOST_IP6\n");
#endif
            PostAddressChanged(IDF_OT_DEF);
            break;

        case OPENTHREAD_EVENT_DETACHED:
#ifdef PRINT_NET_EVENT
            ets_printf("OPENTHREAD_EVENT_DETACHED\n");
#endif
            PostThreadEvent(OpenThreadEventType_StateChanged, 0, event_id);
            PostAvailabilityOff(IDF_OT_DEF);
            break;

        case OPENTHREAD_EVENT_ATTACHED:
#ifdef PRINT_NET_EVENT
            ets_printf("OPENTHREAD_EVENT_ATTACHED\n");
#endif
            PostAvailabilityOn(IDF_OT_DEF);
            PostThreadEvent(OpenThreadEventType_StateChanged, 0, event_id);
            break;

        case OPENTHREAD_EVENT_ROLE_CHANGED:
        {
            esp_openthread_role_changed_event_t *optevent = (esp_openthread_role_changed_event_t *)event_data;
#ifdef PRINT_NET_EVENT
            ets_printf("OPENTHREAD_EVENT_ROLE_CHANGED %d->%d\n", optevent->previous_role, optevent->current_role);
#endif
            PostThreadEvent(
                OpenThreadEventType_RoleChanged,
                0,
                (optevent->previous_role << 8) + optevent->current_role);
        }
        break;

        default:
#ifdef PRINT_NET_EVENT
            ets_printf("Thread Event %d, ID: %d\n", event_base, event_id);
#endif
            break;
    }
}
#endif

void nanoHAL_Network_Initialize()
{
    esp_err_t result;

    // Initialise the lwIP CLR signal call-back
    set_signal_sock_function(&sys_signal_sock_event);

    // initialize network interface
    ESP_ERROR_CHECK(esp_netif_init());

    // set default hostname
    compose_esp32_hostname();

    // create the default event loop if required
    result = esp_event_loop_create_default();

    // If the default event loop is already created (ESP_ERR_INVALID_STATE) then don't need to register
    // This happens when debugging in VS, as it does a warm reboot
    if (result != ESP_ERR_INVALID_STATE)
    {
        ESP_ERROR_CHECK(result);

#if defined(CONFIG_SOC_WIFI_SUPPORTED)
        // register the handler for WIFI events
        ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));
#endif

        // register the event handler for IP events
        ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));

#ifdef ESP32_ETHERNET_SUPPORT
        // register the event handler for Ethernet events
        ESP_ERROR_CHECK(esp_event_handler_instance_register(ETH_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));
#endif

#if HAL_USE_THREAD == TRUE
        // register the event handler for OpenThread events
        ESP_ERROR_CHECK(
            esp_event_handler_instance_register(OPENTHREAD_EVENT, ESP_EVENT_ANY_ID, &thread_event_handler, NULL, NULL));
#endif
    }
}

void nanoHAL_Network_Uninitialize()
{
}
