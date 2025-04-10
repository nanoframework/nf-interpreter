//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoHAL_v2.h>
#include <nanoWeak.h>
// #include <network_options.h>
#include <em_device.h>

#if defined(WIFI_DRIVER_ISM43362) && defined(I_AM_NANOCLR)
#include <wifi.h>
#endif

uint8_t programWidth = 0;

uint32_t GetExistingConfigSize()
{
    uint32_t currentConfigSize = 0;

    currentConfigSize =
        g_TargetConfiguration.NetworkInterfaceConfigs->Count * sizeof(HAL_Configuration_NetworkInterface);
    currentConfigSize += g_TargetConfiguration.Wireless80211Configs->Count * sizeof(HAL_Configuration_Wireless80211);

    return currentConfigSize;
}

// initialization of configuration manager
// provided as weak so it can be replaced at target level, if required because of the target implementing the storage
// with a mechanism other then saving to flash
__nfweak void ConfigurationManager_Initialize()
{
    BlockStorageStream stream;
    memset(&stream, 0, sizeof(BlockStorageStream));
    BlockStorageStream_Initialize(&stream, BlockUsage_CONFIG);

    BlockStorageDevice *device = BlockStorageList_GetFirstDevice();
    DeviceBlockInfo *deviceBlockInfo = BlockStorageDevice_GetDeviceInfo(device);

    if (deviceBlockInfo->Regions[stream.RegionIndex].Attributes & BlockRegionAttribute_ProgramWidthIs64bits)
    {
        programWidth = 64 / 8;
    }

    // init g_TargetConfiguration
    memset(&g_TargetConfiguration, 0, sizeof(HAL_TARGET_CONFIGURATION));

    // enumerate the blocks
    ConfigurationManager_EnumerateConfigurationBlocks();
};

// Enumerates the configuration blocks from the configuration flash sector
// it's implemented with 'weak' attribute so it can be replaced at target level if a different persistance mechanism is
// used
__nfweak void ConfigurationManager_EnumerateConfigurationBlocks()
{
    // start checking if this device has config block
    if (((uint32_t)&__nanoConfig_end__ - (uint32_t)&__nanoConfig_start__) > 0)
    {
        // find network configuration blocks
        HAL_CONFIGURATION_NETWORK *networkConfigs =
            (HAL_CONFIGURATION_NETWORK *)ConfigurationManager_FindNetworkConfigurationBlocks(
                (uint32_t)&__nanoConfig_start__,
                (uint32_t)&__nanoConfig_end__);

        // check network configs count
        if (networkConfigs->Count == 0)
        {
            // there is no network config block available, get a default
            HAL_Configuration_NetworkInterface *networkConfig =
                (HAL_Configuration_NetworkInterface *)platform_malloc(sizeof(HAL_Configuration_NetworkInterface));

            if (InitialiseNetworkDefaultConfig(networkConfig, 0))
            {
                // config block created, store it
                ConfigurationManager_StoreConfigurationBlock(
                    networkConfig,
                    DeviceConfigurationOption_Network,
                    0,
                    sizeof(HAL_Configuration_NetworkInterface),
                    0,
                    false);

                // have to enumerate again to pick it up
                networkConfigs = (HAL_CONFIGURATION_NETWORK *)ConfigurationManager_FindNetworkConfigurationBlocks(
                    (uint32_t)&__nanoConfig_start__,
                    (uint32_t)&__nanoConfig_end__);
            }

            platform_free(networkConfig);
        }

        // find wireless 80211 network configuration blocks
        HAL_CONFIGURATION_NETWORK_WIRELESS80211 *networkWirelessConfigs =
            (HAL_CONFIGURATION_NETWORK_WIRELESS80211 *)ConfigurationManager_FindNetworkWireless80211ConfigurationBlocks(
                (uint32_t)&__nanoConfig_start__,
                (uint32_t)&__nanoConfig_end__);

#if defined(TARGET_HAS_WIFI_SUPPORT) && (TARGET_HAS_WIFI_SUPPORT == 1)

        if (networkWirelessConfigs->Count == 0)
        {
            // there is no network config block available, get a default
            HAL_Configuration_Wireless80211 *wirelessConfig =
                (HAL_Configuration_Wireless80211 *)platform_malloc(sizeof(HAL_Configuration_Wireless80211));

            InitialiseWirelessDefaultConfig(wirelessConfig, 0);

            // config block created, store it
            ConfigurationManager_StoreConfigurationBlock(
                wirelessConfig,
                DeviceConfigurationOption_Wireless80211Network,
                0,
                sizeof(HAL_Configuration_Wireless80211),
                0,
                false);

            // have to enumerate again to pick it up
            networkWirelessConfigs = (HAL_CONFIGURATION_NETWORK_WIRELESS80211 *)
                ConfigurationManager_FindNetworkWireless80211ConfigurationBlocks(
                    (uint32_t)&__nanoConfig_start__,
                    (uint32_t)&__nanoConfig_end__);

            platform_free(wirelessConfig);
        }

#endif

        // find X509 certificate blocks
        HAL_CONFIGURATION_X509_CERTIFICATE *certificateStore =
            (HAL_CONFIGURATION_X509_CERTIFICATE *)ConfigurationManager_FindX509CertificateConfigurationBlocks(
                (uint32_t)&__nanoConfig_start__,
                (uint32_t)&__nanoConfig_end__);

        // find X509 device certificate blocks
        HAL_CONFIGURATION_X509_DEVICE_CERTIFICATE *deviceCertificates = (HAL_CONFIGURATION_X509_DEVICE_CERTIFICATE *)
            ConfigurationManager_FindX509DeviceCertificatesConfigurationBlocks(
                (uint32_t)&__nanoConfig_start__,
                (uint32_t)&__nanoConfig_end__);

        // alloc memory for g_TargetConfiguration
        // because this is a struct of structs that use flexible members the memory has to be allocated from the heap
        // the malloc size for each struct is computed separately
        uint32_t sizeOfNetworkInterfaceConfigs =
            offsetof(HAL_CONFIGURATION_NETWORK, Configs) + networkConfigs->Count * sizeof(networkConfigs->Configs[0]);
        uint32_t sizeOfWireless80211Configs =
            offsetof(HAL_CONFIGURATION_NETWORK_WIRELESS80211, Configs) +
            networkWirelessConfigs->Count * sizeof(networkWirelessConfigs->Configs[0]);
        uint32_t sizeOfWirelessAPConfigs = offsetof(HAL_CONFIGURATION_NETWORK_WIRELESSAP, Configs);
        uint32_t sizeOfX509CertificateStore = offsetof(HAL_CONFIGURATION_X509_CERTIFICATE, Certificates) +
                                              certificateStore->Count * sizeof(certificateStore->Certificates[0]);
        uint32_t sizeOfX509DeviceCertificate = offsetof(HAL_CONFIGURATION_X509_DEVICE_CERTIFICATE, Certificates) +
                                               deviceCertificates->Count * sizeof(deviceCertificates->Certificates[0]);

        g_TargetConfiguration.NetworkInterfaceConfigs =
            (HAL_CONFIGURATION_NETWORK *)platform_malloc(sizeOfNetworkInterfaceConfigs);
        g_TargetConfiguration.Wireless80211Configs =
            (HAL_CONFIGURATION_NETWORK_WIRELESS80211 *)platform_malloc(sizeOfWireless80211Configs);
        g_TargetConfiguration.WirelessAPConfigs =
            (HAL_CONFIGURATION_NETWORK_WIRELESSAP *)platform_malloc(sizeOfWirelessAPConfigs);
        g_TargetConfiguration.CertificateStore =
            (HAL_CONFIGURATION_X509_CERTIFICATE *)platform_malloc(sizeOfX509CertificateStore);
        g_TargetConfiguration.DeviceCertificates =
            (HAL_CONFIGURATION_X509_DEVICE_CERTIFICATE *)platform_malloc(sizeOfX509DeviceCertificate);

        // copy structs to g_TargetConfiguration
        memcpy(
            (HAL_CONFIGURATION_NETWORK *)g_TargetConfiguration.NetworkInterfaceConfigs,
            networkConfigs,
            sizeOfNetworkInterfaceConfigs);
        memcpy(
            (HAL_CONFIGURATION_NETWORK_WIRELESS80211 *)g_TargetConfiguration.Wireless80211Configs,
            networkWirelessConfigs,
            sizeOfWireless80211Configs);
        // memcpy(
        //     (HAL_CONFIGURATION_NETWORK_WIRELESSAP *)g_TargetConfiguration.WirelessAPConfigs,
        //     networkWirelessApConfigs,
        //     sizeOfWirelessAPConfigs);
        g_TargetConfiguration.WirelessAPConfigs->Count = 0;
        memcpy(
            (HAL_CONFIGURATION_X509_CERTIFICATE *)g_TargetConfiguration.CertificateStore,
            certificateStore,
            sizeOfX509CertificateStore);
        memcpy(
            (HAL_CONFIGURATION_X509_DEVICE_CERTIFICATE *)g_TargetConfiguration.DeviceCertificates,
            deviceCertificates,
            sizeOfX509DeviceCertificate);

        // now free the memory of the original structs
        platform_free(networkConfigs);
        platform_free(networkWirelessConfigs);
        platform_free(certificateStore);
        platform_free(deviceCertificates);
    }
    else
    {
        // no config block
    }
}

// Gets the network configuration block from the configuration flash sector
// it's implemented with 'weak' attribute so it can be replaced at target level if a different persistance mechanism is
// used
__nfweak bool ConfigurationManager_GetConfigurationBlock(
    void *configurationBlock,
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex)
{
    int sizeOfBlock = 0;
    uint8_t *blockAddress = NULL;

    // validate if the requested block exists
    // Count has to be non zero
    // requested Index has to exist (array index starts at zero, so need to add one)
    if (configuration == DeviceConfigurationOption_Network)
    {
        if (g_TargetConfiguration.NetworkInterfaceConfigs->Count == 0 ||
            (configurationIndex + 1) > g_TargetConfiguration.NetworkInterfaceConfigs->Count)
        {
            // the requested config block is beyond the available count
            return false;
        }

        // set block size
        sizeOfBlock = sizeof(HAL_Configuration_NetworkInterface);

        // get block address
        blockAddress = (uint8_t *)g_TargetConfiguration.NetworkInterfaceConfigs->Configs[configurationIndex];
    }
    else if (configuration == DeviceConfigurationOption_Wireless80211Network)
    {
        if (g_TargetConfiguration.Wireless80211Configs->Count == 0 ||
            (configurationIndex + 1) > g_TargetConfiguration.Wireless80211Configs->Count)
        {
            return FALSE;
        }

        // set block size
        sizeOfBlock = sizeof(HAL_Configuration_Wireless80211);

        // get block address
        blockAddress = (uint8_t *)g_TargetConfiguration.Wireless80211Configs->Configs[configurationIndex];
    }
    else if (configuration == DeviceConfigurationOption_X509CaRootBundle)
    {
        if (g_TargetConfiguration.CertificateStore->Count == 0 ||
            (configurationIndex + 1) > g_TargetConfiguration.CertificateStore->Count)
        {
            return FALSE;
        }

        // get block address
        blockAddress = (uint8_t *)g_TargetConfiguration.CertificateStore->Certificates[configurationIndex];

        // set block size
        // because X509 certificate has a variable length need to compute the block size in two steps
        sizeOfBlock = offsetof(HAL_Configuration_X509CaRootBundle, Certificate);
        sizeOfBlock += ((HAL_Configuration_X509CaRootBundle *)blockAddress)->CertificateSize;
    }
    else if (configuration == DeviceConfigurationOption_X509CaRootBundle)
    {
        if (g_TargetConfiguration.DeviceCertificates->Count == 0 ||
            (configurationIndex + 1) > g_TargetConfiguration.DeviceCertificates->Count)
        {
            return FALSE;
        }

        // get block address
        blockAddress = (uint8_t *)g_TargetConfiguration.DeviceCertificates->Certificates[configurationIndex];

        // set block size
        // because X509 certificate has a variable length need to compute the block size in two steps
        sizeOfBlock = offsetof(HAL_Configuration_X509DeviceCertificate, Certificate);
        sizeOfBlock += ((HAL_Configuration_X509DeviceCertificate *)blockAddress)->CertificateSize;
    }

    // copy the config block content to the pointer in the argument
    memcpy(configurationBlock, blockAddress, sizeOfBlock);

    return TRUE;
}

// Stores the configuration block to the configuration flash sector
// NOTE: because inserting or removing a configuration block it's very 'RAM expensive' we choose not to support those
// operations the host debugger will have to be used to manage these operations on the device configuration collection
// it's implemented with 'weak' attribute so it can be replaced at target level if a different persistance mechanism is
// used
__nfweak bool ConfigurationManager_StoreConfigurationBlock(
    void *configurationBlock,
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex,
    uint32_t blockSize,
    uint32_t offset,
    bool done)
{
    ByteAddress storageAddress = 0;
    bool requiresEnumeration = FALSE;
    bool success = FALSE;

    if (configuration == DeviceConfigurationOption_Network)
    {
        if (g_TargetConfiguration.NetworkInterfaceConfigs == NULL ||
            (g_TargetConfiguration.NetworkInterfaceConfigs->Count == 0 && configurationIndex == 0))
        {
            // there is no network config block, we are storing the default one
            // THIS IS THE FIRST CONFIG BLOCK THAT'S AUTO-CREATED
            // OK to continue
            // set storage address as the start of the flash configuration sector
            storageAddress = (ByteAddress)&__nanoConfig_start__;
        }
        else
        {
            // the requested config block is beyond the available count
            if ((configurationIndex + 1) > g_TargetConfiguration.NetworkInterfaceConfigs->Count)
            {
                return FALSE;
            }

            // set storage address from block address, plus the requested offset
            storageAddress =
                (ByteAddress)g_TargetConfiguration.NetworkInterfaceConfigs->Configs[configurationIndex] + offset;
        }

        // set block size, in case it's not already set
        blockSize = sizeof(HAL_Configuration_NetworkInterface);

        // make sure the config block marker is set
        memcpy(configurationBlock, c_MARKER_CONFIGURATION_NETWORK_V1, sizeof(c_MARKER_CONFIGURATION_NETWORK_V1));

        _ASSERTE(((HAL_Configuration_NetworkInterface *)configurationBlock)->StartupAddressMode > 0);
    }
    else if (configuration == DeviceConfigurationOption_Wireless80211Network)
    {

        // #if (TARGET_HAS_WIFI_SUPPORT == 1)

        //         if (g_TargetConfiguration.Wireless80211Configs == NULL ||
        //             (g_TargetConfiguration.Wireless80211Configs->Count == 0 && configurationIndex == 0))
        //         {
        //             // there is no wireless 80211 config block, so we are storing the default one
        //             // THIS IS THE SECOND CONFIG BLOCK THAT'S AUTO-CREATED
        //             // OK to continue
        //             // set storage address contiguous to the network config block
        //             storageAddress = (uint32_t)&__nanoConfig_start__ + sizeof(HAL_Configuration_NetworkInterface);

        //             // check programming width
        //             if(programWidth > 0)
        //             {
        //                 // round address to the next valid programming width
        //                 storageAddress += programWidth - storageAddress % programWidth;
        //             }
        //         }
        //         else
        //         {
        //             // the requested config block is beyond the available count
        //             if ((configurationIndex + 1) > g_TargetConfiguration.Wireless80211Configs->Count)
        //             {
        //                 return FALSE;
        //             }

        //             // set storage address from block address, plus the requested offset
        //             storageAddress =
        //                 (ByteAddress)g_TargetConfiguration.Wireless80211Configs->Configs[configurationIndex] +
        //                 offset;
        //         }

        //         // set block size, in case it's not already set
        //         blockSize = sizeof(HAL_Configuration_Wireless80211);

        //         // make sure the config block marker is set
        //         memcpy(
        //             configurationBlock,
        //             c_MARKER_CONFIGURATION_WIRELESS80211_V1,
        //             sizeof(c_MARKER_CONFIGURATION_WIRELESS80211_V1));

        // #else
        //         // no support for WIFI in this STM32 build
        //         return FALSE;
        // #endif
    }
    else if (configuration == DeviceConfigurationOption_X509CaRootBundle)
    {
        // compute block size
        // because X509 certificate has a variable length need to compute the block size in two steps
        blockSize = offsetof(HAL_Configuration_X509CaRootBundle, Certificate);
        blockSize += ((HAL_Configuration_X509CaRootBundle *)configurationBlock)->CertificateSize;

        //
        if (g_TargetConfiguration.CertificateStore->Count == 0)
        {
            // there is nothing at the certificate store
            // find size of existing config blocks
            storageAddress = (uint32_t)&__nanoConfig_start__ + GetExistingConfigSize();
        }
        else
        {
            // set storage address from block address, plus the requested offset
            storageAddress =
                (ByteAddress)g_TargetConfiguration.CertificateStore->Certificates[configurationIndex] + offset;
        }

        if (g_TargetConfiguration.CertificateStore == NULL ||
            (g_TargetConfiguration.CertificateStore->Count == 0 ||
             (configurationIndex + 1) > g_TargetConfiguration.CertificateStore->Count))
        {
            // there is no block stored
            // check if there is room for it
            if (((uint32_t)&__nanoConfig_end__ - storageAddress) < blockSize)
            {
                // not enough room
                return FALSE;
            }

            // // now check if memory is erase, so the block can be stored
            // if (!STM32FlashDriver_IsBlockErased(NULL, storageAddress, blockSize))
            // {
            //     // memory not erased, can't store
            //     return FALSE;
            // }
        }

        // make sure the config block marker is set, ONLY required on the 1st chunk
        if (offset == 0)
        {
            memcpy(
                configurationBlock,
                c_MARKER_CONFIGURATION_X509CAROOTBUNDLE_V1,
                sizeof(c_MARKER_CONFIGURATION_X509CAROOTBUNDLE_V1));
        }
    }
    else if (configuration == DeviceConfigurationOption_X509DeviceCertificates)
    {
        // compute block size
        // because X509 certificate has a variable length need to compute the block size in two steps
        blockSize = offsetof(HAL_Configuration_X509DeviceCertificate, Certificate);
        blockSize += ((HAL_Configuration_X509DeviceCertificate *)configurationBlock)->CertificateSize;

        //
        if (g_TargetConfiguration.DeviceCertificates->Count == 0)
        {
            // there is nothing at the certificate store
            // find size of existing config blocks
            storageAddress = (uint32_t)&__nanoConfig_start__ + GetExistingConfigSize();
        }
        else
        {
            // set storage address from block address, plus the requested offset
            storageAddress =
                (ByteAddress)g_TargetConfiguration.DeviceCertificates->Certificates[configurationIndex] + offset;
        }

        if (g_TargetConfiguration.DeviceCertificates == NULL ||
            (g_TargetConfiguration.DeviceCertificates->Count == 0 ||
             (configurationIndex + 1) > g_TargetConfiguration.DeviceCertificates->Count))
        {
            // there is no block stored
            // check if there is room for it
            if (((uint32_t)&__nanoConfig_end__ - storageAddress) < blockSize)
            {
                // not enough room
                return FALSE;
            }

            // // now check if memory is erase, so the block can be stored
            // if (!STM32FlashDriver_IsBlockErased(NULL, storageAddress, blockSize))
            // {
            //     // memory not erased, can't store
            //     return FALSE;
            // }
        }

        // make sure the config block marker is set, ONLY required on the 1st chunk
        if (offset == 0)
        {
            memcpy(
                configurationBlock,
                c_MARKER_CONFIGURATION_X509DEVICECERTIFICATE_V1,
                sizeof(c_MARKER_CONFIGURATION_X509DEVICECERTIFICATE_V1));
        }
    }
    else if (configuration == DeviceConfigurationOption_All)
    {
        // particular situation where we are receiving the full configuration block

        // set storage address as the start of the flash configuration sector, plus the requested offset
        storageAddress = (ByteAddress)&__nanoConfig_start__ + offset;

        // for save all the block size has to be provided, check that
        if (blockSize == 0)
        {
            return FALSE;
        }
    }

    // // copy the config block content to the config block storage
    // success = STM32FlashDriver_Write(NULL, storageAddress, blockSize, (unsigned char *)configurationBlock, true);

    // enumeration is required after we are DONE with SUCCESSFULLY storing all the config chunks
    requiresEnumeration = (success && done);

    if (requiresEnumeration)
    {
        // free the current allocation(s)
        platform_free(g_TargetConfiguration.NetworkInterfaceConfigs);
        platform_free(g_TargetConfiguration.Wireless80211Configs);
        platform_free(g_TargetConfiguration.CertificateStore);
        platform_free(g_TargetConfiguration.DeviceCertificates);

        // perform enumeration of configuration blocks
        ConfigurationManager_EnumerateConfigurationBlocks();
    }

    return success;
}

// Updates a configuration block in the configuration flash sector
// The flash sector has to be erased before writing the updated block
// it's implemented with 'weak' attribute so it can be replaced at target level if a different persistance mechanism is
// used
__nfweak UpdateConfigurationResult ConfigurationManager_UpdateConfigurationBlock(
    void *configurationBlock,
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex)
{
    ByteAddress storageAddress;
    (void)storageAddress;
    // uint32_t blockOffset;
    // uint8_t *blockAddressInCopy;
    uint32_t blockSize;
    UpdateConfigurationResult success = UpdateConfigurationResult_Failed;

    // config sector size
    int sizeOfConfigSector = (uint32_t)&__nanoConfig_end__ - (uint32_t)&__nanoConfig_start__;

    // allocate memory from CRT heap
    uint8_t *configSectorCopy = (uint8_t *)platform_malloc(sizeOfConfigSector);

    if (configSectorCopy != NULL)
    {
        // copy config sector from flash to RAM
        memcpy(configSectorCopy, &__nanoConfig_start__, sizeOfConfigSector);

        // find out the address for the config block to update in the configSectorCopy
        // because we are copying back the config block to flash and just replacing the config block content
        // the addresses in g_TargetConfiguration will remain the same
        // plus we can calculate the offset of the config block from g_TargetConfiguration
        if (configuration == DeviceConfigurationOption_Network)
        {
            // make sure the config block marker is set
            memcpy(configurationBlock, c_MARKER_CONFIGURATION_NETWORK_V1, sizeof(c_MARKER_CONFIGURATION_NETWORK_V1));

            // check if the configuration block is the same
            if (ConfigurationManager_CheckExistingConfigurationBlock(
                    g_TargetConfiguration.NetworkInterfaceConfigs->Configs[configurationIndex],
                    configurationBlock,
                    sizeof(HAL_Configuration_NetworkInterface),
                    sizeof(HAL_Configuration_NetworkInterface)))
            {
                // block is the same
                // free memory
                platform_free(configSectorCopy);

                // operation is successful (nothing to update)
                return UpdateConfigurationResult_NoChanges;
            }

            // get storage address from block address
            storageAddress = (ByteAddress)g_TargetConfiguration.NetworkInterfaceConfigs->Configs[configurationIndex];

            // set block size, in case it's not already set
            blockSize = sizeof(HAL_Configuration_NetworkInterface);

            _ASSERTE(((HAL_Configuration_NetworkInterface *)configurationBlock)->StartupAddressMode > 0);
        }
        else if (configuration == DeviceConfigurationOption_Wireless80211Network)
        {
            // make sure the config block marker is set
            memcpy(
                configurationBlock,
                c_MARKER_CONFIGURATION_WIRELESS80211_V1,
                sizeof(c_MARKER_CONFIGURATION_WIRELESS80211_V1));

            // check if the configuration block is the same
            if (ConfigurationManager_CheckExistingConfigurationBlock(
                    g_TargetConfiguration.Wireless80211Configs->Configs[configurationIndex],
                    configurationBlock,
                    sizeof(HAL_Configuration_Wireless80211),
                    sizeof(HAL_Configuration_Wireless80211)))
            {
                // block is the same
                // free memory
                platform_free(configSectorCopy);

                // operation is successful (nothing to update)
                return UpdateConfigurationResult_NoChanges;
            }

            // storage address from block address
            storageAddress = (ByteAddress)g_TargetConfiguration.Wireless80211Configs->Configs[configurationIndex];

            // set block size, in case it's not already set
            blockSize = sizeof(HAL_Configuration_Wireless80211);
        }
        else if (configuration == DeviceConfigurationOption_X509CaRootBundle)
        {
            // make sure the config block marker is set
            memcpy(
                configurationBlock,
                c_MARKER_CONFIGURATION_X509CAROOTBUNDLE_V1,
                sizeof(c_MARKER_CONFIGURATION_X509CAROOTBUNDLE_V1));

            // check if certificate is the same
            if (ConfigurationManager_CheckExistingConfigurationBlock(
                    g_TargetConfiguration.CertificateStore->Certificates[configurationIndex]->Certificate,
                    ((HAL_Configuration_X509CaRootBundle *)configurationBlock)->Certificate,
                    g_TargetConfiguration.CertificateStore->Certificates[configurationIndex]->CertificateSize,
                    ((HAL_Configuration_X509CaRootBundle *)configurationBlock)->CertificateSize))
            {
                // block is the same
                // free memory
                platform_free(configSectorCopy);

                // operation is successful (nothing to update)
                return UpdateConfigurationResult_NoChanges;
            }

            // storage address from block address
            storageAddress = (ByteAddress)g_TargetConfiguration.CertificateStore->Certificates[configurationIndex];

            // set block size, in case it's not already set
            // because X509 certificate has a variable length need to compute the block size in two steps
            blockSize = offsetof(HAL_Configuration_X509CaRootBundle, Certificate);
            blockSize += ((HAL_Configuration_X509CaRootBundle *)configurationBlock)->CertificateSize;
        }
        else if (configuration == DeviceConfigurationOption_X509DeviceCertificates)
        {
            // make sure the config block marker is set
            memcpy(
                configurationBlock,
                c_MARKER_CONFIGURATION_X509DEVICECERTIFICATE_V1,
                sizeof(c_MARKER_CONFIGURATION_X509DEVICECERTIFICATE_V1));

            // check if certificate is the same
            if (ConfigurationManager_CheckExistingConfigurationBlock(
                    g_TargetConfiguration.DeviceCertificates->Certificates[configurationIndex]->Certificate,
                    ((HAL_Configuration_X509DeviceCertificate *)configurationBlock)->Certificate,
                    g_TargetConfiguration.DeviceCertificates->Certificates[configurationIndex]->CertificateSize,
                    ((HAL_Configuration_X509DeviceCertificate *)configurationBlock)->CertificateSize))
            {
                // block is the same
                // free memory
                platform_free(configSectorCopy);

                // operation is successful (nothing to update)
                return UpdateConfigurationResult_NoChanges;
            }

            // storage address from block address
            storageAddress = (ByteAddress)g_TargetConfiguration.DeviceCertificates->Certificates[configurationIndex];

            // set block size, in case it's not already set
            // because X509 certificate has a variable length need to compute the block size in two steps
            blockSize = offsetof(HAL_Configuration_X509DeviceCertificate, Certificate);
            blockSize += ((HAL_Configuration_X509DeviceCertificate *)configurationBlock)->CertificateSize;
        }
        else
        {
            // this not a valid configuration option to update, quit
            // free memory first
            platform_free(configSectorCopy);

            return UpdateConfigurationResult_Failed;
        }

        // // erase config sector
        // if (STM32FlashDriver_EraseBlock(NULL, (uint32_t)&__nanoConfig_start__) == TRUE)
        // {
        //     // flash block is erased

        //     // subtract the start address of config sector to get the offset
        //     blockOffset = storageAddress - (uint32_t)&__nanoConfig_start__;

        //     // set pointer to block to udpate
        //     blockAddressInCopy = configSectorCopy + blockOffset;

        //     // replace config block with new content by replacing memory
        //     memcpy(blockAddressInCopy, configurationBlock, blockSize);

        //     // copy the config block copy back to the config block storage
        //     success = STM32FlashDriver_Write(
        //         NULL,
        //         (uint32_t)&__nanoConfig_start__,
        //         sizeOfConfigSector,
        //         (unsigned char *)configSectorCopy,
        //         true);
        // }

        // free memory
        platform_free(configSectorCopy);
    }

    return success;
}

//  Default initialisation for wireless config block
// it's implemented with 'weak' attribute so it can be replaced at target level if different configurations are intended
__nfweak void InitialiseWirelessDefaultConfig(HAL_Configuration_Wireless80211 *config, uint32_t configurationIndex)
{
    memset(config, 0, sizeof(HAL_Configuration_Wireless80211));

    // make sure the config block marker is set
    memcpy(config, c_MARKER_CONFIGURATION_WIRELESS80211_V1, sizeof(c_MARKER_CONFIGURATION_WIRELESS80211_V1));

    // Wireless station
    config->Id = configurationIndex;

    config->Options =
        (Wireless80211Configuration_ConfigurationOptions)(Wireless80211Configuration_ConfigurationOptions_AutoConnect |
                                                          Wireless80211Configuration_ConfigurationOptions_Enable);
}

//  Default initialisation for Network interface config blocks
// it's implemented with 'weak' attribute so it can be replaced at target level if different configurations are intended
__nfweak bool InitialiseNetworkDefaultConfig(HAL_Configuration_NetworkInterface *config, uint32_t configurationIndex)
{
    (void)configurationIndex;

    // #if (TARGET_HAS_WIFI_SUPPORT == 1)

    //     memset(config, 0, sizeof(HAL_Configuration_NetworkInterface));

    //     // make sure the config block marker is set
    //     memcpy(config->Marker, c_MARKER_CONFIGURATION_NETWORK_V1, sizeof(c_MARKER_CONFIGURATION_NETWORK_V1));

    //     // currently only Wireless station is supported
    //     config->InterfaceType = NetworkInterfaceType_Wireless80211;
    //     config->StartupAddressMode = AddressMode_DHCP;
    //     config->AutomaticDNS = 1;
    //     config->SpecificConfigId = 0;

    //     // fill in MAX with 0xFF to allow it updating it later
    //     memset(config->MacAddress, 0xFF, sizeof(config->MacAddress));

    // // get default MAC
    // #if defined(WIFI_DRIVER_ISM43362) && defined(I_AM_NANOCLR)
    //     // OK to ignore the return value, no harm done if it fails
    //     WIFI_GetMAC_Address(config->MacAddress);
    // #endif

    //     return TRUE;

    // #else

    (void)config;

    // can't create a "default" network config because we are lacking definition of a MAC address

    return FALSE;
    // #endif
}

int32_t ConfigurationManager_FindNetworkConfigurationMatchingWirelessConfigurationFromId(uint32_t configurationId)
{
    // loop though all Network config blocks trying to find one that has this ID as its SpecificConfig ID
    for (int index = 0; index < g_TargetConfiguration.NetworkInterfaceConfigs->Count; index++)
    {
        if (g_TargetConfiguration.NetworkInterfaceConfigs->Configs[index]->SpecificConfigId == configurationId)
        {
            return index;
        }
    }

    // not found
    return -1;
}

// default implementation
// this is weak so a manufacturer can provide a strong implementation
__nfweak void ConfigurationManager_GetSystemSerialNumber(char *serialNumber, size_t serialNumberSize)
{
    memset(serialNumber, 0, serialNumberSize);

    // Use the device Unique ID which is 64 bits long
    // Put it in the LSB of the serial number
    int startOfId = serialNumberSize - 8;

    // high 32 bits
    uint32_t rawId = DEVINFO->UNIQUEH;
    for (int i = 3; i >= 0; --i)
    {
        serialNumber[startOfId + i] = rawId & 0xFF;
        rawId >>= 8;
    }

    // low 32 bits
    rawId = DEVINFO->UNIQUEL;
    for (int i = 7; i >= 4; --i)
    {
        serialNumber[startOfId + i] = rawId & 0xFF;
        rawId >>= 8;
    }

    // Disambiguation is needed because the hardware-specific identifier used to create the
    // default serial number on other platforms may be in the same range.
    // Set the first byte to a number that is unique (within the nanoFramework CLR) for the Giant Gecko.
    serialNumber[0] = 3;
}
