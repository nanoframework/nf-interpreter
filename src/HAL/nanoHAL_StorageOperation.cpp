//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoHAL_v2.h>
#include <nanoWeak.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Types.h>
#include <nanoPAL_FileSystem.h>
#include <nanoCLR_FileStream.h>
#include <nanoHAL_StorageOperation.h>
#include <WireProtocol_MonitorCommands.h>

#if CONFIG_NF_FEATURE_HAS_ACCESSIBLE_STORAGE

uint32_t HAL_StorageOperation(uint8_t operation, uint32_t dataLength, uint32_t offset, uint8_t *data)
{
    (void)offset;

    char *storageName = NULL;
    char *rootName = NULL;
    char *relativePath = NULL;
    uint8_t *fileData = NULL;
    uint32_t rootNameLength = 0;
    FileSystemVolume *volume = NULL;
    StorageOperationErrorCode errorCode = StorageOperationErrorCode::NoError;

    // point to the storage name in the data buffer
    storageName = (char *)data;

    // split the path into root name and relative path
    if (FAILED(CLR_RT_FileStream::SplitFilePath(storageName, rootName, rootNameLength, relativePath)))
    {
        errorCode = StorageOperationErrorCode::PlatformError;
        goto done;
    }

    // find the volume for this path
    volume = FileSystemVolumeList::FindVolume(rootName, rootNameLength);

    if (volume == NULL)
    {
        errorCode = StorageOperationErrorCode::PlatformError;
        goto done;
    }

    // point to the file data in the data buffer (after the storage name and null terminator)
    fileData = (uint8_t *)(data + hal_strlen_s(storageName) + 1);

    if (operation == StorageOperation_Monitor::StorageOperation_Write)
    {
        void *fileHandle = NULL;
        char dirPath[FS_MAX_DIRECTORY_LENGTH];
        char *lastSeparator;
        int64_t position;
        int bytesWritten = 0;

        // extract parent directory from relative path and create it if needed
        snprintf(dirPath, sizeof(dirPath), "%s", relativePath);

        lastSeparator = strrchr(dirPath, '\\');
        if (lastSeparator == NULL)
        {
            lastSeparator = strrchr(dirPath, '/');
        }

        if (lastSeparator != NULL && lastSeparator != dirPath)
        {
            *lastSeparator = '\0';

            if (FAILED(volume->CreateDirectory(dirPath)))
            {
                errorCode = StorageOperationErrorCode::WriteError;
                goto done;
            }
        }

        // open the file (creates it, if it doesn't exist)
        if (FAILED(volume->Open(relativePath, fileHandle)))
        {
            errorCode = StorageOperationErrorCode::WriteError;
            goto done;
        }

        // truncate and seek to the beginning, for a full write
        volume->SetLength(fileHandle, 0);
        volume->Seek(fileHandle, 0, SEEKORIGIN_BEGIN, &position);

        // write the data
        volume->Write(fileHandle, fileData, (int)dataLength, &bytesWritten);

        // close file
        volume->Close(fileHandle);

        if ((uint32_t)bytesWritten != dataLength)
        {
            // failed to write expected number of bytes
            errorCode = StorageOperationErrorCode::WriteError;
        }
    }
    else if (operation == StorageOperation_Monitor::StorageOperation_Append)
    {
        void *fileHandle = NULL;
        int64_t position;
        int bytesWritten = 0;

        // open the file
        if (FAILED(volume->Open(relativePath, fileHandle)))
        {
            errorCode = StorageOperationErrorCode::WriteError;
            goto done;
        }

        // seek to the end, to append
        volume->Seek(fileHandle, 0, SEEKORIGIN_END, &position);

        // append the data
        volume->Write(fileHandle, fileData, (int)dataLength, &bytesWritten);

        // close file
        volume->Close(fileHandle);

        if ((uint32_t)bytesWritten != dataLength)
        {
            // failed to write expected number of bytes
            errorCode = StorageOperationErrorCode::WriteError;
        }
    }
    else if (operation == StorageOperation_Monitor::StorageOperation_Delete)
    {
        // remove the file
        if (FAILED(volume->Delete(relativePath, false)))
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

#endif // CONFIG_NF_FEATURE_HAS_ACCESSIBLE_STORAGE
