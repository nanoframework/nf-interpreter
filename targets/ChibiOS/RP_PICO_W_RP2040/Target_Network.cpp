//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// WiFi-aware Target_Network for ChibiOS + CYW43 (Pico W).
// This replaces the Ethernet-based _common/Target_Network.cpp.

#include <nanoHAL.h>
#include <lwip/netifapi.h>

extern "C" {
#include <nf_lwipthread_wifi.h>
}

// Start scan outcome codes (must match nanoPAL_Sockets.h)
#define StartScanOutcome_Success                0
#define StartScanOutcome_FailedToGetConfiguration 1
#define StartScanOutcome_WrongInterfaceType     2

bool Network_Interface_Bind(int index)
{
    (void)index;
    return true;
}

int Network_Interface_Open(int index)
{
    HAL_Configuration_NetworkInterface networkConfiguration;

    // load network interface configuration from storage
    if (!ConfigurationManager_GetConfigurationBlock(
            (void *)&networkConfiguration,
            DeviceConfigurationOption_Network,
            index))
    {
        return SOCK_SOCKET_ERROR;
    }
    _ASSERTE(networkConfiguration.StartupAddressMode > 0);

    switch (index)
    {
        case 0:
        {
            struct netif *nptr = nf_getNetif();
            return nptr->num;
        }
        break;
    }
    return SOCK_SOCKET_ERROR;
}

bool Network_Interface_Close(int index)
{
    switch (index)
    {
        case 0:
            cyw43_wifi_disconnect();
            return true;
    }
    return false;
}

int Network_Interface_Disconnect(int index)
{
    (void)index;
    return cyw43_wifi_disconnect();
}

int Network_Interface_Start_Connect(int index, const char *ssid, const char *passphrase, int options)
{
    (void)index;
    (void)options;

    uint32_t auth_type = 0;
    if (passphrase != NULL && passphrase[0] != '\0')
    {
        auth_type = 0x00400004; // WPA2_AES_PSK
    }

    return cyw43_wifi_connect(ssid, passphrase, auth_type);
}

int Network_Interface_Connect_Result(int configIndex)
{
    (void)configIndex;
    return cyw43_wifi_is_connected() ? 0 : -1;
}

int Network_Interface_Start_Scan(int index)
{
    (void)index;

    HAL_Configuration_NetworkInterface networkConfiguration;

    if (!ConfigurationManager_GetConfigurationBlock(
            (void *)&networkConfiguration,
            DeviceConfigurationOption_Network,
            0))
    {
        return StartScanOutcome_FailedToGetConfiguration;
    }

    if (networkConfiguration.InterfaceType != NetworkInterfaceType_Wireless80211)
    {
        return StartScanOutcome_WrongInterfaceType;
    }

    int result = cyw43_wifi_scan_start();
    return result == 0 ? StartScanOutcome_Success : result;
}
