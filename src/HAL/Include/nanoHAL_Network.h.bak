//
// Copyright (c) 2017 The nanoFramework project contributors
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
    NetworkInterfaceType_Unknown        = 1,
    NetworkInterfaceType_Ethernet       = 6,
    NetworkInterfaceType_Wireless80211  = 71,

}NetworkInterfaceType;

///////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH System.Net.NetworkInformation.AddressMode (in managed code) !!! //
///////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////
// Description: Startup network IP assigning modes
typedef enum __nfpack AddressMode
{
    AddressMode_DHCP       = 0x01,
    AddressMode_Static     = 0x02,

    ////////////////////////////////////////
    // for this option to be valid 
    // LWIP_AUTOIP (lwIP option) has to be defined
    AddressMode_AutoIP     = 0x03
    
}AddressMode;

//////////////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH System.Net.NetworkInformation.AuthenticationType (in managed code) !!! //
//////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Description: authentication used in wireless network
typedef enum __nfpack AuthenticationType
{
    AuthenticationType_None     = 0x00,
    AuthenticationType_EAP      = 0x01,
    AuthenticationType_PEAP     = 0x02,
    AuthenticationType_WCN      = 0x03,
    AuthenticationType_Open     = 0x04,
    AuthenticationType_Shared   = 0x05,
    AuthenticationType_WEP      = 0x06,
    AuthenticationType_WPA      = 0x07,
    AuthenticationType_WPA2     = 0x08,
    
}AuthenticationType;

//////////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH System.Net.NetworkInformation.EncryptionType (in managed code) !!! //
//////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Description: encryption used in wireless network
typedef enum __nfpack EncryptionType
{
    EncryptionType_None         = 0x00,
    EncryptionType_WEP          = 0x01,
    EncryptionType_WPA          = 0x02,
    EncryptionType_WPA2         = 0x03,
    EncryptionType_WPA_PSK      = 0x04,
    EncryptionType_WPA2_PSK     = 0x05,
    EncryptionType_Certificate  = 0x06,
    
}EncryptionType;

/////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH System.Net.NetworkInformation.RadioType (in managed code) !!! //
/////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Description: type of radio that the wireless network uses
typedef enum __nfpack RadioType
{
    RadioType_NotSpecified  = 0x00,
    RadioType_802_11a       = 0x01,
    RadioType_802_11b       = 0x02,
    RadioType_802_11g       = 0x03,
    RadioType_802_11n       = 0x04,
}RadioType;

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

    // Type of authentication used on the wireless network
    AuthenticationType Authentication;

    // Type of encryption used on the wireless network.
    EncryptionType Encryption;

    // Type of radio used by the wireless network adapter.
    RadioType Radio;

    // Network SSID
    uint8_t Ssid[32];

    // Network password
    uint8_t Password[64];

} HAL_Configuration_Wireless80211;

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
