//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_v2.h>

// Configuration storage for the virtual devices (win32, netcore and posix).
// The configuration blocks are kept in RAM, with the same layout as in the flash region of the MCU targets, so the
// configuration storage operations can be exercised without hardware.
// The storage starts empty (with a default wireless network interface) on every run. To keep it between runs, the host
// sets the handlers to load and save it with ConfigurationManager_SetVirtualStorageHandlers.

// size of the configuration storage
#define VIRTUAL_CONFIG_STORAGE_SIZE (64 * 1024)

// maximum count of wireless station profiles
#define VIRTUAL_CONFIG_MAX_WIRELESS_PROFILES 10

// maximum size of a certificate block
#define VIRTUAL_CONFIG_MAX_CERTIFICATE_BLOCK_SIZE (32 * 1024)

HAL_TARGET_CONFIGURATION g_TargetConfiguration;

static uint8_t s_configurationStorage[VIRTUAL_CONFIG_STORAGE_SIZE];
static bool s_configurationStorageLoaded = false;

static ConfigurationStorageLoadHandler s_loadHandler = nullptr;
static ConfigurationStorageSaveHandler s_saveHandler = nullptr;

void ConfigurationManager_SetVirtualStorageHandlers(
    ConfigurationStorageLoadHandler loadHandler,
    ConfigurationStorageSaveHandler saveHandler)
{
    s_loadHandler = loadHandler;
    s_saveHandler = saveHandler;
}

// writes the configuration image (or part of it) to the storage in RAM, then saves it, if there is a handler for that
static bool WriteConfigurationImage(uint32_t offset, const uint8_t *data, uint32_t length, bool eraseFirst)
{
    if (offset > sizeof(s_configurationStorage) || length > sizeof(s_configurationStorage) - offset)
    {
        return false;
    }

    if (eraseFirst)
    {
        memset(s_configurationStorage, 0xFF, sizeof(s_configurationStorage));
    }

    memcpy(s_configurationStorage + offset, data, length);

    return s_saveHandler == nullptr || s_saveHandler(s_configurationStorage, sizeof(s_configurationStorage));
}

// finds the blocks with the marker and allocates the HAL_CONFIGURATION_xxx struct with their addresses
static void *FindVirtualConfigurationBlocks(const unsigned char *marker)
{
    // all the HAL_CONFIGURATION_xxx structs have the same layout
    HAL_CONFIGURATION_NETWORK *configs;

    uint32_t count = ConfigurationManager_FindConfigurationBlocksInImage(
        s_configurationStorage,
        sizeof(s_configurationStorage),
        marker,
        nullptr,
        0);

    configs = (HAL_CONFIGURATION_NETWORK *)platform_malloc(
        offsetof(HAL_CONFIGURATION_NETWORK, Configs) + count * sizeof(configs->Configs[0]));

    if (configs != nullptr)
    {
        configs->Count = (uint8_t)count;

        ConfigurationManager_FindConfigurationBlocksInImage(
            s_configurationStorage,
            sizeof(s_configurationStorage),
            marker,
            (const uint8_t **)configs->Configs,
            count);
    }

    return configs;
}

void ConfigurationManager_EnumerateConfigurationBlocks()
{
    platform_free(g_TargetConfiguration.NetworkInterfaceConfigs);
    platform_free(g_TargetConfiguration.Wireless80211Configs);
    platform_free(g_TargetConfiguration.WirelessAPConfigs);
    platform_free(g_TargetConfiguration.CertificateStore);
    platform_free(g_TargetConfiguration.DeviceCertificates);

    g_TargetConfiguration.NetworkInterfaceConfigs =
        (HAL_CONFIGURATION_NETWORK *)FindVirtualConfigurationBlocks(c_MARKER_CONFIGURATION_NETWORK_V1);
    g_TargetConfiguration.Wireless80211Configs =
        (HAL_CONFIGURATION_NETWORK_WIRELESS80211 *)FindVirtualConfigurationBlocks(
            c_MARKER_CONFIGURATION_WIRELESS80211_V1);
    g_TargetConfiguration.WirelessAPConfigs =
        (HAL_CONFIGURATION_NETWORK_WIRELESSAP *)FindVirtualConfigurationBlocks(c_MARKER_CONFIGURATION_WIRELESS_AP_V1);
    g_TargetConfiguration.CertificateStore = (HAL_CONFIGURATION_X509_CERTIFICATE *)FindVirtualConfigurationBlocks(
        c_MARKER_CONFIGURATION_X509CAROOTBUNDLE_V1);
    g_TargetConfiguration.DeviceCertificates =
        (HAL_CONFIGURATION_X509_DEVICE_CERTIFICATE *)FindVirtualConfigurationBlocks(
            c_MARKER_CONFIGURATION_X509DEVICECERTIFICATE_V1);
}

void ConfigurationManager_Initialize()
{
    // the storage is kept across soft reboots
    if (!s_configurationStorageLoaded)
    {
        memset(s_configurationStorage, 0xFF, sizeof(s_configurationStorage));
        memset(&g_TargetConfiguration, 0, sizeof(g_TargetConfiguration));

        if (s_loadHandler != nullptr && !s_loadHandler(s_configurationStorage, sizeof(s_configurationStorage)))
        {
            // nothing loaded, start empty
            memset(s_configurationStorage, 0xFF, sizeof(s_configurationStorage));
        }

        // a new storage gets a wireless network interface, so the wireless profiles can be used
        if (ConfigurationManager_FindConfigurationBlocksInImage(
                s_configurationStorage,
                sizeof(s_configurationStorage),
                c_MARKER_CONFIGURATION_NETWORK_V1,
                nullptr,
                0) == 0)
        {
            HAL_Configuration_NetworkInterface networkConfig;

            memset(&networkConfig, 0, sizeof(networkConfig));
            networkConfig.InterfaceType = NetworkInterfaceType_Wireless80211;

            InitialiseNetworkDefaultConfig(&networkConfig, 0);

            ConfigurationManager_ApplyConfigurationOperationToImage(
                ConfigurationOperation_Add,
                DeviceConfigurationOption_Network,
                0,
                &networkConfig,
                sizeof(networkConfig),
                s_configurationStorage,
                sizeof(s_configurationStorage),
                1,
                WriteConfigurationImage);
        }

        s_configurationStorageLoaded = true;
    }

    ConfigurationManager_EnumerateConfigurationBlocks();
}

// gets the list of blocks of a configuration type
static uint8_t **GetVirtualConfigurationBlocks(DeviceConfigurationOption configuration, uint32_t *count)
{
    // all the HAL_CONFIGURATION_xxx structs have the same layout
    HAL_CONFIGURATION_NETWORK *configs = nullptr;

    switch (configuration)
    {
        case DeviceConfigurationOption_Network:
            configs = g_TargetConfiguration.NetworkInterfaceConfigs;
            break;

        case DeviceConfigurationOption_Wireless80211Network:
            configs = (HAL_CONFIGURATION_NETWORK *)g_TargetConfiguration.Wireless80211Configs;
            break;

        case DeviceConfigurationOption_WirelessNetworkAP:
            configs = (HAL_CONFIGURATION_NETWORK *)g_TargetConfiguration.WirelessAPConfigs;
            break;

        case DeviceConfigurationOption_X509CaRootBundle:
            configs = (HAL_CONFIGURATION_NETWORK *)g_TargetConfiguration.CertificateStore;
            break;

        case DeviceConfigurationOption_X509DeviceCertificates:
            configs = (HAL_CONFIGURATION_NETWORK *)g_TargetConfiguration.DeviceCertificates;
            break;

        default:
            break;
    }

    if (configs == nullptr)
    {
        *count = 0;
        return nullptr;
    }

    *count = configs->Count;

    return (uint8_t **)configs->Configs;
}

// gets the size of a configuration block
static uint32_t GetVirtualConfigurationBlockSize(DeviceConfigurationOption configuration, const void *block)
{
    uint32_t certificateSize;

    switch (configuration)
    {
        case DeviceConfigurationOption_Network:
            return sizeof(HAL_Configuration_NetworkInterface);

        case DeviceConfigurationOption_Wireless80211Network:
            return sizeof(HAL_Configuration_Wireless80211);

        case DeviceConfigurationOption_WirelessNetworkAP:
            return sizeof(HAL_Configuration_WirelessAP);

        case DeviceConfigurationOption_X509CaRootBundle:
        case DeviceConfigurationOption_X509DeviceCertificates:
            // both certificate blocks have the same layout, the block may not be aligned
            memcpy(
                &certificateSize,
                (const uint8_t *)block + offsetof(HAL_Configuration_X509CaRootBundle, CertificateSize),
                sizeof(uint32_t));
            return offsetof(HAL_Configuration_X509CaRootBundle, Certificate) + certificateSize;

        default:
            return 0;
    }
}

bool ConfigurationManager_GetConfigurationBlock(
    void *configurationBlock,
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex)
{
    uint32_t count;
    uint8_t **blocks = GetVirtualConfigurationBlocks(configuration, &count);

    if (blocks == nullptr || configurationIndex >= count)
    {
        return false;
    }

    memcpy(
        configurationBlock,
        blocks[configurationIndex],
        GetVirtualConfigurationBlockSize(configuration, blocks[configurationIndex]));

    return true;
}

bool ConfigurationManager_StoreConfigurationBlock(
    void *configurationBlock,
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex,
    uint32_t blockSize,
    uint32_t offset,
    bool done)
{
    (void)blockSize;
    (void)done;

    // only complete blocks are supported: the configuration storage operations handle chunks
    if (offset != 0)
    {
        return false;
    }

    ConfigurationOperationError result = ConfigurationManager_ExecuteConfigurationOperation(
        configurationIndex < ConfigurationManager_GetBlockCount(configuration) ? ConfigurationOperation_Replace
                                                                               : ConfigurationOperation_Add,
        configuration,
        configurationIndex,
        0,
        configurationBlock,
        GetVirtualConfigurationBlockSize(configuration, configurationBlock),
        nullptr,
        nullptr,
        nullptr);

    return result == ConfigurationOperationError_Success || result == ConfigurationOperationError_NoChanges;
}

UpdateConfigurationResult ConfigurationManager_UpdateConfigurationBlock(
    void *configurationBlock,
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex)
{
    return ConfigurationManager_StoreConfigurationBlock(
               configurationBlock,
               configuration,
               configurationIndex,
               0,
               0,
               true)
               ? UpdateConfigurationResult_Success
               : UpdateConfigurationResult_Failed;
}

void InitialiseWirelessDefaultConfig(HAL_Configuration_Wireless80211 *pconfig, uint32_t configurationIndex)
{
    memset(pconfig, 0, sizeof(HAL_Configuration_Wireless80211));

    memcpy(pconfig, c_MARKER_CONFIGURATION_WIRELESS80211_V1, sizeof(c_MARKER_CONFIGURATION_WIRELESS80211_V1));

    pconfig->Id = configurationIndex;
    pconfig->Options = Wireless80211Configuration_ConfigurationOptions_Enable;
}

void InitialiseWirelessAPDefaultConfig(HAL_Configuration_WirelessAP *pconfig, uint32_t configurationIndex)
{
    memset(pconfig, 0, sizeof(HAL_Configuration_WirelessAP));

    memcpy(pconfig, c_MARKER_CONFIGURATION_WIRELESS_AP_V1, sizeof(c_MARKER_CONFIGURATION_WIRELESS_AP_V1));

    pconfig->Id = configurationIndex;
    pconfig->Options = WirelessAPConfiguration_ConfigurationOptions_Disable;
}

bool InitialiseNetworkDefaultConfig(HAL_Configuration_NetworkInterface *pconfig, uint32_t configurationIndex)
{
    (void)configurationIndex;

    memcpy(pconfig, c_MARKER_CONFIGURATION_NETWORK_V1, sizeof(c_MARKER_CONFIGURATION_NETWORK_V1));

    pconfig->StartupAddressMode = AddressMode_DHCP;
    pconfig->AutomaticDNS = 1;
    pconfig->SpecificConfigId = 0;

    return true;
}

void ConfigurationManager_GetBlockInfo(DeviceConfigurationOption configuration, HAL_ConfigurationBlockInfo *info)
{
    ConfigurationManager_GetFlashBlockInfo(
        configuration,
        info,
        s_configurationStorage,
        sizeof(s_configurationStorage),
        1,
        VIRTUAL_CONFIG_MAX_WIRELESS_PROFILES,
        VIRTUAL_CONFIG_MAX_CERTIFICATE_BLOCK_SIZE);
}

void ConfigurationManager_GetStorageInfo(uint32_t *totalSize, uint32_t *freeSize, uint32_t *flags)
{
    ConfigurationManager_GetFlashStorageInfo(
        s_configurationStorage,
        sizeof(s_configurationStorage),
        1,
        totalSize,
        freeSize,
        flags);

    // it's RAM, nothing is erased
    *flags = 0;
}

ConfigurationOperationError ConfigurationManager_ApplyConfigurationOperation(
    ConfigurationOperation operation,
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex,
    void *block,
    uint32_t blockSize)
{
    ConfigurationOperationError result = ConfigurationManager_ApplyConfigurationOperationToImage(
        operation,
        configuration,
        configurationIndex,
        block,
        blockSize,
        s_configurationStorage,
        sizeof(s_configurationStorage),
        1,
        WriteConfigurationImage);

    if (result == ConfigurationOperationError_Success)
    {
        ConfigurationManager_EnumerateConfigurationBlocks();
    }

    return result;
}
