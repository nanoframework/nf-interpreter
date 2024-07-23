//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoHAL_v2.h>
#include <nanoWeak.h>
#include <nanoCLR_Types.h>
#include "esp32_idf.h"
#include <esp_littlefs.h>
#include <target_platform.h>
#include <targetHAL_StorageOperation.h>
#include <targetHAL_FileOperation.h>

// Ensure Storage is initialized
void EnsureStorageInitialized()
{
    if (!esp_littlefs_mounted(LITTLEFS_PARTITION_LABEL))
    {
        ConfigStorage_Initialise();
    }
}

uint32_t HAL_StorageOperation(
    uint8_t operation,
    uint32_t nameLength,
    uint32_t dataLength,
    uint32_t offset,
    uint8_t *data)
{
    (void)offset;

    size_t result;
    StorageOperationErrorCode errorCode = StorageOperationErrorCode::NoError;

    EnsureStorageInitialized();

    // check if the name is not empty
    if (nameLength == 0)
    {
        return StorageOperationErrorCode::WriteError;
    }

    // convert storageName to char*
    char *strName = (char *)platform_malloc(nameLength + 1);

    // sanity check for successfull malloc
    if (strName == NULL)
    {
        return StorageOperationErrorCode::PlatformError;
    }

    // go and copy the name
    memcpy(strName, data, nameLength);

    // Just making sure it's properly 0 terminated
    strName[nameLength] = '\0';

    // !! the returned char* needs to be freed !!
    char *storageNameChar = ConvertToVfsPath(strName);

    // Cleaning the temporary name buffer
    platform_free(strName);

    if (operation == StorageOperation_Monitor::StorageOperation_Write)
    {
        // Remove the file if already exists
        remove(storageNameChar);

        // Open the file in read mode
        FILE *file = fopen(storageNameChar, "w");

        // append more data
        result = fwrite((const void *)(data + nameLength), 1, (size_t)dataLength, file);
        fclose(file);

        // check if the data was written
        if (result != (size_t)dataLength)
        {
            errorCode = StorageOperationErrorCode::WriteError;
        }
    }
    else if (operation == StorageOperation_Monitor::StorageOperation_Append)
    {
        // Open the file in read mode
        FILE *file = fopen(storageNameChar, "a");
        fseek(file, 0, SEEK_END);

        // append more data
        result = fwrite((const void *)(data + nameLength), 1, (size_t)dataLength, file);
        fclose(file);

        // check if the data was written
        if (result != (size_t)dataLength)
        {
            errorCode = StorageOperationErrorCode::WriteError;
        }
    }
    else if (operation == StorageOperation_Monitor::StorageOperation_Delete)
    {
        result = remove(storageNameChar);

        // check if the file was deleted
        if (result != 0)
        {
            errorCode = StorageOperationErrorCode::DeleteError;
        }
    }

    // free buffer memory
    if (storageNameChar != NULL)
    {
        platform_free(storageNameChar);
    }

    return errorCode;
}
