//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _NF_WIRELESS_H_
#define _NF_WIRELESS_H_

#include <nanoHAL.h>
#include <nf_netxduo.h>

extern TX_QUEUE WiFiConnectQueue;
extern uint32_t WiFiConnectQueueStorage;
extern TX_MUTEX WiFiMutex;

extern bool NF_ConnectInProgress;
extern int NF_ConnectResult;
extern bool NF_IsToConnect;

// event flags for WiFi event group
#define WIFI_REQUEST_CONNECT_STATION (uint32_t)0x80000000
#define WIFI_REQUEST_STOP_STATION (uint32_t)0x40000000
#define WIFI_REQUEST_MASK (WIFI_REQUEST_CONNECT_STATION | WIFI_REQUEST_STOP_STATION)


#ifdef __cplusplus
 extern "C" {
#endif

bool NF_Wireless_Start_Connect(HAL_Configuration_Wireless80211 *config);
int NF_Wireless_Open(HAL_Configuration_NetworkInterface *config);
bool NF_Wireless_Close();

#ifdef __cplusplus
}
#endif

#endif // _NF_WIRELESS_H_
