//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NF_ESP32_NETWORK_H
#define NF_ESP32_NETWORK_H

#include <nanoHAL.h>
#include <lwIP_Sockets.h>
#include <esp32_idf.h>

// ESP IDF 4.0 it's using an abstraction layer (esp_netif) that hides the netif index
// we need that for our interface with lwIP, so we have to clone those here
// not very elegant but will have to work for now
#define IDF_WIFI_STA_DEF    0
#define IDF_WIFI_AP_DEF     1
#define IDF_ETH_DEF         2

extern bool NF_ESP32_ConnectInProgress;
extern int NF_ESP32_ConnectResult;
// flag to signal if connect is to happen
extern bool NF_ESP32_IsToConnect;

// Wireless
int NF_ESP32_Wireless_Open(HAL_Configuration_NetworkInterface *config);
bool NF_ESP32_Wireless_Close();

int NF_ESP32_WirelessAP_Open(HAL_Configuration_NetworkInterface *config);
bool NF_ESP32_WirelessAP_Close();
esp_err_t NF_ESP32_WirelessAP_Configure(HAL_Configuration_NetworkInterface *config);

int NF_ESP32_Wireless_Scan();
int NF_ESP32_Wireless_Disconnect();
int NF_ESP32_Wireless_Start_Connect(HAL_Configuration_Wireless80211 *config);

// Ethernet
int NF_ESP32_Ethernet_Open(HAL_Configuration_NetworkInterface *config);
bool NF_ESP32_Ethernet_Close();

// Smart config
void NF_ESP32_Start_wifi_smart_config(void);

// Helpers
int NF_ESP32_Wait_NetNumber(int num);

#endif // NF_ESP32_NETWORK_H
