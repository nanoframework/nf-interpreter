//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoHAL_v2.h>
#include "Target_BlockStorage_iMXRTFlashDriver.h"

// initialization of configuration manager
void ConfigurationManager_Initialize()
{
    // enumerate the blocks
    ConfigurationManager_EnumerateConfigurationBlocks();
};

// Enumerates the configuration blocks from the configuration flash sector 
void ConfigurationManager_EnumerateConfigurationBlocks()
{
    // start checking if this device has config block
    if(((uint32_t)&__nanoConfig_end__ - (uint32_t)&__nanoConfig_start__) > 0)
    {
        // find network configuration blocks
        HAL_CONFIGURATION_NETWORK* networkConfigs = (HAL_CONFIGURATION_NETWORK*)ConfigurationManager_FindNetworkConfigurationBlocks((uint32_t)&__nanoConfig_start__, (uint32_t)&__nanoConfig_end__);

        // find wireless 80211 network configuration blocks
        HAL_CONFIGURATION_NETWORK_WIRELESS80211* networkWirelessConfigs = (HAL_CONFIGURATION_NETWORK_WIRELESS80211*)ConfigurationManager_FindNetworkWireless80211ConfigurationBlocks((uint32_t)&__nanoConfig_start__, (uint32_t)&__nanoConfig_end__);

        // find X509 certificate blocks
        HAL_CONFIGURATION_X509_CERTIFICATE* certificateStore = (HAL_CONFIGURATION_X509_CERTIFICATE*)ConfigurationManager_FindX509CertificateConfigurationBlocks((uint32_t)&__nanoConfig_start__, (uint32_t)&__nanoConfig_end__);

        // alloc memory for g_TargetConfiguration
        // because this is a struct of structs that use flexible members the memory has to be allocated from the heap
        // the malloc size for each struct is computed separately 
        uint32_t sizeOfNetworkInterfaceConfigs = offsetof(HAL_CONFIGURATION_NETWORK, Configs) + networkConfigs->Count * sizeof(networkConfigs->Configs[0]);
        uint32_t sizeOfWireless80211Configs = offsetof(HAL_CONFIGURATION_NETWORK_WIRELESS80211, Configs) + networkWirelessConfigs->Count * sizeof(networkWirelessConfigs->Configs[0]);
        uint32_t sizeOfX509CertificateStore = offsetof(HAL_CONFIGURATION_X509_CERTIFICATE, Certificates) + certificateStore->Count * sizeof(certificateStore->Certificates[0]);

        g_TargetConfiguration.NetworkInterfaceConfigs = (HAL_CONFIGURATION_NETWORK*)platform_malloc(sizeOfNetworkInterfaceConfigs);
        g_TargetConfiguration.Wireless80211Configs = (HAL_CONFIGURATION_NETWORK_WIRELESS80211*)platform_malloc(sizeOfWireless80211Configs);
        g_TargetConfiguration.CertificateStore = (HAL_CONFIGURATION_X509_CERTIFICATE*)platform_malloc(sizeOfX509CertificateStore);

        // copy structs to g_TargetConfiguration
        memcpy((HAL_CONFIGURATION_NETWORK*)g_TargetConfiguration.NetworkInterfaceConfigs, networkConfigs, sizeOfNetworkInterfaceConfigs);
        memcpy((HAL_CONFIGURATION_NETWORK_WIRELESS80211*)g_TargetConfiguration.Wireless80211Configs, networkWirelessConfigs, sizeOfWireless80211Configs);
        memcpy((HAL_CONFIGURATION_X509_CERTIFICATE*)g_TargetConfiguration.CertificateStore, certificateStore, sizeOfX509CertificateStore);

        // now free the memory of the original structs
        platform_free(networkConfigs);
        platform_free(networkWirelessConfigs);
        platform_free(certificateStore);
    }
    else
    {
        // no config block
    }
}

// Gets the network configuration block from the configuration flash sector 
bool ConfigurationManager_GetConfigurationBlock(void* configurationBlock, DeviceConfigurationOption configuration, uint32_t configurationIndex)
{
    int sizeOfBlock = 0;
    uint8_t* blockAddress = NULL;

    // validate if the requested block exists
    // Count has to be non zero
    // requested Index has to exist (array index starts at zero, so need to add one)
    if(configuration == DeviceConfigurationOption_Network)
    {
        if(g_TargetConfiguration.NetworkInterfaceConfigs->Count == 0)
        {
            // there is no network config block, init one with default settings
            if(!InitialiseNetworkDefaultConfig(NULL, 0))
            {
                return FALSE;
            }
        }
        else
        {
            if((configurationIndex + 1) > g_TargetConfiguration.NetworkInterfaceConfigs->Count)
            {
                return FALSE;
            }
        }

        // set block size
        sizeOfBlock = sizeof(HAL_Configuration_NetworkInterface);

        // get block address
        blockAddress = (uint8_t*)g_TargetConfiguration.NetworkInterfaceConfigs->Configs[configurationIndex];
    }
    else if(configuration == DeviceConfigurationOption_Wireless80211Network)
    {
        if(g_TargetConfiguration.Wireless80211Configs->Count == 0 ||
            (configurationIndex + 1) > g_TargetConfiguration.Wireless80211Configs->Count)
        {
            return FALSE;
        }

        // set block size
        sizeOfBlock = sizeof(HAL_Configuration_Wireless80211);

        // get block address
        blockAddress = (uint8_t*)g_TargetConfiguration.Wireless80211Configs->Configs[configurationIndex];
    }
    else if(configuration == DeviceConfigurationOption_X509CaRootBundle)
    {
        if(g_TargetConfiguration.CertificateStore->Count == 0 ||
            (configurationIndex + 1) > g_TargetConfiguration.CertificateStore->Count)
        {
            return FALSE;
        }

        // get block address
        blockAddress = (uint8_t*)g_TargetConfiguration.CertificateStore->Certificates[configurationIndex];

        // set block size
        // because X509 certificate has a variable length need to compute the block size in two steps
        sizeOfBlock = offsetof(HAL_Configuration_X509CaRootBundle, Certificate);
        sizeOfBlock += ((HAL_Configuration_X509CaRootBundle*)blockAddress)->CertificateSize;
    }

    // copy the config block content to the pointer in the argument
    memcpy(configurationBlock, blockAddress, sizeOfBlock);

    return TRUE;
}

// Stores the configuration block to the configuration flash sector
// NOTE: because inserting or removing a configuration block it's very 'RAM expensive' we choose not to support those operations
// the host debugger will have to be used to manage these operations on the device configuration collection 
bool ConfigurationManager_StoreConfigurationBlock(void* configurationBlock, DeviceConfigurationOption configuration, uint32_t configurationIndex, uint32_t blockSize, uint32_t offset, bool done)
{
    ByteAddress storageAddress = 0;
    bool requiresEnumeration = FALSE;
    bool success = FALSE;

    if(configuration == DeviceConfigurationOption_Network)
    {
        if( g_TargetConfiguration.NetworkInterfaceConfigs->Count == 0 ||
            (configurationIndex + 1) > g_TargetConfiguration.NetworkInterfaceConfigs->Count)
        {
            // there is no room for this block, or there are no blocks stored at all
            // failing the operation
            return FALSE;
        }

        // set storage address from block address, plus the requested offset
        storageAddress = (ByteAddress)g_TargetConfiguration.NetworkInterfaceConfigs->Configs[configurationIndex] + offset;

        // set block size, in case it's not already set
        blockSize = sizeof(HAL_Configuration_NetworkInterface);

        // make sure the config block marker is set
        memcpy(configurationBlock, c_MARKER_CONFIGURATION_NETWORK_V1, sizeof(c_MARKER_CONFIGURATION_NETWORK_V1)); 
    
        _ASSERTE(((HAL_Configuration_NetworkInterface*)configurationBlock)->StartupAddressMode > 0);
    }
    else if(configuration == DeviceConfigurationOption_Wireless80211Network)
    {
        if( g_TargetConfiguration.Wireless80211Configs->Count == 0 ||
            (configurationIndex + 1) > g_TargetConfiguration.Wireless80211Configs->Count)
        {
            // there is no room for this block, or there are no blocks stored at all
            // failing the operation
            return FALSE;
        }

        // set storage address from block address, plus the requested offset
        storageAddress = (ByteAddress)g_TargetConfiguration.Wireless80211Configs->Configs[configurationIndex] + offset;

        // set block size, in case it's not already set
        blockSize = sizeof(HAL_Configuration_Wireless80211);

        // make sure the config block marker is set
        memcpy(configurationBlock, c_MARKER_CONFIGURATION_WIRELESS80211_V1, sizeof(c_MARKER_CONFIGURATION_WIRELESS80211_V1));        
    }
    else if(configuration == DeviceConfigurationOption_X509CaRootBundle)
    {
        if( g_TargetConfiguration.CertificateStore->Count == 0 ||
            (configurationIndex + 1) > g_TargetConfiguration.CertificateStore->Count)
        {
            // there is no room for this block, or there are no blocks stored at all
            // failing the operation
            return FALSE;
        }

        // set storage address from block address, plus the requested offset
        storageAddress = (ByteAddress)g_TargetConfiguration.CertificateStore->Certificates[configurationIndex] + offset;

        // set block size, in case it's not already set
        // because X509 certificate has a variable length need to compute the block size in two steps
        blockSize = offsetof(HAL_Configuration_X509CaRootBundle, Certificate);
        blockSize += ((HAL_Configuration_X509CaRootBundle*)configurationBlock)->CertificateSize;

        // make sure the config block marker is set
        memcpy(configurationBlock, c_MARKER_CONFIGURATION_X509CAROOTBUNDLE_V1, sizeof(c_MARKER_CONFIGURATION_X509CAROOTBUNDLE_V1));        
    }
    else if(configuration == DeviceConfigurationOption_All)
    {
        // particular situation where we are receiving the full configuration block

        // set storage address as the start of the flash configuration sector, plus the requested offset
        storageAddress = (ByteAddress)&__nanoConfig_start__ + offset;

        // always enumerate the blocks again after storing it
        requiresEnumeration = TRUE;

        // for save all the block size has to be provided, check that 
        if(blockSize == 0)
        {
            return FALSE;
        }
    }

    // copy the config block content to the config block storage
    success = iMXRTFlexSPIDriver_Write(NULL, storageAddress, blockSize, (unsigned char*)configurationBlock, false);

    // enumeration is required after we are DONE with SUCCESSFULLY storing all the config chunks
    requiresEnumeration = (success && done);

    if(requiresEnumeration)
    {
        // free the current allocation(s)
        platform_free(g_TargetConfiguration.NetworkInterfaceConfigs);
        platform_free(g_TargetConfiguration.Wireless80211Configs);
        platform_free(g_TargetConfiguration.CertificateStore);

        // perform enumeration of configuration blocks
        ConfigurationManager_EnumerateConfigurationBlocks();
    }

    return success;
}

// Updates a configuration block in the configuration flash sector
// The flash sector has to be erased before writing the updated block
bool ConfigurationManager_UpdateConfigurationBlock(void* configurationBlock, DeviceConfigurationOption configuration, uint32_t configurationIndex)
{
    ByteAddress storageAddress;
    uint32_t blockOffset;
    uint8_t* blockAddressInCopy;
    uint32_t blockSize;
    bool success = FALSE;

    // config sector size
    int sizeOfConfigSector = (uint32_t)&__nanoConfig_end__ - (uint32_t)&__nanoConfig_start__;

    // allocate memory from CRT heap
    uint8_t* configSectorCopy = (uint8_t*)platform_malloc(sizeOfConfigSector);

    if(configSectorCopy != NULL)
    {
        // copy config sector from flash to RAM
        memcpy(configSectorCopy, &__nanoConfig_start__, sizeOfConfigSector);

        // find out the address for the config block to update in the configSectorCopy
        // because we are copying back the config block to flash and just replacing the config block content
        // the addresses in g_TargetConfiguration will remain the same
        // plus we can calculate the offset of the config block from g_TargetConfiguration
        if(configuration == DeviceConfigurationOption_Network)
        {
            // get storage address from block address
            storageAddress = (ByteAddress)g_TargetConfiguration.NetworkInterfaceConfigs->Configs[configurationIndex];

            // set block size, in case it's not already set
            blockSize = sizeof(HAL_Configuration_NetworkInterface);
                
            // make sure the config block marker is set
            memcpy(configurationBlock, c_MARKER_CONFIGURATION_NETWORK_V1, sizeof(c_MARKER_CONFIGURATION_NETWORK_V1));

            _ASSERTE(((HAL_Configuration_NetworkInterface*)configurationBlock)->StartupAddressMode > 0);
        }
        else if(configuration == DeviceConfigurationOption_Wireless80211Network)
        {
            // storage address from block address
            storageAddress = (ByteAddress)g_TargetConfiguration.Wireless80211Configs->Configs[configurationIndex];

            // set block size, in case it's not already set
            blockSize = sizeof(HAL_Configuration_Wireless80211);

            // make sure the config block marker is set
            memcpy(configurationBlock, c_MARKER_CONFIGURATION_WIRELESS80211_V1, sizeof(c_MARKER_CONFIGURATION_WIRELESS80211_V1));
        }
        else if(configuration == DeviceConfigurationOption_X509CaRootBundle)
        {
            // storage address from block address
            storageAddress = (ByteAddress)g_TargetConfiguration.CertificateStore->Certificates[configurationIndex];

            // set block size, in case it's not already set
            // because X509 certificate has a variable length need to compute the block size in two steps
            blockSize = offsetof(HAL_Configuration_X509CaRootBundle, Certificate);
            blockSize += ((HAL_Configuration_X509CaRootBundle*)configurationBlock)->CertificateSize;

            // make sure the config block marker is set
            memcpy(configurationBlock, c_MARKER_CONFIGURATION_X509CAROOTBUNDLE_V1, sizeof(c_MARKER_CONFIGURATION_X509CAROOTBUNDLE_V1));
        }
        else
        {
            // this not a valid configuration option to update, quit
            // free memory first
            platform_free(configSectorCopy);

            return FALSE;
        }
    
        // erase config sector
        if(iMXRTFlexSPIDriver_EraseBlock(NULL, (uint32_t)&__nanoConfig_start__) == TRUE)
        {
            // flash block is erased

            // subtract the start address of config sector to get the offset
            blockOffset = storageAddress - (uint32_t)&__nanoConfig_start__;

            // set pointer to block to udpate
            blockAddressInCopy = configSectorCopy + blockOffset;
            
            // replace config block with new content by replacing memory
            memcpy(blockAddressInCopy, configSectorCopy, blockSize);

            // copy the config block copy back to the config block storage
            success = iMXRTFlexSPIDriver_Write(NULL, (uint32_t)&__nanoConfig_start__, sizeOfConfigSector, (unsigned char*)configSectorCopy, false);
        }

        // free memory
        platform_free(configSectorCopy);
    }

    return success;
}

//  Default initialisation for wireless config block
void InitialiseWirelessDefaultConfig(HAL_Configuration_Wireless80211 * pconfig, uint32_t configurationIndex)
{
    (void)pconfig;
    (void)configurationIndex;
    
    // currently empty
}

//  Default initialisation for Network interface config blocks
bool InitialiseNetworkDefaultConfig(HAL_Configuration_NetworkInterface * pconfig, uint32_t configurationIndex)
{
    (void)pconfig;
    (void)configurationIndex;

    // can't create a "default" network config because we are lacking definition of a MAC address
    return FALSE;
}
