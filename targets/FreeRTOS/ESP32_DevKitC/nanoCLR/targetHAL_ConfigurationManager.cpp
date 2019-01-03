//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoHAL_v2.h>
#include <nanoWeak.h>
#include "Esp32_os.h"
#include "targetHAL_ConfigStorage.h"
#include <target_platform.h>

// NVS parameters for Interface config
#define NVS_NAMESPACE	    "nanoF"


// #define DEBUG_CONFIG        1

// Saves the remaining bundle size for multiple block cert configs
static int32_t savedBundleSize;
static int32_t remainingBundleSize;
static int32_t savedBundleOffset;

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
    memset( (void*)&g_TargetConfiguration, 0, sizeof(g_TargetConfiguration));

    Config_Initialise();
    
    // enumerate the blocks
    ConfigurationManager_EnumerateConfigurationBlocks();
};


int32_t ConfigurationManager_FindConfigurationBlockSize(DeviceConfigurationOption configuration, uint32_t configurationIndex)
{
	uint32_t handle;
	int32_t configSize = 0;

    handle = Config_OpenFile(configuration, configurationIndex, false);
    if ( handle != CONFIG_ERROR )
    {
        configSize = Config_FileSize(handle);
#ifdef DEBUG_CONFIG
        ets_printf( "Find type %d index %d, length %d\n", (int)configuration, configurationIndex, configSize);
#endif
        Config_CloseFile(handle);
    }

	return configSize;
}

bool ConfigurationManager_GetConfigurationBlockFromStorage( DeviceConfigurationOption configuration, uint32_t configurationIndex, void* configurationBlock, int32_t maxBlockSize)
{
	uint32_t handle;
	uint32_t  readSize = CONFIG_ERROR;

#ifdef DEBUG_CONFIG
    ets_printf("GetConfigFromStorage %d, %d\n", (int)configuration, configurationIndex);
#endif

    handle = Config_OpenFile(configuration, configurationIndex, false);
    if ( handle != CONFIG_ERROR )
    {
        readSize = Config_ReadFile(handle, (uint8_t *)configurationBlock, maxBlockSize );
        Config_CloseFile(handle);
    }

	return (readSize != CONFIG_ERROR);
}



// Allocate HAL_CONFIGURATION_NETWORK block if required
void ConfigurationManager_allocate_network( uint32_t configCount )
{
    if ( g_TargetConfiguration.NetworkInterfaceConfigs == 0)
    {
        uint32_t sizeOfNetworkInterfaceConfigs = offsetof(HAL_CONFIGURATION_NETWORK, Configs) +  configCount * sizeof(HAL_Configuration_NetworkInterface *);
        g_TargetConfiguration.NetworkInterfaceConfigs = (HAL_CONFIGURATION_NETWORK*)platform_malloc(sizeOfNetworkInterfaceConfigs);
        memset( (void *)g_TargetConfiguration.NetworkInterfaceConfigs, 0,  sizeOfNetworkInterfaceConfigs);
    }
}

// Allocate HAL_CONFIGURATION_WIRELESS80211 block if required
void ConfigurationManager_allocate_wireless( uint32_t configCount )
{
    if ( g_TargetConfiguration.Wireless80211Configs == 0 )
    {
        uint32_t sizeOfWireless80211Configs = offsetof(HAL_CONFIGURATION_NETWORK_WIRELESS80211, Configs) + configCount * sizeof(HAL_Configuration_Wireless80211 *);
        g_TargetConfiguration.Wireless80211Configs = (HAL_CONFIGURATION_NETWORK_WIRELESS80211*)platform_malloc(sizeOfWireless80211Configs);
        memset( (void *)g_TargetConfiguration.Wireless80211Configs, 0,  sizeOfWireless80211Configs);
    }
}

// Allocate HAL_CONFIGURATION_X509_CERTIFICATE block if required
void ConfigurationManager_allocate_certificates( uint32_t certificateCount )
{
    if ( g_TargetConfiguration.CertificateStore == 0)
    {
        uint32_t sizeOfX509CertificateStore = offsetof(HAL_CONFIGURATION_X509_CERTIFICATE, Certificates) + certificateCount * sizeof(g_TargetConfiguration.CertificateStore->Certificates[0]);
        g_TargetConfiguration.CertificateStore = (HAL_CONFIGURATION_X509_CERTIFICATE*)platform_malloc(sizeOfX509CertificateStore);
        memset( (void *)g_TargetConfiguration.CertificateStore, 0,  sizeOfX509CertificateStore);
    }
    
    // Init pointers to cert bundles to 0 ( not allocated )
    for( uint32_t index=0; index < certificateCount; index++ )
        g_TargetConfiguration.CertificateStore->Certificates[index] = 0;
}

bool StoreConfigBlock(DeviceConfigurationOption configType, uint32_t configurationIndex, void * pConfigBlock, size_t writeSize )
{
    bool result = false;
    uint32_t fileHandle;

    fileHandle = Config_OpenFile(configType, configurationIndex, true);
    if (fileHandle != CONFIG_ERROR )
    {
        result = Config_WriteFile(fileHandle, (uint8_t*)pConfigBlock, writeSize) ;
#ifdef DEBUG_CONFIG
        ets_printf( "store type %d index %d, length %d result %d\n", (int)configType, configurationIndex, writeSize, (int)result);
#endif
        Config_CloseFile(fileHandle);
    }

    return result;
}

// Enumerates the configuration blocks from the configuration flash sector 
// it's implemented with 'weak' attribute so it can be replaced at target level if a different persistance mechanism is used
void ConfigurationManager_EnumerateConfigurationBlocks()
{
    // Fix adapter counts -  will get these from NVS 
    // Still need to do wireless AP
    int networkCount = 3;           // Esp32 has 3 network interfaces, Ethernet, Wireless Station & Wireless APn       
    int wirelessCount = 2;
    int certificateCount = 1;
  
    // Allocate main structures for each type
    ConfigurationManager_allocate_network( networkCount );
    ConfigurationManager_allocate_wireless( wirelessCount );
    ConfigurationManager_allocate_certificates( certificateCount );
    
    // For each network interface allocate Network config if required and load from storage
    for( int configIndex = 0; configIndex < networkCount; configIndex++)
    {
        if ( g_TargetConfiguration.NetworkInterfaceConfigs->Configs[configIndex] == 0)
        {
            g_TargetConfiguration.NetworkInterfaceConfigs->Configs[configIndex] = (HAL_Configuration_NetworkInterface*)platform_malloc(sizeof(HAL_Configuration_NetworkInterface));
        }

        if ( ConfigurationManager_GetConfigurationBlockFromStorage( 
                    DeviceConfigurationOption_Network, 
                    configIndex, 
                    g_TargetConfiguration.NetworkInterfaceConfigs->Configs[configIndex], 
                    sizeof(HAL_Configuration_NetworkInterface) 
                    ) == false )
        {
            // No config saved so init default block
            HAL_Configuration_NetworkInterface* configPtr = g_TargetConfiguration.NetworkInterfaceConfigs->Configs[configIndex];
            InitialiseNetworkDefaultConfig(configPtr,configIndex);
            StoreConfigBlock(DeviceConfigurationOption_Network, configIndex, configPtr, sizeof(HAL_Configuration_NetworkInterface) );
        }
    } 

    g_TargetConfiguration.NetworkInterfaceConfigs->Count = networkCount;

    // For each Wireless interface allocate Wireless config if required and load from storage
    for( int configIndex = 0; configIndex < wirelessCount; configIndex++)
    {
        if ( g_TargetConfiguration.Wireless80211Configs->Configs[configIndex] == 0 )
        {
            g_TargetConfiguration.Wireless80211Configs->Configs[configIndex] = (HAL_Configuration_Wireless80211*)platform_malloc(sizeof(HAL_Configuration_Wireless80211));
        }
        if ( ConfigurationManager_GetConfigurationBlockFromStorage( 
                    DeviceConfigurationOption_Wireless80211Network, 
                    configIndex, 
                    g_TargetConfiguration.Wireless80211Configs->Configs[configIndex], 
                    sizeof(HAL_Configuration_Wireless80211)
                    ) == false )
        {
            HAL_Configuration_Wireless80211* configPtr = g_TargetConfiguration.Wireless80211Configs->Configs[configIndex];
            InitialiseWirelessDefaultConfig(configPtr,configIndex);
            StoreConfigBlock(DeviceConfigurationOption_Wireless80211Network, configIndex, configPtr, sizeof(HAL_Configuration_Wireless80211) );
        }
    }
    g_TargetConfiguration.Wireless80211Configs->Count = wirelessCount;

 
    // For each Certificate bundle allocate memory and load from storage
    for( int certificateIndex = 0; certificateIndex < certificateCount; certificateIndex++)
    {
        // Free any existing bundles first, we need to reallocate as bundles are variable size
        if ( g_TargetConfiguration.CertificateStore->Certificates[certificateIndex] != 0 ) 
        {  
            platform_free(g_TargetConfiguration.CertificateStore->Certificates[certificateIndex]);
            g_TargetConfiguration.CertificateStore->Certificates[certificateIndex] = 0;
        }

        // Find size of saved cert bundle
		int32_t bundleSize = ConfigurationManager_FindConfigurationBlockSize(DeviceConfigurationOption_X509CaRootBundle, certificateIndex);
		if (bundleSize > 0)
		{
            // Bundle exits
            g_TargetConfiguration.CertificateStore->Count = certificateIndex+1;
			g_TargetConfiguration.CertificateStore->Certificates[certificateIndex] = (HAL_Configuration_X509CaRootBundle*)platform_malloc(bundleSize);
			ConfigurationManager_GetConfigurationBlockFromStorage(  DeviceConfigurationOption_X509CaRootBundle, certificateIndex, g_TargetConfiguration.CertificateStore->Certificates[certificateIndex], bundleSize);
		}
    }
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
           // hal_strcpy_s( (char*)pconfig->Ssid, sizeof(pconfig->Ssid), "MySSID" );
           // hal_strcpy_s( (char*)pconfig->Password, sizeof(pconfig->Password), "MyPassword" );

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
bool ConfigurationManager_GetConfigurationBlock(void* configurationBlock, DeviceConfigurationOption configuration, uint32_t configurationIndex)
{
    int sizeOfBlock = 0;
    uint8_t* blockAddress = NULL;

#ifdef DEBUG_CONFIG
    ets_printf("GetConfig %d, %d\n", (int)configuration, configurationIndex);
#endif
    
    // validate if the requested block exists
    if(configuration == DeviceConfigurationOption_Network)
    {
        if( g_TargetConfiguration.NetworkInterfaceConfigs->Count == 0 || 
            (configurationIndex + 1) > g_TargetConfiguration.NetworkInterfaceConfigs->Count )
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
        blockAddress = (uint8_t*)g_TargetConfiguration.NetworkInterfaceConfigs->Configs[configurationIndex];
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

       // set block size
        sizeOfBlock = sizeof(HAL_Configuration_Wireless80211);

        // get block address
        blockAddress = (uint8_t*)g_TargetConfiguration.Wireless80211Configs->Configs[configurationIndex];
    }
    else if(configuration == DeviceConfigurationOption_X509CaRootBundle)
    {
        if( g_TargetConfiguration.CertificateStore->Count == 0 ||
            (configurationIndex + 1) > g_TargetConfiguration.CertificateStore->Count)
        {
#ifdef DEBUG_CONFIG
            ets_printf("GetConfig XC exit false\n");
#endif
            return FALSE;
        }

        // get block address
        blockAddress = (uint8_t*)g_TargetConfiguration.CertificateStore->Certificates[configurationIndex];

        // set block size
        // because X509 certificate has a variable length need to compute the block size in two steps
        sizeOfBlock = offsetof(HAL_Configuration_X509CaRootBundle, Certificate);
        sizeOfBlock += ((HAL_Configuration_X509CaRootBundle*)blockAddress)->CertificateSize;
    }    

    // copy the config block content to the pointer in the argument
    memcpy(configurationBlock, blockAddress, sizeOfBlock);

    return TRUE;
}




DeviceConfigurationOption GetConfigOption( char * pConfig)
{
    switch( *pConfig )
    {
        case CONFIG_TYPE_NETWORK     : return DeviceConfigurationOption::DeviceConfigurationOption_Network;
        case CONFIG_TYPE_WIRELESS    : return DeviceConfigurationOption::DeviceConfigurationOption_Wireless80211Network;
        case CONFIG_TYPE_WIRELESSAP  : return DeviceConfigurationOption::DeviceConfigurationOption_WirelessNetworkAP;
        case CONFIG_TYPE_CERTIFICATE : return DeviceConfigurationOption::DeviceConfigurationOption_X509CaRootBundle;
    }
    return DeviceConfigurationOption_Network; 
}

bool ConfigurationManager_StoreConfigurationBlockAll(void* configurationBlock, uint32_t blockSize, uint32_t offset)
{
    size_t  chunkSize = 0;
    uint32_t certificateIndex = 0;
    bool    result = false;
    uint32_t configurationIndex = 0;

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
    char * pEndConfig = (pConfig + blockSize);
    DeviceConfigurationOption currentConfigType;

    while( pConfig < pEndConfig )
    {
        int remainingBlockSize = pEndConfig - pConfig; 

#ifdef DEBUG_CONFIG
        ets_printf("Parse block %d:%d\n", pConfig, pEndConfig);
        PrintBlock( (char *)pConfig, 16 );
#endif
        currentConfigType = GetConfigOption(pConfig);

            // X509 certificate block ?
        if (currentConfigType == DeviceConfigurationOption_X509CaRootBundle || offset > 0 )
        {
            if ( offset == 0 )
            {
                HAL_Configuration_X509CaRootBundle * pX509Certificate = (HAL_Configuration_X509CaRootBundle*)pConfig;

                // First block of certificate bundle
                // set total bundle size including header
                // because X509 certificate has a variable length need to compute the block size in two steps
                savedBundleSize = offsetof(HAL_Configuration_X509CaRootBundle, Certificate);
                savedBundleSize += pX509Certificate->CertificateSize;
                remainingBundleSize = savedBundleSize;
                savedBundleOffset = 0;

                // Free if already allocated ( could be different size )
                if ( g_TargetConfiguration.CertificateStore->Certificates[certificateIndex] != 0 ) 
                     platform_free((void*)g_TargetConfiguration.CertificateStore->Certificates);

                g_TargetConfiguration.CertificateStore->Certificates[certificateIndex] = (HAL_Configuration_X509CaRootBundle *)platform_malloc(savedBundleSize);

#ifdef DEBUG_CONFIG
                ets_printf("X509 certificate block total size:%d\n", pX509Certificate->CertificateSize);
#endif
            }

            // The current chunk size is what remains in current block
            // we can't use offset as it relates to flash memory offset, not useful
            chunkSize = remainingBlockSize;

            // Copy into correct position in memory
            memcpy( (void *)(g_TargetConfiguration.CertificateStore->Certificates[0] + savedBundleOffset),
                    (const void *)pConfig,
                    chunkSize);
#ifdef DEBUG_CONFIG
            ets_printf("X509 certificate chunksize:%d reminaing:%d\n", chunkSize, savedBundleSize);
            PrintBlock( (char *)pConfig, 32 );
#endif
            pConfig += chunkSize;
            remainingBundleSize -= chunkSize;
            savedBundleOffset += chunkSize;
            // Return true for partial blocks
            result = true;

            if ( remainingBundleSize <= 0 ) 
            {
                // Save Certificate Bundle to storage
                result = StoreConfigBlock(  DeviceConfigurationOption_X509CaRootBundle, 
                                            certificateIndex, 
                                            (void*)g_TargetConfiguration.CertificateStore->Certificates[certificateIndex], 
                                            savedBundleSize );
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
        else if ( currentConfigType == DeviceConfigurationOption_Network )
        {
            HAL_Configuration_NetworkInterface * pNetConfig = (HAL_Configuration_NetworkInterface*)pConfig;
            pConfig += sizeof(HAL_Configuration_NetworkInterface);

            result = StoreConfigBlock( currentConfigType, configurationIndex, (void*)pNetConfig, sizeof(HAL_Configuration_NetworkInterface) );
            configurationIndex++;

#ifdef DEBUG_CONFIG
            ets_printf("Network block %X %X ret:%d\n", pNetConfig->InterfaceType, pNetConfig->SpecificConfigId, result);
            PrintBlock( (char *)pNetConfig, sizeof(HAL_Configuration_NetworkInterface) );
#endif
        }
        // Wireless block ?
        else if (currentConfigType == DeviceConfigurationOption_Wireless80211Network )
        {
            HAL_Configuration_Wireless80211 * pWirelessConfig = (HAL_Configuration_Wireless80211*)pConfig;
            pConfig += sizeof(HAL_Configuration_Wireless80211);

            result = StoreConfigBlock( currentConfigType, pWirelessConfig->Id, (void*)pWirelessConfig, sizeof(HAL_Configuration_Wireless80211) );
            
#ifdef DEBUG_CONFIG
            ets_printf("Wireless block %d ssid:%s password:%s ret:%d\n", pWirelessConfig->Id, pWirelessConfig->Ssid, pWirelessConfig->Password, result);
            PrintBlock( (char *)pWirelessConfig, sizeof(HAL_Configuration_Wireless80211) );
#endif
        }
        else
            break;
    } // while

#ifdef DEBUG_CONFIG
    ets_printf("StoreConfig ALL exit %d\n",(int)result);
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
//   offset             : Offset of data when multiple blocks ( currently Certificate Bundles only ), For single type this is offset withing the type
//                        For ALL type blocks this is the offset in the total data of multiple types.
// Return:-
//    true - OK 
//    false- Error/Not found
//
bool ConfigurationManager_StoreConfigurationBlock(void* configurationBlock, DeviceConfigurationOption configuration, uint32_t configurationIndex, uint32_t blockSize, uint32_t offset)
{
	bool result = false;
    bool requiresEnumeration = false;
    size_t  blobSize = 0;

#ifdef DEBUG_CONFIG
    ets_printf("StoreConfig config:%d, index:%d  size:%d  offset:%d\n", (int)configuration, configurationIndex, blockSize, offset);
#endif

    if ( configuration == DeviceConfigurationOption_All )
        return ConfigurationManager_StoreConfigurationBlockAll(configurationBlock, blockSize, offset);

    if(configuration == DeviceConfigurationOption_Network)
    {
        // set blob size
        blobSize = sizeof(HAL_Configuration_NetworkInterface);
    }
    else if(configuration == DeviceConfigurationOption_Wireless80211Network)
    {
        // set blob size
        blobSize = sizeof(HAL_Configuration_Wireless80211);
    }
    else if(configuration == DeviceConfigurationOption_X509CaRootBundle)
    {
        // set blob size ( Total size of  X509 certificate )
        // because X509 certificate has a variable length need to compute the block size in two steps
        blobSize = offsetof(HAL_Configuration_X509CaRootBundle, Certificate);
        blobSize += ((HAL_Configuration_X509CaRootBundle*)configurationBlock)->CertificateSize;

#ifdef DEBUG_CONFIG
        ets_printf("StoreConfig x509 blobSize:%d, certsize:%d", blobSize, ((HAL_Configuration_X509CaRootBundle*)configurationBlock)->CertificateSize);
#endif
    }
    else
    {
        // Invalid Config
        return false;
    }

    // Anything to save
    if  (blobSize != 0 ) 
    {
        result = StoreConfigBlock( configuration, configurationIndex, configurationBlock, blobSize);
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

// Updates a configuration block in the configuration storage
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
