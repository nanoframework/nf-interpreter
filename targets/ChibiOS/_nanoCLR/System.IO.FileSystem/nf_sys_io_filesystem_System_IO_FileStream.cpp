//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nf_sys_io_filesystem.h>
#include <hal_littlefs.h>

extern void CombinePaths(char *outpath, const char *path1, const char *path2);
extern int32_t GetInternalDriveIndex(char *drive);

HRESULT Library_nf_sys_io_filesystem_System_IO_FileStream::OpenFileNative___VOID__STRING__STRING__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(NF_FEATURE_USE_LITTLEFS) && (NF_FEATURE_USE_LITTLEFS == TRUE)

    int32_t driveIndex;
    bool fileExists = false;
    FileMode mode;
    int32_t modeFlags = 0;
    char workingDrive[DRIVE_LETTER_LENGTH];
    char *filePath = NULL;
    lfs_file_t *lfsFile = NULL;
    lfs_t *fsDrive = NULL;

    const char *workingPath = stack.Arg1().RecoverString();
    const char *fileName = stack.Arg2().RecoverString();

    FAULT_ON_NULL_ARG(workingPath);
    FAULT_ON_NULL_ARG(fileName);
    mode = (FileMode)(stack.Arg3().NumericByRef().s4);

    // copy the first 2 letters of the path for the drive
    // path is 'D:\folder\file.txt', so we need 'D:'
    memcpy(workingDrive, workingPath, DRIVE_LETTER_LENGTH);

    // get littlefs drive index...
    driveIndex = GetInternalDriveIndex(workingDrive);

    //... and pointer to the littlefs instance
    fsDrive = hal_lfs_get_fs_from_index(driveIndex);

    // allocate memory for file path
    filePath = (char *)platform_malloc(LFS_NAME_MAX + 1);

    // sanity check for successful malloc
    if (filePath == NULL)
    {
        // failed to allocate memory
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    // clear working buffer
    memset(filePath, 0, FF_LFN_BUF + 1);

    // compose file path
    CombinePaths(filePath, workingPath, fileName);

    // check if file exists
    if (lfs_file_open(fsDrive, lfsFile, filePath, LFS_O_RDONLY) == LFS_ERR_OK)
    {
        lfs_file_close(fsDrive, lfsFile);
        fileExists = true;
    }

    // compute mode flags from FileMode
    switch (mode)
    {
        // Create new file. If already exist give IOException exception.
        case FileMode_CreateNew:
            if (fileExists)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_PATH_ALREADY_EXISTS);
            }
            modeFlags = LFS_O_CREAT | LFS_O_WRONLY;
            break;

        // Create new file. If already exists then overwrite.
        case FileMode_Create:
            modeFlags = LFS_O_CREAT | LFS_O_WRONLY | LFS_O_TRUNC;
            break;

        // Open existing file. If not found then FileNotFoundException exception
        case FileMode_Open:
            if (!fileExists)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
            }
            modeFlags = LFS_O_CREAT | LFS_O_RDONLY;
            break;

        case FileMode_OpenOrCreate:
            if (fileExists)
            {
                modeFlags = LFS_O_CREAT | LFS_O_RDONLY;
            }
            else
            {
                modeFlags = LFS_O_CREAT | LFS_O_WRONLY;
            }
            break;

        // Open existing file, if not found then FileNotFoundException exception
        case FileMode_Truncate:
            if (!fileExists)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
            }
            modeFlags = LFS_O_WRONLY | LFS_O_TRUNC;
            break;

        // Opens the file if it exists and seeks to the end of the file, or creates a new file.
        case FileMode_Append:
            modeFlags = LFS_O_CREAT | LFS_O_WRONLY | LFS_O_APPEND;
            break;

        default:
            NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    // open file
    if (lfs_file_open(fsDrive, lfsFile, workingPath, modeFlags) == LFS_ERR_OK)
    {
        lfs_file_close(fsDrive, lfsFile);
    }
    else
    {
        // failed to create the file
        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
    }

#else

    stack.NotImplementedStub();

#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_FileStream::ReadNative___I4__STRING__STRING__I8__SZARRAY_U1__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(NF_FEATURE_USE_LITTLEFS) && (NF_FEATURE_USE_LITTLEFS == TRUE)

    int32_t driveIndex;
    lfs_file_t *lfsFile = NULL;
    lfs_t *fsDrive = NULL;
    CLR_INT64 position = 0;
    CLR_INT64 readCount = 0;
    CLR_INT32 length = 0;
    CLR_RT_HeapBlock_Array *pArray = NULL;

    char workingDrive[DRIVE_LETTER_LENGTH];
    char *filePath = NULL;
    unsigned char *buffer = NULL;

    const char *workingPath = stack.Arg1().RecoverString();
    const char *fileName = stack.Arg2().RecoverString();
    pArray = stack.Arg4().DereferenceArray();

    FAULT_ON_NULL_ARG(workingPath);
    FAULT_ON_NULL_ARG(fileName);
    FAULT_ON_NULL_ARG(pArray);

    position = stack.Arg3().NumericByRef().s8;
    length = stack.Arg5().NumericByRef().s4;
    buffer = pArray->GetFirstElement();

    // copy the first 2 letters of the path for the drive
    // path is 'D:\folder\file.txt', so we need 'D:'
    memcpy(workingDrive, workingPath, DRIVE_LETTER_LENGTH);

    // get littlefs drive index...
    driveIndex = GetInternalDriveIndex(workingDrive);

    //... and pointer to the littlefs instance
    fsDrive = hal_lfs_get_fs_from_index(driveIndex);

    // allocate memory for file path
    filePath = (char *)platform_malloc(LFS_NAME_MAX + 1);

    // sanity check for successful malloc
    if (filePath == NULL)
    {
        // failed to allocate memory
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    // clear working buffer
    memset(filePath, 0, FF_LFN_BUF + 1);

    // compose file path
    CombinePaths(filePath, workingPath, fileName);

    // open file for read
    if (lfs_file_open(fsDrive, lfsFile, filePath, LFS_O_RDONLY) == LFS_ERR_OK)
    {
        // file opened successfully

        // Change to actual position in file to start read
        if (lfs_file_seek(fsDrive, lfsFile, position, LFS_SEEK_SET) != LFS_ERR_OK)
        {
            // failed to change position
            NANOCLR_SET_AND_LEAVE(CLR_E_INDEX_OUT_OF_RANGE);
        }

        // read the file
        if (lfs_file_read(fsDrive, lfsFile, buffer, length) != length)
        {
            // Error or eof
            // failed to read the file
            NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
        }
    }
    else
    {
        // failed to open the file
        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
    }

    stack.SetResult_I4(readCount);

    NANOCLR_CLEANUP();

    // free buffer memory, if allocated
    if (filePath != NULL)
    {
        platform_free(filePath);
    }

    if (lfsFile != NULL)
    {
        lfs_file_close(fsDrive, lfsFile);
    }

    NANOCLR_CLEANUP_END();

#else

    stack.NotImplementedStub();

    NANOCLR_NOCLEANUP();

#endif
}

HRESULT Library_nf_sys_io_filesystem_System_IO_FileStream::WriteNative___VOID__STRING__STRING__I8__SZARRAY_U1__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(NF_FEATURE_USE_LITTLEFS) && (NF_FEATURE_USE_LITTLEFS == TRUE)

    int32_t driveIndex;
    lfs_file_t *lfsFile = NULL;
    lfs_t *fsDrive = NULL;
    CLR_INT64 position = 0;
    CLR_INT32 length = 0;
    CLR_RT_HeapBlock_Array *pArray = NULL;

    char workingDrive[DRIVE_LETTER_LENGTH];
    char *filePath = NULL;
    unsigned char *buffer = NULL;

    const char *workingPath = stack.Arg1().RecoverString();
    const char *fileName = stack.Arg2().RecoverString();
    pArray = stack.Arg4().DereferenceArray();

    FAULT_ON_NULL_ARG(workingPath);
    FAULT_ON_NULL_ARG(fileName);
    FAULT_ON_NULL_ARG(pArray);

    position = stack.Arg3().NumericByRef().s8;
    length = stack.Arg5().NumericByRef().s4;
    buffer = pArray->GetFirstElement();

    // copy the first 2 letters of the path for the drive
    // path is 'D:\folder\file.txt', so we need 'D:'
    memcpy(workingDrive, workingPath, DRIVE_LETTER_LENGTH);

    // get littlefs drive index...
    driveIndex = GetInternalDriveIndex(workingDrive);

    //... and pointer to the littlefs instance
    fsDrive = hal_lfs_get_fs_from_index(driveIndex);

    // allocate memory for file path
    filePath = (char *)platform_malloc(LFS_NAME_MAX + 1);

    // sanity check for successful malloc
    if (filePath == NULL)
    {
        // failed to allocate memory
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    // clear working buffer
    memset(filePath, 0, FF_LFN_BUF + 1);

    // compose file path
    CombinePaths(filePath, workingPath, fileName);

    // open file for write
    if (lfs_file_open(fsDrive, lfsFile, filePath, LFS_O_WRONLY) == LFS_ERR_OK)
    {
        // file opened successfully

        // Change to actual position in file to start read
        if (lfs_file_seek(fsDrive, lfsFile, position, LFS_SEEK_SET) != LFS_ERR_OK)
        {
            // failed to change position
            NANOCLR_SET_AND_LEAVE(CLR_E_INDEX_OUT_OF_RANGE);
        }

        // write to the file
        if (lfs_file_write(fsDrive, lfsFile, buffer, length) != length)
        {
            // failed to write to the file
            NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
        }
    }
    else
    {
        // failed to open the file
        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
    }

    NANOCLR_CLEANUP();

    // free buffer memory, if allocated
    if (filePath != NULL)
    {
        platform_free(filePath);
    }

    if (lfsFile != NULL)
    {
        lfs_file_close(fsDrive, lfsFile);
    }

    NANOCLR_CLEANUP_END();

#else

    stack.NotImplementedStub();

    NANOCLR_NOCLEANUP();

#endif
}

HRESULT Library_nf_sys_io_filesystem_System_IO_FileStream::GetLengthNative___I8__STRING__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(NF_FEATURE_USE_LITTLEFS) && (NF_FEATURE_USE_LITTLEFS == TRUE)

    int32_t driveIndex;
    char workingDrive[DRIVE_LETTER_LENGTH];
    char *filePath = NULL;
    lfs_info info;
    lfs_t *fsDrive = NULL;
    CLR_INT64 length = 0;

    const char *workingPath = stack.Arg1().RecoverString();
    const char *fileName = stack.Arg2().RecoverString();

    FAULT_ON_NULL_ARG(workingPath);
    FAULT_ON_NULL_ARG(fileName);

    // copy the first 2 letters of the path for the drive
    // path is 'D:\folder\file.txt', so we need 'D:'
    memcpy(workingDrive, workingPath, DRIVE_LETTER_LENGTH);

    // get littlefs drive index...
    driveIndex = GetInternalDriveIndex(workingDrive);

    //... and pointer to the littlefs instance
    fsDrive = hal_lfs_get_fs_from_index(driveIndex);

    // allocate memory for file path
    filePath = (char *)platform_malloc(LFS_NAME_MAX + 1);

    // sanity check for successful malloc
    if (filePath == NULL)
    {
        // failed to allocate memory
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    // clear working buffer
    memset(filePath, 0, FF_LFN_BUF + 1);

    // compose file path
    CombinePaths(filePath, workingPath, fileName);

    // check if file exists
    if (lfs_stat(fsDrive, filePath, &info) == LFS_ERR_OK)
    {
        length = info.size;
        stack.SetResult_I8(length);
    }
    else
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
    }

    NANOCLR_CLEANUP();

    // free buffers memory, if allocated
    if (filePath != NULL)
    {
        platform_free(filePath);
    }

    NANOCLR_CLEANUP_END();

#else

    stack.NotImplementedStub();

    NANOCLR_NOCLEANUP();

#endif
}
