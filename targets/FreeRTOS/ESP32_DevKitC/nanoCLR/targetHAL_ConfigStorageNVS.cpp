//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoHAL_v2.h>
#include <nanoWeak.h>
#include "Esp32_os.h"
#include "targetHAL_ConfigStorage.h"

// NVS parameters for Interface config
#define NVS_NAMESPACE	    "nanoF"


// Save File details
static nvs_handle configHandle = -1;
static DeviceConfigurationOption configType;
static uint32_t configIndex;

enum  ConfigIoType { Read, Write, GetSize };

void Config_Initialise()
{
    
}


//  Create config name in NVS from option/index
//  return name in configName
void Config_GetConfigFileName(DeviceConfigurationOption configuration, uint32_t configurationIndex, char * configName)
{
	switch (configuration)
	{
	case DeviceConfigurationOption_Network:
		*configName++ = CONFIG_TYPE_NETWORK;
		break;

	case DeviceConfigurationOption_Wireless80211Network:
		*configName++ = CONFIG_TYPE_WIRELESS;
		break;

	case DeviceConfigurationOption_WirelessNetworkAP:
		*configName++ = CONFIG_TYPE_WIRELESSAP;
		break;

	case DeviceConfigurationOption_X509CaRootBundle:
		*configName++ = CONFIG_TYPE_CERTIFICATE;
		break;

	default:
		*configName++ = '?';
		break;
	}

	itoa(configurationIndex, configName, 10);
}

//
//  Config_OpenFile -
//
//  Open file for configuration / index
//  For the case of NVS we just open the NVS partition, save file details, handle
//  
// Parameters:-
//   configuration      : Type of configuration block to open
//   configurationIndex : Index of type to open
//
// Return :-
//    true  : returns handle
//    false -1
//
uint32_t Config_OpenFile( DeviceConfigurationOption configuration, uint32_t configurationIndex )
{
    // Open NVS storage using NanoFramework namespace
    esp_err_t ret = nvs_open( NVS_NAMESPACE, NVS_READWRITE, &configHandle);
    if ( ret == ESP_OK ) 
    {
        configType = configuration;
        configIndex = configurationIndex;
        return configHandle;
    }

    return CONFIG_ERROR;
}


//
//  Config_CloseFile - Close opened file / NVS system
//  
// Parameters:-
//   handle : Handle returned from Config_openFile
//
// return:-
//    true - File closed ok 
//    false- Error/File not open/Invalid handle
//
bool Config_CloseFile( uint32_t handle )
{
    // Check valid handle
    if ( handle != configHandle) return false;

    nvs_close(configHandle);
    configHandle = CONFIG_ERROR;
    return true;
}

//
//  Shared IO 
//
uint32_t Config_IO(uint32_t handle, ConfigIoType ioType, uint8_t * pData, int32_t size )
{
    int32_t result = CONFIG_ERROR;
    char configName[10];
    esp_err_t ret;
    size_t blobSize;

    // Check valid handle
    if ( handle != configHandle) return CONFIG_ERROR;

    // Build file system name
    Config_GetConfigFileName( configType, configIndex, configName);

    while(true)
    {
        if ( ioType == GetSize || ioType == Read )
        {
                // Get size of blob before we read it
                ret = nvs_get_blob(configHandle, configName, (void *)0, &blobSize);
                if (ret != ESP_OK) break;
                result = (int32_t)blobSize;

                // If GetSize then finnish
                if ( ioType == GetSize) break;
            
                // Do read
                // Check room in buffer
                if ( (int32_t)blobSize > size ) break;   // error

                // Read blob into pData 
                
                ret = nvs_get_blob(configHandle, configName, (void *)pData, &blobSize);
                if (ret == ESP_OK) 
                    result = (int32_t)blobSize;
                else
                    result = CONFIG_ERROR;
                
                break;
        }
        else if ( ioType == Write )
        {
            ret = nvs_set_blob(configHandle, configName, (const void *)pData, (size_t)size);
            if ( ret == ESP_OK ) 
            {
                // Write OK so commit it
                ret = nvs_commit(configHandle);
                if (ret == ESP_OK) result = 0;
            }
            break;
        }
        else
            break;
       
    } // while

    return result;
}

//
//  Config_FileSize - Return size of file
//
// Parameters:-
//   handle : Handle returned from Config_openFile
//
// return:-
//    File size 
//    -1 = Error/Not found/Invalid handle
//
int32_t Config_FileSize( uint32_t handle )
{
    return Config_IO( handle, GetSize, 0, 0);
}

//
//  Config_WriteFile -  Write data to NVS system
//
// Parameters:-
//   handle    : Handle returned from Config_openFile
//   pData     : Pointer data to write
//   writeSize : Size of data to write
//
// return:-
//    true - OK 
//    false- Error/Not found
//
bool Config_WriteFile(uint32_t handle, uint8_t * pData, int32_t writeSize )
{
    return ( Config_IO( handle, Write, pData, writeSize) != CONFIG_ERROR);
}

//
// Config_ReadFile - Read the data from file opened in NVS
//
// Parameters:-
//   handle : Handle returned from Config_openFile
//   pData  : Ponter to where data is to be stored
//
// return:-
//    Read size 
//    -1 = Error/Not found/Invalid handle
//
int32_t Config_ReadFile( uint32_t handle, uint8_t * pData, int32_t maxSizeData )
{
    return Config_IO( handle, Read, pData, maxSizeData);
}

