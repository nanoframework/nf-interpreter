//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_v2.h>
#include <nanoHAL_Network.h>

// walks the configuration blocks stored in a region (defined below)
static uint32_t WalkConfigurationBlocks(
    const uint8_t *start,
    uint32_t size,
    const unsigned char *marker,
    const uint8_t **addresses,
    uint32_t maxCount,
    uint32_t *usedSize);

// finds the configuration blocks with the marker in a memory region
// returns a struct with the count followed by the addresses of the blocks (the layout of all the HAL_CONFIGURATION_xxx
// structs), allocated from the heap
static void *FindConfigurationBlocks(uint32_t startAddress, uint32_t endAddress, const unsigned char *marker)
{
    // all the HAL_CONFIGURATION_xxx structs have the same layout
    HAL_CONFIGURATION_NETWORK *configs;
    uint32_t size = endAddress > startAddress ? endAddress - startAddress : 0;

    // first pass: find out how many blocks of this type we have
    uint32_t blockCount =
        WalkConfigurationBlocks((const uint8_t *)(uintptr_t)startAddress, size, marker, NULL, 0, NULL);

    // allocate config struct
    configs = (HAL_CONFIGURATION_NETWORK *)platform_malloc(
        offsetof(HAL_CONFIGURATION_NETWORK, Configs) + blockCount * sizeof(configs->Configs[0]));

    if (configs == NULL)
    {
        return NULL;
    }

    // set collection count
    configs->Count = blockCount;

    // second pass: get address of each config block
    WalkConfigurationBlocks(
        (const uint8_t *)(uintptr_t)startAddress,
        size,
        marker,
        (const uint8_t **)configs->Configs,
        blockCount,
        NULL);

    return configs;
}

__nfweak void *ConfigurationManager_FindNetworkConfigurationBlocks(uint32_t startAddress, uint32_t endAddress)
{
    return FindConfigurationBlocks(startAddress, endAddress, c_MARKER_CONFIGURATION_NETWORK_V1);
}

__nfweak void *ConfigurationManager_FindNetworkWireless80211ConfigurationBlocks(
    uint32_t startAddress,
    uint32_t endAddress)
{
    return FindConfigurationBlocks(startAddress, endAddress, c_MARKER_CONFIGURATION_WIRELESS80211_V1);
}

__nfweak void *ConfigurationManager_FindX509CertificateConfigurationBlocks(uint32_t startAddress, uint32_t endAddress)
{
    return FindConfigurationBlocks(startAddress, endAddress, c_MARKER_CONFIGURATION_X509CAROOTBUNDLE_V1);
}

__nfweak void *ConfigurationManager_FindX509DeviceCertificatesConfigurationBlocks(
    uint32_t startAddress,
    uint32_t endAddress)
{
    return FindConfigurationBlocks(startAddress, endAddress, c_MARKER_CONFIGURATION_X509DEVICECERTIFICATE_V1);
}

__nfweak void *ConfigurationManager_FindWPAuthTokenConfigurationBlocks(uint32_t startAddress, uint32_t endAddress)
{
    return FindConfigurationBlocks(startAddress, endAddress, c_MARKER_CONFIGURATION_WPAUTHTOKEN_V1);
}

__nfweak HAL_Configuration_Wireless80211 *ConfigurationManager_GetWirelessConfigurationFromId(uint32_t configurationId)
{
    for (int i = 0; i < g_TargetConfiguration.Wireless80211Configs->Count; i++)
    {
        if (g_TargetConfiguration.Wireless80211Configs->Configs[i]->Id == configurationId)
        {
            // need to make a copy
            HAL_Configuration_Wireless80211 *configBlock =
                (HAL_Configuration_Wireless80211 *)platform_malloc(sizeof(HAL_Configuration_Wireless80211));

            // check allocation
            if (configBlock)
            {
                memcpy(
                    configBlock,
                    g_TargetConfiguration.Wireless80211Configs->Configs[i],
                    sizeof(HAL_Configuration_Wireless80211));

                return configBlock;
            }
        }
    }

    // not found
    return NULL;
}

__nfweak HAL_Configuration_WirelessAP *ConfigurationManager_GetWirelessAPConfigurationFromId(uint32_t configurationId)
{
    for (int i = 0; i < g_TargetConfiguration.WirelessAPConfigs->Count; i++)
    {
        if (g_TargetConfiguration.WirelessAPConfigs->Configs[i]->Id == configurationId)
        {
            // need to make a copy
            HAL_Configuration_WirelessAP *configBlock =
                (HAL_Configuration_WirelessAP *)platform_malloc(sizeof(HAL_Configuration_WirelessAP));

            // check allocation
            if (configBlock)
            {
                memcpy(
                    configBlock,
                    g_TargetConfiguration.WirelessAPConfigs->Configs[i],
                    sizeof(HAL_Configuration_WirelessAP));

                return configBlock;
            }
        }
    }

    // not found
    return NULL;
}

__nfweak HAL_Configuration_X509CaRootBundle *ConfigurationManager_GetCertificateStore()
{
    if (g_TargetConfiguration.CertificateStore->Count)
    {
        // need to make a copy
        // need to compute size as the cert size is variable
        int32_t blockSize = offsetof(HAL_Configuration_X509CaRootBundle, Certificate) +
                            g_TargetConfiguration.CertificateStore->Certificates[0]->CertificateSize;

        HAL_Configuration_X509CaRootBundle *configBlock =
            (HAL_Configuration_X509CaRootBundle *)platform_malloc(blockSize);

        // check allocation
        if (configBlock)
        {
            memcpy(configBlock, g_TargetConfiguration.CertificateStore->Certificates[0], blockSize);

            return configBlock;
        }
    }

    // not found
    return NULL;
}

__nfweak HAL_Configuration_X509DeviceCertificate *ConfigurationManager_GetDeviceCertificate()
{
    if (g_TargetConfiguration.DeviceCertificates->Count)
    {
        // need to make a copy
        // need to compute size as the cert size is variable
        int32_t blockSize = offsetof(HAL_Configuration_X509DeviceCertificate, Certificate) +
                            g_TargetConfiguration.DeviceCertificates->Certificates[0]->CertificateSize;

        HAL_Configuration_X509DeviceCertificate *configBlock =
            (HAL_Configuration_X509DeviceCertificate *)platform_malloc(blockSize);

        // check allocation
        if (configBlock)
        {
            memcpy(configBlock, g_TargetConfiguration.DeviceCertificates->Certificates[0], blockSize);

            return configBlock;
        }
    }

    // not found
    return NULL;
}

__nfweak bool ConfigurationManager_CheckExistingConfigurationBlock(
    void *existingConfigBlock,
    void *newConfigBlock,
    uint32_t existingConfigBlockSize,
    uint32_t newConfigBlockSize)
{
    // config blocks parameters are addresses
    uint8_t *cursor1 = (uint8_t *)existingConfigBlock;
    uint8_t *cursor2 = (uint8_t *)newConfigBlock;

    // obvious check
    if (existingConfigBlockSize != newConfigBlockSize)
    {
        return false;
    }

    return memcmp(cursor1, cursor2, existingConfigBlockSize) == 0;
}

__nfweak void ConfigurationManager_GetOemModelSku(char *model, size_t modelSkuSize)
{
    // default implementation
    // this is weak so the target can provide the implementation
    memset(model, 0, modelSkuSize);
}

__nfweak void ConfigurationManager_GetModuleSerialNumber(char *serialNumber, size_t serialNumberSize)
{
    // default implementation
    // this is weak so a manufacturer can provide a strong implementation
    memset(serialNumber, 0, serialNumberSize);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// configuration storage operations
// the device handles every add, replace and delete of a configuration block
// the target provides ConfigurationManager_ApplyConfigurationOperation, ConfigurationManager_GetBlockInfo and
// ConfigurationManager_GetStorageInfo, the validation and the handling of the wireless profiles Ids is common

// operation being received in chunks
typedef struct StagedConfigurationOperation
{
    uint8_t *Buffer;
    uint32_t TotalLength;
    uint32_t Received;
    uint32_t Operation;
    uint32_t Configuration;
    uint32_t Index;
    bool Active;

} StagedConfigurationOperation;

static StagedConfigurationOperation s_stagedOperation;

uint32_t ConfigurationManager_GetBlockCount(DeviceConfigurationOption configuration)
{
    switch (configuration)
    {
        case DeviceConfigurationOption_Network:
            return g_TargetConfiguration.NetworkInterfaceConfigs != NULL
                       ? g_TargetConfiguration.NetworkInterfaceConfigs->Count
                       : 0;

        case DeviceConfigurationOption_Wireless80211Network:
            return g_TargetConfiguration.Wireless80211Configs != NULL
                       ? g_TargetConfiguration.Wireless80211Configs->Count
                       : 0;

        case DeviceConfigurationOption_WirelessNetworkAP:
            return g_TargetConfiguration.WirelessAPConfigs != NULL ? g_TargetConfiguration.WirelessAPConfigs->Count : 0;

        case DeviceConfigurationOption_X509CaRootBundle:
            return g_TargetConfiguration.CertificateStore != NULL ? g_TargetConfiguration.CertificateStore->Count : 0;

        case DeviceConfigurationOption_X509DeviceCertificates:
            return g_TargetConfiguration.DeviceCertificates != NULL ? g_TargetConfiguration.DeviceCertificates->Count
                                                                    : 0;

        case DeviceConfigurationOption_WPAuthToken:
            return g_TargetConfiguration.WPAuthTokenConfigs != NULL ? g_TargetConfiguration.WPAuthTokenConfigs->Count
                                                                    : 0;

        default:
            return 0;
    }
}

#if !defined(VIRTUAL_DEVICE)

__nfweak void ConfigurationManager_GetBlockInfo(
    DeviceConfigurationOption configuration,
    HAL_ConfigurationBlockInfo *info)
{
    (void)configuration;

    // default: not supported
    memset(info, 0, sizeof(HAL_ConfigurationBlockInfo));
}

__nfweak void ConfigurationManager_GetStorageInfo(uint32_t *totalSize, uint32_t *freeSize, uint32_t *flags)
{
    // default: unknown
    *totalSize = 0;
    *freeSize = 0xFFFFFFFF;
    *flags = 0;
}

__nfweak ConfigurationOperationError ConfigurationManager_ApplyConfigurationOperation(
    ConfigurationOperation operation,
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex,
    void *block,
    uint32_t blockSize)
{
    (void)operation;
    (void)configuration;
    (void)configurationIndex;
    (void)block;
    (void)blockSize;

    return ConfigurationOperationError_NotSupported;
}

#endif

// gets the size of a fixed size configuration block, 0 for the variable size ones (certificates)
static uint32_t GetFixedConfigurationBlockSize(DeviceConfigurationOption configuration)
{
    switch (configuration)
    {
        case DeviceConfigurationOption_Network:
            return sizeof(HAL_Configuration_NetworkInterface);

        case DeviceConfigurationOption_Wireless80211Network:
            return sizeof(HAL_Configuration_Wireless80211);

        case DeviceConfigurationOption_WirelessNetworkAP:
            return sizeof(HAL_Configuration_WirelessAP);

        case DeviceConfigurationOption_WPAuthToken:
            return sizeof(HAL_Configuration_WPAuthToken);

        default:
            return 0;
    }
}

static const unsigned char *GetConfigurationBlockMarker(DeviceConfigurationOption configuration)
{
    switch (configuration)
    {
        case DeviceConfigurationOption_Network:
            return c_MARKER_CONFIGURATION_NETWORK_V1;

        case DeviceConfigurationOption_Wireless80211Network:
            return c_MARKER_CONFIGURATION_WIRELESS80211_V1;

        case DeviceConfigurationOption_WirelessNetworkAP:
            return c_MARKER_CONFIGURATION_WIRELESS_AP_V1;

        case DeviceConfigurationOption_X509CaRootBundle:
            return c_MARKER_CONFIGURATION_X509CAROOTBUNDLE_V1;

        case DeviceConfigurationOption_X509DeviceCertificates:
            return c_MARKER_CONFIGURATION_X509DEVICECERTIFICATE_V1;

        case DeviceConfigurationOption_WPAuthToken:
            return c_MARKER_CONFIGURATION_WPAUTHTOKEN_V1;

        default:
            return NULL;
    }
}

static bool IsWirelessProfile(DeviceConfigurationOption configuration)
{
    return configuration == DeviceConfigurationOption_Wireless80211Network ||
           configuration == DeviceConfigurationOption_WirelessNetworkAP;
}

// offset of the Id in a wireless profile block (the blocks are packed, so the Id is copied, not pointed to)
static uint32_t GetWirelessProfileIdOffset(DeviceConfigurationOption configuration)
{
    return configuration == DeviceConfigurationOption_Wireless80211Network
               ? offsetof(HAL_Configuration_Wireless80211, Id)
               : offsetof(HAL_Configuration_WirelessAP, Id);
}

static uint32_t GetWirelessProfileId(DeviceConfigurationOption configuration, const void *block)
{
    uint32_t id;

    memcpy(&id, (const uint8_t *)block + GetWirelessProfileIdOffset(configuration), sizeof(id));

    return id;
}

static void SetWirelessProfileId(DeviceConfigurationOption configuration, void *block, uint32_t id)
{
    memcpy((uint8_t *)block + GetWirelessProfileIdOffset(configuration), &id, sizeof(id));
}

// reads the wireless profile stored at an index into the buffer, which has to be large enough for the block
static bool ReadWirelessProfile(DeviceConfigurationOption configuration, uint32_t index, void *block)
{
    memset(block, 0, GetFixedConfigurationBlockSize(configuration));

    return ConfigurationManager_GetConfigurationBlock(block, configuration, index);
}

// checks if a wireless profile, other than the one at excludeIndex, has this Id
// a profile that can't be read means the Id can't be verified: that's a storage error
static ConfigurationOperationError IsWirelessProfileIdInUse(
    DeviceConfigurationOption configuration,
    uint32_t id,
    uint32_t excludeIndex,
    void *scratch,
    bool *inUse)
{
    uint32_t count = ConfigurationManager_GetBlockCount(configuration);

    *inUse = false;

    for (uint32_t i = 0; i < count && !*inUse; i++)
    {
        if (i == excludeIndex)
        {
            continue;
        }

        if (!ReadWirelessProfile(configuration, i, scratch))
        {
            return ConfigurationOperationError_StorageError;
        }

        *inUse = GetWirelessProfileId(configuration, scratch) == id;
    }

    return ConfigurationOperationError_Success;
}

// checks if a network interface uses the wireless profile with this Id
// if a network interface can't be read, the profile is taken as used: better safe
static bool IsWirelessProfileUsedByNetworkInterface(DeviceConfigurationOption configuration, uint32_t id)
{
    bool inUse = false;
    uint32_t count = ConfigurationManager_GetBlockCount(DeviceConfigurationOption_Network);
    NetworkInterfaceType interfaceType = configuration == DeviceConfigurationOption_Wireless80211Network
                                             ? NetworkInterfaceType_Wireless80211
                                             : NetworkInterfaceType_WirelessAP;

    HAL_Configuration_NetworkInterface *networkConfig =
        (HAL_Configuration_NetworkInterface *)platform_malloc(sizeof(HAL_Configuration_NetworkInterface));

    if (networkConfig == NULL)
    {
        // can't check, better be safe
        return true;
    }

    for (uint32_t i = 0; i < count && !inUse; i++)
    {
        memset(networkConfig, 0, sizeof(HAL_Configuration_NetworkInterface));

        if (ConfigurationManager_GetConfigurationBlock(networkConfig, DeviceConfigurationOption_Network, i))
        {
            inUse = networkConfig->InterfaceType == interfaceType && networkConfig->SpecificConfigId == id;
        }
        else
        {
            // can't check, better be safe
            inUse = true;
        }
    }

    platform_free(networkConfig);

    return inUse;
}

// handles the Id and the password of a wireless profile being added or replaced, or checks if the one being deleted is
// in use
static ConfigurationOperationError PrepareWirelessProfileOperation(
    ConfigurationOperation operation,
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex,
    uint32_t flags,
    void *block)
{
    ConfigurationOperationError result = ConfigurationOperationError_Success;
    uint32_t id;
    uint32_t storedId;
    bool idInUse;

    // large enough for both wireless profile blocks
    void *storedBlock = platform_malloc(
        sizeof(HAL_Configuration_WirelessAP) > sizeof(HAL_Configuration_Wireless80211)
            ? sizeof(HAL_Configuration_WirelessAP)
            : sizeof(HAL_Configuration_Wireless80211));

    if (storedBlock == NULL)
    {
        return ConfigurationOperationError_OutOfMemory;
    }

    if (operation == ConfigurationOperation_Delete)
    {
        if (!(flags & ConfigurationOperationFlags_Force))
        {
            if (!ReadWirelessProfile(configuration, configurationIndex, storedBlock))
            {
                result = ConfigurationOperationError_StorageError;
            }
            else if (IsWirelessProfileUsedByNetworkInterface(
                         configuration,
                         GetWirelessProfileId(configuration, storedBlock)))
            {
                result = ConfigurationOperationError_InUse;
            }
        }
    }
    else
    {
        id = GetWirelessProfileId(configuration, block);

        if (id != CONFIGURATION_BLOCK_ID_AUTO && id > CONFIGURATION_BLOCK_ID_MAX)
        {
            result = ConfigurationOperationError_InvalidBlock;
        }
        else if (operation == ConfigurationOperation_Replace)
        {
            if (!ReadWirelessProfile(configuration, configurationIndex, storedBlock))
            {
                result = ConfigurationOperationError_StorageError;
            }
            else
            {
                storedId = GetWirelessProfileId(configuration, storedBlock);

                if (id == CONFIGURATION_BLOCK_ID_AUTO)
                {
                    // keep the Id of the profile being replaced
                    id = storedId;
                    SetWirelessProfileId(configuration, block, id);
                }
                else if (
                    id != storedId && !(flags & ConfigurationOperationFlags_Force) &&
                    IsWirelessProfileUsedByNetworkInterface(configuration, storedId))
                {
                    // a network interface uses the profile by its current Id, changing it would leave that one without
                    // its profile
                    result = ConfigurationOperationError_InUse;
                }

                if (result == ConfigurationOperationError_Success && (flags & ConfigurationOperationFlags_KeepPassword))
                {
                    // passwords can't be read back, so the client asks to keep the one stored
                    if (configuration == DeviceConfigurationOption_Wireless80211Network)
                    {
                        memcpy(
                            ((HAL_Configuration_Wireless80211 *)block)->Password,
                            ((HAL_Configuration_Wireless80211 *)storedBlock)->Password,
                            sizeof(((HAL_Configuration_Wireless80211 *)block)->Password));
                    }
                    else
                    {
                        memcpy(
                            ((HAL_Configuration_WirelessAP *)block)->Password,
                            ((HAL_Configuration_WirelessAP *)storedBlock)->Password,
                            sizeof(((HAL_Configuration_WirelessAP *)block)->Password));
                    }
                }
            }
        }
        else if (id == CONFIGURATION_BLOCK_ID_AUTO)
        {
            // add: assign the lowest Id that isn't in use
            for (id = 0; id <= CONFIGURATION_BLOCK_ID_MAX; id++)
            {
                result = IsWirelessProfileIdInUse(configuration, id, 0xFFFFFFFF, storedBlock, &idInUse);

                if (result != ConfigurationOperationError_Success || !idInUse)
                {
                    break;
                }
            }

            if (result == ConfigurationOperationError_Success)
            {
                SetWirelessProfileId(configuration, block, id);
            }
        }

        if (result == ConfigurationOperationError_Success)
        {
            result = IsWirelessProfileIdInUse(
                configuration,
                id,
                operation == ConfigurationOperation_Replace ? configurationIndex : 0xFFFFFFFF,
                storedBlock,
                &idInUse);

            if (result == ConfigurationOperationError_Success && idInUse)
            {
                result = ConfigurationOperationError_DuplicateId;
            }
        }
    }

    // the stored block has a password, clear it before releasing the memory
    memset(
        storedBlock,
        0,
        sizeof(HAL_Configuration_WirelessAP) > sizeof(HAL_Configuration_Wireless80211)
            ? sizeof(HAL_Configuration_WirelessAP)
            : sizeof(HAL_Configuration_Wireless80211));
    platform_free(storedBlock);

    return result;
}

// builds the default configuration of a network interface, keeping its type, so a delete resets it
static ConfigurationOperationError BuildDefaultNetworkConfiguration(
    uint32_t configurationIndex,
    HAL_Configuration_NetworkInterface *networkConfig)
{
    NetworkInterfaceType interfaceType;

    memset(networkConfig, 0, sizeof(HAL_Configuration_NetworkInterface));

    if (!ConfigurationManager_GetConfigurationBlock(
            networkConfig,
            DeviceConfigurationOption_Network,
            configurationIndex))
    {
        return ConfigurationOperationError_StorageError;
    }

    interfaceType = (NetworkInterfaceType)networkConfig->InterfaceType;

    memset(networkConfig, 0, sizeof(HAL_Configuration_NetworkInterface));
    networkConfig->InterfaceType = interfaceType;

    if (!InitialiseNetworkDefaultConfig(networkConfig, configurationIndex))
    {
        return ConfigurationOperationError_NotSupported;
    }

    memcpy(networkConfig, c_MARKER_CONFIGURATION_NETWORK_V1, sizeof(c_MARKER_CONFIGURATION_NETWORK_V1));

    return ConfigurationOperationError_Success;
}

ConfigurationOperationError ConfigurationManager_ExecuteConfigurationOperation(
    ConfigurationOperation operation,
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex,
    uint32_t flags,
    void *block,
    uint32_t blockSize,
    uint32_t *resultIndex,
    uint32_t *resultId,
    uint32_t *resultCount)
{
    ConfigurationOperationError result = ConfigurationOperationError_Success;
    HAL_ConfigurationBlockInfo info;
    HAL_Configuration_NetworkInterface *defaultNetworkConfig = NULL;
    const unsigned char *marker = GetConfigurationBlockMarker(configuration);
    uint32_t fixedBlockSize = GetFixedConfigurationBlockSize(configuration);
    uint32_t count;

    if (resultIndex != NULL)
    {
        *resultIndex = 0xFFFFFFFF;
    }

    if (resultId != NULL)
    {
        *resultId = 0xFFFFFFFF;
    }

    if (resultCount != NULL)
    {
        *resultCount = ConfigurationManager_GetBlockCount(configuration);
    }

    if (marker == NULL)
    {
        // unknown configuration type, including "all"
        return ConfigurationOperationError_NotSupported;
    }

    ConfigurationManager_GetBlockInfo(configuration, &info);
    count = ConfigurationManager_GetBlockCount(configuration);

    if (info.MaxCount == 0)
    {
        return ConfigurationOperationError_NotSupported;
    }

    // replace at the end of a single instance type is an add
    if (operation == ConfigurationOperation_Replace && (flags & ConfigurationOperationFlags_Upsert) &&
        configurationIndex == count)
    {
        operation = ConfigurationOperation_Add;
    }

    switch (operation)
    {
        case ConfigurationOperation_Add:
            if (!(info.Flags & ConfigurationBlockFlags_CanAdd))
            {
                return ConfigurationOperationError_NotSupported;
            }

            if (count >= info.MaxCount)
            {
                return ConfigurationOperationError_MaxCount;
            }
            break;

        case ConfigurationOperation_Replace:
            if (!(info.Flags & ConfigurationBlockFlags_CanReplace))
            {
                return ConfigurationOperationError_NotSupported;
            }

            if (configurationIndex >= count)
            {
                return ConfigurationOperationError_InvalidIndex;
            }
            break;

        case ConfigurationOperation_Delete:
            if (!(info.Flags & ConfigurationBlockFlags_CanDelete))
            {
                return ConfigurationOperationError_NotSupported;
            }

            if (count == 0)
            {
                return ConfigurationOperationError_NotFound;
            }

            if (configurationIndex >= count)
            {
                return ConfigurationOperationError_InvalidIndex;
            }
            break;

        default:
            return ConfigurationOperationError_NotSupported;
    }

    if (operation != ConfigurationOperation_Delete)
    {
        // validate the block size
        if (block == NULL)
        {
            return ConfigurationOperationError_InvalidBlock;
        }

        if (fixedBlockSize != 0)
        {
            if (blockSize != fixedBlockSize)
            {
                return ConfigurationOperationError_InvalidBlock;
            }
        }
        else
        {
            // certificates: header plus the certificate, which can't be empty
            // (both certificate blocks have the same layout)
            uint32_t headerSize = offsetof(HAL_Configuration_X509CaRootBundle, Certificate);
            uint32_t certificateSize;

            if (blockSize <= headerSize)
            {
                return ConfigurationOperationError_InvalidBlock;
            }

            certificateSize = ((HAL_Configuration_X509CaRootBundle *)block)->CertificateSize;

            if (certificateSize == 0 || certificateSize != blockSize - headerSize)
            {
                return ConfigurationOperationError_InvalidBlock;
            }

            if (blockSize > info.MaxBlockSize)
            {
                return ConfigurationOperationError_NoSpace;
            }
        }

        // the device sets the marker
        memcpy(block, marker, 4);

        if (configuration == DeviceConfigurationOption_WPAuthToken)
        {
            HAL_Configuration_WPAuthToken *tokenBlock = (HAL_Configuration_WPAuthToken *)block;

            if (tokenBlock->TokenLength > WPAUTHTOKEN_CONFIG_MAX_TOKEN_LENGTH)
            {
                return ConfigurationOperationError_InvalidBlock;
            }

            // the token is text, a NUL would end it
            for (uint32_t i = 0; i < tokenBlock->TokenLength; i++)
            {
                if (tokenBlock->Token[i] == 0)
                {
                    return ConfigurationOperationError_InvalidBlock;
                }
            }
        }
    }

    if (IsWirelessProfile(configuration))
    {
        if (info.Flags & ConfigurationBlockFlags_IdIsSlot)
        {
            // the storage assigns the Id and the password can't be read from it
            if (flags & ConfigurationOperationFlags_KeepPassword)
            {
                return ConfigurationOperationError_NotSupported;
            }

            // a profile used by a network interface can't be deleted, unless forced
            if (operation == ConfigurationOperation_Delete)
            {
                result = PrepareWirelessProfileOperation(operation, configuration, configurationIndex, flags, block);

                if (result != ConfigurationOperationError_Success)
                {
                    return result;
                }
            }
        }
        else
        {
            result = PrepareWirelessProfileOperation(operation, configuration, configurationIndex, flags, block);

            if (result != ConfigurationOperationError_Success)
            {
                return result;
            }
        }
    }

    if (operation == ConfigurationOperation_Delete && (info.Flags & ConfigurationBlockFlags_DeleteResets))
    {
        // network interfaces can't be removed, a delete resets the configuration to its defaults
        defaultNetworkConfig =
            (HAL_Configuration_NetworkInterface *)platform_malloc(sizeof(HAL_Configuration_NetworkInterface));

        if (defaultNetworkConfig == NULL)
        {
            return ConfigurationOperationError_OutOfMemory;
        }

        result = BuildDefaultNetworkConfiguration(configurationIndex, defaultNetworkConfig);

        if (result == ConfigurationOperationError_Success)
        {
            operation = ConfigurationOperation_Replace;
            block = defaultNetworkConfig;
            blockSize = sizeof(HAL_Configuration_NetworkInterface);
        }
    }

    if (result == ConfigurationOperationError_Success)
    {
        result = ConfigurationManager_ApplyConfigurationOperation(
            operation,
            configuration,
            configurationIndex,
            block,
            blockSize);
    }

    if (defaultNetworkConfig != NULL)
    {
        platform_free(defaultNetworkConfig);
    }

    if (result == ConfigurationOperationError_Success || result == ConfigurationOperationError_NoChanges)
    {
        count = ConfigurationManager_GetBlockCount(configuration);

        if (resultCount != NULL)
        {
            *resultCount = count;
        }

        if (operation != ConfigurationOperation_Delete)
        {
            uint32_t index = operation == ConfigurationOperation_Add ? count - 1 : configurationIndex;

            if (resultIndex != NULL)
            {
                *resultIndex = index;
            }

            if (resultId != NULL && IsWirelessProfile(configuration))
            {
                // read it back, the storage may have assigned the Id
                void *storedBlock = platform_malloc(fixedBlockSize);

                if (storedBlock != NULL)
                {
                    if (ReadWirelessProfile(configuration, index, storedBlock))
                    {
                        *resultId = GetWirelessProfileId(configuration, storedBlock);
                    }

                    memset(storedBlock, 0, fixedBlockSize);
                    platform_free(storedBlock);
                }
            }
        }
    }

    return result;
}

static void ResetStagedConfigurationOperation()
{
    if (s_stagedOperation.Buffer != NULL)
    {
        // the staged block can be a certificate or a password, clear it before releasing the memory
        memset(s_stagedOperation.Buffer, 0, s_stagedOperation.TotalLength);
        platform_free(s_stagedOperation.Buffer);
    }

    memset(&s_stagedOperation, 0, sizeof(s_stagedOperation));
}

ConfigurationOperationError ConfigurationManager_ProcessConfigurationOperationChunk(
    ConfigurationOperation operation,
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex,
    uint32_t flags,
    uint32_t totalLength,
    uint32_t offset,
    const uint8_t *data,
    uint32_t length,
    uint32_t *resultIndex,
    uint32_t *resultId,
    uint32_t *resultCount)
{
    ConfigurationOperationError result;

    if (resultIndex != NULL)
    {
        *resultIndex = 0xFFFFFFFF;
    }

    if (resultId != NULL)
    {
        *resultId = 0xFFFFFFFF;
    }

    if (resultCount != NULL)
    {
        *resultCount = ConfigurationManager_GetBlockCount(configuration);
    }

    if (flags & ConfigurationOperationFlags_First)
    {
        // a new operation discards any operation that wasn't completed
        ResetStagedConfigurationOperation();

        if (totalLength > 0)
        {
            HAL_ConfigurationBlockInfo info;

            ConfigurationManager_GetBlockInfo(configuration, &info);

            if (info.MaxCount == 0)
            {
                return ConfigurationOperationError_NotSupported;
            }

            if (totalLength > info.MaxBlockSize)
            {
                return GetFixedConfigurationBlockSize(configuration) != 0 ? ConfigurationOperationError_InvalidBlock
                                                                          : ConfigurationOperationError_NoSpace;
            }

            s_stagedOperation.Buffer = (uint8_t *)platform_malloc(totalLength);

            if (s_stagedOperation.Buffer == NULL)
            {
                return ConfigurationOperationError_OutOfMemory;
            }
        }

        s_stagedOperation.TotalLength = totalLength;
        s_stagedOperation.Operation = operation;
        s_stagedOperation.Configuration = configuration;
        s_stagedOperation.Index = configurationIndex;
        s_stagedOperation.Active = true;
    }
    else if (
        !s_stagedOperation.Active || s_stagedOperation.Operation != (uint32_t)operation ||
        s_stagedOperation.Configuration != (uint32_t)configuration || s_stagedOperation.Index != configurationIndex ||
        s_stagedOperation.TotalLength != totalLength)
    {
        ResetStagedConfigurationOperation();

        return ConfigurationOperationError_Sequence;
    }

    // chunks have to arrive in sequence and can't go beyond the block
    if (offset != s_stagedOperation.Received || length > s_stagedOperation.TotalLength - s_stagedOperation.Received)
    {
        ResetStagedConfigurationOperation();

        return ConfigurationOperationError_Sequence;
    }

    if (length > 0)
    {
        memcpy(s_stagedOperation.Buffer + offset, data, length);
        s_stagedOperation.Received += length;
    }

    if (!(flags & ConfigurationOperationFlags_Last))
    {
        return ConfigurationOperationError_Pending;
    }

    if (s_stagedOperation.Received != s_stagedOperation.TotalLength)
    {
        ResetStagedConfigurationOperation();

        return ConfigurationOperationError_Sequence;
    }

    result = ConfigurationManager_ExecuteConfigurationOperation(
        operation,
        configuration,
        configurationIndex,
        flags,
        s_stagedOperation.Buffer,
        s_stagedOperation.TotalLength,
        resultIndex,
        resultId,
        resultCount);

    ResetStagedConfigurationOperation();

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// configuration storage in a flash region (memory mapped)
// the blocks are stored one after the other, aligned to the target flash write granularity, erased flash (0xFF) after
// the last one

// gets the size of the configuration block stored at this address, 0 if there is no block there
// a block that goes beyond the remaining size is taken as reaching the end
static uint32_t GetStoredConfigurationBlockSize(const uint8_t *block, uint32_t remainingSize)
{
    uint32_t blockSize = 0;

    if (remainingSize < 4)
    {
        return 0;
    }

    if (memcmp(block, c_MARKER_CONFIGURATION_NETWORK_V1, 4) == 0)
    {
        blockSize = sizeof(HAL_Configuration_NetworkInterface);
    }
    else if (memcmp(block, c_MARKER_CONFIGURATION_WIRELESS80211_V1, 4) == 0)
    {
        blockSize = sizeof(HAL_Configuration_Wireless80211);
    }
    else if (memcmp(block, c_MARKER_CONFIGURATION_WIRELESS_AP_V1, 4) == 0)
    {
        blockSize = sizeof(HAL_Configuration_WirelessAP);
    }
    else if (memcmp(block, c_MARKER_CONFIGURATION_WPAUTHTOKEN_V1, 4) == 0)
    {
        blockSize = sizeof(HAL_Configuration_WPAuthToken);
    }
    else if (
        memcmp(block, c_MARKER_CONFIGURATION_X509CAROOTBUNDLE_V1, 4) == 0 ||
        memcmp(block, c_MARKER_CONFIGURATION_X509DEVICECERTIFICATE_V1, 4) == 0)
    {
        // both certificate blocks have the same layout
        uint32_t headerSize = offsetof(HAL_Configuration_X509CaRootBundle, Certificate);
        uint32_t certificateSize;

        if (remainingSize < headerSize)
        {
            return remainingSize;
        }

        // the block may not be aligned
        memcpy(
            &certificateSize,
            block + offsetof(HAL_Configuration_X509CaRootBundle, CertificateSize),
            sizeof(uint32_t));

        blockSize = certificateSize > remainingSize - headerSize ? remainingSize : headerSize + certificateSize;
    }

    return blockSize > remainingSize ? remainingSize : blockSize;
}

// walks the configuration blocks stored in a region, block by block, so the content of a block (certificates) is never
// taken as another block
// returns the count of blocks with the marker and fills the addresses of those blocks (up to maxCount)
// usedSize (optional) gets the end of the last block of any type, relative to the start
static uint32_t WalkConfigurationBlocks(
    const uint8_t *start,
    uint32_t size,
    const unsigned char *marker,
    const uint8_t **addresses,
    uint32_t maxCount,
    uint32_t *usedSize)
{
    uint32_t count = 0;
    uint32_t offset = 0;
    uint32_t end = 0;

    while (size >= 4 && offset <= size - 4)
    {
        uint32_t blockSize = GetStoredConfigurationBlockSize(start + offset, size - offset);

        if (blockSize == 0)
        {
            // padding or erased flash
            offset++;
            continue;
        }

        if (marker != NULL && memcmp(start + offset, marker, 4) == 0)
        {
            if (addresses != NULL && count < maxCount)
            {
                addresses[count] = start + offset;
            }

            count++;
        }

        offset += blockSize;
        end = offset;
    }

    if (usedSize != NULL)
    {
        *usedSize = end;
    }

    return count;
}

uint32_t ConfigurationManager_FindConfigurationBlocksInImage(
    const uint8_t *start,
    uint32_t size,
    const unsigned char *marker,
    const uint8_t **addresses,
    uint32_t maxCount)
{
    return WalkConfigurationBlocks(start, size, marker, addresses, maxCount, NULL);
}

static uint32_t AlignConfigurationSize(uint32_t size, uint32_t alignment)
{
    return ((size + alignment - 1) / alignment) * alignment;
}

uint32_t ConfigurationManager_GetConfigurationImageUsedSize(const uint8_t *start, uint32_t size, uint32_t alignment)
{
    uint32_t usedSize;

    WalkConfigurationBlocks(start, size, NULL, NULL, 0, &usedSize);

    return AlignConfigurationSize(usedSize, alignment);
}

// appends a block to the image being built, padded with erased flash to the alignment
// with a NULL image it only computes the size
static bool AppendConfigurationBlockToImage(
    uint8_t *image,
    uint32_t capacity,
    uint32_t *position,
    const uint8_t *block,
    uint32_t blockSize,
    uint32_t alignment)
{
    uint32_t alignedSize = AlignConfigurationSize(blockSize, alignment);

    if (alignedSize > capacity - *position)
    {
        return false;
    }

    if (image != NULL)
    {
        memcpy(image + *position, block, blockSize);
        memset(image + *position + blockSize, 0xFF, alignedSize - blockSize);
    }

    *position += alignedSize;

    return true;
}

// builds the image of the configuration storage with the operation applied: the blocks are copied in their order,
// the one being replaced gets the new content, the one being deleted is left out, the one being added goes last
// with a NULL image it only computes the size (returned in imageSize)
static ConfigurationOperationError BuildConfigurationImage(
    ConfigurationOperation operation,
    const unsigned char *marker,
    uint32_t configurationIndex,
    const uint8_t *block,
    uint32_t blockSize,
    const uint8_t *storage,
    uint32_t storageSize,
    uint32_t alignment,
    uint8_t *image,
    uint32_t *imageSize)
{
    uint32_t position = 0;
    uint32_t offset = 0;
    uint32_t typeIndex = 0;
    bool found = false;

    while (storageSize >= 4 && offset <= storageSize - 4)
    {
        uint32_t storedSize = GetStoredConfigurationBlockSize(storage + offset, storageSize - offset);

        if (storedSize == 0)
        {
            offset++;
            continue;
        }

        if (operation != ConfigurationOperation_Add && memcmp(storage + offset, marker, 4) == 0 &&
            typeIndex++ == configurationIndex)
        {
            found = true;

            if (operation == ConfigurationOperation_Replace &&
                !AppendConfigurationBlockToImage(image, storageSize, &position, block, blockSize, alignment))
            {
                return ConfigurationOperationError_NoSpace;
            }
        }
        else if (!AppendConfigurationBlockToImage(
                     image,
                     storageSize,
                     &position,
                     storage + offset,
                     storedSize,
                     alignment))
        {
            return ConfigurationOperationError_NoSpace;
        }

        offset += storedSize;
    }

    if (operation == ConfigurationOperation_Add)
    {
        if (!AppendConfigurationBlockToImage(image, storageSize, &position, block, blockSize, alignment))
        {
            return ConfigurationOperationError_NoSpace;
        }
    }
    else if (!found)
    {
        return ConfigurationOperationError_InvalidIndex;
    }

    *imageSize = position;

    return ConfigurationOperationError_Success;
}

static bool IsConfigurationRegionErased(const uint8_t *start, uint32_t size)
{
    for (uint32_t i = 0; i < size; i++)
    {
        if (start[i] != 0xFF)
        {
            return false;
        }
    }

    return true;
}

ConfigurationOperationError ConfigurationManager_ApplyConfigurationOperationToImage(
    ConfigurationOperation operation,
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex,
    void *block,
    uint32_t blockSize,
    const uint8_t *storage,
    uint32_t storageSize,
    uint32_t alignment,
    ConfigurationImageWriter writeImage)
{
    ConfigurationOperationError result;
    const unsigned char *marker = GetConfigurationBlockMarker(configuration);
    uint32_t usedSize = ConfigurationManager_GetConfigurationImageUsedSize(storage, storageSize, alignment);
    uint32_t imageSize;
    uint8_t *image;

    if (marker == NULL)
    {
        return ConfigurationOperationError_NotSupported;
    }

    if (operation == ConfigurationOperation_Add)
    {
        uint32_t alignedSize = AlignConfigurationSize(blockSize, alignment);

        if (alignedSize > storageSize - usedSize)
        {
            return ConfigurationOperationError_NoSpace;
        }

        // the new block goes after the last one: if that space is erased it can be written right away
        if (IsConfigurationRegionErased(storage + usedSize, alignedSize))
        {
            image = (uint8_t *)platform_malloc(alignedSize);

            if (image == NULL)
            {
                return ConfigurationOperationError_OutOfMemory;
            }

            memcpy(image, block, blockSize);
            memset(image + blockSize, 0xFF, alignedSize - blockSize);

            result =
                (writeImage(usedSize, image, alignedSize, false) && memcmp(storage + usedSize, image, alignedSize) == 0)
                    ? ConfigurationOperationError_Success
                    : ConfigurationOperationError_StorageError;

            memset(image, 0, alignedSize);
            platform_free(image);

            return result;
        }
    }

    // compute the size of the new image
    result = BuildConfigurationImage(
        operation,
        marker,
        configurationIndex,
        (const uint8_t *)block,
        blockSize,
        storage,
        storageSize,
        alignment,
        NULL,
        &imageSize);

    if (result != ConfigurationOperationError_Success)
    {
        return result;
    }

    image = (uint8_t *)platform_malloc(imageSize > 0 ? imageSize : 1);

    if (image == NULL)
    {
        return ConfigurationOperationError_OutOfMemory;
    }

    result = BuildConfigurationImage(
        operation,
        marker,
        configurationIndex,
        (const uint8_t *)block,
        blockSize,
        storage,
        storageSize,
        alignment,
        image,
        &imageSize);

    if (result == ConfigurationOperationError_Success)
    {
        if (imageSize == usedSize && memcmp(storage, image, imageSize) == 0)
        {
            result = ConfigurationOperationError_NoChanges;
        }
        else if (!writeImage(0, image, imageSize, true) || memcmp(storage, image, imageSize) != 0)
        {
            // the region has been erased (and maybe partially written), the blocks that couldn't be written are lost
            result = ConfigurationOperationError_StorageError;
        }
    }

    // the image has certificates and passwords, clear it before releasing the memory
    memset(image, 0, imageSize);
    platform_free(image);

    return result;
}

bool ConfigurationManager_HasNetworkInterfaceOfType(NetworkInterfaceType interfaceType)
{
    bool found = false;
    uint32_t count = ConfigurationManager_GetBlockCount(DeviceConfigurationOption_Network);

    HAL_Configuration_NetworkInterface *networkConfig =
        (HAL_Configuration_NetworkInterface *)platform_malloc(sizeof(HAL_Configuration_NetworkInterface));

    if (networkConfig == NULL)
    {
        return false;
    }

    for (uint32_t i = 0; i < count && !found; i++)
    {
        memset(networkConfig, 0, sizeof(HAL_Configuration_NetworkInterface));

        if (ConfigurationManager_GetConfigurationBlock(networkConfig, DeviceConfigurationOption_Network, i))
        {
            found = networkConfig->InterfaceType == interfaceType;
        }
    }

    platform_free(networkConfig);

    return found;
}

void ConfigurationManager_GetFlashBlockInfo(
    DeviceConfigurationOption configuration,
    HAL_ConfigurationBlockInfo *info,
    const uint8_t *storage,
    uint32_t storageSize,
    uint32_t alignment,
    uint8_t maxWirelessProfiles,
    uint32_t maxCertificateBlockSize)
{
    uint32_t count;

    (void)storage;
    (void)storageSize;
    (void)alignment;

    memset(info, 0, sizeof(HAL_ConfigurationBlockInfo));

    switch (configuration)
    {
        case DeviceConfigurationOption_Network:
            // network interfaces are created by the device, a delete resets them to the defaults
            count = ConfigurationManager_GetBlockCount(DeviceConfigurationOption_Network);
            info->MaxCount = count > 0 ? (uint8_t)count : 1;
            info->Flags = ConfigurationBlockFlags_CanReplace | ConfigurationBlockFlags_CanDelete |
                          ConfigurationBlockFlags_DeleteResets | ConfigurationBlockFlags_Readable;
            info->MaxBlockSize = sizeof(HAL_Configuration_NetworkInterface);
            break;

        case DeviceConfigurationOption_Wireless80211Network:
            // only if the device has a wireless station
            if (ConfigurationManager_HasNetworkInterfaceOfType(NetworkInterfaceType_Wireless80211))
            {
                info->MaxCount = maxWirelessProfiles;
                info->Flags = ConfigurationBlockFlags_CanAdd | ConfigurationBlockFlags_CanReplace |
                              ConfigurationBlockFlags_CanDelete | ConfigurationBlockFlags_Readable;
                info->MaxBlockSize = sizeof(HAL_Configuration_Wireless80211);
            }
            break;

        case DeviceConfigurationOption_X509CaRootBundle:
        case DeviceConfigurationOption_X509DeviceCertificates:
            // a single certificate block of each type, write only
            info->MaxCount = 1;
            info->Flags =
                ConfigurationBlockFlags_CanAdd | ConfigurationBlockFlags_CanReplace | ConfigurationBlockFlags_CanDelete;

            // limited by the RAM to receive it, the room in the storage is checked when it's stored (a replace frees
            // the room of the certificate it replaces)
            info->MaxBlockSize = maxCertificateBlockSize;
            break;

        case DeviceConfigurationOption_WPAuthToken:
            // only if the device can debug over the network, a single block that can't be read back (it's a secret)
#if CONFIG_NF_WP_CHANNEL_NETWORK
            info->MaxCount = 1;
            info->Flags =
                ConfigurationBlockFlags_CanAdd | ConfigurationBlockFlags_CanReplace | ConfigurationBlockFlags_CanDelete;
            info->MaxBlockSize = sizeof(HAL_Configuration_WPAuthToken);
#endif
            break;

        default:
            // wireless AP isn't supported on these targets
            break;
    }
}

void ConfigurationManager_GetFlashStorageInfo(
    const uint8_t *storage,
    uint32_t storageSize,
    uint32_t alignment,
    uint32_t *totalSize,
    uint32_t *freeSize,
    uint32_t *flags)
{
    *totalSize = storageSize;
    *freeSize = storageSize - ConfigurationManager_GetConfigurationImageUsedSize(storage, storageSize, alignment);

    // storing a block erases and rewrites the region
    *flags = ConfigurationStorageFlags_ErasesStorage;
}
