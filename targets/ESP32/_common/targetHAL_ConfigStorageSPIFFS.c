//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// #include <nanoHAL.h>
// #include <nanoWeak.h>
#include "targetHAL_ConfigStorage.h"
#include <esp_vfs.h>

// this one is following the the same "drive letter" as the HAL Windows Storage INTERNAL_DRIVE0_LETTER
#define NANO_SPIFFS_BASE_PATH   "/I"
#define NANO_SPIFFS_CONFIG_NAME "con_"

// label for the config partition (same as partition CSV file)
#define SPIFFS_PARTITION_LABEL "config"

static const char *TAG = "Config";

typedef enum ConfigIoType
{
    ConfigIoType_Read,
    ConfigIoType_Write,
    ConfigIoType_GetSize,
    ConfigIoType_Append,
} ConfigIoType;

// Initialise SPIFFS
void ConfigStorage_Initialise()
{
    esp_err_t ret;

    esp_vfs_spiffs_conf_t conf = {
        .base_path = NANO_SPIFFS_BASE_PATH,
        .partition_label = SPIFFS_PARTITION_LABEL,
        .max_files = 5,
        .format_if_mount_failed = true};

    ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "NANO: Failed to mount or format filesystem");
        }
        else if (ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG, "NANO: Failed to find SPIFFS partition");
        }
        else
        {
            ESP_LOGE(TAG, "NANO: Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(SPIFFS_PARTITION_LABEL, &total, &used);

    if (ret == ESP_OK)
    {
        ESP_LOGI(TAG, "NANO: SPIFFS Partition size: total: %d, used: %d", total, used);
    }
}

//  Create config name in NVS from option/index
//  return name in configName
//
//  Return filename in format "/I/conf_MMMM_N"
//  MMMM it's the marker
//  N it's the config block index
void ConfigStorage_GetConfigFileName(
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex,
    char *configName)
{
    // buffer for config index
    // this is only working for single digit indexes
    // it's assumed that this validation has occurred in the caller
    char configIndexAsString[2] = {0};

    // config index to string
    itoa(configurationIndex, configIndexAsString, 10);

    // Add filesystem name
    strcat(configName, NANO_SPIFFS_BASE_PATH);

    // separator
    strcat(configName, "/");

    // Add base name
    strcat(configName, NANO_SPIFFS_CONFIG_NAME);

    // add configuration code
    switch (configuration)
    {
        case DeviceConfigurationOption_Network:
            strcat(configName, (const char *)c_MARKER_CONFIGURATION_NETWORK_V1);
            break;

        case DeviceConfigurationOption_Wireless80211Network:
            strcat(configName, (const char *)c_MARKER_CONFIGURATION_WIRELESS80211_V1);
            break;

        case DeviceConfigurationOption_WirelessNetworkAP:
            strcat(configName, (const char *)c_MARKER_CONFIGURATION_WIRELESS_AP_V1);
            break;

        case DeviceConfigurationOption_X509CaRootBundle:
            strcat(configName, (const char *)c_MARKER_CONFIGURATION_X509CAROOTBUNDLE_V1);
            break;

        case DeviceConfigurationOption_X509DeviceCertificates:
            strcat(configName, (const char *)c_MARKER_CONFIGURATION_X509DEVICECERTIFICATE_V1);
            break;

        default:
            // unknown device configuration
            // shouldn't EVER EVER get here
            ASSERT(false);
            break;
    }

    // separator
    strcat(configName, "_");

    // add configuration INDEX
    strcat(configName, configIndexAsString);
}

//
//  ConfigStorage_OpenFile -
//
//  Open file for configuration / index
//  For the case of NVS we just open the NVS partition, save file details, handle
//
// Parameters:-
//   configuration      : Type of configuration block to open
//   configurationIndex : Index of type to open
//   isWrite            : flag to signal writting to the file
//   isAppend           : flag to signal appending to the file
//
// Return :handle
//
FILE *ConfigStorage_OpenFile(
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex,
    bool isWrite,
    bool isAppend)
{
    // buffer for file name
    // add extra position for terminator
    char fileName[SPIFFS_OBJ_NAME_LEN + 1] = {0};

    ConfigStorage_GetConfigFileName(configuration, configurationIndex, fileName);

    // Open SPIFFS config storage
    FILE *file = fopen(fileName, isWrite ? (isAppend ? "a" : "w") : "r");

    if (file != NULL)
    {
        return file;
    }

    ESP_LOGE(TAG, "NANO: Failed to open file (%s)", fileName);

    return NULL;
}

//
//  ConfigStorage_DeleteFile -
//
//  Deletes configuration file
//
// Parameters:-
//   configuration      : Type of configuration block
//   configurationIndex : Index of config to delete
//
// Return : operation success
//
bool ConfigStorage_DeleteFile(DeviceConfigurationOption configuration, uint32_t configurationIndex)
{
    // buffer for file name
    // add extra position for terminator
    char fileName[SPIFFS_OBJ_NAME_LEN + 1] = {0};

    ConfigStorage_GetConfigFileName(configuration, configurationIndex, fileName);

    // Open SPIFFS config storage
    return (unlink(fileName) == 0);
}

//
//  ConfigStorage_CloseFile - Close opened file / NVS system
//
// Parameters:-
//   handle : Handle returned from ConfigStorage_OpenFile
//
// return:-
//    true - File closed ok
//    false- Error/File not open/Invalid handle
//
bool ConfigStorage_CloseFile(FILE *handle)
{
    fclose(handle);
    return true;
}

//
//  Shared IO
//
int32_t Config_IO(FILE *handle, ConfigIoType ioType, uint8_t *pData, int32_t size)
{
    FILE *file = handle;
    int32_t result = -1;
    size_t ret;
    long blobSize;

    while (true)
    {
        if (ioType == ConfigIoType_GetSize || ioType == ConfigIoType_Read)
        {
            // Get size of blob before we read it
            fseek(file, 0, SEEK_END);
            blobSize = ftell(file);

            result = (int32_t)blobSize;

            // If GetSize then finish
            if (ioType == ConfigIoType_GetSize)
            {
                break;
            }

            // Do read
            // Check room in buffer
            if ((int32_t)blobSize > size)
            {
                // error
                break;
            }

            // Read blob into pData
            fseek(file, 0, SEEK_SET);
            ret = fread(pData, 1, (size_t)blobSize, file);

            if (ret != (size_t)blobSize)
            {
                result = -1;
            }

            break;
        }
        else if (ioType == ConfigIoType_Write)
        {
            result = fwrite((const void *)pData, 1, (size_t)size, file);

            break;
        }
        else if (ioType == ConfigIoType_Append)
        {
            // get to the end of the file
            fseek(file, 0, SEEK_END);

            // append more data
            result = fwrite((const void *)pData, 1, (size_t)size, file);

            break;
        }
        else
        {
            break;
        }
    }

    return result;
}

//
//  ConfigStorage_FileSize - Return size of file
//
// Parameters:-
//   handle : Handle returned from ConfigStorage_OpenFile
//
// return:-
//    File size
//    -1 = Error/Not found/Invalid handle
//
int32_t ConfigStorage_FileSize(FILE *handle)
{
    return Config_IO(handle, ConfigIoType_GetSize, 0, 0);
}

//
//  ConfigStorage_WriteFile -  Write data to NVS system
//
// Parameters:-
//   handle    : Handle returned from ConfigStorage_OpenFile
//   pData     : Pointer data to write
//   writeSize : Size of data to write
//
// return:-
//    true - OK
//    false- Error/Not found
//
bool ConfigStorage_WriteFile(FILE *handle, uint8_t *pData, int32_t writeSize)
{
    return (Config_IO(handle, ConfigIoType_Write, pData, writeSize) == writeSize);
}

//
//  ConfigStorage_AppendFile -  Append data to NVS system
//
// Parameters:-
//   handle    : Handle returned from ConfigStorage_OpenFile
//   pData     : Pointer data to write
//   writeSize : Size of data to write
//
// return:-
//    true - OK
//    false- Error/Not found
//
bool ConfigStorage_AppendFile(FILE *handle, uint8_t *pData, int32_t writeSize)
{
    return (Config_IO(handle, ConfigIoType_Append, pData, writeSize) == writeSize);
}

//
// ConfigStorage_ReadFile - Read the data from file opened in NVS
//
// Parameters:-
//   handle : Handle returned from ConfigStorage_OpenFile
//   pData  : Ponter to where data is to be stored
//
// return:-
//    Read size
//    -1 = Error/Not found/Invalid handle
//
int32_t ConfigStorage_ReadFile(FILE *handle, uint8_t *pData, int32_t maxSizeData)
{
    return Config_IO(handle, ConfigIoType_Read, pData, maxSizeData);
}

HAL_CONFIGURATION_NETWORK *ConfigStorage_FindNetworkConfigurationBlocks()
{
    DIR *dir = opendir(NANO_SPIFFS_BASE_PATH);
    struct dirent *dirInfo = {0};

    uint32_t configCount = 0;

    // 1st pass, count the config files
    for (;;)
    {
        dirInfo = readdir(dir);

        // check for "no more files"
        if (!dirInfo)
        {
            break;
        }

        if (strstr(dirInfo->d_name, (const char *)c_MARKER_CONFIGURATION_NETWORK_V1))
        {
            configCount++;
        }
    }

    // allocate config struct
    // allocation size
    int32_t allocationSize = sizeof(HAL_CONFIGURATION_NETWORK);

    HAL_CONFIGURATION_NETWORK *networkConfigs = (HAL_CONFIGURATION_NETWORK *)platform_malloc(allocationSize);

    // check allocation
    if (networkConfigs == NULL)
    {
        return NULL;
    }

    // clear memory
    memset(networkConfigs, 0, allocationSize);

    // set collection count
    networkConfigs->Count = configCount;

    return networkConfigs;
}

HAL_CONFIGURATION_NETWORK_WIRELESS80211 *ConfigStorage_FindNetworkWireless80211ConfigurationBlocks()
{
    DIR *dir = opendir(NANO_SPIFFS_BASE_PATH);
    struct dirent *dirInfo = {0};

    uint32_t configCount = 0;

    // 1st pass, count the config files
    for (;;)
    {
        dirInfo = readdir(dir);

        // check for "no more files"
        if (!dirInfo)
        {
            break;
        }

        if (strstr(dirInfo->d_name, (const char *)c_MARKER_CONFIGURATION_WIRELESS80211_V1))
        {
            configCount++;
        }
    }

    // allocate config struct
    // allocation size
    int32_t allocationSize = sizeof(HAL_Configuration_Wireless80211 *);

    HAL_CONFIGURATION_NETWORK_WIRELESS80211 *networkWirelessConfigs =
        (HAL_CONFIGURATION_NETWORK_WIRELESS80211 *)platform_malloc(allocationSize);

    // check allocation
    if (networkWirelessConfigs == NULL)
    {
        return NULL;
    }

    // clear memory
    memset(networkWirelessConfigs, 0, allocationSize);

    // set collection count
    networkWirelessConfigs->Count = configCount;

    return networkWirelessConfigs;
}

HAL_CONFIGURATION_NETWORK_WIRELESSAP *ConfigStorage_FindNetworkWirelessAPConfigurationBlocks()
{
    DIR *dir = opendir(NANO_SPIFFS_BASE_PATH);
    struct dirent *dirInfo = {0};

    uint32_t configCount = 0;

    // 1st pass, count the config files
    for (;;)
    {
        dirInfo = readdir(dir);

        // check for "no more files"
        if (!dirInfo)
        {
            break;
        }

        if (strstr(dirInfo->d_name, (const char *)c_MARKER_CONFIGURATION_WIRELESS_AP_V1))
        {
            configCount++;
        }
    }

    // allocate config struct
    // allocation size
    int32_t allocationSize = sizeof(HAL_CONFIGURATION_NETWORK_WIRELESSAP);

    HAL_CONFIGURATION_NETWORK_WIRELESSAP *networkWirelessAPConfigs =
        (HAL_CONFIGURATION_NETWORK_WIRELESSAP *)platform_malloc(allocationSize);

    // check allocation
    if (networkWirelessAPConfigs == NULL)
    {
        return NULL;
    }

    // clear memory
    memset(networkWirelessAPConfigs, 0, allocationSize);

    // set collection count
    networkWirelessAPConfigs->Count = configCount;

    return networkWirelessAPConfigs;
}

HAL_CONFIGURATION_X509_CERTIFICATE *ConfigStorage_FindX509CertificateConfigurationBlocks()
{
    DIR *dir = opendir(NANO_SPIFFS_BASE_PATH);
    struct dirent *dirInfo = {0};

    uint32_t certCount = 0;

    // 1st pass, count the config files
    for (;;)
    {
        dirInfo = readdir(dir);

        // check for "no more files"
        if (!dirInfo)
        {
            break;
        }

        if (strstr(dirInfo->d_name, (const char *)c_MARKER_CONFIGURATION_X509CAROOTBUNDLE_V1))
        {
            certCount++;

            // there is only one CA root bundle, so we're good here
            break;
        }
    }

    // allocate config struct
    int32_t allocationSize = offsetof(HAL_CONFIGURATION_X509_CERTIFICATE, Certificates) +
                             certCount * sizeof(HAL_Configuration_X509CaRootBundle *);

    // allocate config struct
    HAL_CONFIGURATION_X509_CERTIFICATE *certificateStore =
        (HAL_CONFIGURATION_X509_CERTIFICATE *)platform_malloc(allocationSize);

    // check allocation
    if (certificateStore == NULL)
    {
        return NULL;
    }

    // clear memory
    memset(certificateStore, 0, allocationSize);

    // set collection count
    certificateStore->Count = certCount;

    if (certCount > 0)
    {
        // allocate cert struct
        HAL_Configuration_X509CaRootBundle *certificate =
            (HAL_Configuration_X509CaRootBundle *)platform_malloc(sizeof(HAL_Configuration_X509CaRootBundle));

        // check allocation
        if (certificate == NULL)
        {
            return NULL;
        }

        // clear memory
        memset(certificate, 0, sizeof(HAL_Configuration_X509CaRootBundle));

        // make sure the config block marker is set
        memcpy(
            certificate,
            c_MARKER_CONFIGURATION_X509CAROOTBUNDLE_V1,
            sizeof(c_MARKER_CONFIGURATION_X509CAROOTBUNDLE_V1));

        // get size of certificate
        certificate->CertificateSize =
            ConfigurationManager_GetConfigurationBlockSize(DeviceConfigurationOption_X509CaRootBundle, 0);

        certificateStore->Certificates[0] = certificate;
    }

    return certificateStore;
}

HAL_CONFIGURATION_X509_DEVICE_CERTIFICATE *IRAM_ATTR ConfigStorage_FindX509DeviceCertificatesConfigurationBlocks()
{
    DIR *dir = opendir(NANO_SPIFFS_BASE_PATH);
    struct dirent *dirInfo = {0};

    uint32_t certCount = 0;

    // 1st pass, count the config files
    for (;;)
    {
        dirInfo = readdir(dir);

        // check for "no more files"
        if (!dirInfo)
        {
            break;
        }

        if (strstr(dirInfo->d_name, (const char *)c_MARKER_CONFIGURATION_X509DEVICECERTIFICATE_V1))
        {
            certCount++;

            // there is only one device cert, so we're good here
            break;
        }
    }

    // allocate config struct
    int32_t allocationSize = offsetof(HAL_CONFIGURATION_X509_DEVICE_CERTIFICATE, Certificates) +
                             certCount * sizeof(HAL_Configuration_X509DeviceCertificate *);

    // allocate config struct
    HAL_CONFIGURATION_X509_DEVICE_CERTIFICATE *deviceCertificate =
        (HAL_CONFIGURATION_X509_DEVICE_CERTIFICATE *)platform_malloc(allocationSize);

    // check allocation
    if (deviceCertificate == NULL)
    {
        return NULL;
    }

    // clear memory
    memset(deviceCertificate, 0, allocationSize);

    // set collection count
    deviceCertificate->Count = certCount;

    if (certCount > 0)
    {
        // allocate cert struct
        HAL_Configuration_X509DeviceCertificate *certificate =
            (HAL_Configuration_X509DeviceCertificate *)platform_malloc(sizeof(HAL_Configuration_X509DeviceCertificate));

        // check allocation
        if (certificate == NULL)
        {
            return NULL;
        }

        // clear memory
        memset(certificate, 0, sizeof(HAL_Configuration_X509DeviceCertificate));

        // make sure the config block marker is set
        memcpy(
            certificate,
            c_MARKER_CONFIGURATION_X509DEVICECERTIFICATE_V1,
            sizeof(c_MARKER_CONFIGURATION_X509DEVICECERTIFICATE_V1));

        // get size of certificate
        certificate->CertificateSize =
            ConfigurationManager_GetConfigurationBlockSize(DeviceConfigurationOption_X509DeviceCertificates, 0);

        deviceCertificate->Certificates[0] = certificate;
    }

    return deviceCertificate;
}

bool ConfigurationManager_GetConfigurationBlockFromStorage(
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex,
    uint8_t *configurationBlock,
    int32_t maxBlockSize)
{
    FILE *handle;
    int32_t readSize = 0;

#ifdef DEBUG_CONFIG
    ets_printf("GetConfigFromStorage %d, %d\n", (int)configuration, configurationIndex);
#endif

    handle = ConfigStorage_OpenFile(configuration, configurationIndex, false, false);

    if (handle != NULL)
    {
        readSize = ConfigStorage_ReadFile(handle, configurationBlock, maxBlockSize);

#ifdef DEBUG_CONFIG
        ets_printf("GetConfigFromStorage read %d, %d size %d\n", (int)configuration, configurationIndex, readSize);
#endif

        ConfigStorage_CloseFile(handle);
    }

    return (readSize > 0);
}

int32_t ConfigurationManager_GetConfigurationBlockSize(
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex)
{
    FILE *handle;
    int32_t readSize = 0;

    handle = ConfigStorage_OpenFile(configuration, configurationIndex, false, false);

    if (handle != NULL)
    {
        readSize = ConfigStorage_FileSize(handle);

        ConfigStorage_CloseFile(handle);
    }

    return readSize;
}