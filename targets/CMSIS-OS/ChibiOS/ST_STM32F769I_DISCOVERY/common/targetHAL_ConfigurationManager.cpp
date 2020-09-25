//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoHAL_v2.h>
#include <nanoWeak.h>
#include <Target_BlockStorage_STM32FlashDriver.h>

// Default initialisation for Network interface config blocks
// strong implementation replacing ChibiOS 'weak' one
bool InitialiseNetworkDefaultConfig(HAL_Configuration_NetworkInterface * pconfig, uint32_t configurationIndex)
{
    (void)configurationIndex;

    // make sure the config block marker is set
    memcpy(pconfig->Marker, c_MARKER_CONFIGURATION_NETWORK_V1, sizeof(c_MARKER_CONFIGURATION_NETWORK_V1));
    
    pconfig->InterfaceType = NetworkInterfaceType_Ethernet;
    pconfig->StartupAddressMode = AddressMode_DHCP;
    pconfig->AutomaticDNS = 1;
    pconfig->SpecificConfigId = 0;

    // set MAC address with ST provided MAC for development boards
    // 00:80:E1:01:35:D1
    pconfig->MacAddress[0] = 0x00;
    pconfig->MacAddress[1] = 0x80;
    pconfig->MacAddress[2] = 0xE1;
    pconfig->MacAddress[3] = 0x01;
    pconfig->MacAddress[4] = 0x35;
    pconfig->MacAddress[5] = 0xD1;

    return true;
}
