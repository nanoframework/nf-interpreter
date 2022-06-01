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

bool ethernetEnabled = false;

// NVS parameters for Interface config
#define NVS_NAMESPACE "nanoF"

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
    void *configBlock,
    size_t writeSize)
{
    bool result = false;
    FILE *fileHandle;

    fileHandle = ConfigStorage_OpenFile(configType, configurationIndex, true, false);
    if (fileHandle != NULL)
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

    if (fileHandle != NULL)
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
// it's implemented with 'weak' attribute so it can be replaced at target level if a different persistence mechanism is
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
        ethernetEnabled = true;
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
    HAL_CONFIGURATION_NETWORK_WIRELESSAP *wirelessAPconfigs = ConfigStorage_FindNetworkWirelessAPConfigurationBlocks();

    // check wireless AP configs count
    if (wirelessAPconfigs->Count == 0)
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
        wirelessAPconfigs = ConfigStorage_FindNetworkWirelessAPConfigurationBlocks();
    }

    // find X509 CA certificate blocks
    HAL_CONFIGURATION_X509_CERTIFICATE *certificateStore = ConfigStorage_FindX509CertificateConfigurationBlocks();

    // find X509 device certificate blocks
    HAL_CONFIGURATION_X509_DEVICE_CERTIFICATE *deviceCertificates =
        ConfigStorage_FindX509DeviceCertificatesConfigurationBlocks();

    g_TargetConfiguration.NetworkInterfaceConfigs = networkConfigs;
    g_TargetConfiguration.Wireless80211Configs = networkWirelessConfigs;
    g_TargetConfiguration.WirelessAPConfigs = wirelessAPconfigs;
    g_TargetConfiguration.CertificateStore = certificateStore;
    g_TargetConfiguration.DeviceCertificates = deviceCertificates;
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
    if (ethernetEnabled)
    {
        config->Options = Wireless80211Configuration_ConfigurationOptions_Disable;
    }
    else
    {
        config->Options =
            (Wireless80211Configuration_ConfigurationOptions)(Wireless80211Configuration_ConfigurationOptions_AutoConnect | Wireless80211Configuration_ConfigurationOptions_Enable | Wireless80211Configuration_ConfigurationOptions_SmartConfig);
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
    memset(config, 0, sizeof(HAL_Configuration_NetworkInterface));

    // make sure the config block marker is set
    memcpy(config->Marker, c_MARKER_CONFIGURATION_NETWORK_V1, sizeof(c_MARKER_CONFIGURATION_NETWORK_V1));

    switch (configurationIndex)
    {
        case 0: // Wireless Station
            config->InterfaceType = NetworkInterfaceType_Wireless80211;
            config->StartupAddressMode = AddressMode_DHCP;
            config->AutomaticDNS = 1;
            config->SpecificConfigId = 0;
            break;

        case 1: // Wireless AP
            config->InterfaceType = NetworkInterfaceType_WirelessAP;
            config->StartupAddressMode = AddressMode_Static;
            config->SpecificConfigId = 0;
            // Set default address 192.168.1.1
            // config->IPv4Address
            // config->IPv4NetMask
            // config->IPv4GatewayAddress
            break;

        case 2: // Ethernet
            config->InterfaceType = NetworkInterfaceType_Ethernet;
            config->StartupAddressMode = AddressMode_DHCP;
            config->AutomaticDNS = 1;
            break;
    }

    // get default MAC
    esp_efuse_mac_get_default(config->MacAddress);

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
            if (configuration == DeviceConfigurationOption_X509CaRootBundle)
            {
                // need to free memory before enumerating again
                if (g_TargetConfiguration.CertificateStore->Count > 0)
                {
                    platform_free(g_TargetConfiguration.CertificateStore->Certificates[0]);
                }
                platform_free(g_TargetConfiguration.CertificateStore);

                g_TargetConfiguration.CertificateStore = ConfigStorage_FindX509CertificateConfigurationBlocks();
            }
            else if (configuration == DeviceConfigurationOption_X509DeviceCertificates)
            {
                // need to free memory before enumerating again
                if (g_TargetConfiguration.DeviceCertificates->Count > 0)
                {
                    platform_free(g_TargetConfiguration.DeviceCertificates->Certificates[0]);
                }
                platform_free(g_TargetConfiguration.DeviceCertificates);

                g_TargetConfiguration.DeviceCertificates =
                    ConfigStorage_FindX509DeviceCertificatesConfigurationBlocks();
            }
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

                // check if operation was successful
                if (result)
                {
                    // reset this
                    blockSize = 0;

                    // enumeration is required
                    requiresEnumeration = true;
                }
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

                // check if operation was successful
                if (result)
                {
                    // reset this
                    blockSize = 0;

                    // enumeration is required
                    requiresEnumeration = true;
                }
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

        case DeviceConfigurationOption_X509DeviceCertificates:
            blockSize = offsetof(HAL_Configuration_X509DeviceCertificate, Certificate);
            blockSize += ((HAL_Configuration_X509DeviceCertificate *)configurationBlock)->CertificateSize;
            break;

        default:
            // shouldn't ever reach here
            return false;
    }

    return ConfigurationManager_StoreConfigurationBlock(
        configurationBlock,
        configuration,
        configurationIndex,
        blockSize,
        0,
        false);
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

    if (wirelessConfig != NULL)
    {
        platform_free(wirelessConfig);
    }

    // not found, or failed to allocate memory
    return NULL;
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

    if (wirelessAPConfig != NULL)
    {
        platform_free(wirelessAPConfig);
    }

    // not found, or failed to allocate memory
    return NULL;
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

            if (certStore != NULL)
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
    return NULL;
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

            if (deviceCert != NULL)
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
    return NULL;
}
