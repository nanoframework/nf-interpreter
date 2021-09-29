//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoHAL_v2.h>
#include <nanoWeak.h>
#include "esp32_idf.h"
#include "targetHAL_ConfigStorage.h"
#include <target_platform.h>

// NVS parameters for Interface config
#define NVS_NAMESPACE "nanoF"

// #define DEBUG_CONFIG        1

// Saves the remaining bundle size for multiple block cert configs
static int32_t savedBundleSize;
static int32_t remainingBundleSize;
static int32_t savedBundleOffset;

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
    memset((void *)&g_TargetConfiguration, 0, sizeof(g_TargetConfiguration));

    ConfigStorage_Initialise();

    // enumerate the blocks
    ConfigurationManager_EnumerateConfigurationBlocks();
};

int32_t ConfigurationManager_FindConfigurationBlockSize(
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex)
{
    FILE *handle;
    int32_t configSize = 0;

    handle = ConfigStorage_OpenFile(configuration, configurationIndex, false);
    if (handle != NULL)
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
    void *pConfigBlock,
    size_t writeSize)
{
    bool result = false;
    FILE *fileHandle;

    fileHandle = ConfigStorage_OpenFile(configType, configurationIndex, true);
    if (fileHandle != NULL)
    {
        result = ConfigStorage_WriteFile(fileHandle, (uint8_t *)pConfigBlock, writeSize);
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
// it's implemented with 'weak' attribute so it can be replaced at target level if a different persistance mechanism is
// used
void ConfigurationManager_EnumerateConfigurationBlocks()
{
    HAL_CONFIGURATION_NETWORK *networkConfigs = ConfigStorage_FindNetworkConfigurationBlocks();

    // check network configs count
    if (networkConfigs->Count == 0)
    {
        // there is no network config block available, need to create a default for each network interface
        int networkCount;

        // ESP32 can have has much as 3 network interfaces: Wireless Station, Wireless AP and Ethernet
#ifdef ESP32_ETHERNET_SUPPORT
        networkCount = 3;
#else
        networkCount = 2;
#endif

        // allocate memory for ONE network configuration
        HAL_Configuration_NetworkInterface *networkConfig =
            (HAL_Configuration_NetworkInterface *)platform_malloc(sizeof(HAL_Configuration_NetworkInterface));

        for (int configIndex = 0; configIndex < networkCount; configIndex++)
        {
            // clear memory
            memset(networkConfig, 0, sizeof(HAL_Configuration_NetworkInterface));

            // init to default
            InitialiseNetworkDefaultConfig(networkConfig, configIndex);

            // store
            StoreConfigBlock(
                DeviceConfigurationOption_Network,
                configIndex,
                networkConfig,
                sizeof(HAL_Configuration_NetworkInterface));
        }

        // free memory
        platform_free(networkConfig);

        // have to enumerate again to pick it up
        networkConfigs = ConfigStorage_FindNetworkConfigurationBlocks();
    }

    // find wireless 80211 network configuration blocks
    HAL_CONFIGURATION_NETWORK_WIRELESS80211 *networkWirelessConfigs =
        (HAL_CONFIGURATION_NETWORK_WIRELESS80211 *)ConfigStorage_FindNetworkWireless80211ConfigurationBlocks();

    // check wireless configs count
    if (networkWirelessConfigs->Count == 0)
    {
        // allocate memory for ONE network configuration
        HAL_Configuration_Wireless80211 *wirelessConfig =
            (HAL_Configuration_Wireless80211 *)platform_malloc(sizeof(HAL_Configuration_Wireless80211));

        InitialiseWirelessDefaultConfig(wirelessConfig, 0);

        StoreConfigBlock(
            DeviceConfigurationOption_Wireless80211Network,
            0,
            wirelessConfig,
            sizeof(HAL_Configuration_Wireless80211));

        // free memory
        platform_free(wirelessConfig);

        // have to enumerate again to pick it up
        networkWirelessConfigs = ConfigStorage_FindNetworkWireless80211ConfigurationBlocks();
    }

    // find wireless AP configuration blocks
    HAL_CONFIGURATION_NETWORK_WIRELESSAP *wirelessAPConfigs = ConfigStorage_FindNetworkWirelessAPConfigurationBlocks();

    // check wireless AP configs count
    if (wirelessAPConfigs->Count == 0)
    {
        // allocate memory for ONE wireless AP configuration
        HAL_Configuration_WirelessAP *wirelessAPConfig =
            (HAL_Configuration_WirelessAP *)platform_malloc(sizeof(HAL_Configuration_WirelessAP));

        InitialiseWirelessAPDefaultConfig(wirelessAPConfig, 0);

        StoreConfigBlock(
            DeviceConfigurationOption_WirelessNetworkAP,
            0,
            wirelessAPConfig,
            sizeof(HAL_Configuration_WirelessAP));

        // free memory
        platform_free(wirelessAPConfig);

        // have to enumerate again to pick it up
        wirelessAPConfigs = ConfigStorage_FindNetworkWirelessAPConfigurationBlocks();
    }

    // find X509 CA certificate blocks
    HAL_CONFIGURATION_X509_CERTIFICATE *certificateStore = ConfigStorage_FindX509CertificateConfigurationBlocks();

    // find X509 device certificate blocks
    HAL_CONFIGURATION_X509_DEVICE_CERTIFICATE *deviceCertificates =
        ConfigStorage_FindX509DeviceCertificatesConfigurationBlocks();

    // allocate memory for g_TargetConfiguration
    // because this is a struct of structs that use flexible members the memory has to be allocated from the heap
    // the malloc size for each struct is computed separately
    uint32_t sizeOfNetworkInterfaceConfigs =
        offsetof(HAL_CONFIGURATION_NETWORK, Configs) + networkConfigs->Count * sizeof(networkConfigs->Configs[0]);

    uint32_t sizeOfWireless80211Configs = offsetof(HAL_CONFIGURATION_NETWORK_WIRELESS80211, Configs) +
                                          networkWirelessConfigs->Count * sizeof(networkWirelessConfigs->Configs[0]);

    uint32_t sizeOfWirelessAPConfigs = offsetof(HAL_CONFIGURATION_NETWORK_WIRELESSAP, Configs) +
                                       wirelessAPConfigs->Count * sizeof(wirelessAPConfigs->Configs[0]);

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

    memcpy(
        (HAL_CONFIGURATION_NETWORK_WIRELESSAP *)g_TargetConfiguration.WirelessAPConfigs,
        wirelessAPConfigs,
        sizeOfWirelessAPConfigs);

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
    platform_free(wirelessAPConfigs);
    platform_free(certificateStore);
    platform_free(deviceCertificates);
}

// Build AP default ssid based on last 3 bytes of MAC addr
// nano_xxxxxx
//
void CreateDefaultApSSID(HAL_Configuration_WirelessAP *pconfig)
{
    uint8_t mac[6];
    esp_efuse_mac_get_default(mac);

    hal_strcpy_s((char *)pconfig->Ssid, sizeof(pconfig->Ssid), "nano_");
    char *pMac = (char *)pconfig->Ssid + 5;
    // Last 3 bytes of MAC address
    for (int index = 3; index < 6; index++)
    {
        sprintf(pMac, "%02X", (int)mac[index]);
        pMac += 2;
    }
}

//  Default initialisation of wireless config blocks for ESP32 targets
void InitialiseWirelessDefaultConfig(HAL_Configuration_Wireless80211 *pconfig, uint32_t configurationIndex)
{
    memset(pconfig, 0, sizeof(HAL_Configuration_Wireless80211));

    // Wireless station
    pconfig->Id = configurationIndex;

    // test default data for AP
    // hal_strcpy_s( (char*)pconfig->Ssid, sizeof(pconfig->Ssid), "MySSID" );
    // hal_strcpy_s( (char*)pconfig->Password, sizeof(pconfig->Password), "MyPassword" );

    // Set default to Auto Connect + Enable + WirelessFlags_SmartConfig so station can be started by default
    // Once smart config has run will start up automatically and reconnect of disconnected
    // Application will have to disable wifi to save power etc
    pconfig->Options =
        (Wireless80211Configuration_ConfigurationOptions)(Wireless80211Configuration_ConfigurationOptions_AutoConnect | Wireless80211Configuration_ConfigurationOptions_Enable | Wireless80211Configuration_ConfigurationOptions_SmartConfig);
}

//  Default initialisation of wireless config blocks for ESP32 targets
void InitialiseWirelessAPDefaultConfig(HAL_Configuration_WirelessAP *pconfig, uint32_t configurationIndex)
{
    memset(pconfig, 0, sizeof(HAL_Configuration_WirelessAP));

    pconfig->Id = configurationIndex;

    // Build AP default ssid/password based on MAC addr
    // but don't enable by default
    CreateDefaultApSSID(pconfig);

    hal_strcpy_s((char *)pconfig->Password, sizeof(pconfig->Password), "nanoframework");

    pconfig->Authentication = AuthenticationType_WPA2;
    pconfig->Encryption = EncryptionType_WPA2;
    pconfig->Radio = RadioType__802_11n;
    pconfig->Channel = 11;
    pconfig->MaxConnections = 4;

    // Disable Soft AP (default)
    pconfig->Options = WirelessAPConfiguration_ConfigurationOptions_Disable;
}

//  Default initialisation of Network interface config blocks for ESP32 targets
bool InitialiseNetworkDefaultConfig(HAL_Configuration_NetworkInterface *pconfig, uint32_t configurationIndex)
{
    memset(pconfig, 0, sizeof(HAL_Configuration_NetworkInterface));

    // make sure the config block marker is set
    memcpy(pconfig->Marker, c_MARKER_CONFIGURATION_NETWORK_V1, sizeof(c_MARKER_CONFIGURATION_NETWORK_V1));

    switch (configurationIndex)
    {
        case 0: // Wireless Station
            pconfig->InterfaceType = NetworkInterfaceType_Wireless80211;
            pconfig->StartupAddressMode = AddressMode_DHCP;
            pconfig->AutomaticDNS = 1;
            pconfig->SpecificConfigId = 0;
            break;

        case 1: // Wireless AP
            pconfig->InterfaceType = NetworkInterfaceType_WirelessAP;
            pconfig->StartupAddressMode = AddressMode_Static;
            pconfig->SpecificConfigId = 0;
            // Set default address 192.168.1.1
            // pconfig->IPv4Address
            // pconfig->IPv4NetMask
            // pconfig->IPv4GatewayAddress
            break;

        case 2: // Ethernet
            pconfig->InterfaceType = NetworkInterfaceType_Ethernet;
            pconfig->StartupAddressMode = AddressMode_DHCP;
            pconfig->AutomaticDNS = 1;
            break;
    }

    // get default MAC
    esp_efuse_mac_get_default(pconfig->MacAddress);

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
    uint8_t *blockAddress = NULL;

#ifdef DEBUG_CONFIG
    ets_printf("GetConfig %d, %d\n", (int)configuration, configurationIndex);
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
            return FALSE;
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
#ifdef DEBUG_CONFIG
            ets_printf("GetConfig WN exit false\n");
#endif
            return FALSE;
        }

        // set block size
        sizeOfBlock = sizeof(HAL_Configuration_Wireless80211);

        // get block address
        blockAddress = (uint8_t *)g_TargetConfiguration.Wireless80211Configs->Configs[configurationIndex];
    }
    else if (configuration == DeviceConfigurationOption_WirelessNetworkAP)
    {
        if (g_TargetConfiguration.WirelessAPConfigs->Count == 0 ||
            (configurationIndex + 1) > g_TargetConfiguration.WirelessAPConfigs->Count)
        {
#ifdef DEBUG_CONFIG
            ets_printf("GetConfig AP exit false\n");
#endif
            return FALSE;
        }

        // set block size
        sizeOfBlock = sizeof(HAL_Configuration_WirelessAP);

        // get block address
        blockAddress = (uint8_t *)g_TargetConfiguration.WirelessAPConfigs->Configs[configurationIndex];
    }
    else if (configuration == DeviceConfigurationOption_X509CaRootBundle)
    {
        if (g_TargetConfiguration.CertificateStore->Count == 0 ||
            (configurationIndex + 1) > g_TargetConfiguration.CertificateStore->Count)
        {
#ifdef DEBUG_CONFIG
            ets_printf("GetConfig XC exit false\n");
#endif
            return FALSE;
        }

        // get block address
        blockAddress = (uint8_t *)g_TargetConfiguration.CertificateStore->Certificates[configurationIndex];

        // set block size
        // because X509 certificate has a variable length need to compute the block size in two steps
        sizeOfBlock = offsetof(HAL_Configuration_X509CaRootBundle, Certificate);
        sizeOfBlock += ((HAL_Configuration_X509CaRootBundle *)blockAddress)->CertificateSize;
    }

    // copy the config block content to the pointer in the argument
    memcpy(configurationBlock, blockAddress, sizeOfBlock);

    return TRUE;
}

DeviceConfigurationOption GetConfigOption(char *pConfig)
{
    if (!memcmp(c_MARKER_CONFIGURATION_NETWORK_V1, pConfig, sizeof(c_MARKER_CONFIGURATION_NETWORK_V1)))
    {
        return DeviceConfigurationOption_Network;
    }
    else if (!memcmp(c_MARKER_CONFIGURATION_WIRELESS80211_V1, pConfig, sizeof(c_MARKER_CONFIGURATION_WIRELESS80211_V1)))
    {
        return DeviceConfigurationOption_Wireless80211Network;
    }
    else if (!memcmp(c_MARKER_CONFIGURATION_WIRELESS_AP_V1, pConfig, sizeof(c_MARKER_CONFIGURATION_WIRELESS_AP_V1)))
    {
        return DeviceConfigurationOption_WirelessNetworkAP;
    }
    else if (!memcmp(
                 c_MARKER_CONFIGURATION_X509CAROOTBUNDLE_V1,
                 pConfig,
                 sizeof(c_MARKER_CONFIGURATION_X509CAROOTBUNDLE_V1)))
    {
        return DeviceConfigurationOption_X509CaRootBundle;
    }
    else if (!memcmp(
                 c_MARKER_CONFIGURATION_X509DEVICECERTIFICATE_V1,
                 pConfig,
                 sizeof(c_MARKER_CONFIGURATION_X509DEVICECERTIFICATE_V1)))
    {
        return DeviceConfigurationOption_X509DeviceCertificates;
    }
    else
    {
        // shouldn't EVER EVER get here
        ASSERT(false);
        return (DeviceConfigurationOption)0;
    }
}

bool ConfigurationManager_StoreConfigurationBlockAll(void *configurationBlock, uint32_t blockSize, uint32_t offset)
{
    size_t chunkSize = 0;
    uint32_t certificateIndex = 0;
    bool result = false;
    uint32_t configurationIndex = 0;

    // All configuration blocks in one block
    // Separate and update

#ifdef DEBUG_CONFIG
    ets_printf("Block size %d\n", blockSize);

    ets_printf("sizeof HAL_Configuration_NetworkInterface %d\n", sizeof(HAL_Configuration_NetworkInterface));
    ets_printf("sizeof HAL_Configuration_Wireless80211 %d\n", sizeof(HAL_Configuration_Wireless80211));
    ets_printf("sizeof of X509Certificate varies\n");
#endif

    configurationIndex = 0;

    char *pConfig = (char *)configurationBlock;
    char *pEndConfig = (pConfig + blockSize);
    DeviceConfigurationOption currentConfigType;

    while (pConfig < pEndConfig)
    {
        int remainingBlockSize = pEndConfig - pConfig;

#ifdef DEBUG_CONFIG
        ets_printf("Parse block %d:%d\n", pConfig, pEndConfig);
        PrintBlock((char *)pConfig, 16);
#endif
        currentConfigType = GetConfigOption(pConfig);

        // X509 certificate block ?
        if (currentConfigType == DeviceConfigurationOption_X509CaRootBundle || offset > 0)
        {
            if (offset == 0)
            {
                HAL_Configuration_X509CaRootBundle *pX509Certificate = (HAL_Configuration_X509CaRootBundle *)pConfig;

                // First block of certificate bundle
                // set total bundle size including header
                // because X509 certificate has a variable length need to compute the block size in two steps
                savedBundleSize = offsetof(HAL_Configuration_X509CaRootBundle, Certificate);
                savedBundleSize += pX509Certificate->CertificateSize;
                remainingBundleSize = savedBundleSize;
                savedBundleOffset = 0;

                // Free if already allocated ( could be different size )
                if (g_TargetConfiguration.CertificateStore->Certificates[certificateIndex] != 0)
                    platform_free((void *)g_TargetConfiguration.CertificateStore->Certificates);

                g_TargetConfiguration.CertificateStore->Certificates[certificateIndex] =
                    (HAL_Configuration_X509CaRootBundle *)platform_malloc(savedBundleSize);

#ifdef DEBUG_CONFIG
                ets_printf("X509 certificate block total size:%d\n", pX509Certificate->CertificateSize);
#endif
            }

            // The current chunk size is what remains in current block
            // we can't use offset as it relates to flash memory offset, not useful
            chunkSize = remainingBlockSize;

            // Copy into correct position in memory
            memcpy(
                (void *)(g_TargetConfiguration.CertificateStore->Certificates[0] + savedBundleOffset),
                (const void *)pConfig,
                chunkSize);
#ifdef DEBUG_CONFIG
            ets_printf("X509 certificate chunksize:%d reminaing:%d\n", chunkSize, savedBundleSize);
            PrintBlock((char *)pConfig, 32);
#endif
            pConfig += chunkSize;
            remainingBundleSize -= chunkSize;
            savedBundleOffset += chunkSize;
            // Return true for partial blocks
            result = true;

            if (remainingBundleSize <= 0)
            {
                // Save Certificate Bundle to storage
                result = StoreConfigBlock(
                    DeviceConfigurationOption_X509CaRootBundle,
                    certificateIndex,
                    (void *)g_TargetConfiguration.CertificateStore->Certificates[certificateIndex],
                    savedBundleSize);
                certificateIndex++;
#ifdef DEBUG_CONFIG
                ets_printf("X509 certificate complete, saving\n");
#endif
            }

#ifdef DEBUG_CONFIG
            ets_printf("X509 certificate block ret:%d\n", result);
#endif
        }
        // Network interface block ?
        else if (currentConfigType == DeviceConfigurationOption_Network)
        {
            HAL_Configuration_NetworkInterface *pNetConfig = (HAL_Configuration_NetworkInterface *)pConfig;
            pConfig += sizeof(HAL_Configuration_NetworkInterface);

            result = StoreConfigBlock(
                currentConfigType,
                configurationIndex,
                (void *)pNetConfig,
                sizeof(HAL_Configuration_NetworkInterface));
            configurationIndex++;

#ifdef DEBUG_CONFIG
            ets_printf("Network block %X %X ret:%d\n", pNetConfig->InterfaceType, pNetConfig->SpecificConfigId, result);
            PrintBlock((char *)pNetConfig, sizeof(HAL_Configuration_NetworkInterface));
#endif
        }
        // Wireless block ?
        else if (currentConfigType == DeviceConfigurationOption_Wireless80211Network)
        {
            HAL_Configuration_Wireless80211 *pWirelessConfig = (HAL_Configuration_Wireless80211 *)pConfig;
            pConfig += sizeof(HAL_Configuration_Wireless80211);

            result = StoreConfigBlock(
                currentConfigType,
                pWirelessConfig->Id,
                (void *)pWirelessConfig,
                sizeof(HAL_Configuration_Wireless80211));

#ifdef DEBUG_CONFIG
            ets_printf(
                "Wireless block %d ssid:%s password:%s ret:%d\n",
                pWirelessConfig->Id,
                pWirelessConfig->Ssid,
                pWirelessConfig->Password,
                result);
            PrintBlock((char *)pWirelessConfig, sizeof(HAL_Configuration_Wireless80211));
#endif
        }
        else
            break;
    } // while

#ifdef DEBUG_CONFIG
    ets_printf("StoreConfig ALL exit %d\n", (int)result);
#endif
    return result;
}

//
// Stores the configuration block to the storage
//
// Parameters:-
//   configurationBlock : Pointer to block to store
//   configuration      : Type of configuration block to store or All for block with multiple types
//   configurationIndex : Index of type to store ( not used for All )
//   blockSize          : Size of data pointed to by configurationBlock
//   offset             : Offset of data when multiple blocks ( currently Certificate Bundles only ), For single type
//   this is offset withing the type
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
        result = ConfigurationManager_StoreConfigurationBlockAll(configurationBlock, blockSize, offset);

        // enumeration is required after we are DONE with SUCCESSFULLY storing all the config chunks
        requiresEnumeration = (result && done);
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
            // set blockSize size ( Total size of  X509 certificate )
            // because X509 certificate has a variable length need to compute the block size in two steps
            blockSize = offsetof(HAL_Configuration_X509CaRootBundle, Certificate);
            blockSize += ((HAL_Configuration_X509CaRootBundle *)configurationBlock)->CertificateSize;

#ifdef DEBUG_CONFIG
            ets_printf(
                "StoreConfig x509 blockSize:%d, certsize:%d",
                blockSize,
                ((HAL_Configuration_X509CaRootBundle *)configurationBlock)->CertificateSize);
#endif
        }
        else
        {
            // Invalid Config
            return false;
        }
    }

    // Anything to save
    if (blockSize != 0)
    {
        result = StoreConfigBlock(configuration, configurationIndex, configurationBlock, blockSize);
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
bool ConfigurationManager_UpdateConfigurationBlock(
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

        default:
            // shouldn't ever reach here
            return FALSE;
    }

    return ConfigurationManager_StoreConfigurationBlock(
        configurationBlock,
        configuration,
        configurationIndex,
        blockSize,
        0,
        false);
}
