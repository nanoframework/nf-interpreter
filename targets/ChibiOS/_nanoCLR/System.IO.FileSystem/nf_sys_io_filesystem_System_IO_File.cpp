//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nf_sys_io_filesystem.h>
#include <hal_littlefs.h>

extern void CombinePaths(char *outpath, const char *path1, const char *path2);
extern int32_t GetInternalDriveIndex(char *drive);

bool IsInternalFilePath(const char *filePath)
{
    if (filePath[0] == INTERNAL_DRIVE0_LETTER[0])
    {
        return true;
    }
    return false;
}

HRESULT Library_nf_sys_io_filesystem_System_IO_File::ExistsNative___STATIC__BOOLEAN__STRING__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(NF_FEATURE_USE_LITTLEFS) && (NF_FEATURE_USE_LITTLEFS == TRUE)

    int32_t driveIndex;
    int32_t statResult;
    char workingDrive[DRIVE_LETTER_LENGTH];
    bool exists = false;
    lfs_t *fsDrive = NULL;
    lfs_info info;
    char *workingPath = NULL;

    const char *filePath = stack.Arg0().RecoverString();
    const char *fileName = stack.Arg1().RecoverString();

    // check for null argument
    FAULT_ON_NULL_ARG(filePath);
    FAULT_ON_NULL_ARG(fileName);

    // copy the first 2 letters of the path for the drive
    // path is 'D:\folder\file.txt', so we need 'D:'
    memcpy(workingDrive, filePath, DRIVE_LETTER_LENGTH);

    // get littlefs drive index...
    driveIndex = GetInternalDriveIndex(workingDrive);
    //... and pointer to the littlefs instance
    fsDrive = hal_lfs_get_fs_from_index(driveIndex);

    // allocate memory for file path
    workingPath = (char *)platform_malloc(LFS_NAME_MAX + 1);

    // sanity check for successful malloc
    if (workingPath == NULL)
    {
        // failed to allocate memory
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    // clear working buffer
    memset(workingPath, 0, LFS_NAME_MAX + 1);

    // compose file path
    CombinePaths(workingPath, filePath, fileName);

    statResult = lfs_stat(fsDrive, workingPath, &info);
    if (statResult == LFS_ERR_NOENT)
    {
        // file doesn't exist
        exists = false;
    }
    else
    {
        // check if entry is a file
        if (statResult == LFS_ERR_OK && info.type == LFS_TYPE_REG)
        {
            exists = true;
        }
        else
        {
            // not a file
            exists = false;
        }
    }

    stack.SetResult_Boolean(exists);

    NANOCLR_CLEANUP();

    // free buffer memory, if allocated
    if (workingPath != NULL)
    {
        platform_free(workingPath);
    }

    NANOCLR_CLEANUP_END();

#else

    stack.NotImplementedStub();

    NANOCLR_NOCLEANUP();

#endif
}

HRESULT Library_nf_sys_io_filesystem_System_IO_File::MoveNative___STATIC__VOID__STRING__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(NF_FEATURE_USE_LITTLEFS) && (NF_FEATURE_USE_LITTLEFS == TRUE)

    int32_t driveIndexSrc = 0;
    int32_t driveIndexDest = 0;
    int32_t operationResult;
    char workingDriveSrc[DRIVE_LETTER_LENGTH];
    lfs_t *fsDriveSrc = NULL;

    const char *filePathSrc = stack.Arg0().RecoverString();
    const char *filePathDest = stack.Arg1().RecoverString();

    FAULT_ON_NULL_ARG(filePathSrc);
    FAULT_ON_NULL_ARG(filePathDest);

    // copy the first 2 letters of the path for the drive
    // path is 'D:\folder\file.txt', so we need 'D:'
    memcpy(workingDriveSrc, filePathSrc, DRIVE_LETTER_LENGTH);

    // get littlefs drive index...
    driveIndexSrc = GetInternalDriveIndex(workingDriveSrc);

    // littlefs can only move files within the same drive
    if (driveIndexSrc != driveIndexDest)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    //... and pointer to the littlefs instance
    fsDriveSrc = hal_lfs_get_fs_from_index(driveIndexSrc);

    operationResult = lfs_rename(fsDriveSrc, filePathSrc, filePathDest);

    if (operationResult == LFS_ERR_NOENT)
    {
        // file doesn't exist
        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
    }
    else if (operationResult != LFS_ERR_OK)
    {
        // failed to move the file
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

#else

    stack.NotImplementedStub();

#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_File::DeleteNative___STATIC__VOID__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(NF_FEATURE_USE_LITTLEFS) && (NF_FEATURE_USE_LITTLEFS == TRUE)

    int32_t driveIndex;
    int32_t operationResult;
    char workingDrive[DRIVE_LETTER_LENGTH];
    lfs_t *fsDrive = NULL;

    const char *workingPath = stack.Arg0().RecoverString();
    FAULT_ON_NULL_ARG(workingPath);

    // copy the first 2 letters of the path for the drive
    // path is 'D:\folder\file.txt', so we need 'D:'
    memcpy(workingDrive, workingPath, DRIVE_LETTER_LENGTH);

    // get littlefs drive index...
    driveIndex = GetInternalDriveIndex(workingDrive);

    //... and pointer to the littlefs instance
    fsDrive = hal_lfs_get_fs_from_index(driveIndex);

    // delete file
    operationResult = lfs_remove(fsDrive, workingPath);

    if (operationResult == LFS_ERR_NOENT)
    {
        // file doesn't exist
        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
    }
    else if (operationResult != LFS_ERR_OK)
    {
        // failed to delete the file
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

#else

    stack.NotImplementedStub();

#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_File::GetAttributesNative___STATIC__U1__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(NF_FEATURE_USE_LITTLEFS) && (NF_FEATURE_USE_LITTLEFS == TRUE)

    int32_t driveIndex;
    char workingDrive[DRIVE_LETTER_LENGTH];
    lfs_t *fsDrive = NULL;
    lfs_info info;
    CLR_UINT8 attributes = 0xFF;

    const char *workingPath = stack.Arg0().RecoverString();
    FAULT_ON_NULL_ARG(workingPath);

    // copy the first 2 letters of the path for the drive
    // path is 'D:\folder\file.txt', so we need 'D:'
    memcpy(workingDrive, workingPath, DRIVE_LETTER_LENGTH);

    // get littlefs drive index...
    driveIndex = GetInternalDriveIndex(workingDrive);

    //... and pointer to the littlefs instance
    fsDrive = hal_lfs_get_fs_from_index(driveIndex);

    if (lfs_stat(fsDrive, workingPath, &info) == LFS_ERR_OK)
    {
        // Convert attributes
        attributes = 0;
        if (info.type == LFS_TYPE_DIR)
        {
            attributes |= FileAttributes::FileAttributes_Directory;
        }

        stack.SetResult_U1(attributes);
    }

#else

    stack.NotImplementedStub();

#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_File::SetAttributesNative___STATIC__VOID__STRING__U1(
    CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();

#if defined(NF_FEATURE_USE_LITTLEFS) && (NF_FEATURE_USE_LITTLEFS == TRUE)

    // TODO: implement attributes for LittleFS
    // for the time being just return success
    NANOCLR_SET_AND_LEAVE(S_OK);

#else

    stack.NotImplementedStub();

#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_File::GetLastWriteTimeNative___STATIC__SystemDateTime__STRING(
    CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();

#if defined(NF_FEATURE_USE_LITTLEFS) && (NF_FEATURE_USE_LITTLEFS == TRUE)

    // not supported in this platform
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);

#else

    stack.NotImplementedStub();

#endif

    NANOCLR_NOCLEANUP();
}