//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nf_sys_io_filesystem.h>
#include "fatfs_FS_Driver.h"
#include <stdlib.h>

extern FileSystemVolume *g_FS_Volumes;

static int32_t RemoveAllFiles(const char *path);
static int NormalizePath(const char *root, const char *path, char *buffer, size_t bufferSize);

bool FATFS_FS_Driver::LoadMedia(const void *driverInterface)
{
    (void)driverInterface;
    return TRUE;
}

STREAM_DRIVER_DETAILS *FATFS_FS_Driver::DriverDetails(const VOLUME_ID *volume)
{
    (void)volume;

    static STREAM_DRIVER_DETAILS driverDetail = {DIRECT_IO, NULL, NULL, 0, 0, TRUE, TRUE, TRUE, 0, 0};

    return &driverDetail;
}

//--//

void FATFS_FS_Driver::Initialize()
{
    // nothing to do here as IDF takes care of this
}

bool FATFS_FS_Driver::InitializeVolume(const VOLUME_ID *volume, const char *path)
{
    // nothing to do here as the mount API will take care of this

    return TRUE;
}

bool FATFS_FS_Driver::UnInitializeVolume(const VOLUME_ID *volume)
{
    // nothing to do here as the unmount API will take care of this

    return TRUE;
}

HRESULT FATFS_FS_Driver::Format(const VOLUME_ID *volume, const char *volumeLabel, uint32_t parameters)
{
    NANOCLR_HEADER();

    //     (void)volumeLabel;
    //     (void)parameters;

    //     FileSystemVolume *currentVolume = FileSystemVolumeList::FindVolume(volume->volumeId);
    //     f_chdrive(currentVolume->m_rootName);

    // #if FF_USE_MKFS
    //     MKFS_PARM mkfsParameters = {FM_FAT32, 1, 0, 0, 0};

    //     if (f_mkfs(currentVolume->m_rootName, &mkfsParameters, fatfs_inputBuffer, sizeof(fatfs_inputBuffer)) ==
    //     FR_OK)
    //     {
    //         // volume formatted
    //         hr = S_OK;
    //     }
    //     else
    //     {
    //         // failed to format the volume
    //         NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
    //     }
    // #else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
    // #endif

    NANOCLR_NOCLEANUP();
}

HRESULT FATFS_FS_Driver::GetSizeInfo(const VOLUME_ID *volume, int64_t *totalSize, int64_t *totalFreeSpace)
{
    (void)totalSize;

    // FATFS *fsPtr = &fs;
    // char buffer[3];
    // DWORD freeClusters, freeSectors, totalSectors;

    // FATFS *fs = GetFatFsByVolumeId(volume, false);

    FileSystemVolume *currentVolume = FileSystemVolumeList::FindVolume(volume->volumeId);

    f_chdrive(currentVolume->m_rootName);

    // this call is prone to take a long time, thus hitting the watchdog, therefore we are skipping this for now
    //     // get free clusters
    //     f_getfree(buffer, &freeClusters, &fsPtr);

    //     // Get total sectors and free sectors
    //     totalSectors = (fs.n_fatent - 2) * fs.csize;
    //     freeSectors = freeClusters * fs.csize;

    // #if FF_MAX_SS != FF_MIN_SS
    //     *totalSize = (int64_t)totalSectors * fs.ssize;
    //     *totalFreeSpace = (int64_t)freeSectors * fs.ssize;
    // #else
    //     *totalSize = (int64_t)totalSectors * FF_MAX_SS;
    //     *totalFreeSpace = (int64_t)freeSectors * FF_MAX_SS;
    // #endif

    *totalSize = -1;
    *totalFreeSpace = -1;

    return S_OK;
}

HRESULT FATFS_FS_Driver::FlushAll(const VOLUME_ID *volume)
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

HRESULT FATFS_FS_Driver::GetVolumeLabel(const VOLUME_ID *volume, char *volumeLabel, int32_t volumeLabelLen)
{
    (void)volume;
    (void)volumeLabel;
    (void)volumeLabelLen;

    // FATFS *fs = NULL;

    // //fs = GetFileSystemForVolume(volume, true);

    // if (fs == NULL)
    // {
    //     return FALSE;
    // }

    // memcpy(volumeLabel, fatFS.lfnbuf, volumeLabelLen);

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

HRESULT FATFS_FS_Driver::Open(const VOLUME_ID *volume, const char *path, void *&handle)
{
    NANOCLR_HEADER();

#ifdef DEBUG
    int32_t result;
#endif

    FATFS_FileHandle *fileHandle = NULL;
    FILINFO info;
    int32_t flags;
    char normalizedPath[FS_MAX_DIRECTORY_LENGTH];
    bool fileExists = false;
    FileSystemVolume *currentVolume;

    // allocate file handle
    fileHandle = (FATFS_FileHandle *)platform_malloc(sizeof(FATFS_FileHandle));

    if (fileHandle == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    memset(fileHandle, 0, sizeof(FATFS_FileHandle));

    currentVolume = FileSystemVolumeList::FindVolume(volume->volumeId);

    if (NormalizePath(currentVolume->m_rootName, path, normalizedPath, sizeof(normalizedPath)) < 0)
    {
        // handle error
        return CLR_E_PATH_TOO_LONG;
    }

    // check for file existence
#ifdef DEBUG
    result = f_stat(normalizedPath, &info);

    fileExists = result == FR_OK;
#else
    fileExists = f_stat(normalizedPath, &info) == FR_OK;
#endif

    if (fileExists)
    {
        // file already exists, open for R/W
        flags = FA_OPEN_EXISTING | FA_WRITE | FA_READ;
    }
    else
    {
        // file doesn't exist, creat and open for R/W
        flags = FA_CREATE_NEW | FA_WRITE | FA_READ;
    }

    if (f_open(&fileHandle->file, normalizedPath, flags) == FR_OK)
    {
        // store the handle
        handle = fileHandle;

        // store attributes
        f_chmod(normalizedPath, AM_ARC, AM_ARC);

        if (!fileExists)
        {
            // sync file to save attributes
            f_sync(&fileHandle->file);
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

HRESULT FATFS_FS_Driver::Close(void *handle)
{
    NANOCLR_HEADER();

    FATFS_FileHandle *fileHandle;

    if (handle == 0)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    fileHandle = (FATFS_FileHandle *)handle;

    NANOCLR_SET_AND_LEAVE(f_close(&fileHandle->file) == FR_OK ? S_OK : CLR_E_FILE_IO);

    NANOCLR_CLEANUP();

    platform_free(fileHandle);

    NANOCLR_CLEANUP_END();
}

HRESULT FATFS_FS_Driver::Read(void *handle, uint8_t *buffer, int size, int *bytesRead)
{
    NANOCLR_HEADER();

    unsigned int readCount = 0;
    FRESULT result = FR_OK;
    FATFS_FileHandle *fileHandle;

    if (handle == 0)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    if (!buffer || size < 0 || !bytesRead)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    fileHandle = (FATFS_FileHandle *)handle;

    // read from the file
    result = f_read(&fileHandle->file, buffer, size, &readCount);

    if (result == FR_OK)
    {
        *bytesRead = readCount;

        // check for EOF
        if (readCount == 0 && f_eof(&fileHandle->file))
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

HRESULT FATFS_FS_Driver::Write(void *handle, uint8_t *buffer, int size, int *bytesWritten)
{
    NANOCLR_HEADER();

    unsigned int writeCount = 0;
    FRESULT result;
    FATFS_FileHandle *fileHandle;

    if (handle == 0)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    if (size < 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    fileHandle = (FATFS_FileHandle *)handle;

    // write to the file
    result = f_write(&fileHandle->file, buffer, size, &writeCount);

    if (result != FR_OK)
    {
        // failed to write to the file
        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
    }

    f_sync(&fileHandle->file);

    *bytesWritten = writeCount;

    hr = S_OK;

    NANOCLR_NOCLEANUP();
}

HRESULT FATFS_FS_Driver::Flush(void *handle)
{
    FATFS_FileHandle *fileHandle;

    if (handle == 0)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    fileHandle = (FATFS_FileHandle *)handle;

    if (f_sync(&fileHandle->file) != FR_OK)
    {
        // failed to access the file
        return CLR_E_FILE_IO;
    }

    return S_OK;
}

HRESULT FATFS_FS_Driver::Seek(void *handle, int64_t offset, uint32_t origin, int64_t *position)
{
    FATFS_FileHandle *fileHandle;
    FRESULT result;

    if (handle == 0)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    fileHandle = (FATFS_FileHandle *)handle;

    // map the origin to littlefs
    switch (origin)
    {
        case SEEKORIGIN_BEGIN:
            // 1st need to rewind the file to get to a known position
            f_rewind(&fileHandle->file);
            break;

        case SEEKORIGIN_CURRENT:
            // get the current position
            offset += f_tell(&fileHandle->file);
            break;

        case SEEKORIGIN_END:
            // get the file size
            offset += f_size(&fileHandle->file);
            break;
    }

    // seek to the position
    result = f_lseek(&fileHandle->file, offset);

    if (result != FR_OK)
    {
        // failed to access the file
        return CLR_E_FILE_IO;
    }

    // get the current position
    *position = f_tell(&fileHandle->file);

    return S_OK;
}

HRESULT FATFS_FS_Driver::GetLength(void *handle, int64_t *length)
{
    FATFS_FileHandle *fileHandle;

    if (handle == 0)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    fileHandle = (FATFS_FileHandle *)handle;

    *length = f_size(&fileHandle->file);

    return S_OK;
}

HRESULT FATFS_FS_Driver::SetLength(void *handle, int64_t length)
{
    FATFS_FileHandle *fileHandle;
    FRESULT result;
    uint32_t currentPosition;

    if (handle == 0)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    fileHandle = (FATFS_FileHandle *)handle;

    // store current position
    currentPosition = f_tell(&fileHandle->file);

    // move the file pointer to the desired position
    result = f_lseek(&fileHandle->file, length);

    if (result != FR_OK)
    {
        return CLR_E_FILE_IO;
    }

    // truncate the file at the current position
    result = f_truncate(&fileHandle->file);

    if (result != FR_OK)
    {
        return CLR_E_FILE_IO;
    }

    // restore file position
    f_lseek(&fileHandle->file, currentPosition);

    f_sync(&fileHandle->file);

    return S_OK;
}

//--//

HRESULT FATFS_FS_Driver::FindOpen(const VOLUME_ID *volume, const char *path, void *&handle)
{
    NANOCLR_HEADER();

    char normalizedPath[FS_MAX_DIRECTORY_LENGTH];
    FATFS_FindFileHandle *findHandle = NULL;
    FileSystemVolume *currentVolume;

    // allocate file handle
    findHandle = (FATFS_FindFileHandle *)platform_malloc(sizeof(FATFS_FindFileHandle));

    if (findHandle == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    memset(findHandle, 0, sizeof(FATFS_FindFileHandle));

    currentVolume = FileSystemVolumeList::FindVolume(volume->volumeId);

    if (NormalizePath(currentVolume->m_rootName, path, normalizedPath, sizeof(normalizedPath)) < 0)
    {
        // handle error
        return CLR_E_PATH_TOO_LONG;
    }

    // open directory for seek
    if (f_opendir(&findHandle->dir, normalizedPath) == FR_OK)
    {
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

HRESULT FATFS_FS_Driver::FindNext(void *handle, FS_FILEINFO *fi, bool *fileFound)
{
    NANOCLR_HEADER();

    FATFS_FindFileHandle *findHandle;
    FILINFO info;
    FRESULT result;

    if (handle == 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // check file parameters
    if (!fi || !fileFound)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
    }

    findHandle = (FATFS_FindFileHandle *)handle;

    // read the next entry
    result = f_readdir(&findHandle->dir, &info);

    if (result != FR_OK)
    {
        // something went wrong
        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
    }
    else if (result == FR_OK && info.fname[0] == 0)
    {
        // no more entries
        *fileFound = false;
        NANOCLR_SET_AND_LEAVE(S_OK);
    }

    // found an entry
    *fileFound = true;

    // set name size
    fi->FileNameSize = hal_strlen_s(info.fname);

    // allocate memory for the name
    // MUST BE FREED BY THE CALLER
    fi->FileName = (char *)platform_malloc(fi->FileNameSize + 1);

    // sanity check for successfull malloc
    if (fi->FileName == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    // copy the name, including the string terminator
    hal_strcpy_s((char *)fi->FileName, fi->FileNameSize + 1, info.fname);

    // store the attributes
    fi->Attributes = info.fattrib;

    // set the size
    fi->Size = info.fsize;

    NANOCLR_NOCLEANUP();
}

HRESULT FATFS_FS_Driver::FindClose(void *handle)
{
    FATFS_FindFileHandle *findHandle;

    if (handle != 0)
    {
        findHandle = (FATFS_FindFileHandle *)handle;

        f_closedir(&findHandle->dir);

        platform_free(findHandle);
    }

    return S_OK;
}

HRESULT FATFS_FS_Driver::GetFileInfo(const VOLUME_ID *volume, const char *path, FS_FILEINFO *fileInfo, bool *found)
{
    FILINFO info;
    char normalizedPath[FS_MAX_DIRECTORY_LENGTH];

    FileSystemVolume *currentVolume = FileSystemVolumeList::FindVolume(volume->volumeId);

    if (NormalizePath(currentVolume->m_rootName, path, normalizedPath, sizeof(normalizedPath)) < 0)
    {
        // handle error
        return CLR_E_PATH_TOO_LONG;
    }

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
        if (f_stat(normalizedPath, &info) != FR_OK)
        {
            *found = false;
            return S_OK;
        }

        // set found flag
        *found = true;

        // store the attributes
        if (S_ISDIR(info.fattrib))
        {
            fileInfo->Attributes = FileAttributes::FileAttributes_Directory;
        }
        else
        {
            fileInfo->Attributes = FileAttributes::FileAttributes_Normal;
        }

        // store the attributes
        fileInfo->Attributes = info.fattrib;

        // set the file size
        fileInfo->Size = info.fsize;

        // no need to set the file name details as managed code already has this info
    }

    return S_OK;
}

HRESULT FATFS_FS_Driver::GetAttributes(const VOLUME_ID *volume, const char *path, uint32_t *attributes)
{
    NANOCLR_HEADER();

    FILINFO info;
    int32_t result;
    char normalizedPath[FS_MAX_DIRECTORY_LENGTH];
    size_t pathLength;

    // set to empty attributes
    *attributes = EMPTY_ATTRIBUTE;

    pathLength = hal_strlen_s(path);

    FileSystemVolume *currentVolume = FileSystemVolumeList::FindVolume(volume->volumeId);

    if (NormalizePath(currentVolume->m_rootName, path, normalizedPath, sizeof(normalizedPath)) < 0)
    {
        // handle error
        return CLR_E_PATH_TOO_LONG;
    }

    // check for file existence
    result = f_stat(normalizedPath, &info);

    if (result == FR_NO_PATH || result == FR_NO_FILE)
    {
        // file doesn't exist
        // even if this fails we return success as attributes have been set to EMPTY_ATTRIBUTE
        NANOCLR_SET_AND_LEAVE(S_OK);
    }

    // if this is home directory
    if (pathLength == 1 && *normalizedPath == '\0')
    {
        *attributes = FileAttributes::FileAttributes_Directory;

        NANOCLR_SET_AND_LEAVE(S_OK);
    }

    // store attributes
    *attributes = info.fattrib;

    // check if this is a directory and fix the attributes
    if (info.fattrib & AM_DIR)
    {
        *attributes |= FileAttributes::FileAttributes_Directory;
    }

    NANOCLR_SET_AND_LEAVE(S_OK);

    NANOCLR_NOCLEANUP();
}

HRESULT FATFS_FS_Driver::SetAttributes(const VOLUME_ID *volume, const char *path, uint32_t attributes)
{
    FILINFO info;
    char normalizedPath[FS_MAX_DIRECTORY_LENGTH];

    FileSystemVolume *currentVolume = FileSystemVolumeList::FindVolume(volume->volumeId);

    if (NormalizePath(currentVolume->m_rootName, path, normalizedPath, sizeof(normalizedPath)) < 0)
    {
        // handle error
        return CLR_E_PATH_TOO_LONG;
    }

    // check for file existence
    if (f_stat(normalizedPath, &info) != FR_OK)
    {
        return CLR_E_FILE_NOT_FOUND;
    }

    if (f_chmod((const char *)normalizedPath, attributes, attributes) != FR_OK)
    {
        return CLR_E_FILE_IO;
    }

    return S_OK;
}

HRESULT FATFS_FS_Driver::CreateDirectory(const VOLUME_ID *volume, const char *path)
{
    FILINFO info;
    int32_t result = FR_OK;
    char normalizedPath[FS_MAX_DIRECTORY_LENGTH];
    char tempPath[FS_MAX_DIRECTORY_LENGTH + 1];
    char *segment;
    int32_t dirExists;

    (void)dirExists;

    FileSystemVolume *currentVolume = FileSystemVolumeList::FindVolume(volume->volumeId);

    if (NormalizePath(currentVolume->m_rootName, path, normalizedPath, sizeof(normalizedPath)) < 0)
    {
        // handle error
        return CLR_E_PATH_TOO_LONG;
    }

    memset(tempPath, 0, sizeof(tempPath));

    // iterate over the path segments and create the directories
    segment = strtok(normalizedPath, "/");

    while (segment && (result == FR_OK || result == FR_EXIST))
    {
        strcat(tempPath, segment);

        result = f_mkdir(tempPath);

        if (result != FR_OK && result != FR_EXIST)
        {
            return CLR_E_FILE_IO;
        }

        // add back the '/' separator
        strcat(tempPath, "/");

        segment = strtok(NULL, "/");
    }

    // remove trailing '/'
    tempPath[hal_strlen_s(tempPath) - 1] = '\0';

    // sanity check for success
    dirExists = f_stat(tempPath, &info);

    // sanity check for success
    if (dirExists == FR_OK || dirExists == FR_EXIST)
    {
        return S_OK;
    }
    else
    {
        return CLR_E_FILE_IO;
    }

    return CLR_E_INVALID_DRIVER;
}

HRESULT FATFS_FS_Driver::Move(const VOLUME_ID *volume, const char *oldPath, const char *newPath)
{
    char normalizedNewPath[FS_MAX_DIRECTORY_LENGTH];
    char normalizedOldPath[FS_MAX_DIRECTORY_LENGTH];
    int32_t result = FR_OK;

    FileSystemVolume *currentVolume = FileSystemVolumeList::FindVolume(volume->volumeId);

    if (NormalizePath(currentVolume->m_rootName, newPath, normalizedNewPath, sizeof(normalizedNewPath)) < 0)
    {
        // handle error
        return CLR_E_PATH_TOO_LONG;
    }

    if (NormalizePath(currentVolume->m_rootName, oldPath, normalizedOldPath, sizeof(normalizedOldPath)) < 0)
    {
        // handle error
        return CLR_E_PATH_TOO_LONG;
    }

    // the check for source file and destination file existence has already been made in managed code
    result = f_rename(normalizedOldPath, normalizedNewPath);

    if (result == FR_OK)
    {
        return S_OK;
    }
    else if (result == FR_EXIST)
    {
        // failed to move directory because it's not empty
        return S_FALSE;
    }
    else
    {
        return CLR_E_FILE_IO;
    }
}

HRESULT FATFS_FS_Driver::Delete(const VOLUME_ID *volume, const char *path, bool recursive)
{
    FILINFO info;
    char normalizedPath[FS_MAX_DIRECTORY_LENGTH];
    int32_t result;

    FileSystemVolume *currentVolume = FileSystemVolumeList::FindVolume(volume->volumeId);

    if (NormalizePath(currentVolume->m_rootName, path, normalizedPath, sizeof(normalizedPath)) < 0)
    {
        // handle error
        return CLR_E_PATH_TOO_LONG;
    }

    // check for file existence
    if (f_stat(normalizedPath, &info) != FR_OK)
    {
        return CLR_E_FILE_NOT_FOUND;
    }

remove_entry:
    // remove the directory
    result = f_rmdir(normalizedPath);

    if (result == FR_DENIED)
    {
        if (!recursive)
        {
            // directory is not empty and we are not in recursive mode
            return CLR_E_DIRECTORY_NOT_EMPTY;
        }

        // recursivelly delete all files and subdirectories
        result = RemoveAllFiles(normalizedPath);
        if (result != FR_OK)
        {
            return CLR_E_FILE_IO;
        }

        goto remove_entry;
    }
    else if (result != FR_OK)
    {
        return CLR_E_FILE_IO;
    }

    return S_OK;
}

static int32_t RemoveAllFiles(const char *path)
{
    FF_DIR dir;
    FILINFO info;
    int32_t result;
    char buffer[FS_MAX_PATH_LENGTH];
    char *bufferP = buffer;
    size_t bufferSize = FS_MAX_PATH_LENGTH;

    // open the directory
    result = f_opendir(&dir, path);
    if (result != FR_OK)
    {
        return result;
    }

    // read the directory
    while (!(f_readdir(&dir, &info) == FR_OK && info.fname[0] == 0))
    {
        // reset and prepare the buffer
        memset(buffer, 0, FS_MAX_PATH_LENGTH);
        bufferP = buffer;

        // join filename with the path
        CLR_SafeSprintf(bufferP, bufferSize, "%s/%s", path, info.fname);

        // check if this is a directory
        if (info.fattrib & AM_DIR)
        {
            // recursivelly delete all files and subdirectories
            result = RemoveAllFiles(buffer);

            if (result != FR_OK)
            {
                return result;
            }
        }

        // remove the entry (OK to remove files and directories)
        result = f_rmdir(buffer);
        if (result != FR_OK)
        {
            return result;
        }
    }

    // close the directory
    return f_closedir(&dir);
}

// normalize path and convert to ESP32 VFS by:
// - replace \ with /
// - remove leading slash
// - handle other normalization issues
static int NormalizePath(const char *root, const char *path, char *buffer, size_t bufferSize)
{
    const char *workingPath = path;
    char *bufferP = buffer;

    // start with /
    *bufferP++ = '/';

    // copy the root (presumed to be a drive letter
    *bufferP++ = *root++;

    // drop ':' from the path
    // add a '/' after the drive letter
    *bufferP++ = '/';

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

    return 0;
}

//////////////////

STREAM_DRIVER_INTERFACE g_FATFS_STREAM_DriverInterface = {
    &FATFS_FS_Driver::Initialize,
    &FATFS_FS_Driver::InitializeVolume,
    &FATFS_FS_Driver::UnInitializeVolume,
    &FATFS_FS_Driver::DriverDetails,
    &FATFS_FS_Driver::Open,
    &FATFS_FS_Driver::Close,
    &FATFS_FS_Driver::Read,
    &FATFS_FS_Driver::Write,
    &FATFS_FS_Driver::Flush,
    &FATFS_FS_Driver::Seek,
    &FATFS_FS_Driver::GetLength,
    &FATFS_FS_Driver::SetLength,
};

FILESYSTEM_DRIVER_INTERFACE g_FATFS_FILE_SYSTEM_DriverInterface = {
    &FATFS_FS_Driver::FindOpen,
    &FATFS_FS_Driver::FindNext,
    &FATFS_FS_Driver::FindClose,

    &FATFS_FS_Driver::GetFileInfo,

    &FATFS_FS_Driver::CreateDirectory,
    &FATFS_FS_Driver::Move,
    &FATFS_FS_Driver::Delete,

    &FATFS_FS_Driver::GetAttributes,
    &FATFS_FS_Driver::SetAttributes,

    &FATFS_FS_Driver::Format,
    &FATFS_FS_Driver::LoadMedia,
    &FATFS_FS_Driver::GetSizeInfo,
    &FATFS_FS_Driver::FlushAll,
    &FATFS_FS_Driver::GetVolumeLabel,

    "FATFS",
    0,
};
