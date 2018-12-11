//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoHAL_v2.h>
#include <nanoWeak.h>
#include "Esp32_os.h"

// NVS parameters for Interface config
#define NVS_NAMESPACE	    "nanoF"

//#define DEBUG_CONFIG        1

#ifdef DEBUG_CONFIG
void PrintBlock( char * pBlock, int bsize )
{
    char * pCurrStart;
    char * pCurrPos;
    char * pCurrEnd;
    char * pCurrEndBlock = pBlock + bsize;
    
    pCurrStart = pBlock;
    
    while( pCurrStart <= pCurrEndBlock)
    {
        pCurrPos = pCurrStart;
        pCurrEnd = pCurrStart + 32;

        while(pCurrPos < pCurrEnd && pCurrPos <= pCurrEndBlock)
        {
            ets_printf( "%02X ", *pCurrPos );
            pCurrPos++;
        }

        ets_printf( " " );

        pCurrPos = pCurrStart;
        while(pCurrPos < pCurrEnd && pCurrPos <= pCurrEndBlock)
        {
            if ( *pCurrPos < 32 )
                ets_printf( "." );
            else
                ets_printf( "%c", *pCurrPos );
            pCurrPos++;
        }
        ets_printf( "\n" );

        pCurrStart = pCurrEnd;
    }
}
#endif

// initialization of configuration manager
// provided as weak so it can be replaced at target level, if required because of the target implementing the storage with a mechanism other then saving to flash
void ConfigurationManager_Initialize()
{
    // enumerate the blocks
    ConfigurationManager_EnumerateConfigurationBlocks();
};

// Allocate HAL_CONFIGURATION_NETWORK block
void ConfigurationManager_allocate_network( uint32_t configCount )
{
    uint32_t sizeOfNetworkInterfaceConfigs = offsetof(HAL_CONFIGURATION_NETWORK, Configs) +  configCount * sizeof(HAL_Configuration_NetworkInterface *);
    g_TargetConfiguration.NetworkInterfaceConfigs = (HAL_CONFIGURATION_NETWORK*)platform_malloc(sizeOfNetworkInterfaceConfigs);
    g_TargetConfiguration.NetworkInterfaceConfigs->Count = configCount;
}

// Allocate HAL_CONFIGURATION_WIRELESS80211 block
void ConfigurationManager_allocate_wireless( uint32_t configCount )
{
    uint32_t sizeOfWireless80211Configs = offsetof(HAL_CONFIGURATION_NETWORK_WIRELESS80211, Configs) + configCount * sizeof(HAL_Configuration_Wireless80211 *);
    g_TargetConfiguration.Wireless80211Configs = (HAL_CONFIGURATION_NETWORK_WIRELESS80211*)platform_malloc(sizeOfWireless80211Configs);
    g_TargetConfiguration.Wireless80211Configs->Count = configCount;
}

// Allocate HAL_CONFIGURATION_X509_CERTIFICATE block
// TODO FIXME
// void ConfigurationManager_allocate_certificates( uint32_t certificateCount )
// {
// }

// Enumerates the configuration blocks from the configuration flash sector 
// it's implemented with 'weak' attribute so it can be replaced at target level if a different persistance mechanism is used
void ConfigurationManager_EnumerateConfigurationBlocks()
{
    // Fix adapter counts -  will get these from NVS 
    // Still need to do wireless AP
    int networkCount = 3;           // Esp32 has 3 network interfaces, Ethernet, Wireless Station & Wireless APn       
    int wirelessCount = 2;
    int certificateCount = 1;
  
    ConfigurationManager_allocate_network( networkCount );
    ConfigurationManager_allocate_wireless( wirelessCount );
    // TODO FIXME
    //ConfigurationManager_allocate_certificates( certificateCount );
    (void)certificateCount;
    
    for( int configIndex = 0; configIndex < networkCount; configIndex++)
    {
        g_TargetConfiguration.NetworkInterfaceConfigs->Configs[configIndex] = (HAL_Configuration_NetworkInterface*)platform_malloc(sizeof(HAL_Configuration_NetworkInterface));
        ConfigurationManager_GetConfigurationBlock(g_TargetConfiguration.NetworkInterfaceConfigs->Configs[configIndex], DeviceConfigurationOption_Network, configIndex);
    }

    for( int configIndex = 0; configIndex < wirelessCount; configIndex++)
    {
        g_TargetConfiguration.Wireless80211Configs->Configs[configIndex] = (HAL_Configuration_Wireless80211*)platform_malloc(sizeof(HAL_Configuration_Wireless80211));
        ConfigurationManager_GetConfigurationBlock( g_TargetConfiguration.Wireless80211Configs->Configs[configIndex], DeviceConfigurationOption_Wireless80211Network, configIndex);
    }

    // TODO FIXME
    // for( int certificateIndex = 0; certificateIndex < certificateCount; configIndex++)
    // {
    //     g_TargetConfiguration.CertificateStore->Certificate[certificateIndex] = (HAL_Configuration_X509CaRootBundle*)platform_malloc(sizeof(HAL_Configuration_X509CaRootBundle));
    //     ConfigurationManager_GetConfigurationBlock( g_TargetConfiguration.CertificateStore->Certificates[certificateIndex], DeviceConfigurationOption_X509CaRootBundle, certificateIndex);
    // }
}

//  Default initialisation of wireless config blocks for ESP32 targets
void InitialiseWirelessDefaultConfig(HAL_Configuration_Wireless80211 * pconfig, uint32_t configurationIndex)
{
    memset( pconfig, 0, sizeof(HAL_Configuration_Wireless80211));

    pconfig->Id = configurationIndex;

    switch(configurationIndex)
    {
        case 0: // Wireless Station

            // test default data for AP
           // hal_strcpy_s( (char*)pconfig->Ssid, sizeof(pconfig->Ssid), "myssid" );
           // hal_strcpy_s( (char*)pconfig->Password, sizeof(pconfig->Password), "mypassphase" );

            pconfig->Authentication = AuthenticationType_WPA2;
            pconfig->Encryption = EncryptionType_WPA2;
            break;

        case 1: // Wireless AP
            // Build AP default ssid based on MAC addr
            uint8_t mac[6];
            esp_efuse_mac_get_custom(mac);

            hal_strcpy_s(  (char *)pconfig->Ssid, sizeof(pconfig->Ssid), "nano_");
            char * pMac = (char*)pconfig->Ssid + 5;
            for( int index=0; index < 6; index++) 
            {
                sprintf( pMac, "%02X", (int)mac[index] );
                pMac+=2;
            }
            *pMac = 0; 

            hal_strcpy_s(  (char *)pconfig->Password, sizeof(pconfig->Password), "password");
            pconfig->Authentication = AuthenticationType_WPA2;
            pconfig->Encryption = EncryptionType_WPA2;
            break;
    }
}

//  Default initialisation of Network interface config blocks for ESP32 targets
bool InitialiseNetworkDefaultConfig(HAL_Configuration_NetworkInterface * pconfig, uint32_t configurationIndex)
{
    memset( pconfig, 0, sizeof(HAL_Configuration_NetworkInterface));
    
    switch(configurationIndex)
    {
        case 0: // Wireless Station
            pconfig->InterfaceType = NetworkInterfaceType_Wireless80211;
            pconfig->StartupAddressMode = AddressMode_DHCP;
            pconfig->SpecificConfigId = 0;
            break;

        case 1: // Wireless AP
            pconfig->InterfaceType = NetworkInterfaceType_Wireless80211;
            pconfig->StartupAddressMode = AddressMode_Static;
            pconfig->SpecificConfigId = 1;
            // Set default address 192.168.1.1
            //pconfig->IPv4Address
            //pconfig->IPv4NetMask
           // pconfig->IPv4GatewayAddress
            break;

        case 2: // Ethernet
            pconfig->InterfaceType = NetworkInterfaceType_Ethernet;
            pconfig->StartupAddressMode = AddressMode_DHCP;
            break;
    }

    // always good
    return TRUE;
}

// Gets a configuration block from the configuration block stored in the NVS block, 
// maybe better to store each config item under a separate key which would work better if config block changes
bool ConfigurationManager_GetConfigurationBlock(void* configurationBlock, DeviceConfigurationOption configuration, uint32_t configurationIndex)
{
	bool result = FALSE;
	nvs_handle out_handle;
	size_t     blobSize = 0;
    char configName[10];

#ifdef DEBUG_CONFIG
    ets_printf("GetConfig %d, %d\n", (int)configuration, configurationIndex);
#endif
    
    // validate if the requested block exists
    if(configuration == DeviceConfigurationOption_Network)
    {
        if(g_TargetConfiguration.NetworkInterfaceConfigs->Count == 0 ||
            (configurationIndex + 1) > g_TargetConfiguration.NetworkInterfaceConfigs->Count)
        {
#ifdef DEBUG_CONFIG
            ets_printf("GetConfig CN exit false\n");
#endif
            return FALSE;
        }

        // set blob size
        blobSize = sizeof(HAL_Configuration_NetworkInterface);
        configName[0] = 'N';
    }
    else if(configuration == DeviceConfigurationOption_Wireless80211Network)
    {
        if(g_TargetConfiguration.Wireless80211Configs->Count == 0 ||
            (configurationIndex + 1) > g_TargetConfiguration.Wireless80211Configs->Count)
        {
#ifdef DEBUG_CONFIG
            ets_printf("GetConfig WN exit false\n");
#endif
            return FALSE;
        }

        // set blob size
        blobSize = sizeof(HAL_Configuration_Wireless80211);
        configName[0] = 'W';
    }
    else if(configuration == DeviceConfigurationOption_X509CaRootBundle)
    {
        if(g_TargetConfiguration.CertificateStore->Count == 0 ||
            (configurationIndex + 1) > g_TargetConfiguration.CertificateStore->Count)
        {
#ifdef DEBUG_CONFIG
            ets_printf("GetConfig XC exit false\n");
#endif
            return FALSE;
        }

        // set blob size
        // TODO FIXME
        blobSize = offsetof(HAL_Configuration_X509CaRootBundle, Certificate);
        blobSize += ((HAL_Configuration_X509CaRootBundle*)configurationBlock)->CertificateSize;

        configName[0] = 'X';
    }    

    // Anything to get
    if  (blobSize != 0 ) 
    {
        // Open NVS storage using NanoFramework namespace
        esp_err_t ret = nvs_open( NVS_NAMESPACE, NVS_READWRITE, &out_handle);
        if ( ret == ESP_OK )
        {
            bool storeConfig = false;
            itoa(configurationIndex, configName+1, 10);

            // copy the config block content to the pointer in the argument
            ret = nvs_get_blob(out_handle, configName, (void *)configurationBlock, &blobSize);
            if (ret != ESP_OK)
            {
                if ( configuration == DeviceConfigurationOption_Wireless80211Network )
                {
                    InitialiseWirelessDefaultConfig((HAL_Configuration_Wireless80211 *)configurationBlock,configurationIndex);
                    storeConfig = true;
                }
                else if ( configuration == DeviceConfigurationOption_Network )
                {
                    // OK to skip checking return value
                    InitialiseNetworkDefaultConfig((HAL_Configuration_NetworkInterface *)configurationBlock,configurationIndex);
                    storeConfig = true;
                }
                else if ( configuration == DeviceConfigurationOption_X509CaRootBundle )
                {
                    // OK to skip checking return value
                    storeConfig = false;
                }
                else
                {
                    // If not found just return initialized block
    //                    memset( configurationBlock, 0, blobSize);
                }
            } 
            
#ifdef DEBUG_CONFIG
            PrintBlock( (char*)configurationBlock, blobSize);
#endif

            result = TRUE;
            nvs_close(out_handle);
            if ( storeConfig )
            {
                // TODO FIXME, offset parameter is 0
                ConfigurationManager_StoreConfigurationBlock(configurationBlock, configuration, configurationIndex, blobSize, 0);
            }
        }
    }

#ifdef DEBUG_CONFIG
    ets_printf("GetConfig exit %d\n", result);
#endif

    return result;
}


bool StoreConfigBlock(char ConfigType, uint32_t configurationIndex, void * pConfigBlock, size_t blobSize)
{
	nvs_handle out_handle;
    char configName[10];
    bool result = false;

    configName[0] = ConfigType;

    // copy the config block content to the pointer in the argument
    esp_err_t ret = nvs_open( NVS_NAMESPACE, NVS_READWRITE, &out_handle);
    if ( ret == ESP_OK )
    {
        itoa(configurationIndex, configName+1, 10);
        ret = nvs_set_blob(out_handle, configName, pConfigBlock, blobSize);
        if ( ret == ESP_OK ) 
        {
            ret = nvs_commit(out_handle);
            if (ret == ESP_OK) result = true;
        }
        nvs_close(out_handle);
    }

    return result;
}

// Stores the network configuration block to the EPS32 storage 
bool ConfigurationManager_StoreConfigurationBlock(void* configurationBlock, DeviceConfigurationOption configuration, uint32_t configurationIndex, uint32_t blockSize, uint32_t offset)
{
	bool result = false;
    bool requiresEnumeration = false;
    size_t  blobSize = 0;
    //size_t  chunkSize = 0;
    char ConfigType;

    // TODO FIXME
    (void)offset;

#ifdef DEBUG_CONFIG
    ets_printf("StoreConfig %d, %d", (int)configuration, configurationIndex);
#endif

    if(configuration == DeviceConfigurationOption_Network)
    {
        // set blob size
        blobSize = sizeof(HAL_Configuration_NetworkInterface);
        ConfigType = 'N';
    }
    else if(configuration == DeviceConfigurationOption_Wireless80211Network)
    {
        // set blob size
        blobSize = sizeof(HAL_Configuration_Wireless80211);
        ConfigType = 'W';
    }
    else if(configuration == DeviceConfigurationOption_X509CaRootBundle)
    {
        // set blob size
        // because X509 certificate has a variable length need to compute the block size in two steps
        blobSize = offsetof(HAL_Configuration_X509CaRootBundle, Certificate);
        blobSize += ((HAL_Configuration_X509CaRootBundle*)configurationBlock)->CertificateSize;

        ConfigType = 'C';
    }
    else if(configuration == DeviceConfigurationOption_All)
    {
        // All configuration blocks in one block
        // Separate and update

#ifdef DEBUG_CONFIG
        ets_printf( "Block size %d\n", blockSize);
    
        ets_printf( "sizeof HAL_Configuration_NetworkInterface %d\n", sizeof(HAL_Configuration_NetworkInterface));
        ets_printf( "sizeof HAL_Configuration_Wireless80211 %d\n", sizeof(HAL_Configuration_Wireless80211));
        ets_printf( "sizeof of X509Certificate varies\n");
#endif

        configurationIndex = 0;

        char * pConfig = (char *)configurationBlock;
        while( pConfig < (pConfig + blockSize) )
        {
            // Network interface block ?
            if ( *pConfig == 'N')
            {
                HAL_Configuration_NetworkInterface * pNetConfig = (HAL_Configuration_NetworkInterface*)pConfig;
                pConfig += sizeof(HAL_Configuration_NetworkInterface);

                result = StoreConfigBlock( 'N', configurationIndex, (void*)pNetConfig, sizeof(HAL_Configuration_NetworkInterface) );
                configurationIndex++;

#ifdef DEBUG_CONFIG
                ets_printf("Network block %X %X ret:%d\n", pNetConfig->InterfaceType, pNetConfig->SpecificConfigId, result);
                PrintBlock( (char *)pNetConfig, sizeof(HAL_Configuration_NetworkInterface) );
#endif
            }
            // Wireless block ?
            else if (*pConfig == 'W')
            {
                HAL_Configuration_Wireless80211 * pWirelessConfig = (HAL_Configuration_Wireless80211*)pConfig;
                pConfig += sizeof(HAL_Configuration_Wireless80211);

                result = StoreConfigBlock( 'W', pWirelessConfig->Id, (void*)pWirelessConfig, sizeof(HAL_Configuration_Wireless80211) );
                
#ifdef DEBUG_CONFIG
                ets_printf("Wireless block %d ssid:%s password:%s ret:%d\n", pWirelessConfig->Id, pWirelessConfig->Ssid, pWirelessConfig->Password, result);
                PrintBlock( (char *)pWirelessConfig, sizeof(HAL_Configuration_Wireless80211) );
#endif
            }
            // X509 certificate block ?
            else if (*pConfig == 'C')
            {
                // TODO
//                 HAL_Configuration_X509CaRootBundle * pX509Certificate = (HAL_Configuration_X509CaRootBundle*)pConfig;

//                 // set block size, in case it's not already set
//                 // because X509 certificate has a variable length need to compute the block size in two steps
//                 chunkSize = offsetof(HAL_Configuration_X509CaRootBundle, Certificate);
//                 chunkSize += pX509Certificate->CertificateSize;

//                 pConfig += chunkSize;

//                 result = StoreConfigBlock( 'C', pX509Certificate->Id, (void*)pX509Certificate, chunkSize );
                
// #ifdef DEBUG_CONFIG
//                 ets_printf("X509 certificate block ret:%d\n", result);
//                 PrintBlock( (char *)pX509Certificate, chunkSize );
// #endif
            }            
            else
                break;
        }

        return result;
    }
    
    // Anything to save
    if  (blobSize != 0 ) 
    {
        result = StoreConfigBlock( ConfigType, configurationIndex, configurationBlock, blobSize);
    }

    if(requiresEnumeration)
    {
        // perform enumeration of configuration blocks
        ConfigurationManager_EnumerateConfigurationBlocks();
    }

#ifdef DEBUG_CONFIG
    ets_printf("StoreConfig exit %d", result);
#endif

    return result;
}

// Updates a configuration block in the configuration flash sector
bool ConfigurationManager_UpdateConfigurationBlock(void* configurationBlock, DeviceConfigurationOption configuration, uint32_t configurationIndex)
{
    // figure out the block size first
    uint32_t blockSize = 0;

    switch(configuration)
    {
        case DeviceConfigurationOption_Network:
            blockSize = sizeof(HAL_Configuration_NetworkInterface);
            break;

        case DeviceConfigurationOption_Wireless80211Network:
            blockSize = sizeof(HAL_Configuration_Wireless80211);
            break;

        case DeviceConfigurationOption_X509CaRootBundle:
            blockSize = offsetof(HAL_Configuration_X509CaRootBundle, Certificate);
            blockSize += ((HAL_Configuration_X509CaRootBundle*)configurationBlock)->CertificateSize;
            break;

        default:
            // shouldn't ever reach here
            return FALSE;
    }

    return ConfigurationManager_StoreConfigurationBlock(configurationBlock, configuration, configurationIndex, blockSize, 0);    
}
