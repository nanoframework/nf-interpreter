//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _NETWORK_DECL_H_
#define _NETWORK_DECL_H_ 1


#ifdef PLATFORM_DEPENDENT__SOCKETS_MAX_COUNT
#define SOCKETS_MAX_COUNT                PLATFORM_DEPENDENT__SOCKETS_MAX_COUNT
#else
#define SOCKETS_MAX_COUNT                1 // required to have at least one for array init
#endif

#ifdef PLATFORM_DEPENDENT__SOCKETS_MAX_SEND_LENGTH
#define SOCKETS_MAX_SEND_LENGTH         PLATFORM_DEPENDENT__SOCKETS_MAX_SEND_LENGTH
#endif

#if !defined(DEBGGER_NETWORK_INTERFACE_INDEX)
#define DEBGGER_NETWORK_INTERFACE_INDEX  0
#endif

#if !defined(NETWORK_INTERFACE_COUNT)
#define NETWORK_INTERFACE_COUNT          1
#endif

#if !defined(WIRELESS_INTERFACE_COUNT)
#define WIRELESS_INTERFACE_COUNT          1
#endif

struct SOCK_NetworkConfiguration
{  
    /// Bits 0-15 regular flags.
    ///     16-19 Type
    ///     20-23 SubIndex.
    uint32_t flags;

    uint32_t ipaddr;
    uint32_t subnetmask;
    uint32_t gateway;
    uint32_t dnsServer1;
    uint32_t dnsServer2;
    uint32_t networkInterfaceType;
    uint32_t macAddressLen;
    uint8_t  macAddressBuffer[64];
};

struct NETWORK_CONFIG
{
    HAL_DRIVER_CONFIG_HEADER Header;

    //--//

    int                       NetworkInterfaceCount;
    SOCK_NetworkConfiguration NetworkInterfaces[NETWORK_INTERFACE_COUNT];
    
    //--//

    static LPCSTR GetDriverName() { return "NETWORK"; }
};

#define WIRELESS_PASSPHRASE_LENGTH             64
#define WIRELESS_NETWORKKEY_LENGTH             256
#define WIRELESS_REKEYINTERNAL_LENGTH          32
#define WIRELESS_SSID_LENGTH                   32

struct SOCK_WirelessConfiguration
{  
    /// Bits 0-3: Authetication, 4-7: Encryption, 8-11: Radio.
    uint32_t        wirelessFlags;
    char          passPhrase[WIRELESS_PASSPHRASE_LENGTH];
    uint32_t        networkKeyLength;
    uint8_t         networkKey[WIRELESS_NETWORKKEY_LENGTH];
    uint32_t        reKeyLength;
    uint8_t         reKeyInternal[WIRELESS_REKEYINTERNAL_LENGTH];
    char          ssid[WIRELESS_SSID_LENGTH];
};


struct WIRELESS_CONFIG
{
    HAL_DRIVER_CONFIG_HEADER   Header;

    //--//

    int                        WirelessInterfaceCount;
    SOCK_WirelessConfiguration WirelessInterfaces[WIRELESS_INTERFACE_COUNT];
    
    //--//

    static LPCSTR GetDriverName() { return "WIRELESS"; }
};

#endif //_NETWORK_DECL_H_
