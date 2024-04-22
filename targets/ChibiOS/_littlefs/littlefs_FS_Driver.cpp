//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include <hal.h>
// #include <ch.h>
// #include <nanoCLR_Headers.h>
#include <target_littlefs.h>
#include <nanoHAL_v2.h>
#include <nanoPAL_FileSystem.h>
#include <nf_sys_io_filesystem.h>
#include "littlefs_FS_Driver.h"
#include "hal_littlefs.h"

bool LITTLEFS_FS_Driver::LoadMedia(const void *driverInterface)
{
    (void)driverInterface;
    return TRUE;
}

STREAM_DRIVER_DETAILS *LITTLEFS_FS_Driver::DriverDetails(const VOLUME_ID *volume)
{
    (void)volume;

    static STREAM_DRIVER_DETAILS driverDetail = {SYSTEM_BUFFERED_IO, NULL, NULL, 0, 0, TRUE, TRUE, TRUE, 0, 0};

    return &driverDetail;
}

//--//

void LITTLEFS_FS_Driver::Initialize()
{
    // nothing to do here as the littlefs instances are already initialized in the target
}

bool LITTLEFS_FS_Driver::InitializeVolume(const VOLUME_ID *volume)
{
    return hal_lfs_mount(volume->volumeId) == LFS_ERR_OK ? TRUE : FALSE;
}

bool LITTLEFS_FS_Driver::UnInitializeVolume(const VOLUME_ID *volume)
{
    return lfs_unmount(hal_lfs_get_fs_from_index(volume->volumeId)) == LFS_ERR_OK ? TRUE : FALSE;
}

HRESULT LITTLEFS_FS_Driver::Format(const VOLUME_ID *volume, const char *volumeLabel, uint32_t parameters)
{
    (void)volume;
    (void)volumeLabel;
    (void)parameters;

    // ::Watchdog_GetSetEnabled(FALSE, TRUE);

    // HRESULT hr = FAT_LogicDisk::Format(volume, volumeLabel, parameters);

    // ::Watchdog_GetSetEnabled(TRUE, TRUE);
    ASSERT(FALSE);
    return S_FALSE; // hr
}

HRESULT LITTLEFS_FS_Driver::GetSizeInfo(const VOLUME_ID *volume, int64_t *totalSize, int64_t *totalFreeSpace)
{
    (void)volume;
    (void)totalSize;
    (void)totalFreeSpace;

    // FAT_LogicDisk *logicDisk = FAT_MemoryManager::GetLogicDisk(volume);

    // if (logicDisk)
    // {
    //     *totalSize = (int64_t)logicDisk->GetDiskTotalSize();
    //     *totalFreeSpace = (int64_t)logicDisk->GetDiskFreeSize();

    //     return S_OK;
    // }
    ASSERT(FALSE);
    return CLR_E_INVALID_DRIVER;
}

HRESULT LITTLEFS_FS_Driver::FlushAll(const VOLUME_ID *volume)
{
    (void)volume;
    // FAT_LogicDisk *logicDisk = FAT_MemoryManager::GetLogicDisk(volume);

    // if (logicDisk)
    // {
    //     logicDisk->SectorCache.FlushAll();

    //     return S_OK;
    // }
    ASSERT(FALSE);
    return CLR_E_INVALID_DRIVER;
}

HRESULT LITTLEFS_FS_Driver::GetVolumeLabel(const VOLUME_ID *volume, const char *volumeLabel, int32_t volumeLabelLen)
{
    (void)volume;
    (void)volumeLabel;
    (void)volumeLabelLen;

    // FAT_LogicDisk *logicDisk = FAT_MemoryManager::GetLogicDisk(volume);

    // if (logicDisk)
    // {
    //     if (volumeLabelLen < FAT_Directory::DIR_Name__size)
    //     {
    //         return CLR_E_BUFFER_TOO_SMALL;
    //     }

    //     return logicDisk->GetDiskVolLab(volumeLabel);
    // }
    ASSERT(FALSE);
    return CLR_E_INVALID_DRIVER;
}

//--//

HRESULT LITTLEFS_FS_Driver::Open(const VOLUME_ID *volume, const char *path, uint32_t *handle)
{
    NANOCLR_HEADER();

#ifdef DEBUG
    int32_t result;
#endif

    LITTLEFS_FileHandle *fileHandle = NULL;
    lfs_info info;
    int32_t flags;
    bool fileExists = false;

    // allocate file handle
    fileHandle = (LITTLEFS_FileHandle *)platform_malloc(sizeof(LITTLEFS_FileHandle));

    if (fileHandle == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    memset(fileHandle, 0, sizeof(LITTLEFS_FileHandle));

    // get littlefs instance
    fileHandle->fs = hal_lfs_get_fs_from_index(volume->volumeId);

    if (!fileHandle->fs)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
    }

    // adjust path to remove leading backslash
    if (*path == '\\')
    {
        path++;
    }

    // check for file existence
#ifdef DEBUG
    result = lfs_stat(fileHandle->fs, path, &info);
    // ASSERT(result == LFS_ERR_CORRUPT);
    if (result == LFS_ERR_CORRUPT)
    {
        __NOP();
    }
#else
    fileExists = lfs_stat(fileHandle->fs, path, &info) == LFS_ERR_OK;
#endif

    // setup attributes stuff (with default value)
    // developer note: with the current littlefs implementation (v2.9), need to use the alternative API to handle
    // attributes this is planned to be improved in a future version see (and related)
    // https://github.com/littlefs-project/littlefs/issues/759
    fileHandle->nanoAttributes = FileAttributes_Normal;
    fileHandle->attr = {NANO_LITTLEFS_ATTRIBUTE, &fileHandle->nanoAttributes, NANO_LITTLEFS_ATTRIBUTE};
    fileHandle->fileConfig = {
        .buffer = NULL,
        .attrs = &fileHandle->attr,
        .attr_count = 1,
    };

    if (fileExists)
    {
        // file already exists, open for R/W
        flags = LFS_O_RDWR;
    }
    else
    {
        // file doesn't exist
        flags = LFS_O_CREAT | LFS_O_RDWR;
    }

    // need to use the alternative API to handle attributes
    if (lfs_file_opencfg(fileHandle->fs, &fileHandle->file, path, flags, &fileHandle->fileConfig) == LFS_ERR_OK)
    {
        // store the handle
        *handle = (uint32_t)fileHandle;

        if (!fileExists)
        {
            // sync file to save attributes
            ASSERT(lfs_file_sync(fileHandle->fs, &fileHandle->file) == LFS_ERR_OK);
        }

        // done here, return imediatly so handle doesn't get cleared
        hr = S_OK;
        NANOCLR_RETURN();
    }
    else
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
    }

    NANOCLR_CLEANUP();

    if (fileHandle != NULL)
    {
        platform_free(fileHandle);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT LITTLEFS_FS_Driver::Close(uint32_t handle)
{
    LITTLEFS_FileHandle *fileHandle;

    if (handle == 0)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    fileHandle = (LITTLEFS_FileHandle *)handle;

#ifdef DEBUG
    int32_t result = lfs_file_close(fileHandle->fs, &fileHandle->file);
    // ASSERT(result == LFS_ERR_CORRUPT);
    if (result == LFS_ERR_CORRUPT)
    {
        __NOP();
    }
#else
    lfs_file_close(fileHandle->fs, &fileHandle->file);
#endif

    platform_free(fileHandle);

    return S_OK;
}

HRESULT LITTLEFS_FS_Driver::Read(uint32_t handle, uint8_t *buffer, int size, int *bytesRead)
{
    NANOCLR_HEADER();

    lfs_ssize_t readResult;
    LITTLEFS_FileHandle *fileHandle;

    if (handle == 0)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    if (!buffer || size < 0 || !bytesRead)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    fileHandle = (LITTLEFS_FileHandle *)handle;

    readResult = lfs_file_read(fileHandle->fs, &fileHandle->file, buffer, size);

    // check for EOF
    if (readResult > 0)
    {
        *bytesRead = readResult;
    }
    else if (readResult == 0)
    {
        if (lfs_file_tell(fileHandle->fs, &fileHandle->file) == lfs_file_size(fileHandle->fs, &fileHandle->file))
        {
            // signal EOF
            *bytesRead = -1;
        }
    }
    else
    {
        // failed to read from the file
        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
    }

    hr = S_OK;

    NANOCLR_NOCLEANUP();
}

HRESULT LITTLEFS_FS_Driver::Write(uint32_t handle, uint8_t *buffer, int size, int *bytesWritten)
{
    NANOCLR_HEADER();

    LITTLEFS_FileHandle *fileHandle;

    if (handle == 0)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    if (size < 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    fileHandle = (LITTLEFS_FileHandle *)handle;

    // write to the file
    *bytesWritten = lfs_file_write(fileHandle->fs, &fileHandle->file, buffer, size);

    if (*bytesWritten != size)
    {
        // failed to write to the file
        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
    }

    hr = S_OK;

    NANOCLR_NOCLEANUP();
}

HRESULT LITTLEFS_FS_Driver::Flush(uint32_t handle)
{
    LITTLEFS_FileHandle *fileHandle;

    if (handle == 0)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    fileHandle = (LITTLEFS_FileHandle *)handle;

    if (lfs_file_sync(fileHandle->fs, &fileHandle->file) < LFS_ERR_OK)
    {
        // failed to access the file
        return CLR_E_FILE_IO;
    }

    return S_OK;
}

HRESULT LITTLEFS_FS_Driver::Seek(uint32_t handle, int64_t offset, uint32_t origin, int64_t *position)
{
    LITTLEFS_FileHandle *fileHandle;

    // set to invalid value
    int32_t whence = -1;

    if (handle == 0)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    fileHandle = (LITTLEFS_FileHandle *)handle;

    // map the origin to littlefs
    switch (origin)
    {
        case SEEKORIGIN_BEGIN:
            whence = LFS_SEEK_SET;
            // 1st need to rewind the file to get to a known position
            lfs_file_rewind(fileHandle->fs, &fileHandle->file);
            break;

        case SEEKORIGIN_CURRENT:
            whence = LFS_SEEK_CUR;
            break;

        case SEEKORIGIN_END:
            whence = LFS_SEEK_END;
            break;
    }

    // seek to the position
    *position = lfs_file_seek(fileHandle->fs, &fileHandle->file, offset, whence);

    if (*position < LFS_ERR_OK)
    {
        // failed to access the file
        return CLR_E_FILE_IO;
    }

    return S_OK;
}

HRESULT LITTLEFS_FS_Driver::GetLength(uint32_t handle, int64_t *length)
{
    LITTLEFS_FileHandle *fileHandle;

    if (handle == 0)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    fileHandle = (LITTLEFS_FileHandle *)handle;

    *length = lfs_file_size(fileHandle->fs, &fileHandle->file);

    if (*length < LFS_ERR_OK)
    {
        // failed to access the file
        return CLR_E_FILE_IO;
    }

    return S_OK;
}

HRESULT LITTLEFS_FS_Driver::SetLength(uint32_t handle, int64_t length)
{
    LITTLEFS_FileHandle *fileHandle;
    uint32_t currentLength;

    if (handle == 0)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    fileHandle = (LITTLEFS_FileHandle *)handle;

    currentLength = lfs_file_size(fileHandle->fs, &fileHandle->file);

    if (currentLength < LFS_ERR_OK)
    {
        // failed to access the file
        return CLR_E_FILE_IO;
    }

    if (currentLength == length)
    {
        return S_OK;
    }

    if (currentLength > length)
    {
        // truncate the file
        if (lfs_file_truncate(fileHandle->fs, &fileHandle->file, length) != LFS_ERR_OK)
        {
            return CLR_E_FILE_IO;
        }
    }
    else
    {
        // extend the file, or keep the same size

        // store the current position
        int64_t currentPosition = lfs_file_tell(fileHandle->fs, &fileHandle->file);

        // grow the file to the required size
        if (lfs_file_seek(fileHandle->fs, &fileHandle->file, length, LFS_SEEK_END) < LFS_ERR_OK)
        {
            return CLR_E_FILE_IO;
        }

        // need to write at the current position to extend the file
        if (lfs_file_sync(fileHandle->fs, &fileHandle->file) != LFS_ERR_OK)
        {
            return CLR_E_FILE_IO;
        }

        // restore the position
        if (lfs_file_seek(fileHandle->fs, &fileHandle->file, currentPosition, LFS_SEEK_SET) < LFS_ERR_OK)
        {
            return CLR_E_FILE_IO;
        }
    }

    return S_OK;
}

//--//

HRESULT LITTLEFS_FS_Driver::FindOpen(const VOLUME_ID *volume, const char *path, uint32_t *findHandle)
{
    (void)volume;
    (void)path;
    (void)findHandle;

    // FAT_LogicDisk *logicDisk = FAT_MemoryManager::GetLogicDisk(volume);

    // if (logicDisk)
    // {
    //     return logicDisk->FindOpen(path, findHandle);
    // }
    ASSERT(FALSE);
    return CLR_E_INVALID_DRIVER;
}

HRESULT LITTLEFS_FS_Driver::FindNext(uint32_t handle, FS_FILEINFO *fi, bool *fileFound)
{
    (void)handle;
    (void)fi;
    (void)fileFound;

    // if (handle == 0)
    //     return CLR_E_INVALID_PARAMETER;

    // FAT_FINDFILES *fileInfo = (FAT_FINDFILES *)handle;

    // return fileInfo->FindNext(fi, fileFound);
    ASSERT(FALSE);
    return CLR_E_INVALID_DRIVER;
}

HRESULT LITTLEFS_FS_Driver::FindClose(uint32_t handle)
{
    (void)handle;

    // if (handle == 0)
    //     return CLR_E_INVALID_PARAMETER;

    // FAT_FINDFILES *fileInfo = (FAT_FINDFILES *)handle;

    // return fileInfo->FindClose();
    ASSERT(FALSE);
    return CLR_E_INVALID_DRIVER;
}

HRESULT LITTLEFS_FS_Driver::GetFileInfo(const VOLUME_ID *volume, const char *path, FS_FILEINFO *fileInfo, bool *found)
{
    (void)volume;
    (void)path;
    (void)fileInfo;
    (void)found;

    // FAT_LogicDisk *logicDisk = FAT_MemoryManager::GetLogicDisk(volume);

    // if (logicDisk)
    // {
    //     return logicDisk->GetFileInfo(path, fileInfo, found);
    // }
    ASSERT(FALSE);
    return CLR_E_INVALID_DRIVER;
}

HRESULT LITTLEFS_FS_Driver::GetAttributes(const VOLUME_ID *volume, const char *path, uint32_t *attributes)
{
    lfs_t *fsDrive = NULL;
    lfs_info info;

    // set to empty attributes
    *attributes = EMPTY_ATTRIBUTE;

    // adjust path to remove leading backslash
    if (*path == '\\')
    {
        path++;
    }

    // get littlefs instance
    fsDrive = hal_lfs_get_fs_from_index(volume->volumeId);

    if (fsDrive)
    {
        // check for file existence
#ifdef DEBUG
        int32_t result = lfs_stat(fsDrive, path, &info);
        // ASSERT(result == LFS_ERR_CORRUPT);
        if (result == LFS_ERR_CORRUPT)
        {
            __NOP();
        }

        if (result != LFS_ERR_OK)
#else
        if (lfs_stat(fsDrive, path, &info) != LFS_ERR_OK)
#endif
        {
            return S_OK;
        }

        // if this is home directory
        if (info.type == LFS_TYPE_DIR && *path == '\0')
        {
            *attributes = FileAttributes::FileAttributes_Directory;
            return S_OK;
        }

        // try to get the attributes

        // even if this fails we return success as attributes have been set to EMPTY_ATTRIBUTE
#ifdef DEBUG
        result = lfs_getattr(fsDrive, (const char *)path, NANO_LITTLEFS_ATTRIBUTE, attributes, NANO_LITTLEFS_ATTRIBUTE_SIZE);
        // ASSERT(result == LFS_ERR_CORRUPT);
        if (result == LFS_ERR_CORRUPT)
        {
            __NOP();
        }

        if (result == LFS_ERR_OK)
#else
        if (lfs_getattr(fsDrive, (const char *)path, NANO_LITTLEFS_ATTRIBUTE, attributes, NANO_LITTLEFS_ATTRIBUTE_SIZE) ==
            LFS_ERR_OK)
#endif
        {
            // add dir attributes
            lfs_stat(fsDrive, (const char *)path, &info);
            if (info.type == LFS_TYPE_DIR)
            {
                *attributes |= FileAttributes::FileAttributes_Directory;
            }
        }

        return S_OK;
    }

    return CLR_E_INVALID_DRIVER;
}

HRESULT LITTLEFS_FS_Driver::SetAttributes(const VOLUME_ID *volume, const char *path, uint32_t attributes)
{
    lfs_t *fsDrive = NULL;
    lfs_info info;

    // adjust path to remove leading backslash
    if (*path == '\\')
    {
        path++;
    }

    // get littlefs instance
    fsDrive = hal_lfs_get_fs_from_index(volume->volumeId);

    if (fsDrive)
    {
        // check for file existence
        if (lfs_stat(fsDrive, path, &info) != LFS_ERR_OK)
        {
            return CLR_E_FILE_NOT_FOUND;
        }

        if (lfs_setattr(
                fsDrive,
                (const char *)path,
                NANO_LITTLEFS_ATTRIBUTE,
                &attributes,
                NANO_LITTLEFS_ATTRIBUTE_SIZE) != LFS_ERR_OK)
        {
            return CLR_E_FILE_IO;
        }

        return S_OK;
    }

    return CLR_E_INVALID_DRIVER;
}

HRESULT LITTLEFS_FS_Driver::CreateDirectory(const VOLUME_ID *volume, const char *path)
{
    (void)volume;
    (void)path;

    // FAT_LogicDisk *logicDisk = FAT_MemoryManager::GetLogicDisk(volume);

    // if (logicDisk)
    // {
    //     return logicDisk->CreateDirectory(path);
    // }
    ASSERT(FALSE);
    return CLR_E_INVALID_DRIVER;
}

HRESULT LITTLEFS_FS_Driver::Move(const VOLUME_ID *volume, const char *oldPath, const char *newPath)
{
    (void)volume;
    (void)oldPath;
    (void)newPath;

    // FAT_LogicDisk *logicDisk = FAT_MemoryManager::GetLogicDisk(volume);

    // if (logicDisk)
    // {
    //     return logicDisk->Move(oldPath, newPath);
    // }
    ASSERT(FALSE);
    return CLR_E_INVALID_DRIVER;
}

HRESULT LITTLEFS_FS_Driver::Delete(const VOLUME_ID *volume, const char *path)
{
    lfs_t *fsDrive = NULL;
    lfs_info info;

    // adjust path to remove leading backslash
    if (*path == '\\')
    {
        path++;
    }

    // get littlefs instance
    fsDrive = hal_lfs_get_fs_from_index(volume->volumeId);

    if (fsDrive)
    {
        // check for file existence
        if (lfs_stat(fsDrive, path, &info) != LFS_ERR_OK)
        {
            return CLR_E_FILE_NOT_FOUND;
        }

        // TODO check read only attribute

        if (lfs_remove(fsDrive, path) != LFS_ERR_OK)
        {
            return CLR_E_FILE_IO;
        }

        return S_OK;
    }

    return CLR_E_INVALID_DRIVER;
}

//////////////////

// FILESYSTEM_DRIVER_INTERFACE g_FAT32_FILE_SYSTEM_DriverInterface = {
//     &LITTLEFS_FS_Driver::FindOpen,
//     &LITTLEFS_FS_Driver::FindNext,
//     &LITTLEFS_FS_Driver::FindClose,

//     &LITTLEFS_FS_Driver::GetFileInfo,

//     &LITTLEFS_FS_Driver::CreateDirectory,
//     &LITTLEFS_FS_Driver::Move,
//     &LITTLEFS_FS_Driver::Delete,

//     &LITTLEFS_FS_Driver::GetAttributes,
//     &LITTLEFS_FS_Driver::SetAttributes,

//     &LITTLEFS_FS_Driver::Format,
//     &LITTLEFS_FS_Driver::LoadMedia,
//     &LITTLEFS_FS_Driver::GetSizeInfo,
//     &LITTLEFS_FS_Driver::FlushAll,
//     &LITTLEFS_FS_Driver::GetVolumeLabel,

//     "LITTLEFS",
//     0,
// };

STREAM_DRIVER_INTERFACE g_LITTLEFS_STREAM_DriverInterface = {
    &LITTLEFS_FS_Driver::Initialize,
    &LITTLEFS_FS_Driver::InitializeVolume,
    &LITTLEFS_FS_Driver::UnInitializeVolume,
    &LITTLEFS_FS_Driver::DriverDetails,
    &LITTLEFS_FS_Driver::Open,
    &LITTLEFS_FS_Driver::Close,
    &LITTLEFS_FS_Driver::Read,
    &LITTLEFS_FS_Driver::Write,
    &LITTLEFS_FS_Driver::Flush,
    &LITTLEFS_FS_Driver::Seek,
    &LITTLEFS_FS_Driver::GetLength,
    &LITTLEFS_FS_Driver::SetLength,
};