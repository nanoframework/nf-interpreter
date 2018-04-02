//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanohal.h>

//--//

//
// Default Network settings
//
NETWORK_CONFIG g_NetworkConfig =
{
    { TRUE },
#if ESP32_ETHERNET_SUPPORT
    3,      // interface count
#else
    2,
#endif
    {   
        //  TCPIP_ADAPTER_IF_STA - Wireless Station interface   
        {                                      
            SOCK_NETWORKCONFIGURATION_FLAGS_TYPE__set(SOCK_NETWORKCONFIGURATION_FLAGS_WIRELESS) | 
            SOCK_NETWORKCONFIGURATION_FLAGS_DHCP  |              /*flags  wireless + DHCP*/ 
            SOCK_NETWORKCONFIGURATION_FLAGS_SUBINDEX__set(0),    /* Index to wireless config */  
            SOCK_MAKE_IP_ADDR_LITTLEEND(  0,  0,  0,  0), /*ip address */  
            SOCK_MAKE_IP_ADDR_LITTLEEND(  0,  0,  0,  0), /*subnet mask*/  
            SOCK_MAKE_IP_ADDR_LITTLEEND(  0,  0,  0,  0), /*gateway    */  
            SOCK_MAKE_IP_ADDR_LITTLEEND(  0,  0,  0,  0), /*dns1       */  
            SOCK_MAKE_IP_ADDR_LITTLEEND(  0,  0,  0,  0), /*dns2       */  
            SOCK_NETWORKCONFIGURATION_INTERFACETYPE_WIRELESS_80211,        /* Change to SOCK_NETWORKCONFIGURATION_INTERFACETYPE_WIRELESS_80211 for wireless */ 
            6,              /*mac address length*/                  
            {               /*mac address*/                         
                0x00, 0x50, 0xC2, 0xF8, 0x90, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
            }                                                       
        },                                                          
        //  TCPIP_ADAPTER_IF_AP - Wireless soft-AP interface   
        {                                      
            SOCK_NETWORKCONFIGURATION_FLAGS_TYPE__set(SOCK_NETWORKCONFIGURATION_FLAGS_WIRELESS) |   /*wireless flag */ 
            SOCK_NETWORKCONFIGURATION_FLAGS_SUBINDEX__set(1),    /* Index to wireless config */  
            SOCK_MAKE_IP_ADDR_LITTLEEND(  192,  168,  4,  1), /*ip address */  
            SOCK_MAKE_IP_ADDR_LITTLEEND(  255,  255,  255,  0), /*subnet mask*/  
            SOCK_MAKE_IP_ADDR_LITTLEEND(  192,  168,  4,  1), /*gateway    */  
            SOCK_MAKE_IP_ADDR_LITTLEEND(  0,  0,  0,  0), /*dns1       */  
            SOCK_MAKE_IP_ADDR_LITTLEEND(  0,  0,  0,  0), /*dns2       */  
            SOCK_NETWORKCONFIGURATION_INTERFACETYPE_WIRELESS_80211,        /* Change to SOCK_NETWORKCONFIGURATION_INTERFACETYPE_WIRELESS_80211 for wireless */ 
            6,              /*mac address length*/                  
            {               /*mac address*/                         
                0x00, 0x50, 0xC2, 0xF8, 0x90, 0x06, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
            }                                                       
        },                                                          
#if ESP32_ETHERNET_SUPPORT
       {                                      
            SOCK_NETWORKCONFIGURATION_FLAGS_TYPE__set(SOCK_NETWORKCONFIGURATION_FLAGS_NETWORK_INTERFACE) | 
            SOCK_NETWORKCONFIGURATION_FLAGS_DHCP,         /*flags */  
            SOCK_MAKE_IP_ADDR_LITTLEEND(  0,  0,  0,  0), /*ip address */  
            SOCK_MAKE_IP_ADDR_LITTLEEND(  0,  0,  0,  0), /*subnet mask*/  
            SOCK_MAKE_IP_ADDR_LITTLEEND(  0,  0,  0,  0), /*gateway    */  
            SOCK_MAKE_IP_ADDR_LITTLEEND(  0,  0,  0,  0), /*dns1       */  
            SOCK_MAKE_IP_ADDR_LITTLEEND(  0,  0,  0,  0), /*dns2       */  
            SOCK_NETWORKCONFIGURATION_INTERFACETYPE_ETHERNET,        /* Change to SOCK_NETWORKCONFIGURATION_INTERFACETYPE_WIRELESS_80211 for wireless */ 
            6,              /*mac address length*/                  
            {               /*mac address*/                         
                0x00, 0x50, 0xC2, 0xF8, 0x90, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
            }                                                       
        },                                                          
#endif
    },
};


WIRELESS_CONFIG g_WirelessConfig = 
{
    { TRUE },  // Enable
    2,         // Interface count
    {
        {
            WIRELESS_FLAG_AUTHENTICATION__set(WIRELESS_FLAG_AUTHENTICATION_EAP)              |
            WIRELESS_FLAG_ENCRYPTION__set    (WIRELESS_FLAG_ENCRYPTION_WPA)                  |
            WIRELESS_FLAG_RADIO__set         (WIRELESS_FLAG_RADIO_b | WIRELESS_FLAG_RADIO_g),

            {
                '\0',
            },
            8,  // Network key length
            {   // Netork key
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
            },
            0,  // reKeyLength;
            {   // reKeyInternal
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            },
            {   // ssid
                '\0',
            },
        },
        {
            WIRELESS_FLAG_AUTHENTICATION__set(WIRELESS_FLAG_AUTHENTICATION_EAP)              |
            WIRELESS_FLAG_ENCRYPTION__set    (WIRELESS_FLAG_ENCRYPTION_WPA)                  |
            WIRELESS_FLAG_RADIO__set         (WIRELESS_FLAG_RADIO_b | WIRELESS_FLAG_RADIO_g),

            {
                'P','A','S','S','W','O','R','D','\0',
            },
            8,  // Network key length
            {   // Netork key
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
            },
            0,  // reKeyLength;
            {   // reKeyInternal
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            },
            {   // ssid
                'S','S','I','D','_','2','\0',
            },
        },
    },
};


