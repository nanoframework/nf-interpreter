//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nf_sys_io_filesystem.h>
#include "littlefs_FS_Driver.h"
#include <stdlib.h>

extern FileSystemVolume *g_FS_Volumes;

static int32_t RemoveAllFiles(const char *path);
static int NormalizePath(const char *root, const char *path, char *buffer, size_t bufferSize);

bool LITTLEFS_FS_Driver::LoadMedia(const void *driverInterface)
{
    (void)driverInterface;
    return TRUE;
}

STREAM_DRIVER_DETAILS *LITTLEFS_FS_Driver::DriverDetails(const VOLUME_ID *volume)
{
    (void)volume;

    static STREAM_DRIVER_DETAILS driverDetail = {DIRECT_IO, NULL, NULL, 0, 0, TRUE, TRUE, TRUE, 0, 0};

    return &driverDetail;
}

//--//

void LITTLEFS_FS_Driver::Initialize()
{
    // nothing to do here as this was already initilized with config storage init
}

bool LITTLEFS_FS_Driver::InitializeVolume(const VOLUME_ID *volume, const char *path)
{
    // nothing to do here as the mount API will take care of this

    return TRUE;
}

bool LITTLEFS_FS_Driver::UnInitializeVolume(const VOLUME_ID *volume)
{
    // nothing to do here as the unmount API will take care of this

    return TRUE;
}

HRESULT LITTLEFS_FS_Driver::Format(const VOLUME_ID *volume, const char *volumeLabel, uint32_t parameters)
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

HRESULT LITTLEFS_FS_Driver::GetSizeInfo(const VOLUME_ID *volume, int64_t *totalSize, int64_t *totalFreeSpace)
{
    (void)totalSize;

    // FATFS *fsPtr = &fs;
    // char buffer[3];
    // DWORD freeClusters, freeSectors, totalSectors;

    // FATFS *fs = GetFatFsByVolumeId(volume, false);

    // FileSystemVolume *currentVolume = FileSystemVolumeList::FindVolume(volume->volumeId);

    // f_chdrive(currentVolume->m_rootName);

    // // this call is prone to take a long time, thus hitting the watchdog, therefore we are skipping this for now
    // //     // get free clusters
    // //     f_getfree(buffer, &freeClusters, &fsPtr);

    // //     // Get total sectors and free sectors
    // //     totalSectors = (fs.n_fatent - 2) * fs.csize;
    // //     freeSectors = freeClusters * fs.csize;

    // // #if FF_MAX_SS != FF_MIN_SS
    // //     *totalSize = (int64_t)totalSectors * fs.ssize;
    // //     *totalFreeSpace = (int64_t)freeSectors * fs.ssize;
    // // #else
    // //     *totalSize = (int64_t)totalSectors * FF_MAX_SS;
    // //     *totalFreeSpace = (int64_t)freeSectors * FF_MAX_SS;
    // // #endif

    *totalSize = -1;
    *totalFreeSpace = -1;

    return S_OK;
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

HRESULT LITTLEFS_FS_Driver::Open(const VOLUME_ID *volume, const char *path, void *&handle)
{
    NANOCLR_HEADER();

#ifdef DEBUG
    int32_t result;
#endif

    LITTLEFS_FileHandle *fileHandle = NULL;
    struct stat info;
    const char *flags;
    char normalizedPath[FS_MAX_DIRECTORY_LENGTH];
    bool fileExists = false;
    FileSystemVolume *currentVolume;

    // allocate file handle
    fileHandle = (LITTLEFS_FileHandle *)platform_malloc(sizeof(LITTLEFS_FileHandle));

    if (fileHandle == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    memset(fileHandle, 0, sizeof(LITTLEFS_FileHandle));

    currentVolume = FileSystemVolumeList::FindVolume(volume->volumeId);

    if (NormalizePath(currentVolume->m_rootName, path, normalizedPath, sizeof(normalizedPath)) < 0)
    {
        // handle error
        return CLR_E_PATH_TOO_LONG;
    }

    // check for file existence
#ifdef DEBUG
    result = stat(normalizedPath, &info);

    fileExists = (result == FR_OK);
#else
    fileExists = (stat(normalizedPath, &info) == FR_OK);
#endif

    if (fileExists)
    {
        // file already exists, open for R/W
        flags = "r+";
    }
    else
    {
        // file doesn't exist, create and open for R/W
        flags = "w+";
    }
    fileHandle->file = fopen(normalizedPath, flags);
    if (fileHandle->file != NULL)
    {
        // store the handle
        handle = fileHandle;

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

    NANOCLR_SET_AND_LEAVE(fclose(fileHandle->file) == FR_OK ? S_OK : CLR_E_FILE_IO);

    NANOCLR_CLEANUP();

    platform_free(fileHandle);

    NANOCLR_CLEANUP_END();
}

HRESULT LITTLEFS_FS_Driver::Read(void *handle, uint8_t *buffer, int size, int *bytesRead)
{
    NANOCLR_HEADER();

    unsigned int readCount = 0;
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

    // read from the file
    readCount = fread(buffer, 1, size, fileHandle->file);

    if (readCount > 0)
    {
        *bytesRead = readCount;
    }
    else if (feof(fileHandle->file))
    {
        // signal EOF
        *bytesRead = -1;
    }
    else if (ferror(fileHandle->file))
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

    unsigned int writeCount = 0;
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
    writeCount = fwrite(buffer, 1, size, fileHandle->file);

    if (writeCount < size)
    {
        // failed to write the full buffer to the file
        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
    }

    fflush(fileHandle->file);

    *bytesWritten = writeCount;

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

    if (fflush(fileHandle->file) != FR_OK)
    {
        // failed to access the file
        return CLR_E_FILE_IO;
    }

    return S_OK;
}

HRESULT LITTLEFS_FS_Driver::Seek(void *handle, int64_t offset, uint32_t origin, int64_t *position)
{
    LITTLEFS_FileHandle *fileHandle;

    if (handle == 0)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    fileHandle = (LITTLEFS_FileHandle *)handle;

    // map the origin to POSIX fseek
    switch (origin)
    {
        case SEEKORIGIN_BEGIN:
            // 1st need to rewind the file to get to a known position
            rewind(fileHandle->file);
            break;

        case SEEKORIGIN_CURRENT:
            // fseek does not require adjustment for SEEK_CUR, it moves from current position
            break;

        case SEEKORIGIN_END:
            // fseek can directly seek to the end of the file
            break;
    }

    // seek to the position
    if (fseek(fileHandle->file, offset, origin) != 0)
    {
        // failed to access the file
        return CLR_E_FILE_IO;
    }

    // get the current position
    *position = ftell(fileHandle->file);

    if (*position == -1L)
    {
        // failed to get the current position
        return CLR_E_FILE_IO;
    }

    return S_OK;
}

HRESULT LITTLEFS_FS_Driver::GetLength(void *handle, int64_t *length)
{
    LITTLEFS_FileHandle *fileHandle;
    long currentPosition;

    if (handle == 0)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    fileHandle = (LITTLEFS_FileHandle *)handle;

    // Store current position
    currentPosition = ftell(fileHandle->file);
    if (currentPosition == -1L)
    {
        return CLR_E_FILE_IO;
    }

    // Move to the end of the file to determine its size
    if (fseek(fileHandle->file, 0, SEEK_END) != 0)
    {
        // Handle error, possibly set *length to 0 or an error code
        return CLR_E_FILE_IO;
    }

    // Get the current position, which is the size of the file
    *length = ftell(fileHandle->file);

    // Restore file position
    if (fseek(fileHandle->file, currentPosition, SEEK_SET) != 0)
    {
        // Handle error
        return CLR_E_FILE_IO;
    }

    return S_OK;
}

HRESULT LITTLEFS_FS_Driver::SetLength(void *handle, int64_t length)
{
    LITTLEFS_FileHandle *fileHandle;
    long currentPosition;

    if (handle == 0)
    {
        return CLR_E_INVALID_PARAMETER;
    }

    fileHandle = (LITTLEFS_FileHandle *)handle;

    // Store current position
    currentPosition = ftell(fileHandle->file);
    if (currentPosition == -1L)
    {
        // Handle error
        return CLR_E_FILE_IO;
    }

    // Move the file pointer to the desired position
    if (fseek(fileHandle->file, length, SEEK_SET) != 0)
    {
        // Handle error
        return CLR_E_FILE_IO;
    }

    // Truncate the file at the current position
    if (ftruncate(fileno(fileHandle->file), length) != 0)
    {
        // Handle error
        return CLR_E_FILE_IO;
    }

    // Restore file position
    if (fseek(fileHandle->file, currentPosition, SEEK_SET) != 0)
    {
        // Handle error
        return CLR_E_FILE_IO;
    }

    // Synchronize the file state
    fflush(fileHandle->file);

    return S_OK;
}

//--//

HRESULT LITTLEFS_FS_Driver::FindOpen(const VOLUME_ID *volume, const char *path, void *&handle)
{
    NANOCLR_HEADER();

    char normalizedPath[FS_MAX_DIRECTORY_LENGTH];
    LITTLEFS_FindFileHandle *findHandle = NULL;
    FileSystemVolume *currentVolume;

    // allocate file handle
    findHandle = (LITTLEFS_FindFileHandle *)platform_malloc(sizeof(LITTLEFS_FindFileHandle));

    if (findHandle == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    memset(findHandle, 0, sizeof(LITTLEFS_FindFileHandle));

    currentVolume = FileSystemVolumeList::FindVolume(volume->volumeId);

    if (NormalizePath(currentVolume->m_rootName, path, normalizedPath, sizeof(normalizedPath)) < 0)
    {
        // handle error
        return CLR_E_PATH_TOO_LONG;
    }

    // save base path
    hal_strcpy_s(findHandle->basePath, sizeof(findHandle->basePath), normalizedPath);

    // open directory for seek
    findHandle->dir = opendir(normalizedPath);
    if (findHandle->dir != NULL)
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

HRESULT LITTLEFS_FS_Driver::FindNext(void *handle, FS_FILEINFO *fi, bool *fileFound)
{
    NANOCLR_HEADER();

    LITTLEFS_FindFileHandle *findHandle;
    char buffer[FS_MAX_PATH_LENGTH];

    struct stat info;
    struct dirent *entry;

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
    entry = readdir(findHandle->dir);

    if (entry == NULL)
    {
        // readdir() returns NULL on error or if end of directory is reached

        // End of directory
        *fileFound = false;

        // To distinguish between the two, you can check errno
        if (errno == 0)
        {
            NANOCLR_SET_AND_LEAVE(S_OK);
        }
        else
        {
            // Error occurred
            NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
        }
    }
    else
    {
        if (entry->d_name[0] == 0)
        {
            // end of directory
            *fileFound = false;
        }
        else
        {
            // Found an entry
            *fileFound = true;

            // Set name size
            fi->FileNameSize = hal_strlen_s(entry->d_name);

            // Allocate memory for the name
            // MUST BE FREED BY THE CALLER
            fi->FileName = (char *)platform_malloc(fi->FileNameSize + 1);

            // Sanity check for successful malloc
            if (fi->FileName == NULL)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
            }

            // Copy the name, including the string terminator
            hal_strcpy_s((char *)fi->FileName, fi->FileNameSize + 1, entry->d_name);

            // compose the full path of the current entry
            snprintf(buffer, FS_MAX_PATH_LENGTH, "%s%s", findHandle->basePath, entry->d_name);

            // POSIX dirent does not directly provide file attributes or size
            // need to use stat() function to retrieve that information
            if (stat((char *)buffer, &info) == 0)
            {
                if (S_ISDIR(info.st_mode))
                {
                    fi->Attributes = FileAttributes::FileAttributes_Directory;
                }
                else
                {
                    fi->Attributes = FileAttributes::FileAttributes_Normal;
                }

                // POSIX API does not provide file size
            }
            else
            {
                // Error occurred
                NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
            }
        }
    }

    NANOCLR_NOCLEANUP();
}

HRESULT LITTLEFS_FS_Driver::FindClose(void *handle)
{
    LITTLEFS_FindFileHandle *findHandle;

    if (handle != 0)
    {
        findHandle = (LITTLEFS_FindFileHandle *)handle;

        closedir(findHandle->dir);

        platform_free(findHandle);
    }

    return S_OK;
}

HRESULT LITTLEFS_FS_Driver::GetFileInfo(const VOLUME_ID *volume, const char *path, FS_FILEINFO *fileInfo, bool *found)
{
    struct stat info;
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
        if (stat(normalizedPath, &info) != FR_OK)
        {
            *found = false;
            return S_OK;
        }

        // set found flag
        *found = true;

        // store the attributes
        if (S_ISDIR(info.st_mode))
        {
            fileInfo->Attributes = FileAttributes::FileAttributes_Directory;
        }
        else
        {
            fileInfo->Attributes = FileAttributes::FileAttributes_Normal;
        }

        // set the file size
        fileInfo->Size = info.st_size;

        // no need to set the file name details as managed code already has this info
    }

    return S_OK;
}

HRESULT LITTLEFS_FS_Driver::GetAttributes(const VOLUME_ID *volume, const char *path, uint32_t *attributes)
{
    NANOCLR_HEADER();

    struct stat info;
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
    result = stat(normalizedPath, &info);

    if (result == -1)
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
    *attributes = 0;

    // check if this is a directory and fix the attributes
    if (S_ISDIR(info.st_mode))
    {
        *attributes |= FileAttributes::FileAttributes_Directory;
    }

    NANOCLR_SET_AND_LEAVE(S_OK);

    NANOCLR_NOCLEANUP();
}

HRESULT LITTLEFS_FS_Driver::SetAttributes(const VOLUME_ID *volume, const char *path, uint32_t attributes)
{
    struct stat info;
    mode_t newMode;
    char normalizedPath[FS_MAX_DIRECTORY_LENGTH];

    FileSystemVolume *currentVolume = FileSystemVolumeList::FindVolume(volume->volumeId);

    if (NormalizePath(currentVolume->m_rootName, path, normalizedPath, sizeof(normalizedPath)) < 0)
    {
        // handle error
        return CLR_E_PATH_TOO_LONG;
    }

    // Retrieve current file status
    if (stat(normalizedPath, &info) != 0)
    {
        // Handle error for stat failure
        return CLR_E_FILE_IO;
    }

    newMode = info.st_mode;

    if (attributes & FileAttributes::FileAttributes_ReadOnly)
    {
        // Set file as read-only (remove write permissions)
        newMode &= ~(S_IWUSR | S_IWGRP | S_IWOTH);
    }
    else
    {
        // Clear read-only attribute (add write permissions)
        // Assuming the default is to add write permission for the owner only
        newMode |= S_IWUSR;
    }

    return S_OK;
}

HRESULT LITTLEFS_FS_Driver::CreateDirectory(const VOLUME_ID *volume, const char *path)
{
    struct stat info;
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

    strcat(tempPath, "/");

    // iterate over the path segments and create the directories
    // 1st segment is the root directory, so need to skip that
    segment = strtok(normalizedPath, "/");
    strcat(tempPath, segment);
    strcat(tempPath, "/");
    segment = strtok(NULL, "/");

    while (segment)
    {
        strcat(tempPath, segment);

        result = mkdir(tempPath, 0);

        if (result != 0 && errno != EEXIST)
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
    dirExists = stat(tempPath, &info);

    // sanity check for success
    if (dirExists == 0)
    {
        return S_OK;
    }
    else
    {
        return CLR_E_FILE_IO;
    }

    return CLR_E_INVALID_DRIVER;
}

HRESULT LITTLEFS_FS_Driver::Move(const VOLUME_ID *volume, const char *oldPath, const char *newPath)
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
    result = rename(normalizedOldPath, normalizedNewPath);

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

HRESULT LITTLEFS_FS_Driver::Delete(const VOLUME_ID *volume, const char *path, bool recursive)
{
    struct stat info;
    char normalizedPath[FS_MAX_DIRECTORY_LENGTH];
    int32_t result;

    FileSystemVolume *currentVolume = FileSystemVolumeList::FindVolume(volume->volumeId);

    if (NormalizePath(currentVolume->m_rootName, path, normalizedPath, sizeof(normalizedPath)) < 0)
    {
        // handle error
        return CLR_E_PATH_TOO_LONG;
    }

remove_entry:

    // check for file existence
    if (stat(normalizedPath, &info) != FR_OK)
    {
        return CLR_E_FILE_NOT_FOUND;
    }

    if (!S_ISDIR(info.st_mode))
    {
        // Entry is a file, remove it
        result = unlink(normalizedPath);
        if (result != 0)
        {
            return CLR_E_FILE_IO;
            ;
        }
    }
    else
    {
        // Entry is a directory...
        // ... remove the directory
        result = rmdir(normalizedPath);

        if (result == -1)
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
    }

    return S_OK;
}

static int32_t RemoveAllFiles(const char *path)
{
    DIR *dir;
    struct dirent *entry;
    struct stat info;
    char buffer[FS_MAX_PATH_LENGTH];
    int32_t result = 0;

    // Open the directory
    dir = opendir(path);
    if (!dir)
    {
        // opendir failed
        return -1;
    }

    while ((entry = readdir(dir)) != nullptr)
    {
        // Skip "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        // Prepare the full path of the current entry
        snprintf(buffer, FS_MAX_PATH_LENGTH, "%s/%s", path, entry->d_name);

        // Get info about the entry
        if (stat(buffer, &info) != 0)
        {
            // stat failed
            result = -1;
            break;
        }

        if (S_ISDIR(info.st_mode))
        {
            // Entry is a directory, recursively remove its contents
            result = RemoveAllFiles(buffer);
            if (result != 0)
            {
                // Failed to remove the contents of the directory
                break;
            }
            // Remove the directory itself
            result = rmdir(buffer);
            if (result != 0)
            {
                result = -1;
                break;
            }
        }
        else
        {
            // Entry is a file, remove it
            result = unlink(buffer);
            if (result != 0)
            {
                result = -1;
                break;
            }
        }
    }

    closedir(dir);
    return result;
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

    // add a '/' after the drive letter
    *bufferP++ = '/';

    // if the path starts with a backslash, skip it
    if (*workingPath == '\\' || *workingPath == '/')
    {
        workingPath++;
    }

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

    "FATFS",
    0,
};
