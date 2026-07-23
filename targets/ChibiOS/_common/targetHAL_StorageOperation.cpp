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

#if CONFIG_NF_FEATURE_HAS_ACCESSIBLE_STORAGE

#if CONFIG_NF_FEATURE_USE_LITTLEFS

#include <hal_littlefs.h>

// Must match FileAttributes::FileAttributes_Normal (managed System.IO.FileAttributes) without pulling in
// the full CLR runtime headers (nf_sys_io_filesystem.h) into this HAL-layer file.
static const uint32_t kFileAttributesNormal = 128;

// Function to create all necessary intermediate directories with LittleFS.
//
// IMPORTANT: 'path' must already be a relative path (no drive prefix, no leading '/') using '/' as the
// separator - this is guaranteed by the normalization done in HAL_StorageOperation() before this is called.
// This function mutates 'path' in place temporarily (splitting on '/') but always restores it before
// returning. It intentionally avoids any local stack buffer: this runs deep in the wire-protocol/debugger
// call chain (WP_Message -> Monitor_StorageOperation -> HAL_StorageOperation -> create_directories), which
// may run on a thread with a small stack; an extra local buffer here previously caused a stack overflow
// (silent hang, no reply sent back) when deploying files inside a subfolder.
int create_directories(lfs_t *lfs, char *path)
{
    char *pos = NULL;
    int res = 0;
    struct lfs_info info;

    if (*path == '\0')
    {
        return 0;
    }

    for (pos = path; *pos; pos++)
    {
        if (*pos == '/')
        {
            *pos = '\0';
            res = lfs_mkdir(lfs, path);
            *pos = '/';

            if (res == LFS_ERR_EXIST)
            {
                // Existing entry must be a directory.
                *pos = '\0';
                bool isDir = (lfs_stat(lfs, path, &info) == LFS_ERR_OK && info.type == LFS_TYPE_DIR);
                *pos = '/';

                if (!isDir)
                {
                    return -1;
                }
            }
            else if (res != LFS_ERR_OK)
            {
                return -1;
            }
        }
    }

    // Create the final directory
    res = lfs_mkdir(lfs, path);
    if (res == LFS_ERR_OK)
    {
        return 0;
    }

    if (res == LFS_ERR_EXIST)
    {
        return (lfs_stat(lfs, path, &info) == LFS_ERR_OK && info.type == LFS_TYPE_DIR) ? 0 : -1;
    }

    return -1;
}

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

    // Normalize path for LittleFS.
    // 1) Strip optional drive prefix (e.g. "I:\\" or "I:/")
    // 2) Convert Windows separators to '/'
    char *lfsPath = storageName;

    if (nameLength >= 2 && storageName[1] == ':')
    {
        lfsPath = storageName + 2;

        // Skip all path separators after drive prefix (handles both X:\foo and X://foo).
        while (*lfsPath == '\\' || *lfsPath == '/')
        {
            lfsPath++;
        }
    }

    for (char *p = lfsPath; *p; p++)
    {
        if (*p == '\\')
        {
            *p = '/';
        }
    }

    //... and pointer to the littlefs instance
    lfsDrive = hal_lfs_get_fs_from_index(driveIndex);

    if (operation == StorageOperation_Monitor::StorageOperation_Write)
    {
        memset(&lfsFile, 0, sizeof(lfsFile));

        // Extract directory path from file path
        char dir_path[256];
        snprintf(dir_path, sizeof(dir_path), "%s", lfsPath);
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

        // Create all necessary intermediate directories
        if (dir_path[0] != '\0')
        {
            int32_t mkdirResult = create_directories(lfsDrive, dir_path);

            if (mkdirResult != 0)
            {
                errorCode = StorageOperationErrorCode::WriteError;
                goto done;
            }
        }

        // Open/create the file in read mode.
        // NOTE: must use lfs_file_opencfg with the NANO_LITTLEFS_ATTRIBUTE custom attribute, matching what
        // LITTLEFS_FS_Driver::Open does. Without this attribute, managed File.GetAttributes()/File.Exists()
        // will report EMPTY_ATTRIBUTE (0xFFFFFFFF) for this file - even though it exists on disk - because
        // lfs_getattr() fails with LFS_ERR_NOATTR when the attribute was never written.
        uint32_t nanoAttributes = kFileAttributesNormal;
        struct lfs_attr attr = {NANO_LITTLEFS_ATTRIBUTE, &nanoAttributes, NANO_LITTLEFS_ATTRIBUTE_SIZE};
        struct lfs_file_config fileConfig = {
            .buffer = NULL,
            .attrs = &attr,
            .attr_count = 1,
        };

        int32_t openResult =
            lfs_file_opencfg(lfsDrive, &lfsFile, lfsPath, LFS_O_RDWR | LFS_O_CREAT, &fileConfig);
        if (openResult != LFS_ERR_OK)
        {
            errorCode = StorageOperationErrorCode::WriteError;

            goto done;
        }

        // persist the custom attribute immediately, so it survives even if the write below fails
        lfs_file_sync(lfsDrive, &lfsFile);

        lfs_ssize_t writeResult = lfs_file_write(lfsDrive, &lfsFile, (data + nameLength), dataLength);
        if (writeResult != (lfs_ssize_t)dataLength)
        {
            // failed to write expected number of bytes
            errorCode = StorageOperationErrorCode::WriteError;
        }

        // close file
        lfs_file_close(lfsDrive, &lfsFile);
    }
    else if (operation == StorageOperation_Monitor::StorageOperation_Append)
    {
        // Extract directory path from file path
        char dir_path[256];
        snprintf(dir_path, sizeof(dir_path), "%s", lfsPath);
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

        // Create all necessary intermediate directories
        if (dir_path[0] != '\0')
        {
            int32_t mkdirResult = create_directories(lfsDrive, dir_path);

            if (mkdirResult != 0)
            {
                errorCode = StorageOperationErrorCode::WriteError;
                goto done;
            }
        }

        // Open/create the file in read mode.
        // Same custom-attribute requirement as the Write branch above - see the comment there.
        uint32_t nanoAttributes = kFileAttributesNormal;
        struct lfs_attr attr = {NANO_LITTLEFS_ATTRIBUTE, &nanoAttributes, NANO_LITTLEFS_ATTRIBUTE_SIZE};
        struct lfs_file_config fileConfig = {
            .buffer = NULL,
            .attrs = &attr,
            .attr_count = 1,
        };

        int32_t openResult =
            lfs_file_opencfg(lfsDrive, &lfsFile, lfsPath, LFS_O_RDWR | LFS_O_APPEND | LFS_O_CREAT, &fileConfig);
        if (openResult != LFS_ERR_OK)
        {
            errorCode = StorageOperationErrorCode::WriteError;

            goto done;
        }

        // persist the custom attribute immediately, so it survives even if the write below fails
        lfs_file_sync(lfsDrive, &lfsFile);

        // append more data
        lfs_ssize_t writeResult = lfs_file_write(lfsDrive, &lfsFile, (data + nameLength), dataLength);
        if (writeResult != (lfs_ssize_t)dataLength)
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
        int32_t removeResult = lfs_remove(lfsDrive, lfsPath);
        if (removeResult != LFS_ERR_OK && removeResult != LFS_ERR_NOENT)
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

#endif // CONFIG_NF_FEATURE_USE_LITTLEFS

#endif // CONFIG_NF_FEATURE_HAS_ACCESSIBLE_STORAGE
#endif // I_AM_NANOCLR
