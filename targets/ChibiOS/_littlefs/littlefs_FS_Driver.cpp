//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include <hal.h>
#include <target_littlefs.h>
#include <nanoHAL_v2.h>
#include <nf_sys_io_filesystem.h>
#include "littlefs_FS_Driver.h"
#include "hal_littlefs.h"

extern "C"
{
    extern uint8_t lfs_inputBuffer[];
    extern uint8_t lfs_outputBuffer[];
    extern int32_t lfs_inputBufferSize;
    extern int32_t lfs_outputBufferSize;
}

static int32_t RemoveAllFiles(lfs_t *fs, const char *path);
static int NormalizePath(const char *path, char *buffer, size_t bufferSize);

bool LITTLEFS_FS_Driver::LoadMedia(const void *driverInterface)
{
    (void)driverInterface;
    return TRUE;
}

STREAM_DRIVER_DETAILS *LITTLEFS_FS_Driver::DriverDetails(const VOLUME_ID *volume)
{
    (void)volume;

    static STREAM_DRIVER_DETAILS driverDetail = {
        DRIVER_BUFFERED_IO,
        lfs_inputBuffer,
        lfs_outputBuffer,
        lfs_inputBufferSize,
        lfs_outputBufferSize,
        TRUE,
        TRUE,
        TRUE,
        0,
        0};

    return &driverDetail;
}

//--//

void LITTLEFS_FS_Driver::Initialize()
{
    // nothing to do here as the littlefs instances are already initialized at target boot
}

bool LITTLEFS_FS_Driver::InitializeVolume(const VOLUME_ID *volume, const char *path)
{
    (void)volume;
    (void)path;

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
    NANOCLR_HEADER();

    (void)volumeLabel;
    (void)parameters;

    LITTLEFS_FileHandle *fileHandle = NULL;
    uint8_t *index = NULL;

    // get littlefs instance
    fileHandle->fs = hal_lfs_get_fs_from_index(volume->volumeId);

    if (!fileHandle->fs)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
    }

    // get index of the partition
    index = (uint8_t *)fileHandle->fs->cfg->context;

    hal_lfs_mount_partition(*index, true);

    NANOCLR_NOCLEANUP();
}

HRESULT LITTLEFS_FS_Driver::GetSizeInfo(const VOLUME_ID *volume, int64_t *totalSize, int64_t *totalFreeSpace)
{
    NANOCLR_HEADER();

    lfs_ssize_t allocBlocks;
    LITTLEFS_FileHandle *fileHandle = NULL;

    // get littlefs instance
    fileHandle->fs = hal_lfs_get_fs_from_index(volume->volumeId);

    if (!fileHandle->fs)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
    }

    // compute the total size
    *totalSize = (int64_t)(fileHandle->fs->cfg->block_size * fileHandle->fs->cfg->block_count);

    // get the littlefs info
    allocBlocks = lfs_fs_size(fileHandle->fs);

    if (allocBlocks < 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
    }

    // compute the total free space
    *totalFreeSpace = (int64_t)(*totalSize - (allocBlocks * fileHandle->fs->cfg->block_size));

    NANOCLR_NOCLEANUP();
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

HRESULT LITTLEFS_FS_Driver::GetVolumeLabel(const VOLUME_ID *volume, char *volumeLabel, int32_t volumeLabelLen)
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

HRESULT LITTLEFS_FS_Driver::Open(const VOLUME_ID *volume, const char *path, void *&handle)
{
    NANOCLR_HEADER();

#ifdef DEBUG
    int32_t result;
#endif

    LITTLEFS_FileHandle *fileHandle = NULL;
    lfs_info info;
    int32_t flags;
    char normalizedPath[FS_MAX_DIRECTORY_LENGTH];
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

    if (NormalizePath(path, normalizedPath, sizeof(normalizedPath)) < 0)
    {
        // handle error
        return CLR_E_PATH_TOO_LONG;
    }

    // check for file existence
#ifdef DEBUG
    result = lfs_stat(fileHandle->fs, normalizedPath, &info);

    if (result == LFS_ERR_CORRUPT)
    {
        __NOP();
        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
    }

    fileExists = result == LFS_ERR_OK;
#else
    fileExists = lfs_stat(fileHandle->fs, normalizedPath, &info) == LFS_ERR_OK;
#endif

    // setup attributes stuff (with default value)
    // developer note: with the current littlefs implementation (v2.9), need to use the alternative API to handle
    // attributes this is planned to be improved in a future version see (and related)
    // https://github.com/littlefs-project/littlefs/issues/759
    fileHandle->nanoAttributes = FileAttributes_Normal;
    fileHandle->attr = {NANO_LITTLEFS_ATTRIBUTE, &fileHandle->nanoAttributes, NANO_LITTLEFS_ATTRIBUTE_SIZE};
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
    if (lfs_file_opencfg(fileHandle->fs, &fileHandle->file, normalizedPath, flags, &fileHandle->fileConfig) ==
        LFS_ERR_OK)
    {
        // store the handle
        handle = fileHandle;

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

HRESULT LITTLEFS_FS_Driver::Close(void *handle)
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

HRESULT LITTLEFS_FS_Driver::Read(void *handle, uint8_t *buffer, int size, int *bytesRead)
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

HRESULT LITTLEFS_FS_Driver::Write(void *handle, uint8_t *buffer, int size, int *bytesWritten)
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

HRESULT LITTLEFS_FS_Driver::Flush(void *handle)
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

HRESULT LITTLEFS_FS_Driver::Seek(void *handle, int64_t offset, uint32_t origin, int64_t *position)
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

HRESULT LITTLEFS_FS_Driver::GetLength(void *handle, int64_t *length)
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

HRESULT LITTLEFS_FS_Driver::SetLength(void *handle, int64_t length)
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

HRESULT LITTLEFS_FS_Driver::FindOpen(const VOLUME_ID *volume, const char *path, void *&handle)
{
    NANOCLR_HEADER();

    char normalizedPath[FS_MAX_DIRECTORY_LENGTH];
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

    if (NormalizePath(path, normalizedPath, sizeof(normalizedPath)) < 0)
    {
        // handle error
        return CLR_E_PATH_TOO_LONG;
    }

    // open directory for seek
    if (lfs_dir_open(findHandle->fs, &findHandle->dir, normalizedPath) == LFS_ERR_OK)
    {
        lfs_dir_seek(findHandle->fs, &findHandle->dir, 0);

        // store the handle
        handle = findHandle;

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

HRESULT LITTLEFS_FS_Driver::FindNext(void *handle, FS_FILEINFO *fi, bool *fileFound)
{
    NANOCLR_HEADER();

    LITTLEFS_FindFileHandle *findHandle;
    lfs_info info;
    int32_t result;

    if (handle == 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // check file parameters
    if (!fi || !fileFound)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
    }

    findHandle = (LITTLEFS_FindFileHandle *)handle;

    // read the next entry
    do
    {
        // read the next entry
        result = lfs_dir_read(findHandle->fs, &findHandle->dir, &info);

        // no more entries
        if (result == LFS_ERR_OK)
        {
            *fileFound = false;
            NANOCLR_SET_AND_LEAVE(S_OK);
        }
        // something went wrong
        else if (result < LFS_ERR_OK)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
        }

        // skip the '.' and '..' entries
    } while (info.name[0] == '.');

    // found a file
    *fileFound = true;

    // set file name size
    fi->FileNameSize = hal_strlen_s(info.name);

    // allocate memory for the file name
    // MUST BE FREED BY THE CALLER
    fi->FileName = (char *)platform_malloc(fi->FileNameSize + 1);

    // sanity check for successfull malloc
    if (fi->FileName == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    // copy the file name, including the string terminator
    hal_strcpy_s((char *)fi->FileName, fi->FileNameSize + 1, info.name);

    // read and set the attributes (don't care about the result, if it fails the attribute will be set to normal)
    lfs_getattr(findHandle->fs, info.name, NANO_LITTLEFS_ATTRIBUTE, &fi->Attributes, NANO_LITTLEFS_ATTRIBUTE_SIZE);

    // adjust attributes, if this is a directory
    if (info.type == LFS_TYPE_DIR)
    {
        fi->Attributes |= FileAttributes::FileAttributes_Directory;
    }

    // set the size
    fi->Size = info.size;

    NANOCLR_NOCLEANUP();
}

HRESULT LITTLEFS_FS_Driver::FindClose(void *handle)
{
    LITTLEFS_FindFileHandle *findHandle;

    if (handle != 0)
    {
        findHandle = (LITTLEFS_FindFileHandle *)handle;

        if (findHandle->fs != NULL)
        {
            lfs_dir_close(findHandle->fs, &findHandle->dir);

            platform_free(findHandle);
        }
    }

    return S_OK;
}

HRESULT LITTLEFS_FS_Driver::GetFileInfo(const VOLUME_ID *volume, const char *path, FS_FILEINFO *fileInfo, bool *found)
{
    lfs_t *fsDrive = NULL;
    lfs_info info;
    char normalizedPath[FS_MAX_DIRECTORY_LENGTH];

    if (NormalizePath(path, normalizedPath, sizeof(normalizedPath)) < 0)
    {
        // handle error
        return CLR_E_PATH_TOO_LONG;
    }

    // get littlefs instance
    fsDrive = hal_lfs_get_fs_from_index(volume->volumeId);

    if (fsDrive)
    {
        // root is different
        if (*normalizedPath == '/' && *(normalizedPath + 1) == '\0')
        {
            // this is the root directory
            fileInfo->Attributes = FileAttributes::FileAttributes_Directory;
            fileInfo->Size = 0;
            fileInfo->FileNameSize = 1;
        }
        else
        {
            // check for file existence
            if (lfs_stat(fsDrive, normalizedPath, &info) != LFS_ERR_OK)
            {
                *found = false;
                return S_OK;
            }

            // set found flag
            *found = true;

            // read and set the attributes
            lfs_getattr(
                fsDrive,
                normalizedPath,
                NANO_LITTLEFS_ATTRIBUTE,
                &fileInfo->Attributes,
                NANO_LITTLEFS_ATTRIBUTE_SIZE);

            fileInfo->Size = info.size;

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
    char normalizedPath[FS_MAX_DIRECTORY_LENGTH];
    // set to empty attributes
    uint32_t attributeBuffer = EMPTY_ATTRIBUTE;

    if (NormalizePath(path, normalizedPath, sizeof(normalizedPath)) < 0)
    {
        // handle error
        return CLR_E_PATH_TOO_LONG;
    }

    // get littlefs instance
    fsDrive = hal_lfs_get_fs_from_index(volume->volumeId);

    if (fsDrive)
    {
        // check for file existence
        result = lfs_stat(fsDrive, normalizedPath, &info);

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
        if (info.type == LFS_TYPE_DIR && *normalizedPath == '\0')
        {
            attributeBuffer = FileAttributes::FileAttributes_Directory;

            NANOCLR_SET_AND_LEAVE(S_OK);
        }

        // try to get the attributes
        // even if this fails we return success as attributes have been set to EMPTY_ATTRIBUTE
        result = lfs_getattr(
            fsDrive,
            (const char *)normalizedPath,
            NANO_LITTLEFS_ATTRIBUTE,
            &attributeBuffer,
            NANO_LITTLEFS_ATTRIBUTE_SIZE);

        if (result >= LFS_ERR_OK || result == LFS_ERR_NOATTR)
        {
            if (info.type == LFS_TYPE_DIR)
            {
                if (result == LFS_ERR_NOATTR)
                {
                    // this is a directory, set dir attribute
                    attributeBuffer = FileAttributes::FileAttributes_Directory;
                }
                else
                {
                    // this is a directory, add dir attribute
                    attributeBuffer |= FileAttributes::FileAttributes_Directory;
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

    NANOCLR_CLEANUP();

    *attributes = attributeBuffer;

    NANOCLR_CLEANUP_END();
}

HRESULT LITTLEFS_FS_Driver::SetAttributes(const VOLUME_ID *volume, const char *path, uint32_t attributes)
{
    lfs_t *fsDrive = NULL;
    lfs_info info;
    uint32_t currentAttributes;
    char normalizedPath[FS_MAX_DIRECTORY_LENGTH];

    if (NormalizePath(path, normalizedPath, sizeof(normalizedPath)) < 0)
    {
        // handle error
        return CLR_E_PATH_TOO_LONG;
    }

    // get littlefs instance
    fsDrive = hal_lfs_get_fs_from_index(volume->volumeId);

    if (fsDrive)
    {
        // check for file existence
        if (lfs_stat(fsDrive, normalizedPath, &info) != LFS_ERR_OK)
        {
            return CLR_E_FILE_NOT_FOUND;
        }

        // read current attributes
        if (lfs_getattr(
                fsDrive,
                (const char *)normalizedPath,
                NANO_LITTLEFS_ATTRIBUTE,
                &currentAttributes,
                NANO_LITTLEFS_ATTRIBUTE_SIZE) < LFS_ERR_OK)
        {
            return CLR_E_FILE_IO;
        }

        // clear attributes
        currentAttributes &= ~ATTRIBUTE_SET_MASK;

        // set new attributes
        currentAttributes |= attributes & ATTRIBUTE_SET_MASK;

        if (lfs_setattr(
                fsDrive,
                (const char *)normalizedPath,
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
    char normalizedPath[FS_MAX_DIRECTORY_LENGTH];
    char tempPath[FS_MAX_DIRECTORY_LENGTH + 1];
    char *segment;
    int32_t dirExists;

    (void)dirExists;

    if (NormalizePath(path, normalizedPath, sizeof(normalizedPath)) < 0)
    {
        // handle error
        return CLR_E_PATH_TOO_LONG;
    }

    // get littlefs instance
    fsDrive = hal_lfs_get_fs_from_index(volume->volumeId);

    if (fsDrive)
    {
        memset(tempPath, 0, sizeof(tempPath));

        // iterate over the path segments and create the directories
        segment = strtok(normalizedPath, "/");

        while (segment && (result == LFS_ERR_OK || result == LFS_ERR_EXIST))
        {
            strcat(tempPath, segment);

            result = lfs_mkdir(fsDrive, tempPath);

            if (result != LFS_ERR_OK && result != LFS_ERR_EXIST)
            {
                __NOP();
                return CLR_E_FILE_IO;
            }

            // add back the '/' separator
            strcat(tempPath, "/");

            segment = strtok(NULL, "/");
        }

        // remove trailing '/'
        tempPath[hal_strlen_s(tempPath) - 1] = '\0';

        // sanity check for success
        lfs_info info;
        dirExists = lfs_stat(fsDrive, tempPath, &info);

        // sanity check for success
        if (dirExists == LFS_ERR_EXIST)
        {
            return S_OK;
        }
        else if (dirExists == LFS_ERR_OK)
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
        else
        {
            __NOP();
            return CLR_E_FILE_IO;
        }
    }

    return CLR_E_INVALID_DRIVER;
}

HRESULT LITTLEFS_FS_Driver::Move(const VOLUME_ID *volume, const char *oldPath, const char *newPath)
{
    lfs_t *fsDrive = NULL;
    char normalizedNewPath[FS_MAX_DIRECTORY_LENGTH];
    char normalizedOldPath[FS_MAX_DIRECTORY_LENGTH];
    int32_t result = LFS_ERR_OK;

    if (NormalizePath(newPath, normalizedNewPath, sizeof(normalizedNewPath)) < 0)
    {
        // handle error
        return CLR_E_PATH_TOO_LONG;
    }

    if (NormalizePath(oldPath, normalizedOldPath, sizeof(normalizedOldPath)) < 0)
    {
        // handle error
        return CLR_E_PATH_TOO_LONG;
    }

    // get littlefs instance
    fsDrive = hal_lfs_get_fs_from_index(volume->volumeId);

    if (fsDrive)
    {
        // the check for source file and destination file existence has already been made in managed code
        result = lfs_rename(fsDrive, normalizedOldPath, normalizedNewPath);
        if (result == LFS_ERR_OK)
        {
            return S_OK;
        }
        else if (result == LFS_ERR_NOTEMPTY)
        {
            // failed to move directory because it's not empty
            return S_FALSE;
        }
        else
        {
            return CLR_E_FILE_IO;
        }
    }

    return CLR_E_INVALID_DRIVER;
}

HRESULT LITTLEFS_FS_Driver::Delete(const VOLUME_ID *volume, const char *path, bool recursive)
{
    lfs_t *fsDrive = NULL;
    lfs_info info;
    char normalizedPath[FS_MAX_DIRECTORY_LENGTH];
    int32_t result;

    if (NormalizePath(path, normalizedPath, sizeof(normalizedPath)) < 0)
    {
        // handle error
        return CLR_E_PATH_TOO_LONG;
    }

    // get littlefs instance
    fsDrive = hal_lfs_get_fs_from_index(volume->volumeId);

    if (fsDrive)
    {
        // check for file existence
        if (lfs_stat(fsDrive, normalizedPath, &info) != LFS_ERR_OK)
        {
            return CLR_E_FILE_NOT_FOUND;
        }

    remove_entry:
        // remove the directory
        result = lfs_remove(fsDrive, normalizedPath);

        if (result == LFS_ERR_NOTEMPTY)
        {
            if (!recursive)
            {
                // directory is not empty and we are not in recursive mode
                return CLR_E_DIRECTORY_NOT_EMPTY;
            }

            // recursivelly delete all files and subdirectories
            result = RemoveAllFiles(fsDrive, normalizedPath);
            if (result != LFS_ERR_OK)
            {
                return CLR_E_FILE_IO;
            }

            goto remove_entry;
        }
        else if (result != LFS_ERR_OK)
        {
            return CLR_E_FILE_IO;
        }

        return S_OK;
    }

    return CLR_E_INVALID_DRIVER;
}

static int32_t RemoveAllFiles(lfs_t *fs, const char *path)
{
    lfs_dir dir;
    lfs_info info;
    int32_t result;
    char buffer[FS_MAX_PATH_LENGTH];
    char *bufferP = buffer;
    size_t bufferSize = FS_MAX_PATH_LENGTH;

    // open the directory
    result = lfs_dir_open(fs, &dir, path);
    if (result != LFS_ERR_OK)
    {
        return result;
    }

    // read the directory
    while (lfs_dir_read(fs, &dir, &info) > LFS_ERR_OK)
    {
        // skip the '.' and '..' entries
        if (info.name[0] == '.')
        {
            continue;
        }

        // reset and prepare the buffer
        memset(buffer, 0, FS_MAX_PATH_LENGTH);
        bufferP = buffer;

        // join filename with the path
        CLR_SafeSprintf(bufferP, bufferSize, "%s/%s", path, info.name);

        // check if this is a directory
        if (info.type == LFS_TYPE_DIR)
        {
            // recursivelly delete all files and subdirectories
            result = RemoveAllFiles(fs, buffer);

            if (result != LFS_ERR_OK)
            {
                return result;
            }
        }

        // remove the entry (OK to remove files and directories)
        result = lfs_remove(fs, buffer);
        if (result != LFS_ERR_OK)
        {
            return result;
        }
    }

    // close the directory
    return lfs_dir_close(fs, &dir);
}

// normalize path by:
// - replace \ with /
// - remove leading slash
// - handle other normalization issues
static int NormalizePath(const char *path, char *buffer, size_t bufferSize)
{
    const char *workingPath = path;
    char *bufferP = buffer;

    while (*workingPath && (static_cast<size_t>(bufferP - buffer) < bufferSize))
    {
        // Copy the character, replacing backslashes with forward slashes
        *bufferP = (*workingPath == '\\') ? '/' : *workingPath;

        workingPath++;
        bufferP++;
    }

    if (static_cast<size_t>(bufferP - buffer) >= bufferSize)
    {
        // path is too long, return an error
        return -1;
    }

    // Null-terminate the path
    *bufferP = '\0';

    // remove trailing slash, if any
    if (bufferP[-1] == '/')
    {
        bufferP--;
        *bufferP = '\0';
    }

    if (buffer[0] == '/')
    {
        // remove leading slash
        memmove(buffer, buffer + 1, hal_strlen_s(buffer));
    }

    return 0;
}

//////////////////

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

FILESYSTEM_DRIVER_INTERFACE g_LITTLEFS_FILE_SYSTEM_DriverInterface = {
    &LITTLEFS_FS_Driver::FindOpen,
    &LITTLEFS_FS_Driver::FindNext,
    &LITTLEFS_FS_Driver::FindClose,

    &LITTLEFS_FS_Driver::GetFileInfo,

    &LITTLEFS_FS_Driver::CreateDirectory,
    &LITTLEFS_FS_Driver::Move,
    &LITTLEFS_FS_Driver::Delete,

    &LITTLEFS_FS_Driver::GetAttributes,
    &LITTLEFS_FS_Driver::SetAttributes,

    &LITTLEFS_FS_Driver::Format,
    &LITTLEFS_FS_Driver::LoadMedia,
    &LITTLEFS_FS_Driver::GetSizeInfo,
    &LITTLEFS_FS_Driver::FlushAll,
    &LITTLEFS_FS_Driver::GetVolumeLabel,

    "LITTLEFS",
    0,
};
