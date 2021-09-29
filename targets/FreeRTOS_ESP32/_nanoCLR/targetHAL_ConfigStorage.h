//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_v2.h>
#include <esp32_idf.h>

#ifdef __cplusplus
extern "C"
{
#endif

    // ConfigStorage_Initialise
    //
    // Initialise underlying filesystem
    //
    void ConfigStorage_Initialise();

    //
    //  ConfigStorage_OpenFile -
    //
    //  Open file for configuration / index
    //  For the case of NVS we just open the NVS partition, save file details, handle
    //
    // Parameters:-
    //   configuration      : Type of configuration block to open
    //   configurationIndex : Index of type to open
    //   write              : True to create/overwrite file
    //
    // Return :-
    //    true  : returns handle
    //    false -1
    //
    FILE *ConfigStorage_OpenFile(DeviceConfigurationOption configuration, uint32_t configurationIndex, bool write);

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
    bool ConfigStorage_CloseFile(FILE *handle);

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
    int32_t ConfigStorage_FileSize(FILE *handle);

    //
    //  ConfigStorage_WriteFile -  Write data to file system
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
    bool ConfigStorage_WriteFile(FILE *handle, uint8_t *pData, int32_t writeSize);

    //
    // ConfigStorage_ReadFile - Read the data from opened file
    //
    // Parameters:-
    //   handle : Handle returned from ConfigStorage_OpenFile
    //   pData  : Ponter to where data is to be stored
    //
    // return:-
    //    true - read length
    //    false- -1 , Error/Not found/Invalid handle
    //
    int32_t ConfigStorage_ReadFile(FILE *handle, uint8_t *pData, int32_t maxSizeData);

    bool ConfigurationManager_GetConfigurationBlockFromStorage(
        DeviceConfigurationOption configuration,
        uint32_t configurationIndex,
        uint8_t *configurationBlock,
        int32_t maxBlockSize);

    HAL_CONFIGURATION_NETWORK *ConfigStorage_FindNetworkConfigurationBlocks();
    HAL_CONFIGURATION_NETWORK_WIRELESS80211 *ConfigStorage_FindNetworkWireless80211ConfigurationBlocks();
    HAL_CONFIGURATION_NETWORK_WIRELESSAP *ConfigStorage_FindNetworkWirelessAPConfigurationBlocks();
    HAL_CONFIGURATION_X509_CERTIFICATE *ConfigStorage_FindX509CertificateConfigurationBlocks();
    HAL_CONFIGURATION_X509_DEVICE_CERTIFICATE *ConfigStorage_FindX509DeviceCertificatesConfigurationBlocks();

#ifdef __cplusplus
}
#endif
