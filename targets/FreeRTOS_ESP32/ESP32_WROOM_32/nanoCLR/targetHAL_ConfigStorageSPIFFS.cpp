//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoHAL_v2.h>
#include <nanoWeak.h>
#include "Esp32_os.h"
#include "targetHAL_ConfigStorage.h"

// SPIFFS config
#define SPIFFS_Base_Path        "/I"
//#define SPIFFS_Base_Path        "/config"
#define SPIFFS_Base_Config_Name "con_"
#define SPIFFS_Partition_Label  "config"
#define SPIFFS_MaxFileNameLen   20

static const char *TAG = "Config"; 

enum  ConfigIoType { Read, Write, GetSize };

// Initialise SPIFFS
void Config_Initialise()
{ 
    esp_err_t ret; 
    
     esp_vfs_spiffs_conf_t conf = {
      .base_path = SPIFFS_Base_Path,
      .partition_label = SPIFFS_Partition_Label,
      .max_files = 5,
      .format_if_mount_failed = true
    };
    
    ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        } 
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(SPIFFS_Partition_Label, &total, &used);
    if (ret == ESP_OK) 
    {
        ESP_LOGI(TAG, "SPIFFS Partition size: total: %d, used: %d", total, used);
    }
}

//  Create config name in NVS from option/index
//  return name in configName
//
//  Return filename
//   "/config/"
void Config_GetConfigFileName(DeviceConfigurationOption configuration, uint32_t configurationIndex, char * configName)
{
    char * startName = configName;

    // Add filesystem name
    hal_strcpy_s(configName, SPIFFS_MaxFileNameLen, SPIFFS_Base_Path );
    configName += hal_strlen_s(configName);
    
    *configName++ = '/';

    // Add basename
    hal_strcpy_s( configName, SPIFFS_MaxFileNameLen - (int)(configName-startName), SPIFFS_Base_Config_Name );
    configName += hal_strlen_s(configName);

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
uint32_t Config_OpenFile( DeviceConfigurationOption configuration, uint32_t configurationIndex, bool write )
{
    char FileName[SPIFFS_MaxFileNameLen+1] = {0};

    Config_GetConfigFileName( configuration, configurationIndex, FileName);

    // Open SPIFFS config storage 
    FILE * file = fopen( FileName, write?"wb":"rb" );
    if ( file != NULL ) 
    {
        return (uint32_t)file;
    }

    ESP_LOGE(TAG, "Failed to open file (%s)", FileName );
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
    fclose( (FILE *)handle);
    return true;
}

//
//  Shared IO 
//
uint32_t Config_IO(uint32_t handle, ConfigIoType ioType, uint8_t * pData, int32_t size )
{
    FILE * file = (FILE *)handle;
    int32_t result = CONFIG_ERROR;
    size_t  ret;
    long  blobSize;

    while(true)
    {
        if ( ioType == GetSize || ioType == Read )
        {
                // Get size of blob before we read it
                fseek(file, 0, SEEK_END);
                blobSize = ftell(file);

                result = (int32_t)blobSize;

                // If GetSize then finnish
                if ( ioType == GetSize) break;
            
                // Do read
                // Check room in buffer
                if ( (int32_t)blobSize > size ) break;   // error

                // Read blob into pData 
                fseek(file, 0, SEEK_SET);
                ret = fread( (void *)pData, 1, (size_t)blobSize, file);
                if (ret != (size_t)blobSize) 
                    result = CONFIG_ERROR;
                
                break;
        }
        else if ( ioType == Write )
        {
            ret = fwrite( (const void *)pData, 1, (size_t)size, file);
            if ( ret == (size_t)size ) result = 0;
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

