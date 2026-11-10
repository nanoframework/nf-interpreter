//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_v2.h>

__nfweak void ConfigurationManager_Initialize() {};

__nfweak void *ConfigurationManager_FindNetworkConfigurationBlocks(uint32_t startAddress, uint32_t endAddress)
{
    (void)startAddress;
    (void)endAddress;

    return NULL;
}

__nfweak void *ConfigurationManager_FindNetworkWireless80211ConfigurationBlocks(
    uint32_t startAddress,
    uint32_t endAddress)
{
    (void)startAddress;
    (void)endAddress;

    return NULL;
}

__nfweak void *ConfigurationManager_FindX509CertificateConfigurationBlocks(uint32_t startAddress, uint32_t endAddress)
{
    (void)startAddress;
    (void)endAddress;

    return NULL;
}

__nfweak HAL_Configuration_Wireless80211 *ConfigurationManager_GetWirelessConfigurationFromId(uint32_t configurationId)
{
    (void)configurationId;

    return NULL;
}

__nfweak HAL_Configuration_WirelessAP *ConfigurationManager_GetWirelessAPConfigurationFromId(uint32_t configurationId)
{
    (void)configurationId;

    return NULL;
}

__nfweak HAL_Configuration_X509CaRootBundle *ConfigurationManager_GetCertificateStore()
{
    return NULL;
}

__nfweak HAL_Configuration_X509DeviceCertificate *ConfigurationManager_GetDeviceCertificate()
{
    return NULL;
}

__nfweak bool ConfigurationManager_CheckExistingConfigurationBlock(
    void *existingConfigBlock,
    void *newConfigBlock,
    uint32_t existingConfigBlockSize,
    uint32_t newConfigBlockSize)
{
    (void)existingConfigBlock;
    (void)newConfigBlock;
    (void)existingConfigBlockSize;
    (void)newConfigBlockSize;

    return false;
}

__nfweak void ConfigurationManager_GetOemModelSku(char *model, size_t modelSkuSize)
{
    memset(model, 0, modelSkuSize);
}

__nfweak void ConfigurationManager_GetModuleSerialNumber(char *serialNumber, size_t serialNumberSize)
{
    memset(serialNumber, 0, serialNumberSize);
}

__nfweak void ConfigurationManager_GetSystemSerialNumber(char *serialNumber, size_t serialNumberSize)
{
    memset(serialNumber, 0, serialNumberSize);
}

__nfweak uint32_t ConfigurationManager_GetBlockCount(DeviceConfigurationOption configuration)
{
    (void)configuration;

    return 0;
}

__nfweak void ConfigurationManager_GetBlockInfo(
    DeviceConfigurationOption configuration,
    HAL_ConfigurationBlockInfo *info)
{
    (void)configuration;

    memset(info, 0, sizeof(HAL_ConfigurationBlockInfo));
}

__nfweak void ConfigurationManager_GetStorageInfo(uint32_t *totalSize, uint32_t *freeSize, uint32_t *flags)
{
    *totalSize = 0;
    *freeSize = 0;
    *flags = 0;
}

__nfweak ConfigurationOperationError ConfigurationManager_ProcessConfigurationOperationChunk(
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
    (void)operation;
    (void)configuration;
    (void)configurationIndex;
    (void)flags;
    (void)totalLength;
    (void)offset;
    (void)data;
    (void)length;

    *resultIndex = 0xFFFFFFFF;
    *resultId = 0xFFFFFFFF;
    *resultCount = 0;

    return ConfigurationOperationError_NotSupported;
}
