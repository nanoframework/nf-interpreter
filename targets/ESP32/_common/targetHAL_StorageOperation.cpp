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

// Function to create all necessary intermediate directories
int create_directories(const char *path)
{
    char temp[256];
    char *pos = NULL;
    size_t len = 0;

    len = hal_strlen_s(path);

    snprintf(temp, sizeof(temp), "%s", path);
    if (temp[len - 1] == '/')
    {
        temp[len - 1] = 0;
    }

    for (pos = temp + 1; *pos; pos++)
    {
        if (*pos == '/')
        {
            *pos = 0;
            if (mkdir(temp, 0700) && errno != EEXIST)
            {
                return -1;
            }
            *pos = '/';
        }
    }

    if (mkdir(temp, 0700) && errno != EEXIST)
    {
        return -1;
    }

    return 0;
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

        // Extract directory path from file path
        char dir_path[256];
        snprintf(dir_path, sizeof(dir_path), "%s", storageNameChar);
        char *last_slash = strrchr(dir_path, '/');
        if (last_slash != NULL)
        {
            *last_slash = '\0';
        }
        else
        {
            // No directory part, file is in the root
            dir_path[0] = '\0';
        }

        // Check if there is only one slash in the path
        if (strchr(dir_path, '/') == strrchr(dir_path, '/'))
        {
            // Only one slash, skip creating the directory
            dir_path[0] = '\0';
        }

        // Create all necessary intermediate directories
        if (dir_path[0] != '\0' && create_directories(dir_path) != 0)
        {
            errorCode = StorageOperationErrorCode::WriteError;
        }

        if (errorCode != StorageOperationErrorCode::WriteError)
        {
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
