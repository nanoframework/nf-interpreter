//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoHAL_v2.h>
#include <nanoWeak.h>
#include <nanoHAL_Network.h>

// Strong implementation replacing ChibiOS 'weak' InitialiseNetworkDefaultConfig.
// Creates a default WiFi (802.11) network configuration for the onboard Inventek
// ISM43362 (ES-WIFI) module.
bool InitialiseNetworkDefaultConfig(HAL_Configuration_NetworkInterface *pconfig, uint32_t configurationIndex)
{
    (void)configurationIndex;

    // zero memory
    memset(pconfig, 0, sizeof(HAL_Configuration_NetworkInterface));

    // make sure the config block marker is set
    memcpy(pconfig->Marker, c_MARKER_CONFIGURATION_NETWORK_V1, sizeof(c_MARKER_CONFIGURATION_NETWORK_V1));

    // WiFi station interface
    pconfig->InterfaceType = NetworkInterfaceType_Wireless80211;
    pconfig->StartupAddressMode = AddressMode_DHCP;
    pconfig->AutomaticDNS = 1;
    pconfig->SpecificConfigId = 0;

    // MAC address is read from the module at runtime (see
    // SOCK_CONFIGURATION_LoadAdapterConfiguration in sockets_ism43362.cpp); leave as zeros here.

    return true;
}

// Strong implementation replacing ChibiOS 'weak' InitialiseWirelessDefaultConfig.
// Creates a default Wireless80211 configuration block for this board.
void InitialiseWirelessDefaultConfig(HAL_Configuration_Wireless80211 *pconfig, uint32_t configurationIndex)
{
    memset(pconfig, 0, sizeof(HAL_Configuration_Wireless80211));

    // set config block marker
    memcpy(pconfig, c_MARKER_CONFIGURATION_WIRELESS80211_V1, sizeof(c_MARKER_CONFIGURATION_WIRELESS80211_V1));

    pconfig->Id = configurationIndex;

    // Enable auto-connect so managed code can store credentials and reconnect
    pconfig->Options =
        (Wireless80211Configuration_ConfigurationOptions)(Wireless80211Configuration_ConfigurationOptions_AutoConnect |
                                                          Wireless80211Configuration_ConfigurationOptions_Enable);
}
