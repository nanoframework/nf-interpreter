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

#if HAS_ACCESSIBLE_STORAGE

#if defined(NF_FEATURE_USE_LITTLEFS) && (NF_FEATURE_USE_LITTLEFS == TRUE)

#include <hal_littlefs.h>

uint32_t HAL_StorageOperation(
    uint8_t operation,
    uint32_t nameLength,
    uint32_t dataLength,
    uint32_t offset,
    uint8_t *data)
{
    (void)offset;

    lfs_file_t lfsFile;
    lfs_t *lfsDrive = NULL;

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

    //... and pointer to the littlefs instance
    lfsDrive = hal_lfs_get_fs_from_index(driveIndex);

    if (operation == StorageOperation_Monitor::StorageOperation_Write)
    {
        memset(&lfsFile, 0, sizeof(lfsFile));

        // Open/create the file in read mode
        if (lfs_file_open(lfsDrive, &lfsFile, storageName, LFS_O_RDWR | LFS_O_CREAT) != LFS_ERR_OK)
        {
            errorCode = StorageOperationErrorCode::WriteError;

            goto done;
        }

        if (lfs_file_write(lfsDrive, &lfsFile, (data + nameLength), dataLength) != (lfs_ssize_t)dataLength)
        {
            // failed to write expected number of bytes
            errorCode = StorageOperationErrorCode::WriteError;
        }

        // close file
        lfs_file_close(lfsDrive, &lfsFile);
    }
    else if (operation == StorageOperation_Monitor::StorageOperation_Append)
    {
        // Open/create the file in read mode
        if (lfs_file_open(lfsDrive, &lfsFile, storageName, LFS_O_RDWR | LFS_O_APPEND) != LFS_ERR_OK)
        {
            errorCode = StorageOperationErrorCode::WriteError;

            goto done;
        }

        // append more data
        if (lfs_file_write(lfsDrive, &lfsFile, (data + nameLength), dataLength) != (lfs_ssize_t)dataLength)
        {
            // failed to write expected number of bytes
            errorCode = StorageOperationErrorCode::WriteError;
        }

        // close file
        lfs_file_close(lfsDrive, &lfsFile);
    }
    else if (operation == StorageOperation_Monitor::StorageOperation_Delete)
    {
        // remove the file
        if (lfs_remove(lfsDrive, storageName) != LFS_ERR_OK)
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

#endif // NF_FEATURE_USE_LITTLEFS

#endif // HAS_ACCESSIBLE_STORAGE
#endif // I_AM_NANOCLR
