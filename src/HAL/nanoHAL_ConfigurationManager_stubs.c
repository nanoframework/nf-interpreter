//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <string.h>
#include <nanoHAL_v2.h>

__nfweak void ConfigurationManager_Initialize()
{
};

__nfweak void* ConfigurationManager_FindNetworkConfigurationBlocks(uint32_t startAddress, uint32_t endAddress)
{
    (void)startAddress;
    (void)endAddress;

    return NULL;
}

__nfweak void* ConfigurationManager_FindNetworkWireless80211ConfigurationBlocks(uint32_t startAddress, uint32_t endAddress)
{
    (void)startAddress;
    (void)endAddress;

    return NULL;
}

__nfweak void* ConfigurationManager_FindX509CertificateConfigurationBlocks(uint32_t startAddress, uint32_t endAddress)
{
    (void)startAddress;
    (void)endAddress;

    return NULL;
}

__nfweak HAL_Configuration_Wireless80211* ConfigurationManager_GetWirelessConfigurationFromId(uint32_t configurationId)
{
    (void)configurationId;

    return NULL;
}

__nfweak HAL_Configuration_WirelessAP* ConfigurationManager_GetWirelessAPConfigurationFromId(uint32_t configurationId)
{
    (void)configurationId;

    return NULL;
}
