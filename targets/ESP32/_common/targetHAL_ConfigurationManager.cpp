//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoHAL_v2.h>
#include <nanoWeak.h>
#include "esp32_idf.h"
#include <targetHAL_ConfigStorage.h>
#include <target_platform.h>
#include "esp_mac.h"

bool ethernetEnabled = false;

// NVS parameters for Interface config
#define NVS_NAMESPACE "nanoF"

static const char *TAG = "cm";

// maximum count of wireless profiles (station and AP)
// ESP-IDF keeps a single station configuration, the profiles are nanoFramework's own (one file per profile)
#define ESP32_CONFIG_MAX_WIRELESS_PROFILES 10

// maximum count of network interfaces: Wireless Station, Wireless AP, Ethernet and OpenThread
#define ESP32_CONFIG_MAX_NETWORK_INTERFACES 4

// maximum size of a certificate block, it's received in RAM before being stored
#define ESP32_CONFIG_MAX_CERTIFICATE_BLOCK_SIZE (32 * 1024)

// highest file index checked when closing gaps in the indexes
#define ESP32_CONFIG_MAX_FILE_INDEX 16

// #define DEBUG_CONFIG        1

#ifdef DEBUG_CONFIG
void PrintBlock(char *pBlock, int bsize)
{
    char *pCurrStart;
    char *pCurrPos;
    char *pCurrEnd;
    char *pCurrEndBlock = pBlock + bsize;

    pCurrStart = pBlock;

    while (pCurrStart <= pCurrEndBlock)
    {
        pCurrPos = pCurrStart;
        pCurrEnd = pCurrStart + 32;

        while (pCurrPos < pCurrEnd && pCurrPos <= pCurrEndBlock)
        {
            ets_printf("%02X ", *pCurrPos);
            pCurrPos++;
        }

        ets_printf(" ");

        pCurrPos = pCurrStart;
        while (pCurrPos < pCurrEnd && pCurrPos <= pCurrEndBlock)
        {
            if (*pCurrPos < 32)
                ets_printf(".");
            else
                ets_printf("%c", *pCurrPos);
            pCurrPos++;
        }
        ets_printf("\n");

        pCurrStart = pCurrEnd;
    }
}
#endif

// initialization of configuration manager
void ConfigurationManager_Initialize()
{
    memset((void *)&g_TargetConfiguration, 0, sizeof(HAL_TARGET_CONFIGURATION));

    ConfigStorage_Initialise();

    // a write interrupted by a power loss can leave the temporary file behind
    ConfigStorage_DeleteTempFile();

    // enumerate the blocks
    ConfigurationManager_EnumerateConfigurationBlocks();
};

int32_t ConfigurationManager_FindConfigurationBlockSize(
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex)
{
    FILE *handle;
    int32_t configSize = 0;

    handle = ConfigStorage_OpenFile(configuration, configurationIndex, false, false);
    if (handle != nullptr)
    {
        configSize = ConfigStorage_FileSize(handle);
#ifdef DEBUG_CONFIG
        ets_printf("Find type %d index %d, length %d\n", (int)configuration, configurationIndex, configSize);
#endif
        ConfigStorage_CloseFile(handle);
    }

    return configSize;
}

bool StoreConfigBlock(
    DeviceConfigurationOption configType,
    uint32_t configurationIndex,
    void *configBlock,
    size_t writeSize)
{
    bool result = false;
    FILE *fileHandle;

    fileHandle = ConfigStorage_OpenFile(configType, configurationIndex, true, false);
    if (fileHandle != nullptr)
    {
        result = ConfigStorage_WriteFile(fileHandle, (uint8_t *)configBlock, writeSize);
#ifdef DEBUG_CONFIG
        ets_printf(
            "store type %d index %d, length %d result %d\n",
            (int)configType,
            configurationIndex,
            writeSize,
            (int)result);
#endif
        ConfigStorage_CloseFile(fileHandle);
    }

    return result;
}

bool AppendConfigBlock(
    DeviceConfigurationOption configType,
    uint32_t configurationIndex,
    void *configBlock,
    size_t writeSize)
{
    bool result = false;
    FILE *fileHandle;

    fileHandle = ConfigStorage_OpenFile(configType, configurationIndex, true, true);

    if (fileHandle != nullptr)
    {
        result = ConfigStorage_AppendFile(fileHandle, (uint8_t *)configBlock, writeSize);

#ifdef DEBUG_CONFIG
        ets_printf(
            "store type %d index %d, length %d result %d\n",
            (int)configType,
            configurationIndex,
            writeSize,
            (int)result);
#endif

        ConfigStorage_CloseFile(fileHandle);
    }

    return result;
}

// Enumerates the configuration blocks from the configuration flash sector
// implemented with 'weak' attribute so it can be replaced at target level
// frees the memory of the enumerated configuration blocks
static void FreeConfigurationBlocks(HAL_TARGET_CONFIGURATION *configuration)
{
    if (configuration->CertificateStore != nullptr && configuration->CertificateStore->Count > 0)
    {
        platform_free(configuration->CertificateStore->Certificates[0]);
    }

    if (configuration->DeviceCertificates != nullptr && configuration->DeviceCertificates->Count > 0)
    {
        platform_free(configuration->DeviceCertificates->Certificates[0]);
    }

    platform_free(configuration->NetworkInterfaceConfigs);
    platform_free(configuration->Wireless80211Configs);
    platform_free(configuration->WirelessAPConfigs);
    platform_free(configuration->CertificateStore);
    platform_free(configuration->DeviceCertificates);
    platform_free(configuration->WPAuthTokenConfigs);

    memset(configuration, 0, sizeof(HAL_TARGET_CONFIGURATION));
}

// Enumerates the configuration blocks from the configuration flash sector
// implemented with 'weak' attribute so it can be replaced at target level
// enumerates the configuration blocks, returns false if that wasn't possible (the previous enumeration is kept)
static bool EnumerateConfigurationBlocks()
{
    HAL_TARGET_CONFIGURATION enumerated;

    // the blocks are read by index, so the indexes have to be contiguous
    ConfigStorage_CompactFiles(DeviceConfigurationOption_Network, ESP32_CONFIG_MAX_FILE_INDEX);
    ConfigStorage_CompactFiles(DeviceConfigurationOption_Wireless80211Network, ESP32_CONFIG_MAX_FILE_INDEX);
    ConfigStorage_CompactFiles(DeviceConfigurationOption_WirelessNetworkAP, ESP32_CONFIG_MAX_FILE_INDEX);

    HAL_CONFIGURATION_NETWORK *networkConfigs = ConfigStorage_FindNetworkConfigurationBlocks();
    if (networkConfigs == nullptr)
    {
        ESP_LOGE(TAG, "FindNetworkConfigurationBlocks returned NULL(out of memory)");
        return false;
    }

    if (networkConfigs->Count == 0)
    {
        // there is no network config block available, need to create a default for each network interface
        int networkCount = 0;
        NetworkInterfaceType netTypes[ESP32_CONFIG_MAX_NETWORK_INTERFACES] = {NetworkInterfaceType_Unknown};

        // ESP32 can have have up to 4 network interfaces: Wireless Station, Wireless AP, Ethernet and OpenThread

        // Allocate count & types of network interfaces
#if defined(CONFIG_SOC_WIFI_SUPPORTED) || defined(CONFIG_SOC_WIRELESS_HOST_SUPPORTED)
        // Wireless Support
        netTypes[networkCount++] = NetworkInterfaceType_Wireless80211;
        netTypes[networkCount++] = NetworkInterfaceType_WirelessAP;
#endif

#if defined(CONFIG_ESP32_ETHERNET_SUPPORT) && CONFIG_ESP32_ETHERNET_SUPPORT == TRUE
        netTypes[networkCount++] = NetworkInterfaceType_Ethernet;
        ethernetEnabled = true;
#endif

#if HAL_USE_THREAD == TRUE
        netTypes[networkCount++] = NetworkInterfaceType_Thread;
#endif
        // allocate memory for ONE network configuration
        HAL_Configuration_NetworkInterface *networkConfig =
            (HAL_Configuration_NetworkInterface *)platform_malloc(sizeof(HAL_Configuration_NetworkInterface));

        if (networkConfig != nullptr)
        {
            for (int configIndex = 0; configIndex < networkCount; configIndex++)
            {
                memset(networkConfig, 0, sizeof(HAL_Configuration_NetworkInterface));

                networkConfig->InterfaceType = netTypes[configIndex];

                // init to default
                InitialiseNetworkDefaultConfig(networkConfig, configIndex);

                StoreConfigBlock(
                    DeviceConfigurationOption_Network,
                    configIndex,
                    networkConfig,
                    sizeof(HAL_Configuration_NetworkInterface));
            }

            platform_free(networkConfig);
        }

        // have to enumerate again to pick it up
        platform_free(networkConfigs);
        networkConfigs = ConfigStorage_FindNetworkConfigurationBlocks();
        if (!networkConfigs)
        {
            ESP_LOGE(TAG, "Re-enumeration of config returned NULL");
            return false;
        }
        ESP_LOGI(TAG, "networkCount %d/%d", networkCount, networkConfigs->Count);
    }

    // wireless station and AP profiles are not created by default: a device can have none
    // the code that starts the network interfaces has to cope with that

    // build the new enumeration before releasing the current one, so it's never left empty
    enumerated.NetworkInterfaceConfigs = networkConfigs;
    enumerated.Wireless80211Configs = ConfigStorage_FindNetworkWireless80211ConfigurationBlocks();
    enumerated.WirelessAPConfigs = ConfigStorage_FindNetworkWirelessAPConfigurationBlocks();
    enumerated.CertificateStore = ConfigStorage_FindX509CertificateConfigurationBlocks();
    enumerated.DeviceCertificates = ConfigStorage_FindX509DeviceCertificatesConfigurationBlocks();
    enumerated.WPAuthTokenConfigs = ConfigStorage_FindWPAuthTokenConfigurationBlocks();

    if (enumerated.Wireless80211Configs == nullptr || enumerated.WirelessAPConfigs == nullptr ||
        enumerated.CertificateStore == nullptr || enumerated.DeviceCertificates == nullptr ||
        enumerated.WPAuthTokenConfigs == nullptr)
    {
        ESP_LOGE(TAG, "Enumeration of config failed (out of memory)");

        FreeConfigurationBlocks(&enumerated);

        return false;
    }

    // release the previous enumeration
    FreeConfigurationBlocks(&g_TargetConfiguration);

    g_TargetConfiguration = enumerated;

    return true;
}

// Enumerates the configuration blocks from the configuration storage
void ConfigurationManager_EnumerateConfigurationBlocks()
{
    EnumerateConfigurationBlocks();
}

// Build AP default SSID based on the last 3 bytes of MAC
// nano_xxxxxx
void CreateDefaultApSSID(HAL_Configuration_WirelessAP *config)
{
    uint8_t mac[6];
    esp_efuse_mac_get_default(mac);

    hal_strcpy_s((char *)config->Ssid, sizeof(config->Ssid), "nano_");
    char *pMac = (char *)config->Ssid + 5;

    // Last 3 bytes of MAC address
    for (int index = 3; index < 6; index++)
    {
        sprintf(pMac, "%02X", (int)mac[index]);
        pMac += 2;
    }
}

//  Default initialisation of wireless config blocks for ESP32 targets
void InitialiseWirelessDefaultConfig(HAL_Configuration_Wireless80211 *config, uint32_t configurationIndex)
{
    memset(config, 0, sizeof(HAL_Configuration_Wireless80211));

    // make sure the config block marker is set
    memcpy(config, c_MARKER_CONFIGURATION_WIRELESS80211_V1, sizeof(c_MARKER_CONFIGURATION_WIRELESS80211_V1));

    // Wireless station
    config->Id = configurationIndex;

    // Set default to Auto Connect + Enable + WirelessFlags_SmartConfig so station can be started by default
    // Once smart config has run will start up automatically and reconnect of disconnected
    // Application will have to disable Wi-Fi to save power etc
    // if Ethernet enable then disable
    // Disable Wi-Fi if Ethernet is enabled for all targets except esp32_p4 which by default will have both active at
    // same time
#if !defined(CONFIG_IDF_TARGET_ESP32P4)
    if (ethernetEnabled)
    {
        config->Options = Wireless80211Configuration_ConfigurationOptions_Disable;
    }
    else
#endif
    {
        config->Options =
            (Wireless80211Configuration_ConfigurationOptions)(Wireless80211Configuration_ConfigurationOptions_AutoConnect |
                                                              Wireless80211Configuration_ConfigurationOptions_Enable |
                                                              Wireless80211Configuration_ConfigurationOptions_SmartConfig);
    }
}

//  Default initialisation of wireless config blocks for ESP32 targets
void InitialiseWirelessAPDefaultConfig(HAL_Configuration_WirelessAP *config, uint32_t configurationIndex)
{
    memset(config, 0, sizeof(HAL_Configuration_WirelessAP));

    // make sure the config block marker is set
    memcpy(config, c_MARKER_CONFIGURATION_WIRELESS_AP_V1, sizeof(c_MARKER_CONFIGURATION_WIRELESS_AP_V1));

    config->Id = configurationIndex;

    // Build AP default SSID/password based on MAC
    // but don't enable by default
    CreateDefaultApSSID(config);

    hal_strcpy_s((char *)config->Password, sizeof(config->Password), "nanoframework");

    config->Authentication = AuthenticationType_WPA2;
    config->Encryption = EncryptionType_WPA2;
    config->Radio = RadioType__802_11n;
    config->Channel = 11;
    config->MaxConnections = 4;

    // Disable Soft AP (default)
    config->Options = WirelessAPConfiguration_ConfigurationOptions_Disable;
}

//  Default initialisation of Network interface config blocks for ESP32 targets
bool InitialiseNetworkDefaultConfig(HAL_Configuration_NetworkInterface *config, uint32_t configurationIndex)
{
    int macType = -1;

    // make sure the config block marker is set
    memcpy(config->Marker, c_MARKER_CONFIGURATION_NETWORK_V1, sizeof(c_MARKER_CONFIGURATION_NETWORK_V1));

    switch (config->InterfaceType)
    {
        case NetworkInterfaceType_Wireless80211: // Wireless Station
            config->StartupAddressMode = AddressMode_DHCP;
            config->AutomaticDNS = 1;
            config->SpecificConfigId = 0;
            macType = ESP_MAC_WIFI_STA;
            break;

        case NetworkInterfaceType_WirelessAP: // Wireless AP
            config->StartupAddressMode = AddressMode_Static;
            config->SpecificConfigId = 0;
            macType = ESP_MAC_WIFI_SOFTAP;
            // Set default address 192.168.1.1
            // config->IPv4Address
            // config->IPv4NetMask
            // config->IPv4GatewayAddress
            break;

        case NetworkInterfaceType_Ethernet: // Ethernet
            config->StartupAddressMode = AddressMode_DHCP;
            config->AutomaticDNS = 1;
            macType = ESP_MAC_ETH;
            break;

#if HAL_USE_THREAD == TRUE
        case NetworkInterfaceType_Thread: // OpenThread  IPV6 only
            config->StartupAddressMode = AddressMode_Static;
            config->AutomaticDNS = 1;
            break;
#endif

        default:
            break;
    }

    if (macType != -1)
    {
        // get default MAC for interface
        if (esp_read_mac(config->MacAddress, (esp_mac_type_t)macType) != ESP_OK)
        {
            // On ESP32_P4 esp_read_mac can fail with host wifi
            esp_efuse_mac_get_default(config->MacAddress);
            config->MacAddress[5] += macType; // make sure each interface has a different MAC
        }
    }

    // always good
    return TRUE;
}

//
// Get the specified configuration block from global memory ( g_TargetConfiguration )
//
// Parameters:-
//   configurationBlock : Destination to copy configuration data
//   configuration      : Type of configuration block to copy
//   configurationIndex : Index of type to copy
//
// return:-
//    true - OK
//    false- Error/Not found
//
bool ConfigurationManager_GetConfigurationBlock(
    void *configurationBlock,
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex)
{
    int sizeOfBlock = 0;

#ifdef DEBUG_CONFIG
    ets_printf("GetConfig %X %d, %d\n", configurationBlock, (int)configuration, configurationIndex);
#endif

    // validate if the requested block exists
    if (configuration == DeviceConfigurationOption_Network)
    {
        if (g_TargetConfiguration.NetworkInterfaceConfigs->Count == 0 ||
            (configurationIndex + 1) > g_TargetConfiguration.NetworkInterfaceConfigs->Count)
        {
            // there is no network config blocks, init one with default settings
#ifdef DEBUG_CONFIG
            ets_printf("GetConfig CN exit false\n");
#endif
            return false;
        }

        // set block size
        sizeOfBlock = sizeof(HAL_Configuration_NetworkInterface);
    }
    else if (configuration == DeviceConfigurationOption_Wireless80211Network)
    {
        if (g_TargetConfiguration.Wireless80211Configs->Count == 0 ||
            (configurationIndex + 1) > g_TargetConfiguration.Wireless80211Configs->Count)
        {
#ifdef DEBUG_CONFIG
            ets_printf("GetConfig WN exit false\n");
#endif
            return false;
        }

        // set block size
        sizeOfBlock = sizeof(HAL_Configuration_Wireless80211);
    }
    else if (configuration == DeviceConfigurationOption_WirelessNetworkAP)
    {
        if (g_TargetConfiguration.WirelessAPConfigs->Count == 0 ||
            (configurationIndex + 1) > g_TargetConfiguration.WirelessAPConfigs->Count)
        {
#ifdef DEBUG_CONFIG
            ets_printf("GetConfig AP exit false\n");
#endif
            return false;
        }

        // set block size
        sizeOfBlock = sizeof(HAL_Configuration_WirelessAP);
    }
    else if (configuration == DeviceConfigurationOption_X509CaRootBundle)
    {
        if (g_TargetConfiguration.CertificateStore->Count == 0 ||
            (configurationIndex + 1) > g_TargetConfiguration.CertificateStore->Count)
        {
#ifdef DEBUG_CONFIG
            ets_printf("GetConfig XC exit false\n");
#endif
            return false;
        }

        // set block size
        sizeOfBlock = ConfigurationManager_GetConfigurationBlockSize(DeviceConfigurationOption_X509CaRootBundle, 0);
    }
    else if (configuration == DeviceConfigurationOption_X509DeviceCertificates)
    {
        if (g_TargetConfiguration.DeviceCertificates->Count == 0 ||
            (configurationIndex + 1) > g_TargetConfiguration.DeviceCertificates->Count)
        {
#ifdef DEBUG_CONFIG
            ets_printf("GetConfig XC exit false\n");
#endif
            return false;
        }

        // set block size
        // because X509 certificate has a variable length need to compute the block size in two steps
        sizeOfBlock =
            ConfigurationManager_GetConfigurationBlockSize(DeviceConfigurationOption_X509DeviceCertificates, 0);
    }
    else if (configuration == DeviceConfigurationOption_WPAuthToken)
    {
        if (g_TargetConfiguration.WPAuthTokenConfigs->Count == 0 ||
            (configurationIndex + 1) > g_TargetConfiguration.WPAuthTokenConfigs->Count)
        {
            return false;
        }

        // set block size
        sizeOfBlock = sizeof(HAL_Configuration_WPAuthToken);
    }
    else
    {
        // unknown configuration option
        return false;
    }

    // clear memory
    memset(configurationBlock, 0, sizeOfBlock);

    return ConfigurationManager_GetConfigurationBlockFromStorage(
        configuration,
        configurationIndex,
        (uint8_t *)configurationBlock,
        sizeOfBlock);
}

bool ConfigurationManager_StoreConfigurationBlockCertificate(
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex,
    void *configurationBlock,
    uint32_t blockSize,
    uint32_t offset)
{
    if (offset == 0)
    {
        // if this is the 1st chunk of data for a certificate, delete the existing one, if that exists
        if (ConfigStorage_DeleteFile(configuration, configurationIndex))
        {
            ConfigurationManager_EnumerateConfigurationBlocks();
        }
    }

    return AppendConfigBlock(configuration, configurationIndex, configurationBlock, blockSize);
}

//
// Stores the configuration block to the storage
//
// Parameters:-
//   configurationBlock : Pointer to block to store
//   configuration      : Type of configuration block to store or All for block with multiple types
//   configurationIndex : Index of type to store ( not used for All )
//   blockSize          : Size of data pointed to by configurationBlock
//   offset             : Offset of data when multiple blocks (used for X502 CA Root and device certificate).
//                        For remaining types this should be zero.
//                        For ALL type blocks this is the offset in the total data of multiple types.
// Return:-
//    true - OK
//    false- Error/Not found
//
bool ConfigurationManager_StoreConfigurationBlock(
    void *configurationBlock,
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex,
    uint32_t blockSize,
    uint32_t offset,
    bool done)
{
    bool result = false;
    bool requiresEnumeration = false;

#ifdef DEBUG_CONFIG
    ets_printf(
        "StoreConfig config:%d, index:%d  size:%d  offset:%d\n",
        (int)configuration,
        configurationIndex,
        blockSize,
        offset);
#endif

    if (configuration == DeviceConfigurationOption_All)
    {
        // not supported
        return false;
    }
    else
    {
        if (configuration == DeviceConfigurationOption_Network)
        {
            // set blockSize size
            blockSize = sizeof(HAL_Configuration_NetworkInterface);
        }
        else if (configuration == DeviceConfigurationOption_Wireless80211Network)
        {
            // set blockSize size
            blockSize = sizeof(HAL_Configuration_Wireless80211);
        }
        else if (configuration == DeviceConfigurationOption_WirelessNetworkAP)
        {
            // set blockSize size
            blockSize = sizeof(HAL_Configuration_WirelessAP);
        }
        else if (configuration == DeviceConfigurationOption_X509CaRootBundle)
        {
            // check for 0 size on certificate
            if (((HAL_Configuration_X509CaRootBundle *)configurationBlock)->CertificateSize)
            {
                result = ConfigurationManager_StoreConfigurationBlockCertificate(
                    configuration,
                    configurationIndex,
                    configurationBlock,
                    blockSize,
                    offset);

                // reset blockSize because storing has been handled
                blockSize = 0;

                // enumeration is required after we are DONE with SUCCESSFULLY storing all the config chunks
                requiresEnumeration = (result && done);
            }
            else
            {
                // 0 size means that there is a request to clear the certificate
                result = ConfigStorage_DeleteFile(configuration, configurationIndex);

                // there is nothing to store
                blockSize = 0;

                // enumeration is required
                requiresEnumeration = result;
            }

#ifdef DEBUG_CONFIG
            ets_printf(
                "StoreConfig x509 blockSize:%d, certsize:%d",
                blockSize,
                ((HAL_Configuration_X509CaRootBundle *)configurationBlock)->CertificateSize);
#endif
        }
        else if (configuration == DeviceConfigurationOption_X509DeviceCertificates)
        {
            // check for 0 size on certificate
            if (((HAL_Configuration_X509DeviceCertificate *)configurationBlock)->CertificateSize)
            {
                result = ConfigurationManager_StoreConfigurationBlockCertificate(
                    configuration,
                    configurationIndex,
                    configurationBlock,
                    blockSize,
                    offset);

                // reset blockSize because storing has been handled
                blockSize = 0;

                // enumeration is required after we are DONE with SUCCESSFULLY storing all the config chunks
                requiresEnumeration = (result && done);
            }
            else
            {
                // 0 size means that there is a request to clear the certificate
                result = ConfigStorage_DeleteFile(configuration, configurationIndex);

                // there is nothing to store, whether there was a certificate to delete or not
                blockSize = 0;

                // enumeration is required
                requiresEnumeration = result;
            }

#ifdef DEBUG_CONFIG
            ets_printf(
                "StoreDeviceCert blockSize:%d, certsize:%d",
                blockSize,
                ((HAL_Configuration_X509DeviceCertificate *)configurationBlock)->CertificateSize);
#endif
        }
        else
        {
            // Invalid Config
            return false;
        }
    }

    // Anything to save?
    if (blockSize != 0)
    {
        result = StoreConfigBlock(configuration, configurationIndex, configurationBlock, blockSize);

        // a block stored in a new index has to be enumerated
        requiresEnumeration = result;
    }

    if (requiresEnumeration)
    {
        // perform enumeration of configuration blocks
        ConfigurationManager_EnumerateConfigurationBlocks();
    }

#ifdef DEBUG_CONFIG
    ets_printf("StoreConfig exit %d", result);
#endif

    return result;
}

// Updates a configuration block in the configuration storage
UpdateConfigurationResult ConfigurationManager_UpdateConfigurationBlock(
    void *configurationBlock,
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex)
{
    // figure out the block size first
    uint32_t blockSize = 0;

    switch (configuration)
    {
        case DeviceConfigurationOption_Network:
            blockSize = sizeof(HAL_Configuration_NetworkInterface);
            break;

        case DeviceConfigurationOption_Wireless80211Network:
            blockSize = sizeof(HAL_Configuration_Wireless80211);
            break;

        case DeviceConfigurationOption_WirelessNetworkAP:
            blockSize = sizeof(HAL_Configuration_WirelessAP);
            break;

        case DeviceConfigurationOption_X509CaRootBundle:
            blockSize = offsetof(HAL_Configuration_X509CaRootBundle, Certificate);
            blockSize += ((HAL_Configuration_X509CaRootBundle *)configurationBlock)->CertificateSize;
            break;

        case DeviceConfigurationOption_X509DeviceCertificates:
            blockSize = offsetof(HAL_Configuration_X509DeviceCertificate, Certificate);
            blockSize += ((HAL_Configuration_X509DeviceCertificate *)configurationBlock)->CertificateSize;
            break;

        default:
            // shouldn't ever reach here
            return UpdateConfigurationResult_Failed;
    }

    if (ConfigurationManager_StoreConfigurationBlock(
            configurationBlock,
            configuration,
            configurationIndex,
            blockSize,
            0,
            false))
    {
        return UpdateConfigurationResult_Success;
    }
    else
    {
        return UpdateConfigurationResult_Failed;
    }
}

HAL_Configuration_Wireless80211 *ConfigurationManager_GetWirelessConfigurationFromId(uint32_t configurationId)
{
    HAL_Configuration_Wireless80211 *wirelessConfig =
        (HAL_Configuration_Wireless80211 *)platform_malloc(sizeof(HAL_Configuration_Wireless80211));

    if (wirelessConfig)
    {
        // loop though all Wireless config blocks trying to find one with this ID
        for (int index = 0; index < g_TargetConfiguration.Wireless80211Configs->Count; index++)
        {
            if (ConfigurationManager_GetConfigurationBlock(
                    wirelessConfig,
                    DeviceConfigurationOption_Wireless80211Network,
                    index))
            {

                if (wirelessConfig->Id == configurationId)
                {
                    return wirelessConfig;
                }
            }
        }
    }

    if (wirelessConfig != nullptr)
    {
        platform_free(wirelessConfig);
    }

    // not found, or failed to allocate memory
    return nullptr;
}

HAL_Configuration_WirelessAP *ConfigurationManager_GetWirelessAPConfigurationFromId(uint32_t configurationId)
{
    HAL_Configuration_WirelessAP *wirelessAPConfig =
        (HAL_Configuration_WirelessAP *)platform_malloc(sizeof(HAL_Configuration_WirelessAP));

    if (wirelessAPConfig)
    {
        // loop though all Wireless AP config blocks trying to find one with this ID
        for (int index = 0; index < g_TargetConfiguration.WirelessAPConfigs->Count; index++)
        {
            if (ConfigurationManager_GetConfigurationBlock(
                    wirelessAPConfig,
                    DeviceConfigurationOption_WirelessNetworkAP,
                    index))
            {

                if (wirelessAPConfig->Id == configurationId)
                {
                    return wirelessAPConfig;
                }
            }
        }
    }

    if (wirelessAPConfig != nullptr)
    {
        platform_free(wirelessAPConfig);
    }

    // not found, or failed to allocate memory
    return nullptr;
}

HAL_Configuration_X509CaRootBundle *ConfigurationManager_GetCertificateStore()
{
    if (g_TargetConfiguration.CertificateStore->Count)
    {
        // get cert store size
        int32_t certSize =
            ConfigurationManager_GetConfigurationBlockSize(DeviceConfigurationOption_X509CaRootBundle, 0);

        if (certSize > 0)
        {
            HAL_Configuration_X509CaRootBundle *certStore =
                (HAL_Configuration_X509CaRootBundle *)platform_malloc(certSize);

            if (certStore != nullptr)
            {
                if (ConfigurationManager_GetConfigurationBlock(
                        certStore,
                        DeviceConfigurationOption_X509CaRootBundle,
                        0))
                {
                    return certStore;
                }

                platform_free(certStore);
            }
        }
    }

    // not found, or failed to allocate memory
    return nullptr;
}

HAL_Configuration_X509DeviceCertificate *ConfigurationManager_GetDeviceCertificate()
{
    if (g_TargetConfiguration.DeviceCertificates->Count)
    {
        // get cert store size
        int32_t certSize =
            ConfigurationManager_GetConfigurationBlockSize(DeviceConfigurationOption_X509DeviceCertificates, 0);

        if (certSize > 0)
        {
            HAL_Configuration_X509DeviceCertificate *deviceCert =
                (HAL_Configuration_X509DeviceCertificate *)platform_malloc(certSize);

            if (deviceCert != nullptr)
            {
                if (ConfigurationManager_GetConfigurationBlock(
                        deviceCert,
                        DeviceConfigurationOption_X509DeviceCertificates,
                        0))
                {
                    return deviceCert;
                }

                platform_free(deviceCert);
            }
        }
    }

    // not found, or failed to allocate memory
    return nullptr;
}

// default implementation
// this is weak so a manufacturer can provide a strong implementation
__nfweak void ConfigurationManager_GetSystemSerialNumber(char *serialNumber, size_t serialNumberSize)
{
    memset(serialNumber, 0, serialNumberSize);

    // Use the factory-provided MAC address as unique ID
    uint8_t macAddress[6];
    esp_err_t err = esp_read_mac(macAddress, ESP_MAC_EFUSE_FACTORY);
    if (err == ESP_OK)
    {
        memcpy(&serialNumber[serialNumberSize - sizeof(macAddress)], macAddress, sizeof(macAddress));

        // Disambiguation is needed because the hardware-specific identifier used to create the
        // default serial number on other platforms may be in the same range.
        // Set the first byte to a number that is unique (within the nanoFramework CLR) for ESP32.
        serialNumber[0] = 1;
    }
}

void ConfigurationManager_GetBlockInfo(DeviceConfigurationOption configuration, HAL_ConfigurationBlockInfo *info)
{
    uint32_t totalSize;
    uint32_t usedSize;

    memset(info, 0, sizeof(HAL_ConfigurationBlockInfo));

    switch (configuration)
    {
        case DeviceConfigurationOption_Network:
            // network interfaces are created by the device, a delete resets them to the defaults
            info->MaxCount = ESP32_CONFIG_MAX_NETWORK_INTERFACES;
            info->Flags = ConfigurationBlockFlags_CanReplace | ConfigurationBlockFlags_CanDelete |
                          ConfigurationBlockFlags_DeleteResets | ConfigurationBlockFlags_Readable;
            info->MaxBlockSize = sizeof(HAL_Configuration_NetworkInterface);
            break;

#if defined(CONFIG_SOC_WIFI_SUPPORTED) || defined(CONFIG_SOC_WIRELESS_HOST_SUPPORTED)
        case DeviceConfigurationOption_Wireless80211Network:
            info->MaxCount = ESP32_CONFIG_MAX_WIRELESS_PROFILES;
            info->Flags = ConfigurationBlockFlags_CanAdd | ConfigurationBlockFlags_CanReplace |
                          ConfigurationBlockFlags_CanDelete | ConfigurationBlockFlags_Readable;
            info->MaxBlockSize = sizeof(HAL_Configuration_Wireless80211);
            break;

        case DeviceConfigurationOption_WirelessNetworkAP:
            info->MaxCount = ESP32_CONFIG_MAX_WIRELESS_PROFILES;
            info->Flags = ConfigurationBlockFlags_CanAdd | ConfigurationBlockFlags_CanReplace |
                          ConfigurationBlockFlags_CanDelete | ConfigurationBlockFlags_Readable;
            info->MaxBlockSize = sizeof(HAL_Configuration_WirelessAP);
            break;
#endif

        case DeviceConfigurationOption_X509CaRootBundle:
        case DeviceConfigurationOption_X509DeviceCertificates:
            // a single certificate block of each type, write only
            info->MaxCount = 1;
            info->Flags =
                ConfigurationBlockFlags_CanAdd | ConfigurationBlockFlags_CanReplace | ConfigurationBlockFlags_CanDelete;

            // limited by the RAM to receive it and by the free space
            ConfigStorage_GetInfo(&totalSize, &usedSize);
            info->MaxBlockSize = ESP32_CONFIG_MAX_CERTIFICATE_BLOCK_SIZE;

            if (totalSize > usedSize && totalSize - usedSize < info->MaxBlockSize)
            {
                info->MaxBlockSize = totalSize - usedSize;
            }
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
            break;
    }
}

void ConfigurationManager_GetStorageInfo(uint32_t *totalSize, uint32_t *freeSize, uint32_t *flags)
{
    uint32_t usedSize;

    ConfigStorage_GetInfo(totalSize, &usedSize);

    *freeSize = *totalSize >= usedSize ? *totalSize - usedSize : 0;

    // blocks are written to a temporary file and then renamed
    *flags = ConfigurationStorageFlags_PowerLossSafe;
}

ConfigurationOperationError ConfigurationManager_ApplyConfigurationOperation(
    ConfigurationOperation operation,
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex,
    void *block,
    uint32_t blockSize)
{
    bool success = false;
    uint32_t newIndex;

    switch (operation)
    {
        case ConfigurationOperation_Add:
            // the new block goes after the last one, in a file that doesn't exist (a gap that couldn't be closed may
            // have left one there)
            newIndex = ConfigurationManager_GetBlockCount(configuration);

            while (newIndex < ESP32_CONFIG_MAX_FILE_INDEX && ConfigStorage_FileExists(configuration, newIndex))
            {
                newIndex++;
            }

            success = newIndex < ESP32_CONFIG_MAX_FILE_INDEX &&
                      ConfigStorage_WriteFileAtomic(configuration, newIndex, (uint8_t *)block, blockSize);
            break;

        case ConfigurationOperation_Replace:
            success = ConfigStorage_WriteFileAtomic(configuration, configurationIndex, (uint8_t *)block, blockSize);
            break;

        case ConfigurationOperation_Delete:
            success = ConfigStorage_DeleteFile(configuration, configurationIndex);

            if (success)
            {
                // the blocks that follow move down one index
                ConfigStorage_CompactFiles(configuration, ESP32_CONFIG_MAX_FILE_INDEX);
            }
            break;

        default:
            return ConfigurationOperationError_NotSupported;
    }

    if (!success)
    {
        return ConfigurationOperationError_StorageError;
    }

    // the block is stored, but the lists in g_TargetConfiguration are stale until they are enumerated again
    if (!EnumerateConfigurationBlocks())
    {
        return ConfigurationOperationError_OutOfMemory;
    }

    return ConfigurationOperationError_Success;
}
