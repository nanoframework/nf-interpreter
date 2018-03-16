//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _NANOHAL_NETWORK_H_
#define _NANOHAL_NETWORK_H_ 1

#include <nanoPackStruct.h>
#include <nanoWeak.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH DeviceConfiguration.NetworkConfigurationProperties.AddressMode (in managed code) !!! //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////
// Description: Startup network assigning modes
typedef enum AddressMode
{
    AddressMode_DHCP       = 0x01,
    AddressMode_Static     = 0x02,

    ////////////////////////////////////////
    // for this option to be valid 
    // LWIP_AUTOIP (lwIP option) has to be defined
    AddressMode_AutoIP     = 0x03
    
}AddressMode;

typedef struct __nfpack Configuration_Network {

    // this is the marker placeholder for this configuration block
    unsigned char Marker[4];

    // Pointer to MAC address as an array of 6 unsigned bytes.
    unsigned char MacAddress[6];

    // Network IPv4 address as 32-bit unsigned integer
    unsigned int  IPv4Address;

    // Network IPv4 subnet mask as 32-bit unsigned integer
    unsigned int  IPv4NetMask;

    // Network gateway IPv4 address as 32-bit unsigned integer
    unsigned int  IPv4GatewayAddress;

    // DNS server 1 IPv4 address as 32-bit unsigned integer
    unsigned int  IPv4DNS1Address;

    // DNS server 2 IPv4 address as 32-bit unsigned integer
    unsigned int  IPv4DNS2Address;

    // Network IPv6 address as an array of 4 32-bit unsigned integers
    unsigned int  IPv6Address[4];

    // Network IPv6 subnet mask as an array of 4 32-bit unsigned integers
    unsigned int  IPv6NetMask[4];

    // Network gateway IPv6 address as an array of 4 32-bit unsigned integers
    unsigned int  IPv6GatewayAddress[4];

    // DNS server 1 IPv6 address as an array of 4 32-bit unsigned integers
    unsigned int  IPv6DNS1Address[4];

    // DNS server 2 IPv6 address as an array of 4 32-bit unsigned integers
    unsigned int  IPv6DNS2Address[4];

    // Startup network addressing mode - static, DHCP, auto
    AddressMode StartupAddressMode;

}Configuration_Network;


// Callback for when init done
typedef void(* tcpip_init_done_fn) (void *arg);

void 	nanoHAL_Network_Initialize(tcpip_init_done_fn initfunc);

#ifdef __cplusplus
extern "C" {
#endif

// GetConfigurationNetwork() is defined in targetHAL_Network.cpp at target level because the targets 
// need to be free to implement the storage of the configuration block as they see fit
__nfweak bool GetConfigurationNetwork(Configuration_Network* configurationNetwork);

// StoreConfigurationNetwork() is defined in targetHAL_Network.cpp at target level because the targets 
// need to be free to implement the storage of the configuration block as they see fit
__nfweak bool StoreConfigurationNetwork(Configuration_Network* configurationNetwork);

#ifdef __cplusplus
}
#endif

#endif //_NANOHAL_NETWORK_H_
