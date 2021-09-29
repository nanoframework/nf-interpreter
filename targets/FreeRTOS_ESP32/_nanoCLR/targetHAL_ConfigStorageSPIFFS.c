//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// #include <nanoHAL.h>
// #include <nanoWeak.h>
#include "targetHAL_ConfigStorage.h"

// ESP32 SPIFFS

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
    ConfigIoType_GetSize
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
    char *startName = configName;
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
//
// Return :handle
//
FILE *ConfigStorage_OpenFile(DeviceConfigurationOption configuration, uint32_t configurationIndex, bool write)
{
    // buffer for file name
    // add extra position for terminator
    char fileName[SPIFFS_OBJ_NAME_LEN + 1] = {0};

    ConfigStorage_GetConfigFileName(configuration, configurationIndex, fileName);

    // Open SPIFFS config storage
    FILE *file = fopen(fileName, write ? "wb" : "rb");

    if (file != NULL)
    {
        return file;
    }

    ESP_LOGE(TAG, "NANO: Failed to open file (%s)", fileName);

    return NULL;
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
uint32_t Config_IO(FILE *handle, ConfigIoType ioType, uint8_t *pData, int32_t size)
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
            ret = fread((void *)pData, 1, (size_t)blobSize, file);

            if (ret != (size_t)blobSize)
            {
                result = -1;
            }

            break;
        }
        else if (ioType == ConfigIoType_Write)
        {
            ret = fwrite((const void *)pData, 1, (size_t)size, file);

            if (ret == (size_t)size)
            {
                result = 0;
            }
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
    return (Config_IO(handle, ConfigIoType_Write, pData, writeSize) != 0);
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
    int32_t allocationSize =
        offsetof(HAL_CONFIGURATION_NETWORK, Configs) + configCount * sizeof(HAL_Configuration_NetworkInterface);

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

    if (configCount > 0)
    {
        // second pass: load each config from storage
        for (uint32_t configIndex = 0; configIndex < configCount; configIndex++)
        {
            // allocate memory for reading the file
            networkConfigs->Configs[configIndex] =
                (HAL_Configuration_NetworkInterface *)platform_malloc(sizeof(HAL_Configuration_NetworkInterface));

            ConfigurationManager_GetConfigurationBlockFromStorage(
                DeviceConfigurationOption_Network,
                configIndex,
                networkConfigs->Configs[configIndex],
                sizeof(HAL_Configuration_NetworkInterface));
        }
    }

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
    int32_t allocationSize = offsetof(HAL_CONFIGURATION_NETWORK_WIRELESS80211, Configs) +
                             configCount * sizeof(HAL_Configuration_Wireless80211);

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

    if (configCount > 0)
    {
        // second pass: load each config from storage
        for (uint32_t configIndex = 0; configIndex < configCount; configIndex++)
        {
            // allocate memory for reading the file
            networkWirelessConfigs->Configs[configIndex] =
                (HAL_Configuration_Wireless80211 *)platform_malloc(sizeof(HAL_Configuration_Wireless80211));

            ConfigurationManager_GetConfigurationBlockFromStorage(
                DeviceConfigurationOption_Wireless80211Network,
                configIndex,
                networkWirelessConfigs->Configs[configIndex],
                sizeof(HAL_Configuration_Wireless80211));
        }
    }

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
    int32_t allocationSize =
        offsetof(HAL_CONFIGURATION_NETWORK_WIRELESSAP, Configs) + configCount * sizeof(HAL_Configuration_WirelessAP);

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

    if (configCount > 0)
    {
        // second pass: load each config from storage
        for (uint32_t configIndex = 0; configIndex < configCount; configIndex++)
        {
            // allocate memory for reading the file
            networkWirelessAPConfigs->Configs[configIndex] =
                (HAL_Configuration_WirelessAP *)platform_malloc(sizeof(HAL_Configuration_WirelessAP));

            ConfigurationManager_GetConfigurationBlockFromStorage(
                DeviceConfigurationOption_WirelessNetworkAP,
                configIndex,
                networkWirelessAPConfigs->Configs[configIndex],
                sizeof(HAL_Configuration_WirelessAP));
        }
    }

    return networkWirelessAPConfigs;
}

HAL_CONFIGURATION_X509_CERTIFICATE *ConfigStorage_FindX509CertificateConfigurationBlocks()
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

        if (strstr(dirInfo->d_name, (const char *)c_MARKER_CONFIGURATION_X509CAROOTBUNDLE_V1))
        {
            configCount++;

            // there is only one CA root bundle, so we're good here
            break;
        }
    }

    // allocate config struct
    int32_t allocationSize = sizeof(HAL_Configuration_X509CaRootBundle);

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
    certificateStore->Count = configCount;

    if (configCount > 0)
    {
        // allocate memory for reading the file
        certificateStore->Certificates[0] =
            (HAL_Configuration_X509CaRootBundle *)platform_malloc(sizeof(HAL_Configuration_X509CaRootBundle));

        // second pass: load CA root bundle from storage
        ConfigurationManager_GetConfigurationBlockFromStorage(
            DeviceConfigurationOption_X509CaRootBundle,
            0,
            certificateStore->Certificates[0],
            sizeof(HAL_Configuration_X509CaRootBundle));
    }

    return certificateStore;
}

HAL_CONFIGURATION_X509_DEVICE_CERTIFICATE *ConfigStorage_FindX509DeviceCertificatesConfigurationBlocks()
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

        if (strstr(dirInfo->d_name, (const char *)c_MARKER_CONFIGURATION_X509DEVICECERTIFICATE_V1))
        {
            configCount++;

            // there is only one device cert, so we're good here
            break;
        }
    }

    // allocate config struct
    int32_t allocationSize = sizeof(HAL_Configuration_X509DeviceCertificate);

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
    deviceCertificate->Count = configCount;

    if (configCount > 0)
    {
        // allocate memory for reading the file
        deviceCertificate->Certificates[0] =
            (HAL_Configuration_X509DeviceCertificate *)platform_malloc(sizeof(HAL_Configuration_X509DeviceCertificate));

        // second pass: load device cert from storage
        ConfigurationManager_GetConfigurationBlockFromStorage(
            DeviceConfigurationOption_X509DeviceCertificates,
            0,
            deviceCertificate->Certificates[0],
            sizeof(HAL_Configuration_X509DeviceCertificate));
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
    uint32_t readSize;

#ifdef DEBUG_CONFIG
    ets_printf("GetConfigFromStorage %d, %d\n", (int)configuration, configurationIndex);
#endif

    handle = ConfigStorage_OpenFile(configuration, configurationIndex, false);

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
