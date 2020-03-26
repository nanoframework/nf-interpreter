//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoHAL_v2.h>
#include <nanoWeak.h>

#include <targetHAL_ConfigurationManager_CC32xx.h>
#include <ti/drivers/net/wifi/simplelink.h>

typedef struct
{
    SlFileAttributes_t attribute;
    char fileName[SL_FS_MAX_FILE_NAME_LENGTH];
}slGetfileList_t;

// This configuration manager implementation is valid for CC32xx devices.
// Because everything that is meant to be stored in the configure block is handled by the SimpleLink 
// persistent storage, this code is either empty or acts as a proxy to the SimpleLink API

// network configuration blocks are stored as files in file storage
void* ConfigurationManagerCC32xx_FindNetworkConfigurationBlocks()
{
    int32_t ret = 1;
    int32_t retGetFileList = 1;
    slGetfileList_t* fileList;
    int32_t index = -1;

    int32_t i;
    uint32_t fileCount = 0;

    // allocate memory for file list buffer
    fileList = (slGetfileList_t*)platform_malloc(sizeof(slGetfileList_t) * NETWORK_CONFIG_MAX_COUNT);

    // check succesfull malloc
    if (fileList == NULL)
    {
        return NULL;
    }

    // clear memory
    memset(fileList, 0x0, sizeof(sizeof(SlFileAttributes_t) * NETWORK_CONFIG_MAX_COUNT));

    // first pass: find out how many files of this type we have
    while(retGetFileList > 0)
    {
        retGetFileList = sl_FsGetFileList( &index, NETWORK_CONFIG_MAX_COUNT, 
                                                (uint8_t)(SL_FS_MAX_FILE_NAME_LENGTH + sizeof(SlFileAttributes_t)),
                                                (unsigned char*)fileList, SL_FS_GET_FILE_ATTRIBUTES);
        if (retGetFileList < 0)
        {
            // error getting file list, or no more files
            break;
        }
        
        for (i = 0; i < retGetFileList; i++)
        {
            // check file name
            if(memcmp(fileList[i].fileName, NETWORK_CONFIG_FILE_NAME, sizeof(NETWORK_CONFIG_FILE_NAME)) == 0)
            {
                fileCount++;
            }
        }
    }

    // allocate config struct
    HAL_CONFIGURATION_NETWORK *networkConfigs = (HAL_CONFIGURATION_NETWORK *)platform_malloc(offsetof(HAL_CONFIGURATION_NETWORK, Configs) + fileCount * sizeof(networkConfigs->Configs[0]));
    // set collection count
    networkConfigs->Count = fileCount;

    // clear memory for file list
    platform_free(fileList);

    return networkConfigs;
}

// wireless profiles are stored as SimpleLink WLAN profile 
void* ConfigurationManagerCC32xx_FindNetworkWireless80211ConfigurationBlocks()
{
    int16_t index, status;
    signed char name[32];
    int16_t nameLength;
    unsigned char macAddr[6];
    SlWlanSecParams_t secParams;
    SlWlanGetSecParamsExt_t secExtParams;
    uint32_t priority;
    uint16_t profileCount = 0;

    // first pass: find out how many profiles are stored
    for(index = 0; index < SL_WLAN_MAX_PROFILES; index++)
    {
        status = sl_WlanProfileGet(index, name, &nameLength, macAddr, &secParams, &secExtParams, &priority);
        if( status >= 0)
        {
            profileCount++;
        }
    }

    // allocate config struct
    HAL_CONFIGURATION_NETWORK_WIRELESS80211 *networkWirelessConfigs = (HAL_CONFIGURATION_NETWORK_WIRELESS80211 *)platform_malloc(offsetof(HAL_CONFIGURATION_NETWORK_WIRELESS80211, Configs) + profileCount * sizeof(networkWirelessConfigs->Configs[0]));

    // set collection count
    networkWirelessConfigs->Count = profileCount;

    return networkWirelessConfigs;
}

// initialization of configuration manager
void ConfigurationManager_Initialize()
{
    memset( (void*)&g_TargetConfiguration, 0, sizeof(g_TargetConfiguration));

    // enumerate the blocks
    ConfigurationManager_EnumerateConfigurationBlocks();
};

// Enumerates the configuration blocks from the configuration flash sector 
void ConfigurationManager_EnumerateConfigurationBlocks()
{
    // find network configuration blocks
    HAL_CONFIGURATION_NETWORK* networkConfigs = (HAL_CONFIGURATION_NETWORK*)ConfigurationManagerCC32xx_FindNetworkConfigurationBlocks();

    // check network configs count
    if(networkConfigs->Count == 0)
    {
        // there is no network config block available, get a default
        HAL_Configuration_NetworkInterface* networkConfig = (HAL_Configuration_NetworkInterface*)platform_malloc(sizeof(HAL_Configuration_NetworkInterface));
        if(InitialiseNetworkDefaultConfig(networkConfig, 0))
        {
            // config block created, store it
            ConfigurationManager_StoreConfigurationBlock(networkConfig, DeviceConfigurationOption_Network, 0, sizeof(HAL_Configuration_NetworkInterface), 0, false);
            
            // have to enumerate again to pick it up
            networkConfigs = (HAL_CONFIGURATION_NETWORK*)ConfigurationManagerCC32xx_FindNetworkConfigurationBlocks();
        }

        platform_free(networkConfig);
    }

    // find wireless 80211 network configuration blocks
    HAL_CONFIGURATION_NETWORK_WIRELESS80211* networkWirelessConfigs = (HAL_CONFIGURATION_NETWORK_WIRELESS80211*)ConfigurationManagerCC32xx_FindNetworkWireless80211ConfigurationBlocks();

    // // find X509 certificate blocks
    // HAL_CONFIGURATION_X509_CERTIFICATE* certificateStore = (HAL_CONFIGURATION_X509_CERTIFICATE*)ConfigurationManager_FindX509CertificateConfigurationBlocks((uint32_t)&__nanoConfig_start__, (uint32_t)&__nanoConfig_end__);

    // alloc memory for g_TargetConfiguration
    // because this is a struct of structs that use flexible members the memory has to be allocated from the heap
    // the malloc size for each struct is computed separately 
    uint32_t sizeOfNetworkInterfaceConfigs = offsetof(HAL_CONFIGURATION_NETWORK, Configs) + networkConfigs->Count * sizeof(networkConfigs->Configs[0]);
    uint32_t sizeOfWireless80211Configs = offsetof(HAL_CONFIGURATION_NETWORK_WIRELESS80211, Configs) + networkWirelessConfigs->Count * sizeof(networkWirelessConfigs->Configs[0]);
    // uint32_t sizeOfX509CertificateStore = offsetof(HAL_CONFIGURATION_X509_CERTIFICATE, Certificates) + certificateStore->Count * sizeof(certificateStore->Certificates[0]);

    g_TargetConfiguration.NetworkInterfaceConfigs = (HAL_CONFIGURATION_NETWORK*)platform_malloc(sizeOfNetworkInterfaceConfigs);
    g_TargetConfiguration.Wireless80211Configs = (HAL_CONFIGURATION_NETWORK_WIRELESS80211*)platform_malloc(sizeOfWireless80211Configs);
    // g_TargetConfiguration.CertificateStore = (HAL_CONFIGURATION_X509_CERTIFICATE*)platform_malloc(sizeOfX509CertificateStore);

    // copy structs to g_TargetConfiguration
    memcpy((HAL_CONFIGURATION_NETWORK*)g_TargetConfiguration.NetworkInterfaceConfigs, networkConfigs, sizeOfNetworkInterfaceConfigs);
    memcpy((HAL_CONFIGURATION_NETWORK_WIRELESS80211*)g_TargetConfiguration.Wireless80211Configs, networkWirelessConfigs, sizeOfWireless80211Configs);
    // memcpy((HAL_CONFIGURATION_X509_CERTIFICATE*)g_TargetConfiguration.CertificateStore, certificateStore, sizeOfX509CertificateStore);

    // now free the memory of the original structs
    platform_free(networkConfigs);
    platform_free(networkWirelessConfigs);
    // platform_free(certificateStore);
}

EncryptionType GetEncryption(SlWlanSecParams_t secParams)
{
    switch (secParams.Type)
    {
        case SL_WLAN_SEC_TYPE_WEP:
            return EncryptionType_WEP;
    
        // deprecated
        // case SL_WLAN_SEC_TYPE_WPA:
        //     return EncryptionType_WPA;
    
        case SL_WLAN_SEC_TYPE_WPA_WPA2:
            return EncryptionType_WPA2;
    
        default:
            return EncryptionType_None;
    }
}

AuthenticationType GetAuthentication(SlWlanSecParams_t secParams)
{
    switch (secParams.Type)
    {
        case SL_WLAN_SEC_TYPE_OPEN:
            return AuthenticationType_Open;
    
        case SL_WLAN_SEC_TYPE_WEP:
            return AuthenticationType_WEP;
    
        // deprecated
        // case SL_WLAN_SEC_TYPE_WPA:
        //     return AuthenticationType_WPA;
    
        case SL_WLAN_SEC_TYPE_WPA_WPA2:
            return AuthenticationType_WPA2;
    
        case SL_WLAN_SEC_TYPE_WEP_SHARED:
            return AuthenticationType_Shared;
    
        default:
            return AuthenticationType_None;
    }
}

// Gets the network configuration block from the configuration flash sector 
bool ConfigurationManager_GetConfigurationBlock(void* configurationBlock, DeviceConfigurationOption configuration, uint32_t configurationIndex)
{
    unsigned char* fileName = NULL;

    int32_t fileHandle;
    uint32_t token = 0;
    int32_t retVal;
    SlWlanSecParams_t secParams;
    int16_t dummyNameLen;
    uint8_t dummyMAC[SL_MAC_ADDR_LEN];
    uint32_t dummyPriority;

    HAL_Configuration_Wireless80211* wirelessConfigBlock = NULL;


    // validate if the requested block exists
    // Count has to be non zero
    // requested Index has to exist (array index starts at zero, so need to add one)
    if(configuration == DeviceConfigurationOption_Network)
    {
        if( g_TargetConfiguration.NetworkInterfaceConfigs->Count == 0 ||
            (configurationIndex + 1) > g_TargetConfiguration.NetworkInterfaceConfigs->Count)
        {
            // the requested config block is beyond the available count
            return false;
        }
    }
    else if(configuration == DeviceConfigurationOption_Wireless80211Network)
    {
        if(g_TargetConfiguration.Wireless80211Configs->Count == 0 ||
            (configurationIndex + 1) > g_TargetConfiguration.Wireless80211Configs->Count)
        {
            return false;
        }
    }
    else if(configuration == DeviceConfigurationOption_X509CaRootBundle)
    {
        if(g_TargetConfiguration.CertificateStore->Count == 0 ||
            (configurationIndex + 1) > g_TargetConfiguration.CertificateStore->Count)
        {
            return false;
        }

        // // get block address
        // blockAddress = (uint8_t*)g_TargetConfiguration.CertificateStore->Certificates[configurationIndex];

        // // set block size
        // // because X509 certificate has a variable length need to compute the block size in two steps
        // sizeOfBlock = offsetof(HAL_Configuration_X509CaRootBundle, Certificate);
        // sizeOfBlock += ((HAL_Configuration_X509CaRootBundle*)blockAddress)->CertificateSize;
    }

    if(configuration == DeviceConfigurationOption_Network)
    {
        // network config blocks are stored as files

        // compose file name
        fileName = (unsigned char*)platform_malloc(sizeof(NETWORK_CONFIG_FILE_NAME));
        memcpy(fileName, NETWORK_CONFIG_FILE_NAME, sizeof(NETWORK_CONFIG_FILE_NAME));
        // insert index number at position N as char
        fileName[NETWORK_CONFIG_FILE_INDEX_POSITION] = '0' + configurationIndex;

        fileHandle = sl_FsOpen( fileName,
                                SL_FS_READ,
                                (uint32_t *)&token);
        
        // on error there is no file handle, rather a negative error code
        if(fileHandle > 0)
        {
            retVal = sl_FsRead(fileHandle, 0, (unsigned char*)configurationBlock, sizeof(HAL_Configuration_NetworkInterface));
            
            // on success the return value is the amount of bytes written
            if(retVal == sizeof(HAL_Configuration_NetworkInterface))
            {
                retVal = sl_FsClose(fileHandle, 0, 0, 0);

                if( retVal < 0 )
                {
                    // error closing file, API ceremony suggests calling "abort" operation
                    uint8_t signature = 'A';
                    sl_FsClose(fileHandle, 0, &signature, 1);
                }
            }
        }

        if(fileName != NULL)
        {
            platform_free(fileName);
        }

        // done!
        return true;
    }
    else if(configuration == DeviceConfigurationOption_Wireless80211Network)
    {
        wirelessConfigBlock = (HAL_Configuration_Wireless80211*)configurationBlock;

        // make sure the config block marker is set
        memcpy(configurationBlock, c_MARKER_CONFIGURATION_WIRELESS80211_V1, sizeof(c_MARKER_CONFIGURATION_WIRELESS80211_V1));

        // get profile from SimpleLink
        retVal = sl_WlanProfileGet(configurationIndex, (signed char *)wirelessConfigBlock->Ssid, &dummyNameLen, &dummyMAC[0], &secParams, NULL, &dummyPriority);
        if(retVal == SL_ERROR_WLAN_GET_PROFILE_INVALID_INDEX)
        {
            return false;
        }

        // fill struct fields
        wirelessConfigBlock->Id = configurationIndex;
        wirelessConfigBlock->Authentication = GetAuthentication(secParams);
        wirelessConfigBlock->Encryption = GetEncryption(secParams);

        // password is hidden, NULL the string
        memset(wirelessConfigBlock->Password, 0, sizeof(wirelessConfigBlock->Password));
        //wirelessConfigBlock->Radio 
        

        // done
        return true;
    }

    return false;
}

uint8_t GetSecurityType(AuthenticationType authentication)
{
    switch (authentication)
    {
        case AuthenticationType_Open:
            return SL_WLAN_SEC_TYPE_OPEN;
    
        case AuthenticationType_WEP:
            return SL_WLAN_SEC_TYPE_WEP;
    
        case AuthenticationType_WPA:
        case AuthenticationType_WPA2:
            return SL_WLAN_SEC_TYPE_WPA_WPA2;
    
        case AuthenticationType_Shared:
            return SL_WLAN_SEC_TYPE_WEP_SHARED;
    
        default:
            return SL_WLAN_SEC_TYPE_OPEN;
    }
}

// Stores the configuration block to the file system 
bool ConfigurationManager_StoreConfigurationBlock(void* configurationBlock, DeviceConfigurationOption configuration, uint32_t configurationIndex, uint32_t blockSize, uint32_t offset, bool done)
{
    // this platform doesn't need to handle this
    (void)done;

    bool requiresEnumeration = false;
    bool success = false;

    unsigned char* fileName = NULL;

    int32_t fileHandle;
    uint32_t token = 0;
    int32_t retVal;
    SlWlanSecParams_t secParams;

    if(configuration == DeviceConfigurationOption_Network)
    {
        // network config blocks are stored as files

        // compose file name
        fileName = (unsigned char*)platform_malloc(sizeof(NETWORK_CONFIG_FILE_NAME));
        memcpy(fileName, NETWORK_CONFIG_FILE_NAME, sizeof(NETWORK_CONFIG_FILE_NAME));
        // insert index number at position N as char
        fileName[NETWORK_CONFIG_FILE_INDEX_POSITION] = '0' + configurationIndex;

        fileHandle = sl_FsOpen( fileName,
                                SL_FS_CREATE | SL_FS_OVERWRITE |
                                SL_FS_CREATE_MAX_SIZE(sizeof(HAL_Configuration_NetworkInterface)) |
                                SL_FS_CREATE_PUBLIC_WRITE | SL_FS_CREATE_NOSIGNATURE,
                                (uint32_t *)&token);
        
        // on error there is no file handle, rather a negative error code
        if(fileHandle > 0)
        {
            // make sure the config block marker is set
            memcpy(configurationBlock, c_MARKER_CONFIGURATION_NETWORK_V1, sizeof(c_MARKER_CONFIGURATION_NETWORK_V1)); 

            retVal = sl_FsWrite(fileHandle, 0, (unsigned char*)configurationBlock, sizeof(HAL_Configuration_NetworkInterface));
            
            // on success the return value is the amount of bytes written
            if(retVal == sizeof(HAL_Configuration_NetworkInterface))
            {
                retVal = sl_FsClose(fileHandle, 0, 0, 0);

                if( retVal < 0 )
                {
                    // error closing file, API ceremony suggests calling "abort" operation
                    uint8_t signature = 'A';
                    sl_FsClose(fileHandle, 0, &signature, 1);
                }
                else
                {
                    // configuration stored
                    success = true;
                    requiresEnumeration = true;
                }
            }
        }

        if(fileName != NULL)
        {
            platform_free(fileName);
        }
    }
    else if(configuration == DeviceConfigurationOption_Wireless80211Network)
    {
        HAL_Configuration_Wireless80211* wirelessConfigBlock = (HAL_Configuration_Wireless80211*)configurationBlock;

        secParams.Type = GetSecurityType(wirelessConfigBlock->Authentication);
        secParams.Key = (signed char *)wirelessConfigBlock->Password;
        secParams.KeyLen = hal_strlen_s((const char *)secParams.Key);

        // add profile to SimpleLink
        // TODO - default priority is 0
        retVal = sl_WlanProfileAdd((const signed char *)wirelessConfigBlock->Ssid, hal_strlen_s((const char *)(wirelessConfigBlock->Ssid)), NULL, &secParams, NULL, 0, 0);
        if(retVal < 0)
        {
            return false;
        }

        // done
        success = true;
        requiresEnumeration = true;
    }
    else if(configuration == DeviceConfigurationOption_X509CaRootBundle)
    {
        // CA root certificate bundle is stored as a file /sys/certstore.lst
        // currently we don't support updating this
        success = false;
    }

    if(success == true && requiresEnumeration)
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

// Updates a configuration block
bool ConfigurationManager_UpdateConfigurationBlock(void* configurationBlock, DeviceConfigurationOption configuration, uint32_t configurationIndex)
{
    // CC32xx stores the config blocks on the file system so we don't care about sizes

    switch(configuration)
    {
        case DeviceConfigurationOption_Network:
        case DeviceConfigurationOption_Wireless80211Network:
            return ConfigurationManager_StoreConfigurationBlock(configurationBlock, configuration, configurationIndex, 0, 0, false);    

        // this configuration option is not supported
        case DeviceConfigurationOption_X509CaRootBundle:
        default:
            // shouldn't ever reach here
            return FALSE;
    }
}

//  Default initialisation for wireless config block
void InitialiseWirelessDefaultConfig(HAL_Configuration_Wireless80211 * pconfig, uint32_t configurationIndex)
{
    (void)pconfig;
    (void)configurationIndex;

    // TODO    
}

//  Default initialisation for Network interface config blocks
bool InitialiseNetworkDefaultConfig(HAL_Configuration_NetworkInterface * pconfig, uint32_t configurationIndex)
{
    (void)configurationIndex;

    uint16_t macAddressLen = SL_MAC_ADDR_LEN;

    memset(pconfig, 0, sizeof(HAL_Configuration_NetworkInterface));

    // make sure the config block marker is set
    memcpy(pconfig->Marker, c_MARKER_CONFIGURATION_NETWORK_V1, sizeof(c_MARKER_CONFIGURATION_NETWORK_V1));
    
    pconfig->InterfaceType = NetworkInterfaceType_Wireless80211;
    pconfig->StartupAddressMode = AddressMode_DHCP;
    pconfig->AutomaticDNS = 1;
    pconfig->SpecificConfigId = 0;

    sl_NetCfgGet(SL_NETCFG_MAC_ADDRESS_GET, 0, &macAddressLen, pconfig->MacAddress);

    return true;
}
