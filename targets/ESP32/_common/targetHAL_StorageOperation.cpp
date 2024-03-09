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

void HAL_StorageOperation(uint8_t operation, uint32_t *length, uint8_t *storageName, uint8_t *data, uint32_t *errorCode)
{
    EnsureStorageInitialized();

    // reset error code
    *errorCode = (uint32_t)(StorageOperationErrorCodes::NoError);
    size_t result;

    // convert storageName to char*
    char *storageNameChar = ConvertToVfsPath(reinterpret_cast<char *>(storageName));

    CLR_Debug::Printf("StorageOperation: %s", storageNameChar);

    if (operation == (uint8_t)(StorageOperation_Monitor::StorageOperation_Write))
    {
        // Open the file in read mode
        FILE *file = fopen(storageNameChar, "w");

        // append more data
        result = fwrite((const void *)data, 1, (size_t)length, file);
        fclose(file);

        if (result != (size_t)length)
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