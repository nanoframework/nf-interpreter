//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NANOHAL_CONFIG_MANAGER_H
#define NANOHAL_CONFIG_MANAGER_H

#include <nanoCLR_Headers.h>
#include <nanoHAL_Network.h>

#ifdef __cplusplus
extern "C"
{
#endif

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // !!! KEEP IN SYNC WITH                                                                            //
    // nanoFramework.Tools.Debugger.DeviceConfiguration.DeviceConfigurationOption (in managed code) !!! //
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    typedef enum DeviceConfigurationOption
    {
        // Network configuration block
        DeviceConfigurationOption_Network = 1,

        // Wireless 802.11 Network configuration block
        DeviceConfigurationOption_Wireless80211Network = 2,

        // Wireless Network as AP configuration block
        DeviceConfigurationOption_WirelessNetworkAP = 3,

        // X509 CA Root Certificates bundle block
        DeviceConfigurationOption_X509CaRootBundle = 4,

        // X509 Device Certificates
        DeviceConfigurationOption_X509DeviceCertificates = 5,

        // Wire Protocol authentication token configuration block (network channel)
        DeviceConfigurationOption_WPAuthToken = 6,

        // All configuration blocks
        DeviceConfigurationOption_All = 255,

    } DeviceConfigurationOption;

    typedef enum UpdateConfigurationResult
    {
        // update succesfull
        UpdateConfigurationResult_Success = 1,

        // update failed
        UpdateConfigurationResult_Failed = 2,

        // update not required
        UpdateConfigurationResult_NoChanges = 3,

    } UpdateConfigurationResult;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // !!! KEEP IN SYNC WITH nanoFramework.Tools.Debugger.WireProtocol.Commands.Monitor_ConfigurationOperation (in
    // managed code) !!! //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // version of the configuration storage API, reported by Monitor_ConfigurationInfo
#define CONFIGURATION_STORAGE_API_VERSION 1

    // operations on configuration blocks
    typedef enum ConfigurationOperation
    {
        // add a block, it gets the next index (the current count)
        ConfigurationOperation_Add = 1,

        // replace the block at the index with the one provided, keeping the index
        ConfigurationOperation_Replace = 2,

        // delete the block at the index, the indexes of the blocks that follow shift down by one
        ConfigurationOperation_Delete = 3,

    } ConfigurationOperation;

    // flags of a configuration operation
    typedef enum ConfigurationOperationFlags
    {
        // first chunk of the block: starts a new operation
        ConfigurationOperationFlags_First = 0x01,

        // last chunk of the block: the operation is executed
        ConfigurationOperationFlags_Last = 0x02,

        // delete a wireless profile even if a network interface is using it
        ConfigurationOperationFlags_Force = 0x04,

        // replace at an index equal to the count adds the block (single instance blocks, like certificates)
        ConfigurationOperationFlags_Upsert = 0x08,

        // replace of a wireless profile keeps the password that is stored (it can't be read back)
        ConfigurationOperationFlags_KeepPassword = 0x10,

    } ConfigurationOperationFlags;

    // outcome of a configuration operation
    typedef enum ConfigurationOperationError
    {
        ConfigurationOperationError_Success = 0,

        // chunk accepted, waiting for the next one
        ConfigurationOperationError_Pending = 1,

        // replace with the same content, nothing was written
        ConfigurationOperationError_NoChanges = 2,

        // configuration type or operation not supported by the target
        ConfigurationOperationError_NotSupported = 10,
        ConfigurationOperationError_InvalidIndex = 11,

        // block with wrong size or invalid content
        ConfigurationOperationError_InvalidBlock = 12,

        // maximum count of blocks of this type reached
        ConfigurationOperationError_MaxCount = 13,
        ConfigurationOperationError_NoSpace = 14,
        ConfigurationOperationError_OutOfMemory = 15,

        // chunk out of sequence or without an operation in progress
        ConfigurationOperationError_Sequence = 16,

        // there is already a wireless profile with this Id
        ConfigurationOperationError_DuplicateId = 17,

        // the wireless profile is used by a network interface (see ConfigurationOperationFlags_Force)
        ConfigurationOperationError_InUse = 18,

        // nothing stored to delete
        ConfigurationOperationError_NotFound = 19,

        // error reading or writing the storage
        ConfigurationOperationError_StorageError = 20,

    } ConfigurationOperationError;

    // flags of a configuration block type
    typedef enum ConfigurationBlockFlags
    {
        ConfigurationBlockFlags_CanAdd = 0x01,
        ConfigurationBlockFlags_CanReplace = 0x02,
        ConfigurationBlockFlags_CanDelete = 0x04,

        // blocks of this type can be read with Monitor_QueryConfiguration
        ConfigurationBlockFlags_Readable = 0x08,

        // delete resets the block to its defaults instead of removing it (network interfaces)
        ConfigurationBlockFlags_DeleteResets = 0x10,

        // the Id is assigned by the storage and can change on replace (Id isn't stored in the block)
        ConfigurationBlockFlags_IdIsSlot = 0x20,

    } ConfigurationBlockFlags;

    // flags of the configuration storage
    typedef enum ConfigurationStorageFlags
    {
        // storing a block erases and rewrites the whole configuration storage
        ConfigurationStorageFlags_ErasesStorage = 0x01,

        // operations are atomic in case of power loss
        ConfigurationStorageFlags_PowerLossSafe = 0x02,

    } ConfigurationStorageFlags;

    // Id to set in a wireless profile so the Id is assigned on add
#define CONFIGURATION_BLOCK_ID_AUTO 0xFFFFFFFF

    // largest Id of a wireless profile (the upper bits are free, e.g. to queue a request with the Id)
#define CONFIGURATION_BLOCK_ID_MAX 0x0FFFFFFF

    // information about a configuration block type
    typedef struct HAL_ConfigurationBlockInfo
    {
        // count of blocks stored
        uint8_t Count;

        // maximum count of blocks, 0 if this type isn't supported
        uint8_t MaxCount;

        // ConfigurationBlockFlags
        uint8_t Flags;

        // size of the block, or the maximum size for blocks of variable size (certificates)
        uint32_t MaxBlockSize;

    } HAL_ConfigurationBlockInfo;

#if defined(_MSC_VER)
// C4200: flexible array members below are intentional (standard C, MSVC flags them in C++)
#pragma warning(push)
#pragma warning(disable : 4200)
#endif

    // network interface configuration struct
    // declared with a flexible array member to allow N config blocks totally independent of compilation
    typedef struct HAL_CONFIGURATION_NETWORK
    {
        // count of the configs elements
        uint8_t Count;

        // pointer to the network interface configuration
        HAL_Configuration_NetworkInterface *Configs[];

    } HAL_CONFIGURATION_NETWORK;

    // network wireless interface configuration struct
    // declared with a flexible array member to allow N config blocks totally independent of compilation
    typedef struct HAL_CONFIGURATION_NETWORK_WIRELESS80211
    {
        // count of the configs elements
        uint8_t Count;

        // pointer to the wireless network interface configuration
        HAL_Configuration_Wireless80211 *Configs[];

    } HAL_CONFIGURATION_NETWORK_WIRELESS80211;

    // network wireless interface configuration struct
    // declared with a flexible array member to allow N config blocks totally independent of compilation
    typedef struct HAL_CONFIGURATION_NETWORK_WIRELESSAP
    {
        // count of the configs elements
        uint8_t Count;

        // pointer to the wireless network interface configuration
        HAL_Configuration_WirelessAP *Configs[];

    } HAL_CONFIGURATION_NETWORK_WIRELESSAP;

    // certificate store struct
    // device certificate(s)
    // declared with a flexible array member to allow N config blocks totally independent of compilation
    typedef struct HAL_CONFIGURATION_X509_CERTIFICATE
    {
        // count of the configs elements
        uint8_t Count;

        // pointer to the certificates
        HAL_Configuration_X509CaRootBundle *Certificates[];

    } HAL_CONFIGURATION_X509_CERTIFICATE;

    // device certificates
    // declared with a flexible array member to allow N config blocks totally independent of compilation
    typedef struct HAL_CONFIGURATION_X509_DEVICE_CERTIFICATE
    {
        // count of the configs elements
        uint8_t Count;

        // pointer to the certificates
        HAL_Configuration_X509DeviceCertificate *Certificates[];

    } HAL_CONFIGURATION_X509_DEVICE_CERTIFICATE;

    // Wire Protocol authentication token configuration
    // declared with a flexible array member to allow N config blocks totally independent of compilation
    typedef struct HAL_CONFIGURATION_WPAUTHTOKEN
    {
        // count of the configs elements
        uint8_t Count;

        // pointer to the Wire Protocol authentication token configuration
        HAL_Configuration_WPAuthToken *Configs[];

    } HAL_CONFIGURATION_WPAUTHTOKEN;

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

    // target configuration storage struct
    // the memory allocation for these will have to be done as required according to the number and type of blocks found
    // in memory
    typedef struct HAL_TARGET_CONFIGURATION
    {
        HAL_CONFIGURATION_NETWORK *NetworkInterfaceConfigs;
        HAL_CONFIGURATION_NETWORK_WIRELESS80211 *Wireless80211Configs;
        HAL_CONFIGURATION_NETWORK_WIRELESSAP *WirelessAPConfigs;
        HAL_CONFIGURATION_X509_CERTIFICATE *CertificateStore;
        HAL_CONFIGURATION_X509_DEVICE_CERTIFICATE *DeviceCertificates;
        HAL_CONFIGURATION_WPAUTHTOKEN *WPAuthTokenConfigs;

    } HAL_TARGET_CONFIGURATION;

    // Initialization of configuration manager
    void ConfigurationManager_Initialize();

    // Enumerates the configuration blocks
    // This is provided as a separate function to allow it to be called when there is a need to re-enumerate the blocks
    // for example when a new block is added from the debug or from the managed application
    void ConfigurationManager_EnumerateConfigurationBlocks();

    // GetConfigurationBlock() is defined in targetHAL_ConfigurationManager.cpp at target level because the target
    // needs to be free to implement the storage of the configuration block as they see fit
    bool ConfigurationManager_GetConfigurationBlock(
        void *configurationBlock,
        DeviceConfigurationOption configuration,
        uint32_t configurationIndex);

    // StoreConfigurationBlock() is defined in targetHAL_ConfigurationManager.cpp at target level because the target
    // needs to be free to implement the storage of the configuration block as they see fit
    // X509 certs can be large and are usually received in chunks. In this case, the block size parameter is the size of
    // the current chunck.
    bool ConfigurationManager_StoreConfigurationBlock(
        void *configurationBlock,
        DeviceConfigurationOption configuration,
        uint32_t configurationIndex,
        uint32_t blockSize,
        uint32_t offset,
        bool done);

    // UpdateConfigurationBlock() is defined in targetHAL_ConfigurationManager.cpp at target level because the target
    // needs to be free to implement the storage of the configuration block as they see fit
    UpdateConfigurationResult ConfigurationManager_UpdateConfigurationBlock(
        void *configurationBlock,
        DeviceConfigurationOption configuration,
        uint32_t configurationIndex);

    // Checks if two configuration blocks are the same. Useful to prevent going through the store process when the
    // content is the same. Default implementation provided for memory addressable implementations. defined as weak so
    // targets are free to implement the storage of the configuration block as they see fit
    bool ConfigurationManager_CheckExistingConfigurationBlock(
        void *existingConfigBlock,
        void *newConfigBlock,
        uint32_t existingConfigBlockSize,
        uint32_t newConfigBlockSize);

    // Gets information about a configuration block type: limits and allowed operations.
    // The count is filled in by the caller from g_TargetConfiguration.
    // Defined as weak, the target provides the limits of its storage. Default is "not supported".
    void ConfigurationManager_GetBlockInfo(DeviceConfigurationOption configuration, HAL_ConfigurationBlockInfo *info);

    // Gets information about the configuration storage: total and free size (0xFFFFFFFF if unknown) and
    // ConfigurationStorageFlags. Defined as weak, the target provides it.
    void ConfigurationManager_GetStorageInfo(uint32_t *totalSize, uint32_t *freeSize, uint32_t *flags);

    // Applies an operation to a configuration block in the storage. This is the primitive the target implements.
    // The block (NULL for delete) has been validated, has the marker set and has its Id assigned.
    // Has to enumerate the configuration blocks again after a successful operation.
    // Defined as weak, default is "not supported".
    ConfigurationOperationError ConfigurationManager_ApplyConfigurationOperation(
        ConfigurationOperation operation,
        DeviceConfigurationOption configuration,
        uint32_t configurationIndex,
        void *block,
        uint32_t blockSize);

    // Executes an operation on a configuration block: validates it, assigns or checks the wireless profile Id,
    // checks if a wireless profile in use is being deleted, then calls
    // ConfigurationManager_ApplyConfigurationOperation. resultIndex, resultId and resultCount are optional outputs
    // (0xFFFFFFFF when not applicable).
    ConfigurationOperationError ConfigurationManager_ExecuteConfigurationOperation(
        ConfigurationOperation operation,
        DeviceConfigurationOption configuration,
        uint32_t configurationIndex,
        uint32_t flags,
        void *block,
        uint32_t blockSize,
        uint32_t *resultIndex,
        uint32_t *resultId,
        uint32_t *resultCount);

    // Receives a chunk of a configuration operation, staging the block in RAM, and executes the operation on the last
    // chunk. Returns ConfigurationOperationError_Pending for the chunks before the last one.
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
        uint32_t *resultCount);

    // writes the configuration storage image (or part of it) to the flash region, at an offset from its start
    // eraseFirst: erase the whole configuration region before writing
    typedef bool (*ConfigurationImageWriter)(uint32_t offset, const uint8_t *data, uint32_t length, bool eraseFirst);

    // Applies an operation to the configuration blocks stored in a memory mapped flash region.
    // For targets that keep the configuration in a flash region: the new image is built in RAM (only the space in use),
    // then the region is erased and written with it. Adding a block in erased space after the last one is written
    // directly. The target enumerates the blocks again after a successful operation.
    ConfigurationOperationError ConfigurationManager_ApplyConfigurationOperationToImage(
        ConfigurationOperation operation,
        DeviceConfigurationOption configuration,
        uint32_t configurationIndex,
        void *block,
        uint32_t blockSize,
        const uint8_t *storage,
        uint32_t storageSize,
        uint32_t alignment,
        ConfigurationImageWriter writeImage);

    // Gets the size in use in a memory mapped configuration region (end of the last block, aligned)
    uint32_t ConfigurationManager_GetConfigurationImageUsedSize(
        const uint8_t *start,
        uint32_t size,
        uint32_t alignment);

    // Gets the information of a configuration block type for targets that keep the configuration in a flash region:
    // network interfaces can be replaced and reset, wireless station profiles (if the device has a wireless station)
    // and a single certificate of each type can be added, replaced and deleted. Wireless AP isn't supported.
    void ConfigurationManager_GetFlashBlockInfo(
        DeviceConfigurationOption configuration,
        HAL_ConfigurationBlockInfo *info,
        const uint8_t *storage,
        uint32_t storageSize,
        uint32_t alignment,
        uint8_t maxWirelessProfiles,
        uint32_t maxCertificateBlockSize);

    // Gets the information of the configuration storage for targets that keep the configuration in a flash region
    void ConfigurationManager_GetFlashStorageInfo(
        const uint8_t *storage,
        uint32_t storageSize,
        uint32_t alignment,
        uint32_t *totalSize,
        uint32_t *freeSize,
        uint32_t *flags);

    // Finds the configuration blocks with the marker in a memory image of the configuration storage, walking the
    // blocks one by one. Returns the count of blocks found and fills their addresses (up to maxCount).
    uint32_t ConfigurationManager_FindConfigurationBlocksInImage(
        const uint8_t *start,
        uint32_t size,
        const unsigned char *marker,
        const uint8_t **addresses,
        uint32_t maxCount);

    // Virtual devices only (win32, netcore, posix): the configuration storage is kept in RAM.
    // A host can set handlers to load it at startup and to save it after each change, to keep it between runs.
    // Return false from the load handler when there is nothing stored (the storage starts empty).
    typedef bool (*ConfigurationStorageLoadHandler)(uint8_t *storage, uint32_t size);
    typedef bool (*ConfigurationStorageSaveHandler)(const uint8_t *storage, uint32_t size);

    void ConfigurationManager_SetVirtualStorageHandlers(
        ConfigurationStorageLoadHandler loadHandler,
        ConfigurationStorageSaveHandler saveHandler);

    // Checks if there is a network interface of this type
    bool ConfigurationManager_HasNetworkInterfaceOfType(NetworkInterfaceType interfaceType);

    // Gets the count of blocks stored, for a configuration type
    uint32_t ConfigurationManager_GetBlockCount(DeviceConfigurationOption configuration);

    //  Default initialisation for wireless config block
    void InitialiseWirelessDefaultConfig(HAL_Configuration_Wireless80211 *pconfig, uint32_t configurationIndex);

    //  Default initialisation for wireless AP config block
    void InitialiseWirelessAPDefaultConfig(HAL_Configuration_WirelessAP *pconfig, uint32_t configurationIndex);

    //  Default initialisation for Network interface config blocks
    // returns FALSE if it's not possible to create a default config block
    bool InitialiseNetworkDefaultConfig(HAL_Configuration_NetworkInterface *pconfig, uint32_t configurationIndex);

    // helper functions

    // function that sweeps a memory region searching for network configuration blocks
    void *ConfigurationManager_FindNetworkConfigurationBlocks(uint32_t startAddress, uint32_t endAddress);

    // function that sweeps a memory region searching for wireless network configuration blocks
    void *ConfigurationManager_FindNetworkWireless80211ConfigurationBlocks(uint32_t startAddress, uint32_t endAddress);

    // function that sweeps a memory region searching for X509 certificates configuration blocks
    void *ConfigurationManager_FindX509CertificateConfigurationBlocks(uint32_t startAddress, uint32_t endAddress);

    // function that sweeps a memory region searching for X509 device certificates configuration blocks
    void *ConfigurationManager_FindX509DeviceCertificatesConfigurationBlocks(
        uint32_t startAddress,
        uint32_t endAddress);

    // function that sweeps a memory region searching for Wire Protocol authentication token configuration blocks
    void *ConfigurationManager_FindWPAuthTokenConfigurationBlocks(uint32_t startAddress, uint32_t endAddress);

    // gets the HAL_Configuration_Wireless80211 configuration block that has the specified Id, if that exists
    // defined as weak needs to be free to implement the storage of the configuration block as they see fit
    HAL_Configuration_Wireless80211 *ConfigurationManager_GetWirelessConfigurationFromId(uint32_t configurationId);

    // gets the HAL_Configuration_WirelessAP configuration block that has the specified Id, if that exists
    // memory is allocated for the configuration block, has to be free by the caller
    // defined as weak to allow replacement at platform/target level to allow different storage management
    HAL_Configuration_WirelessAP *ConfigurationManager_GetWirelessAPConfigurationFromId(uint32_t configurationId);

    // gets the HAL_Configuration_X509CaRootBundle certificate store, if that exists
    // memory is allocated for the configuration block, has to be free by the caller
    // defined as weak to allow replacement at platform/target level to allow different storage management
    HAL_Configuration_X509CaRootBundle *ConfigurationManager_GetCertificateStore();

    // gets the HAL_Configuration_X509DeviceCertificate device certificate, if that exists
    // memory is allocated for the configuration block, has to be free by the caller
    // defined as weak to allow replacement at platform/target level to allow different storage management
    HAL_Configuration_X509DeviceCertificate *ConfigurationManager_GetDeviceCertificate();

    // gets the HAL_Configuration_NetworkInterface configuration block that has the SpecificConfig Id, if that exists
    int32_t ConfigurationManager_FindNetworkConfigurationMatchingWirelessConfigurationFromId(uint32_t configurationId);

    // Gets the OEM model SKU.
    // This is defined as weak to allow the target/platform to provide the implementation.
    void ConfigurationManager_GetOemModelSku(char *model, size_t modelSkuSize);

    // Gets the module serial number.
    // This is defined as weak to allow the target/platform to provide the implementation.
    void ConfigurationManager_GetModuleSerialNumber(char *serialNumber, size_t serialNumberSize);

    // Gets the system serial number.
    // This is defined as weak to allow the target/platform to provide the implementation.
    void ConfigurationManager_GetSystemSerialNumber(char *serialNumber, size_t serialNumberSize);

#ifdef __cplusplus
}
#endif

// declaration of Target configuration union as external, has to be provided at target level
extern HAL_TARGET_CONFIGURATION g_TargetConfiguration;

#endif // NANOHAL_CONFIG_MANAGER_H
