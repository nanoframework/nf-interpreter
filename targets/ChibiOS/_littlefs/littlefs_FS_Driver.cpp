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
    // nothing to do here as the littlefs instances are already initialized at target boot
}

bool LITTLEFS_FS_Driver::InitializeVolume(const VOLUME_ID *volume)
{
    (void)volume;

    // nothing to do here as the littlefs instances are already initialized at target boot
    return TRUE;
}

bool LITTLEFS_FS_Driver::UnInitializeVolume(const VOLUME_ID *volume)
{
    (void)volume;

    // nothing to do here as the littlefs instances are already initialized at target boot
    return TRUE;
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
    char *workingPath;
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

    // adjust path to replace \ with /
    workingPath = (char *)path;
    while (*workingPath)
    {
        if (*workingPath == '\\')
        {
            *workingPath = '/';
        }

        workingPath++;
    }

    // adjust path to remove leading backslash
    if (*path == '/')
    {
        path++;
    }

    // check for file existence
#ifdef DEBUG
    result = lfs_stat(fileHandle->fs, path, &info);

    if (result == LFS_ERR_CORRUPT)
    {
        __NOP();
        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
    }

    fileExists = result == LFS_ERR_OK;
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
    NANOCLR_HEADER();

    LITTLEFS_FileHandle *fileHandle;

    if (handle == 0)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    fileHandle = (LITTLEFS_FileHandle *)handle;

#ifdef DEBUG
    int32_t result = lfs_file_close(fileHandle->fs, &fileHandle->file);

    if (result == LFS_ERR_CORRUPT)
    {
        __NOP();
        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
    }
    else
    {
        NANOCLR_SET_AND_LEAVE(result == LFS_ERR_OK ? S_OK : CLR_E_FILE_IO);
    }
#else
    NANOCLR_SET_AND_LEAVE(lfs_file_close(fileHandle->fs, &fileHandle->file) == LFS_ERR_OK ? S_OK : CLR_E_FILE_IO);
#endif

    NANOCLR_CLEANUP();

    platform_free(fileHandle);

    NANOCLR_CLEANUP_END();
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

HRESULT LITTLEFS_FS_Driver::FindOpen(const VOLUME_ID *volume, const char *path, uint32_t *handle)
{
    NANOCLR_HEADER();

    char *workingPath;
    LITTLEFS_FindFileHandle *findHandle = NULL;

    // allocate file handle
    findHandle = (LITTLEFS_FindFileHandle *)platform_malloc(sizeof(LITTLEFS_FindFileHandle));

    if (findHandle == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    memset(findHandle, 0, sizeof(LITTLEFS_FindFileHandle));

    // get littlefs instance
    findHandle->fs = hal_lfs_get_fs_from_index(volume->volumeId);

    if (!findHandle->fs)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
    }

    // adjust path to replace \ with /
    workingPath = (char *)path;
    while (*workingPath)
    {
        if (*workingPath == '\\')
        {
            *workingPath = '/';
        }

        workingPath++;
    }

    // adjust path to remove leading backslash
    if (*path == '/')
    {
        path++;
    }

    // open directory for seek
    if (lfs_dir_open(findHandle->fs, &findHandle->dir, path) == LFS_ERR_OK)
    {
        lfs_dir_seek(findHandle->fs, &findHandle->dir, 0);

        // store the handle
        *handle = (uint32_t)findHandle;

        // done here, return imediatly so handle doesn't get cleared
        hr = S_OK;
        NANOCLR_RETURN();
    }

    NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);

    NANOCLR_CLEANUP();

    if (findHandle != NULL)
    {
        platform_free(findHandle);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT LITTLEFS_FS_Driver::FindNext(uint32_t handle, FS_FILEINFO *fi, bool *fileFound)
{
    LITTLEFS_FindFileHandle *findHandle;
    lfs_info info;

    if (handle == 0)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    findHandle = (LITTLEFS_FindFileHandle *)handle;

    // read the next entry
    do
    {
        if (lfs_dir_read(findHandle->fs, &findHandle->dir, &info) <= LFS_ERR_OK)
        {
            return CLR_E_FILE_IO;
        }

        // skip the '.' and '..' entries
    } while (info.name[0] == '.');

    // found a file
    *fileFound = true;

    // set file name size
    fi->FileNameSize = hal_strlen_s(info.name);

    // copy the file name
    hal_strcpy_s(fi->FileName, fi->FileNameSize, info.name);

    // read and set the attributes
    lfs_getattr(findHandle->fs, info.name, NANO_LITTLEFS_ATTRIBUTE, &fi->Attributes, NANO_LITTLEFS_ATTRIBUTE_SIZE);

    // set the size
    fi->Size = info.size;

    return S_OK;
}

HRESULT LITTLEFS_FS_Driver::FindClose(uint32_t handle)
{
    LITTLEFS_FindFileHandle *findHandle;

    if (handle == 0)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    findHandle = (LITTLEFS_FindFileHandle *)handle;

    lfs_dir_close(findHandle->fs, &findHandle->dir);

    platform_free(findHandle);

    return S_OK;
}

HRESULT LITTLEFS_FS_Driver::GetFileInfo(const VOLUME_ID *volume, const char *path, FS_FILEINFO *fileInfo, bool *found)
{
    lfs_t *fsDrive = NULL;
    lfs_info info;
    char *workingPath;

    // adjust path to replace \ with /
    workingPath = (char *)path;
    while (*workingPath)
    {
        if (*workingPath == '\\')
        {
            *workingPath = '/';
        }

        workingPath++;
    }

    // get littlefs instance
    fsDrive = hal_lfs_get_fs_from_index(volume->volumeId);

    if (fsDrive)
    {
        // root is different
        if (*path == '/' && *(path + 1) == '\0')
        {

            // this is the root directory
            fileInfo->Attributes = FileAttributes::FileAttributes_Directory;
            fileInfo->Size = 0;
            fileInfo->FileNameSize = 1;
        }
        else
        {
            // check for file existence
            if (lfs_stat(fsDrive, path, &info) != LFS_ERR_OK)
            {
                *found = false;
                return S_OK;
            }

            // set found flag
            *found = true;

            // read and set the attributes
            lfs_getattr(fsDrive, path, NANO_LITTLEFS_ATTRIBUTE, &fileInfo->Attributes, NANO_LITTLEFS_ATTRIBUTE_SIZE);

            // no need to set the file name details as managed code already has this info
        }

        return S_OK;
    }

    return CLR_E_INVALID_DRIVER;
}

HRESULT LITTLEFS_FS_Driver::GetAttributes(const VOLUME_ID *volume, const char *path, uint32_t *attributes)
{
    NANOCLR_HEADER();

    lfs_t *fsDrive = NULL;
    lfs_info info;
    int32_t result;
    char *workingPath;

    // set to empty attributes
    *attributes = EMPTY_ATTRIBUTE;

    // adjust path to replace \ with /
    workingPath = (char *)path;
    while (*workingPath)
    {
        if (*workingPath == '\\')
        {
            *workingPath = '/';
        }

        workingPath++;
    }

    // // adjust path to remove leading backslash
    // if (*path == '/')
    // {
    //     path++;
    // }

    // get littlefs instance
    fsDrive = hal_lfs_get_fs_from_index(volume->volumeId);

    if (fsDrive)
    {
        // check for file existence
        result = lfs_stat(fsDrive, path, &info);

        if (result == LFS_ERR_CORRUPT)
        {
            __NOP();
            NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
        }
        else if (result == LFS_ERR_NOENT)
        {
            // file doesn't exist
            // even if this fails we return success as attributes have been set to EMPTY_ATTRIBUTE
            NANOCLR_SET_AND_LEAVE(S_OK);
        }

        // if this is home directory
        if (info.type == LFS_TYPE_DIR && *path == '\0')
        {
            *attributes = FileAttributes::FileAttributes_Directory;

            NANOCLR_SET_AND_LEAVE(S_OK);
        }

        // try to get the attributes
        // even if this fails we return success as attributes have been set to EMPTY_ATTRIBUTE
        result =
            lfs_getattr(fsDrive, (const char *)path, NANO_LITTLEFS_ATTRIBUTE, attributes, NANO_LITTLEFS_ATTRIBUTE_SIZE);

        if (result == LFS_ERR_OK || result == LFS_ERR_NOATTR)
        {
            if (info.type == LFS_TYPE_DIR)
            {
                if (result == LFS_ERR_NOATTR)
                {
                    // this is a directory, set dir attribute
                    *attributes = FileAttributes::FileAttributes_Directory;
                }
                else
                {
                    // this is a directory, add dir attribute
                    *attributes |= FileAttributes::FileAttributes_Directory;
                }
            }

            NANOCLR_SET_AND_LEAVE(S_OK);
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
        }
    }
    else
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_DRIVER);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT LITTLEFS_FS_Driver::SetAttributes(const VOLUME_ID *volume, const char *path, uint32_t attributes)
{
    lfs_t *fsDrive = NULL;
    lfs_info info;
    uint32_t currentAttributes;
    char *workingPath;

    // adjust path to replace \ with /
    workingPath = (char *)path;
    while (*workingPath)
    {
        if (*workingPath == '\\')
        {
            *workingPath = '/';
        }

        workingPath++;
    }

    // adjust path to remove leading backslash
    if (*path == '/')
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

        // read current attributes
        if (lfs_getattr(
                fsDrive,
                (const char *)path,
                NANO_LITTLEFS_ATTRIBUTE,
                &currentAttributes,
                NANO_LITTLEFS_ATTRIBUTE_SIZE) != LFS_ERR_OK)
        {
            return CLR_E_FILE_IO;
        }

        // clear attributes
        currentAttributes &= ~ATTRIBUTE_SET_MASK;

        // set new attributes
        currentAttributes |= attributes & ATTRIBUTE_SET_MASK;

        if (lfs_setattr(
                fsDrive,
                (const char *)path,
                NANO_LITTLEFS_ATTRIBUTE,
                &currentAttributes,
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
    lfs_t *fsDrive = NULL;
    int32_t result = LFS_ERR_OK;
    char *workingPath;
    char tempPath[FS_MAX_DIRECTORY_LENGTH];
    char *tempPathP = tempPath;

    // adjust path to replace \ with /
    workingPath = (char *)path;
    while (*workingPath)
    {
        if (*workingPath == '\\')
        {
            *workingPath = '/';
        }
        
        workingPath++;
        tempPathP++;
    }

    *tempPathP = '/';
    tempPathP++;
    *tempPathP = '\0';

    // adjust path to remove leading backslash
    if (*path == '/')
    {
        path++;
    }

    // get littlefs instance
    fsDrive = hal_lfs_get_fs_from_index(volume->volumeId);

    if (fsDrive)
    {
        // clear the tempPath
        memset(tempPath, 0, FS_MAX_DIRECTORY_LENGTH);

        // store pointer to tempPath
        tempPathP = tempPath;

        // create all the diretories in the path, walking the path
        do
        {
            // find the next path separator
            workingPath = strchr(path, '/');

            if (workingPath)
            {
                // copy the path part to tempPath
                memcpy(tempPathP, path, workingPath - path);

                // store pointer to tempPath
                tempPathP += (workingPath - path);

                // move to the next part of the path
                path = workingPath + 1;
            }
            else
            {
                // this is the last part of the path
                hal_strcpy_s(tempPathP, FS_MAX_PATH_LENGTH, path);
            }

            // create the directory
            result = lfs_mkdir(fsDrive, tempPath);

            // add the separator
            if (workingPath)
            {
                *tempPathP = '/';

                // update pointer
                tempPathP++;
            }

        } while (workingPath && (result == LFS_ERR_OK || result == LFS_ERR_EXIST));

        if (result == LFS_ERR_OK)
        {
            // need to set the attributes for the directory
            // TODO: need to check if DIR attr work the same way as file attr
            // uint32_t attributes = FileAttributes::FileAttributes_Directory;

            // if (lfs_setattr(
            //         fsDrive,
            //         (const char *)tempPath,
            //         NANO_LITTLEFS_ATTRIBUTE,
            //         &attributes,
            //         NANO_LITTLEFS_ATTRIBUTE_SIZE) != LFS_ERR_OK)
            // {
            //     return CLR_E_FILE_IO;
            // }

            // done here
            return S_OK;
        }
        // if the directory already exists, return success
        else if (result == LFS_ERR_EXIST)
        {
            return S_OK;
        }

        return CLR_E_FILE_IO;
    }

    return CLR_E_INVALID_DRIVER;
}

HRESULT LITTLEFS_FS_Driver::Move(const VOLUME_ID *volume, const char *oldPath, const char *newPath)
{
    lfs_t *fsDrive = NULL;
    char *workingPath;

    // adjust path to replace \ with /
    workingPath = (char *)oldPath;
    while (*workingPath)
    {
        if (*workingPath == '\\')
        {
            *workingPath = '/';
        }

        workingPath++;
    }

    workingPath = (char *)newPath;
    while (*workingPath)
    {
        if (*workingPath == '\\')
        {
            *workingPath = '/';
        }

        workingPath++;
    }

    // adjust path to remove leading backslash
    if (*oldPath == '/')
    {
        oldPath++;
    }

    if (*newPath == '/')
    {
        newPath++;
    }

    // get littlefs instance
    fsDrive = hal_lfs_get_fs_from_index(volume->volumeId);

    if (fsDrive)
    {
        // the check for source file and destination file existence has already been made in managed code

        if (lfs_rename(fsDrive, oldPath, newPath) != LFS_ERR_OK)
        {
            return CLR_E_FILE_IO;
        }

        return S_OK;
    }

    return CLR_E_INVALID_DRIVER;
}

HRESULT LITTLEFS_FS_Driver::Delete(const VOLUME_ID *volume, const char *path)
{
    lfs_t *fsDrive = NULL;
    lfs_info info;
    char *workingPath;

    // adjust path to replace \ with /
    workingPath = (char *)path;
    while (*workingPath)
    {
        if (*workingPath == '\\')
        {
            *workingPath = '/';
        }

        workingPath++;
    }

    // adjust path to remove leading backslash
    if (*path == '/')
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