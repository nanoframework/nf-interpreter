//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// make sure to include this only for nanoCLR
#ifdef I_AM_NANOCLR

#include <nanoHAL.h>
#include <nanoHAL_v2.h>
#include <nanoWeak.h>
#include <nanoCLR_Types.h>
#include <target_platform.h>
#include <target_os.h>
#include <targetHAL_StorageOperation.h>
#include <hal_spiffs.h>

#if HAS_ACCESSIBLE_STORAGE

#if defined(NF_FEATURE_USE_SPIFFS) && (NF_FEATURE_USE_SPIFFS == TRUE)

uint32_t HAL_StorageOperation(
    uint8_t operation,
    uint32_t nameLength,
    uint32_t dataLength,
    uint32_t offset,
    uint8_t *data)
{
    (void)offset;

    spiffs_file *spiffsFile = NULL;
    spiffs *driveFs = NULL;
    // default to drive 0
    int32_t driveIndex = 0;
    StorageOperationErrorCode errorCode = StorageOperationErrorCode::NoError;

    // check if the name is not empty
    if (nameLength == 0)
    {
        return StorageOperationErrorCode::WriteError;
    }

    // convert storageName to char*
    char *storageName = (char *)platform_malloc(nameLength + 1);

    // sanity check for successfull malloc
    if (storageName == NULL)
    {
        return StorageOperationErrorCode::PlatformError;
    }

    // go and copy the name
    memcpy(storageName, data, nameLength);

    // Just making sure it's properly 0 terminated
    storageName[nameLength] = '\0';

    //... and pointer to the SPIFFS instance
    driveFs = hal_spiffs_get_fs_from_index(driveIndex);

    if (operation == StorageOperation_Monitor::StorageOperation_Write)
    {
        // Open the file in read mode
        *spiffsFile = SPIFFS_open(driveFs, storageName, SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR, 0);
        if (*spiffsFile < 0)
        {
            errorCode = StorageOperationErrorCode::WriteError;

            goto done;
        }

        if (SPIFFS_write(
                driveFs,
                *spiffsFile,
                (data + nameLength),
                dataLength) < 0)
        {
            // failed to write expected number of bytes
            errorCode = StorageOperationErrorCode::WriteError;
        }

        // close file
        SPIFFS_close(driveFs, *spiffsFile);
    }
    else if (operation == StorageOperation_Monitor::StorageOperation_Append)
    {
        // Open the file in apped mode
        *spiffsFile = SPIFFS_open(driveFs, storageName, SPIFFS_APPEND | SPIFFS_RDWR, 0);
        if (*spiffsFile < 0)
        {
            errorCode = StorageOperationErrorCode::WriteError;

            goto done;
        }

        // append more data
        if (SPIFFS_write(
                driveFs,
                *spiffsFile,
                (data + nameLength),
                dataLength) < 0)
        {
            // failed to write expected number of bytes
            errorCode = StorageOperationErrorCode::WriteError;
        }

        // close file
        SPIFFS_close(driveFs, *spiffsFile);
    }
    else if (operation == StorageOperation_Monitor::StorageOperation_Delete)
    {
        // remove the file
        if (SPIFFS_remove(driveFs, storageName) < 0)
        {
            errorCode = StorageOperationErrorCode::DeleteError;
        }
    }

done:

    // free buffer memory
    if (storageName != NULL)
    {
        platform_free(storageName);
    }

    return errorCode;
}

#endif // HAL_USE_SPIFFS
#endif // HAS_ACCESSIBLE_STORAGE
#endif // I_AM_NANOCLR
