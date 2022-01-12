//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_sys_io_filesystem.h"
#include <dirent.h>
#include <ff.h>

extern void CombinePathAndName(char *outpath, const char *path1, const char *path2);
extern char *ConvertToVfsPath(const char *filepath);
extern SYSTEMTIME GetDateTimeFromStat(time_t *time);

HRESULT Library_nf_sys_io_filesystem_System_IO_Directory::ExistsNative___STATIC__BOOLEAN__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    char *vfsPath = NULL;

    const char *folderPath = stack.Arg0().RecoverString();

    bool exists = false;
    FAULT_ON_NULL_ARG(folderPath);

    vfsPath = ConvertToVfsPath(folderPath);

    struct stat fileInfo;

    if (stat(vfsPath, &fileInfo) == 0)
    {
        // Exists and a Directory
        if (S_ISDIR(fileInfo.st_mode))
        {
            exists = true;
        }
    }

    stack.SetResult_Boolean(exists);

    NANOCLR_CLEANUP();

    // free buffer memory, if allocated
    if (vfsPath != NULL)
    {
        platform_free(vfsPath);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_Directory::MoveNative___STATIC__VOID__STRING__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    char *workingPathSrc = NULL;
    char *workingPathDest = NULL;

    const char *filePathSrc = stack.Arg0().RecoverString();
    const char *filePathDest = stack.Arg1().RecoverString();
    FAULT_ON_NULL_ARG(filePathSrc);
    FAULT_ON_NULL_ARG(filePathDest);

    workingPathSrc = ConvertToVfsPath(filePathSrc);
    workingPathDest = ConvertToVfsPath(filePathDest);

    // rename folder
    if (rename(workingPathSrc, workingPathDest) != 0)
    {
        if (errno == EINVAL)
        {
            // invalid path
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        else
        {
            // folder / file doesn't exist
            NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_FOUND);
        }
    }

    NANOCLR_CLEANUP();

    // free buffer memory, if allocated
    if (workingPathSrc != NULL)
    {
        platform_free(workingPathSrc);
    }

    if (workingPathDest != NULL)
    {
        platform_free(workingPathDest);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_Directory::DeleteNative___STATIC__VOID__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    char *vfsPath = NULL;

    const char *folderPath = stack.Arg0().RecoverString();
    FAULT_ON_NULL_ARG(folderPath);

    vfsPath = ConvertToVfsPath(folderPath);

    // Delete folder
    if (unlink(vfsPath) != 0)
    {
        if (errno == EACCES)
        {
            // directory not empty, cannot delete
            NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_EMPTY);
        }
        else
        {
            // file doesn't exist
            NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
        }
    }

    NANOCLR_CLEANUP();

    // free buffer memory, if allocated
    if (vfsPath != NULL)
    {
        platform_free(vfsPath);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_Directory::CreateNative___STATIC__VOID__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    char *vfsPath = NULL;

    const char *folderPath = stack.Arg0().RecoverString();
    FAULT_ON_NULL_ARG(folderPath);

    vfsPath = ConvertToVfsPath(folderPath);

    if (mkdir(vfsPath, 0) != 0)
    {
        if (errno == EEXIST)
        {
            // folder already exists
            NANOCLR_SET_AND_LEAVE(CLR_E_PATH_ALREADY_EXISTS);
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_FOUND);
        }
    }

    NANOCLR_CLEANUP();

    // free buffer memory, if allocated
    if (vfsPath != NULL)
    {
        platform_free(vfsPath);
    }

    NANOCLR_CLEANUP_END();
}

int CountEntries(const char *folderPath, int type)
{
    uint16_t count = 0;
    DIR *currentDirectory;
    struct dirent *fileInfo;

    currentDirectory = opendir(folderPath);
    if (currentDirectory == NULL)
    {
        if (errno == ENOTDIR)
        {
            return -2;
        }
    }
    else
    {
        // perform 1st pass
        for (;;)
        {
            // read next file item
            fileInfo = readdir(currentDirectory);

            // break on error or at end of dir
            if (fileInfo == NULL)
            {
                break;
            }

            // check if this is correct type
            if ((fileInfo->d_type & type))
            {
                count++;
            }
        }
    }

    closedir(currentDirectory);

    return count;
}

HRESULT BuildPathsArray(const char *vfsFolderPath, const char *folderPath, CLR_RT_HeapBlock arrayPaths, int entryType)
{
    char *stringBuffer = NULL;
    char *workingBuffer = NULL;
    DIR *currentDirectory = NULL;

    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pathEntry;
        struct dirent *fileInfo;

        // get a pointer to the first object in the array (which is of type <String>)
        pathEntry = (CLR_RT_HeapBlock *)arrayPaths.DereferenceArray()->GetFirstElement();

        // allocate memory for buffers
        stringBuffer = (char *)platform_malloc(FF_LFN_BUF + 1);
        workingBuffer = (char *)platform_malloc(2 * FF_LFN_BUF + 1);

        // sanity check for successfull malloc
        if (stringBuffer == NULL || workingBuffer == NULL)
        {
            // failed to allocate memory
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }

        // perform 2nd pass
        // need to rewind the directory read index first
        currentDirectory = opendir(vfsFolderPath);
        if (currentDirectory == NULL)
        {
            // failed to change drive
            NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
        }

        for (;;)
        {
            // read next file item
            fileInfo = readdir(currentDirectory);

            // break on error or at end of dir
            if (fileInfo == NULL)
            {
                break;
            }

            // check if this is correct type
            if ((fileInfo->d_type & entryType))
            {
                // clear working buffer
                memset(workingBuffer, 0, 2 * FF_LFN_BUF + 1);

                // compose file path
                CombinePathAndName(workingBuffer, folderPath, fileInfo->d_name);

                // set file full path in array of strings
                NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(*pathEntry, workingBuffer));

                // move the file array pointer to the next item in the array
                pathEntry++;
            }
        }
    }
    NANOCLR_CLEANUP();

    if (currentDirectory != NULL)
    {
        closedir(currentDirectory);
    }

    if (stringBuffer != NULL)
    {
        platform_free(stringBuffer);
    }

    if (workingBuffer != NULL)
    {
        platform_free(workingBuffer);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_Directory::GetFilesNative___STATIC__SZARRAY_STRING__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    const char *folderPath = stack.Arg0().RecoverString();
    CLR_RT_HeapBlock &folderArrayPaths = stack.PushValue();
    char *vfsPath = NULL;

    int fileCount = 0;

    FAULT_ON_NULL_ARG(folderPath);

    vfsPath = ConvertToVfsPath(folderPath);

    fileCount = CountEntries(vfsPath, DT_REG);
    if (fileCount < 0)
    {
        // failed to change drive
        NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
    }

    // create an array of files paths <String>
    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(
        folderArrayPaths,
        (CLR_UINT32)fileCount,
        g_CLR_RT_WellKnownTypes.m_String));

    if (fileCount > 0)
    {
        // 2nd pass fill directory path names
        NANOCLR_CHECK_HRESULT(BuildPathsArray(vfsPath, folderPath, folderArrayPaths, DT_REG))
    }

    NANOCLR_CLEANUP();

    // free buffer memory, if allocated
    if (vfsPath != NULL)
    {
        platform_free(vfsPath);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_Directory::GetDirectoriesNative___STATIC__SZARRAY_STRING__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    const char *folderPath = stack.Arg0().RecoverString();
    CLR_RT_HeapBlock &folderArrayPaths = stack.PushValue();
    char *vfsPath = NULL;

    int directoryCount = 0;

    FAULT_ON_NULL_ARG(folderPath);

    vfsPath = ConvertToVfsPath(folderPath);

    // 1st pass count directory entries
    directoryCount = CountEntries(vfsPath, DT_DIR);
    if (directoryCount < 0)
    {
        // failed to change drive
        NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
    }

    // create an array of files paths <String> of directoryCount
    NANOCLR_CHECK_HRESULT(
        CLR_RT_HeapBlock_Array::CreateInstance(folderArrayPaths, directoryCount, g_CLR_RT_WellKnownTypes.m_String));

    if (directoryCount > 0)
    {
        // 2nd pass fill directory path names
        NANOCLR_CHECK_HRESULT(BuildPathsArray(vfsPath, folderPath, folderArrayPaths, DT_DIR))
    }

    NANOCLR_CLEANUP();

    // free buffer memory, if allocated
    if (vfsPath != NULL)
    {
        platform_free(vfsPath);
    }

    NANOCLR_CLEANUP_END();
}

// Enumerate drives in system
// if array == null then only count drives
// Return number of drives
static HRESULT EnumerateDrives(CLR_RT_HeapBlock *array, int &count)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *storageFolder = NULL;
        DIR *currentDirectory;
        char outputDrive[] = INDEX0_DRIVE_PATH;
        char workingDrive[] = "/D/";

        if (array)
        {
            // get a pointer to the first object in the array (which is of type <String>)
            storageFolder = (CLR_RT_HeapBlock *)array->DereferenceArray()->GetFirstElement();
        }

        count = 0;
        for (char drive = INDEX0_DRIVE_LETTER[0]; drive <= INTERNAL_DRIVE0_LETTER[0]; drive++)
        {
            workingDrive[1] = drive;
            currentDirectory = opendir(workingDrive);
            if (currentDirectory != NULL)
            {
                count++;
                closedir(currentDirectory);

                if (array)
                {
                    // Add entry to array
                    outputDrive[0] = drive;

                    // set the drive letter in string array
                    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(*storageFolder, outputDrive));

                    // Next element in array
                    storageFolder++;
                }
            }
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_Directory::GetLogicalDrivesNative___STATIC__SZARRAY_STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock &top = stack.PushValueAndClear();
        int count = 0;

        // 1st pass find number of drives
        NANOCLR_CHECK_HRESULT(EnumerateDrives(NULL, count));

        // create an array of files paths <String> for count drives
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(top, count, g_CLR_RT_WellKnownTypes.m_String));

        // 2nd pass fill array with drives
        NANOCLR_CHECK_HRESULT(EnumerateDrives(&top, count));
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_Directory::GetLastWriteTimeNative___STATIC__SystemDateTime__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    SYSTEMTIME fileInfoTime;
    CLR_RT_TypeDescriptor dtType;
    struct stat fileInfo;
    char *vfsPath = NULL;
    CLR_INT64 *pRes;

    CLR_RT_HeapBlock &ref = stack.PushValue();
    ;

    const char *folderPath = stack.Arg0().RecoverString();
    FAULT_ON_NULL_ARG(folderPath);

    vfsPath = ConvertToVfsPath(folderPath);

    if (stat(vfsPath, &fileInfo) != 0)
    {
        // folder doesn't exist
        NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_FOUND);
    }

    // get the date time details and return it on Stack as DateTime object
    fileInfoTime = GetDateTimeFromStat(&fileInfo.st_mtime);

    // initialize <DateTime> type descriptor
    NANOCLR_CHECK_HRESULT(dtType.InitializeFromType(g_CLR_RT_WellKnownTypes.m_DateTime));

    // create an instance of <DateTime>
    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObject(ref, dtType.m_handlerCls));

    pRes = Library_corlib_native_System_DateTime::GetValuePtr(ref);
    *pRes = HAL_Time_ConvertFromSystemTime(&fileInfoTime);

    NANOCLR_CLEANUP();

    // free buffer memory, if allocated
    if (vfsPath != NULL)
    {
        platform_free(vfsPath);
    }

    NANOCLR_CLEANUP_END();
}
