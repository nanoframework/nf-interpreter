//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// WiFi-aware lwIP thread header for ChibiOS + CYW43.
// Replaces nf_lwipthread.h when using WiFi instead of Ethernet MAC.

#ifndef NF_LWIPTHREAD_WIFI_H
#define NF_LWIPTHREAD_WIFI_H

#include <nf_lwipthread.h>

// Re-export the standard lwIP thread types and API.
// The WiFi variant uses the same lwipthread_opts_t and API,
// but the underlying transport uses CYW43 instead of ETHD1 MAC.

#ifdef __cplusplus
extern "C"
{
#endif

    // WiFi-specific initialization — powers on CYW43 and configures the driver
    void cyw43_wifi_power_on(void);
    void cyw43_wifi_power_off(void);

    // Lazy hardware init — call before any WiFi operation
    int cyw43_ensure_wifi_up_impl(void);

    // WiFi connection management — called from Target_Network.cpp
    int cyw43_wifi_connect(const char *ssid, const char *password, uint32_t auth_type);
    int cyw43_wifi_disconnect(void);
    int cyw43_wifi_scan_start(void);
    int cyw43_wifi_is_connected(void);
    int cyw43_wifi_get_link_status(void);
    int cyw43_wifi_get_join_state(void);
    uint32_t cyw43_wifi_get_ip4_address(void);

    // Direct poll — call from NativeConnect wait loop to drain SPI packets
    uint32_t cyw43_wifi_direct_poll(void);

    // Set the lwIP netif MAC address (call after CYW43 chip is initialized)
    void cyw43_wifi_set_netif_mac(const uint8_t *mac);

    // Scan results
    typedef struct
    {
        uint8_t bssid[6];
        uint8_t ssid[33];
        int16_t rssi;
        uint8_t auth_mode;
        uint16_t channel;
    } wifi_scan_record_t;

    int cyw43_wifi_scan_get_count(void);
    const wifi_scan_record_t *cyw43_wifi_scan_get_results(void);

    // Access CYW43 MAC address (read from OTP on chip)
    void cyw43_wifi_get_mac_addr(uint8_t *mac);

    // Accessor for the WiFi netif
    struct netif *nf_getNetif(void);

#ifdef __cplusplus
}
#endif

#endif // NF_LWIPTHREAD_WIFI_H
