//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _NANOHAL_NETWORK_H_
#define _NANOHAL_NETWORK_H_ 1

#include <stdio.h>
#include <nanoPackStruct.h>
#include <nanoWeak.h>

// this is the maximum hardware address length (MAC address)
// setting it to 6 and using the same define that lwIP uses for this
#define NETIF_MAX_HWADDR_LEN 6

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH System.Net.NetworkInformation.Wireless80211Configuration.MaxPasswordLength (in managed code) !!! //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// this is the maximum wireless 802.11 configuration password length
#define WIRELESS82011_CONFIG_MAX_PASSWORD_LEN 64

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH System.Net.NetworkInformation.Wireless80211Configuration.MaxSsidLength (in managed code) !!! //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// this is the maximum wireless 802.11 configuration SSID length
#define WIRELESS82011_CONFIG_MAX_SSID_LEN 32

/////////////////////////////////////////
// network configuration block markers //
/////////////////////////////////////////

// Network configuration block start marker
static const unsigned char c_MARKER_CONFIGURATION_NETWORK_V1[] = "CN1";

// Network configuration block start marker
static const unsigned char c_MARKER_CONFIGURATION_WIRELESS80211_V1[] = "WN1";

// Wireless AP configuration block start marker
static const unsigned char c_MARKER_CONFIGURATION_WIRELESS_AP_V1[] = "AP1";

// X509 certificate configuration block start marker
static const unsigned char c_MARKER_CONFIGURATION_X509CAROOTBUNDLE_V1[] = "XB1";

////////////////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH System.Net.NetworkInformation.NetworkInterfaceType (in managed code) !!! //
////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////
// Description: network interface type 
typedef enum __nfpack NetworkInterfaceType
{
    NetworkInterfaceType_Unknown = 1,
    NetworkInterfaceType_Ethernet = 6,
    NetworkInterfaceType_Wireless80211 = 71,
    NetworkInterfaceType_WirelessAP = 72,
} NetworkInterfaceType;

typedef enum __nfpack Wireless80211Configuration_ConfigurationOptions
{
    Wireless80211Configuration_ConfigurationOptions_None = 0,
    // DISABLE Station or AP
    Wireless80211Configuration_ConfigurationOptions_Disable = 1,
    // Enable Station or AP ( Wifi component started & memory allocated )
    Wireless80211Configuration_ConfigurationOptions_Enable = 2,
    // Wireless station automatically connects on CLR start
    Wireless80211Configuration_ConfigurationOptions_AutoConnect = 4 | Wireless80211Configuration_ConfigurationOptions_Enable,
    // Enable Smart config
    Wireless80211Configuration_ConfigurationOptions_SmartConfig = 8 | Wireless80211Configuration_ConfigurationOptions_Enable,
} Wireless80211Configuration_ConfigurationOptions;

typedef enum __nfpack WirelessAPConfiguration_ConfigurationOptions
{
    WirelessAPConfiguration_ConfigurationOptions_None = 0,
    // DISABLE Wireless AP
    WirelessAPConfiguration_ConfigurationOptions_Disable = 1,
    // Enable Wireless AP ( Wifi component started & memory allocated )
    WirelessAPConfiguration_ConfigurationOptions_Enable = 2,
    // Wireless AP automatically starts on CLR start
    WirelessAPConfiguration_ConfigurationOptions_AutoStart = 4 | WirelessAPConfiguration_ConfigurationOptions_Enable,
    // Wireless AP uses hidden SSID
    WirelessAPConfiguration_ConfigurationOptions_HiddenSSID = 8,
} WirelessAPConfiguration_ConfigurationOptions;

///////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH System.Net.NetworkInformation.AddressMode (in managed code) !!! //
///////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////
// Description: Startup network IP assigning modes
typedef enum __nfpack AddressMode
{
    AddressMode_Invalid = 0,
    AddressMode_DHCP = 1,
    AddressMode_Static = 2,

    ////////////////////////////////////////
    // for this option to be valid 
    // LWIP_AUTOIP (lwIP option) has to be defined
    AddressMode_AutoIP = 3,
} AddressMode;

//////////////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH System.Net.NetworkInformation.AuthenticationType (in managed code) !!! //
//////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Description: authentication used in wireless network
typedef enum __nfpack AuthenticationType
{
    AuthenticationType_None = 0,
    AuthenticationType_EAP = 1,
    AuthenticationType_PEAP = 2,
    AuthenticationType_WCN = 3,
    AuthenticationType_Open = 4,
    AuthenticationType_Shared = 5,
    AuthenticationType_WEP = 6,
    AuthenticationType_WPA = 7,
    AuthenticationType_WPA2 = 8,
} AuthenticationType;

//////////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH System.Net.NetworkInformation.EncryptionType (in managed code) !!! //
//////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Description: encryption used in wireless network
typedef enum __nfpack EncryptionType
{
    EncryptionType_None = 0,
    EncryptionType_WEP = 1,
    EncryptionType_WPA = 2,
    EncryptionType_WPA2 = 3,
    EncryptionType_WPA_PSK = 4,
    EncryptionType_WPA2_PSK = 5,
    EncryptionType_Certificate = 6,
} EncryptionType;

/////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH System.Net.NetworkInformation.RadioType (in managed code) !!! //
/////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Description: type of radio that the wireless network uses
typedef enum __nfpack RadioType
{
    RadioType_NotSpecified = 0,
    RadioType__802_11a = 1,
    RadioType__802_11b = 2,
    RadioType__802_11g = 4,
    RadioType__802_11n = 8,
} RadioType;

typedef struct __nfpack HAL_Configuration_NetworkInterface {

    // this is the marker placeholder for this configuration block
    uint8_t Marker[4];

    // Pointer to MAC address as an array of 6 unsigned bytes.
    uint8_t MacAddress[6];

    // Network IPv4 address as 32-bit unsigned integer
    uint32_t  IPv4Address;

    // Network IPv4 subnet mask as 32-bit unsigned integer
    uint32_t  IPv4NetMask;

    // Network gateway IPv4 address as 32-bit unsigned integer
    uint32_t  IPv4GatewayAddress;

    // DNS server 1 IPv4 address as 32-bit unsigned integer
    uint32_t  IPv4DNSAddress1;

    // DNS server 2 IPv4 address as 32-bit unsigned integer
    uint32_t  IPv4DNSAddress2;

    // Network IPv6 address as an array of 4 32-bit unsigned integers
    uint32_t  IPv6Address[4];

    // Network IPv6 subnet mask as an array of 4 32-bit unsigned integers
    uint32_t  IPv6NetMask[4];

    // Network gateway IPv6 address as an array of 4 32-bit unsigned integers
    uint32_t  IPv6GatewayAddress[4];

    // DNS server 1 IPv6 address as an array of 4 32-bit unsigned integers
    uint32_t  IPv6DNSAddress1[4];

    // DNS server 2 IPv6 address as an array of 4 32-bit unsigned integers
    uint32_t  IPv6DNSAddress2[4];

    // interface type
    NetworkInterfaceType InterfaceType;

    // Specific configuration Id
    // Id of the configuration block specific for this interface type
    // NULL otherwise
    // currently in use:
    // - HAL_Configuration_Wireless80211 for NetworkInterfaceType_Wireless80211
    uint32_t SpecificConfigId;

    // Option to use DNS addresses from DHCP or static ones
    uint8_t AutomaticDNS;

    // Startup network addressing mode - static, DHCP, auto
    AddressMode StartupAddressMode;

} HAL_Configuration_NetworkInterface;

typedef struct __nfpack HAL_Configuration_Wireless80211 {

    // this is the marker placeholder for this configuration block
    uint8_t Marker[4];

    // Id of the configuration
    uint32_t Id;

    // Type of authentication used on the wireless network, For AP is the type of Authenication to use on network.
    AuthenticationType Authentication;

    // Type of encryption used on the wireless network.
    EncryptionType Encryption;

    // Type of radio used by the wireless network adapter.
    RadioType Radio;

    // Network SSID - For AP if no SSID given a default value of nano_xxx where xxx is last part of MAC address
    uint8_t Ssid[32];

    // Network password
    uint8_t Password[64];

    // Wireless options (flags), depends if wireless Station
    Wireless80211Configuration_ConfigurationOptions Options;

     // Rssi, station only

} HAL_Configuration_Wireless80211;

typedef struct __nfpack HAL_Configuration_WirelessAP {

    // this is the marker placeholder for this configuration block
    uint8_t Marker[4];

    // Id of the configuration
    uint32_t Id;

    // Type of authentication used on the wireless network, For AP is the type of Authenication to use on network.
    AuthenticationType Authentication;

    // Type of encryption used on the wireless network.
    EncryptionType Encryption;

    // Type of radio used by the wireless network adapter.
    RadioType Radio;

    // Network SSID - For AP if no SSID given a default value of nano_xxx where xxx is last part of MAC address
    uint8_t Ssid[32];

    // Network password
    uint8_t Password[64];

    // Wireless options for AP (flags)
    WirelessAPConfiguration_ConfigurationOptions Options;

    // channel used for AP
    uint8_t Channel; 

    // Max client connections
    uint8_t MaxConnections;
 
} HAL_Configuration_WirelessAP;

typedef struct __nfpack HAL_Configuration_X509CaRootBundle {

    // this is the marker placeholder for this configuration block
    uint8_t Marker[4];

    // Size of the X509 CA Root certificate bundle
    uint32_t CertificateSize;

    // X509 CA Root certificate bundle
    uint8_t Certificate[1];

} HAL_Configuration_X509CaRootBundle;


void 	nanoHAL_Network_Initialize();
void    sys_signal_sock_event();


#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif //_NANOHAL_NETWORK_H_
