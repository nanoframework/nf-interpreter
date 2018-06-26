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

 

// Enumerates the configuration blocks from the configuration flash sector 
// it's implemented with 'weak' attribute so it can be replaced at target level if a different persistance mechanism is used
void ConfigurationManager_EnumerateConfigurationBlocks()
{
    // Fix adapter counts -  will get these from NVS 
    // Still need to do wireless AP
    int networkCount = 3;           // Esp32 has 3 network interfaces, Ethernet, Wireless Station & Wireless APn       
    int wirelessCount = 2;
  
    ConfigurationManager_allocate_network( networkCount );
    ConfigurationManager_allocate_wireless( wirelessCount );
    
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
}

void InitialiseWirelessDefaultConfig(HAL_Configuration_Wireless80211 * pconfig, int configurationIndex)
{
    memset( pconfig, 0, sizeof(HAL_Configuration_Wireless80211));

    pconfig->Id = configurationIndex;

    switch(configurationIndex)
    {
        case 0: // Wireless Station
// Test code - remove
    hal_strcpy_s(  (char *)pconfig->Ssid, sizeof(pconfig->Ssid), "OpenWrt2");
    hal_strcpy_s(  (char *)pconfig->Password, sizeof(pconfig->Password), "Daniel457");

            pconfig->Authentication = AuthenticationType_WPA2;
            pconfig->Encryption = EncryptionType_WPA2;
            break;

        case 1: // Wireless Station
            hal_strcpy_s(  (char *)pconfig->Ssid, sizeof(pconfig->Ssid), "nanoFramework");
            hal_strcpy_s(  (char *)pconfig->Password, sizeof(pconfig->Password), "password");
            pconfig->Authentication = AuthenticationType_WPA2;
            pconfig->Encryption = EncryptionType_WPA2;
            break;
    }
}

//
//  Default inialisation of Network interface config blocks
//
void InitialiseNetworkDefaultConfig(HAL_Configuration_NetworkInterface * pconfig, int configurationIndex)
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
}

// Gets a configuration block from the configuration block stored in the NVS block, 
// maybe better to store each config item under a separate key which would work better if config block changes
bool ConfigurationManager_GetConfigurationBlock(void* configurationBlock, DeviceConfigurationOption configuration, uint32_t configurationIndex)
{
	bool result = FALSE;
	nvs_handle out_handle;
	size_t     blobSize = 0;
    char configName[10];

    // validate if the requested block exists
    if(configuration == DeviceConfigurationOption_Network)
    {
        // set blob size
        blobSize = sizeof(HAL_Configuration_NetworkInterface);
        configName[0] = 'N';
    }
    else if(configuration == DeviceConfigurationOption_Wireless80211Network)
    {
        // set blob size
        blobSize = sizeof(HAL_Configuration_Wireless80211);
        configName[0] = 'W';
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
                    InitialiseNetworkDefaultConfig((HAL_Configuration_NetworkInterface *)configurationBlock,configurationIndex);
                    storeConfig = true;
                }
                else
                {
                    // If not found just return initialized block
    //                    memset( configurationBlock, 0, blobSize);
                }
            } 
            
            result = TRUE;
            nvs_close(out_handle);
            if ( storeConfig )
            {
                ConfigurationManager_StoreConfigurationBlock(configurationBlock, configuration, configurationIndex, blobSize);
            }
        }
    }

    return result;
}

// Stores the network configuration block to the EPS32 storage 
bool ConfigurationManager_StoreConfigurationBlock(void* configurationBlock, DeviceConfigurationOption configuration, uint32_t configurationIndex, uint32_t blockSize)
{
	bool result = FALSE;
	nvs_handle out_handle;
    bool requiresEnumeration = FALSE;
    size_t     blobSize = 0;
    char configName[10];

    if(configuration == DeviceConfigurationOption_Network)
    {
        // set blob size
        blobSize = sizeof(HAL_Configuration_NetworkInterface);
        configName[0] = 'N';
    }
    else if(configuration == DeviceConfigurationOption_Wireless80211Network)
    {
        // set blob size
        blobSize = sizeof(HAL_Configuration_Wireless80211);
        configName[0] = 'W';
    }
    else if(configuration == DeviceConfigurationOption_All)
    {
        // particular situation where we are receiving the full configuration block

        configName[0] = 'D';
        blobSize =blockSize;
        // always enumerate the blocks again after storing it
        // requiresEnumeration = TRUE;
    }
    
    // Anything to save
    if  (blobSize != 0 ) 
    {
        // copy the config block content to the pointer in the argument
        esp_err_t ret = nvs_open( NVS_NAMESPACE, NVS_READWRITE, &out_handle);
        if ( ret == ESP_OK )
        {
            itoa(configurationIndex, configName+1, 10);
            ret = nvs_set_blob(out_handle, configName, (void *)configurationBlock, blobSize);
            if ( ret == ESP_OK ) 
            {
                ret = nvs_commit(out_handle);
                if (ret == ESP_OK) result = TRUE;
            }
            nvs_close(out_handle);
        }
    }

    if(requiresEnumeration)
    {
        // perform enumeration of configuration blocks
        ConfigurationManager_EnumerateConfigurationBlocks();
    }

    return result;
}

// Updates a configuration block in the configuration flash sector
bool ConfigurationManager_UpdateConfigurationBlock(void* configurationBlock, DeviceConfigurationOption configuration, uint32_t configurationIndex)
{
    return ConfigurationManager_StoreConfigurationBlock(configurationBlock, configuration, configurationIndex, 0);    
}
