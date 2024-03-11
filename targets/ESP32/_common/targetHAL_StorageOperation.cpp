//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoHAL_v2.h>
#include <nanoWeak.h>
#include <nanoCLR_Types.h>
#include "esp32_idf.h"
#include <target_platform.h>
#include <targetHAL_StorageOperation.h>
#include <targetHAL_FileOperation.h>

// Ensure Storage is initialized
void EnsureStorageInitialized()
{
    if (!esp_spiffs_mounted(SPIFFS_PARTITION_LABEL))
    {
        ConfigStorage_Initialise();
    }
}

void HAL_StorageOperation(uint8_t operation, uint32_t nameLength, uint32_t dataLength, uint8_t *data, uint32_t *errorCode)
{    
    EnsureStorageInitialized();

    // reset error code
    *errorCode = (uint32_t)(StorageOperationErrorCodes::NoError);
    size_t result;

    // convert storageName to char*
    char* strName = (char *)platform_malloc(nameLength + 1);
    for(uint32_t i = 0; i < nameLength; i++)
    {
        strName[i] = static_cast<char>(data[i]);
    }

    // Just making sure it's properly 0 terminated
    strName[nameLength] = '\0';

    char *storageNameChar = ConvertToVfsPath(strName);

    // Cleaning the temporary name buffer
    platform_free(strName);

    if (operation == (uint8_t)(StorageOperation_Monitor::StorageOperation_Write))
    {
        // Remove the file if already exists
        remove(storageNameChar);

        // Open the file in read mode
        FILE *file = fopen(storageNameChar, "w");

        // append more data
        result = fwrite((const void *)(data + nameLength), 1, (size_t)dataLength, file);
        fclose(file);

        if (result != (size_t)dataLength)
        {
            *errorCode = (uint32_t)(StorageOperationErrorCodes::WriteError);
        }
    }
    else if(operation == (uint8_t)(StorageOperation_Monitor::StorageOperation_Append))
    {
        // Open the file in read mode
        FILE *file = fopen(storageNameChar, "a");
        fseek(file, 0, SEEK_END);
        // append more data
        result = fwrite((const void *)(data + nameLength), 1, (size_t)dataLength, file);
        fclose(file);

        if (result != (size_t)dataLength)
        {
            *errorCode = (uint32_t)(StorageOperationErrorCodes::WriteError);
        }
    }
    else if (operation == (uint8_t)(StorageOperation_Monitor::StorageOperation_Delete))
    {
        result = remove(storageNameChar);
        if (result != 0)
        {
            *errorCode = (uint32_t)(StorageOperationErrorCodes::DeleteError);
        }
    }

    if (storageNameChar != NULL)
    {
        platform_free(storageNameChar);
    }
}