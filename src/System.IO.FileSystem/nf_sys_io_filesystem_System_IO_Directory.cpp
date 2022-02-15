//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_sys_io_filesystem.h"
#include <ff.h>

extern void CombinePathAndName(char *outpath, const char *path1, const char *path2);
extern SYSTEMTIME GetDateTime(uint16_t date, uint16_t time);

#if defined(PLATFORM_ESP32)
// ESP32 uses the DIR struct for VFS and FatFs uses FF_DIR
#define NANO_DIR FF_DIR
#else
#define NANO_DIR DIR
#endif

HRESULT Library_nf_sys_io_filesystem_System_IO_Directory::ExistsNative___STATIC__BOOLEAN__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    const char *folderPath = stack.Arg0().RecoverString();

    bool exists = false;
    FRESULT operationResult;
    FAULT_ON_NULL_ARG(folderPath);

    FILINFO fno;

    operationResult = f_stat(folderPath, &fno);

    if (operationResult == FR_OK)
    {
        exists = true;
    }
    else
    {
        exists = false;
    }

    stack.SetResult_Boolean(exists);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_Directory::MoveNative___STATIC__VOID__STRING__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        const char *filePathSrc = stack.Arg0().RecoverString();
        FAULT_ON_NULL_ARG(filePathSrc);
        const char *filePathDest = stack.Arg1().RecoverString();
        FAULT_ON_NULL_ARG(filePathDest);

        // rename folder
        FRESULT operationResult = f_rename(filePathSrc, filePathDest);

        if (operationResult == FR_INVALID_NAME)
        {
            // invalid path
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        else if (operationResult != FR_OK)
        {
            // folder doesn't exist
            NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_FOUND);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_Directory::DeleteNative___STATIC__VOID__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        const char *folderPath = stack.Arg0().RecoverString();
        FAULT_ON_NULL_ARG(folderPath);

        // Delete folder
        FRESULT operationResult = f_unlink(folderPath);

        if (operationResult == FR_DENIED)
        {
            // directory not empty, cannot delete
            NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_EMPTY);
        }
        else if (operationResult != FR_OK)
        {
            // file doesn't exist
            NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_Directory::CreateNative___STATIC__VOID__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        const char *folderPath = stack.Arg0().RecoverString();
        FAULT_ON_NULL_ARG(folderPath);

        int operationResult = f_mkdir(folderPath);
        if (operationResult == FR_EXIST)
        {
            // folder already exists
            NANOCLR_SET_AND_LEAVE(CLR_E_PATH_ALREADY_EXISTS)
        }
        else if (operationResult != FR_OK)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_FOUND);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_Directory::GetFilesNative___STATIC__SZARRAY_STRING__STRING(
    CLR_RT_StackFrame &stack)
{
    char *stringBuffer = NULL;
    char *workingBuffer = NULL;

    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *filePathEntry;
        const char *folderPath = stack.Arg0().RecoverString();
        CLR_RT_HeapBlock &filePaths = stack.PushValue();

        uint16_t fileCount = 0;
        NANO_DIR currentDirectory;
        FRESULT operationResult;
        static FILINFO fileInfo;

        FAULT_ON_NULL_ARG(folderPath);

        // open directory
        operationResult = f_opendir(&currentDirectory, folderPath);

        if (operationResult != FR_OK)
        {
            if (operationResult == FR_INVALID_DRIVE)
            {
                // failed to change drive
                NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
            }
            // create an empty array of files paths <String>
            NANOCLR_CHECK_HRESULT(
                CLR_RT_HeapBlock_Array::CreateInstance(filePaths, fileCount, g_CLR_RT_WellKnownTypes.m_String));
        }
        else
        {
            // need to perform this in two steps
            // 1st: count the file objects
            // 2nd: create the array items with each file object

            // perform 1st pass
            for (;;)
            {
                // read next file item
                operationResult = f_readdir(&currentDirectory, &fileInfo);

                // break on error or at end of dir
                if (operationResult != FR_OK || fileInfo.fname[0] == 0)
                {
                    break;
                }

                // check if this is a file
                // but skip if:
                // - has system attribute set
                // - has hidden attribute set
                if ((fileInfo.fattrib & AM_ARC) && !(fileInfo.fattrib & AM_SYS) && !(fileInfo.fattrib & AM_HID))
                {
                    fileCount++;
                }
            }
        }
        // create an array of files paths <String>
        NANOCLR_CHECK_HRESULT(
            CLR_RT_HeapBlock_Array::CreateInstance(filePaths, fileCount, g_CLR_RT_WellKnownTypes.m_String));

        // get a pointer to the first object in the array (which is of type <String>)
        filePathEntry = (CLR_RT_HeapBlock *)filePaths.DereferenceArray()->GetFirstElement();

        if (fileCount > 0)
        {
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
            f_readdir(&currentDirectory, NULL);

            for (;;)
            {
                // read next file item
                operationResult = f_readdir(&currentDirectory, &fileInfo);

                // break on error or at end of dir
                if (operationResult != FR_OK || fileInfo.fname[0] == 0)
                {
                    break;
                }

                // check if this is a file
                // but skip if:
                // - has system attribute set
                // - has hidden attribute set
                if ((fileInfo.fattrib & AM_ARC) && !(fileInfo.fattrib & AM_SYS) && !(fileInfo.fattrib & AM_HID))
                {
                    // clear working buffer
                    memset(workingBuffer, 0, 2 * FF_LFN_BUF + 1);
                    // compose file path
                    CombinePathAndName(workingBuffer, folderPath, fileInfo.fname);
                    // set file full path in array of strings
                    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(*filePathEntry, workingBuffer));

                    // move the file array pointer to the next item in the array
                    filePathEntry++;
                }
            }
        }

        f_closedir(&currentDirectory);
    }

    NANOCLR_CLEANUP();

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

HRESULT Library_nf_sys_io_filesystem_System_IO_Directory::GetDirectoriesNative___STATIC__SZARRAY_STRING__STRING(
    CLR_RT_StackFrame &stack)
{
    char *stringBuffer = NULL;
    char *workingBuffer = NULL;

    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *folderPathEntry;
        const char *folderPath = stack.Arg0().RecoverString();
        CLR_RT_HeapBlock &folderPaths = stack.PushValue();

        uint16_t directoryCount = 0;
        NANO_DIR currentDirectory;
        FRESULT operationResult;
        static FILINFO fileInfo;

        FAULT_ON_NULL_ARG(folderPath);

        // open directory
        operationResult = f_opendir(&currentDirectory, folderPath);

        if (operationResult != FR_OK)
        {
            if (operationResult == FR_INVALID_DRIVE)
            {
                // failed to change drive
                NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
            }
            // create an empty array of files paths <String>
            NANOCLR_CHECK_HRESULT(
                CLR_RT_HeapBlock_Array::CreateInstance(folderPaths, directoryCount, g_CLR_RT_WellKnownTypes.m_String));
        }
        else
        {
            // need to perform this in two steps
            // 1st: count the directory objects
            // 2nd: create the array items with each directory object

            // perform 1st pass
            for (;;)
            {
                // read next directory item
                operationResult = f_readdir(&currentDirectory, &fileInfo);

                // break on error or at end of dir
                if (operationResult != FR_OK || fileInfo.fname[0] == 0)
                {
                    break;
                }

                // check if this is a directory
                // but skip if:
                // - has system attribute set
                // - has hidden attribute set
                if ((fileInfo.fattrib & AM_DIR) && !(fileInfo.fattrib & AM_SYS) && !(fileInfo.fattrib & AM_HID))
                {
                    directoryCount++;
                }
            }
        }
        // create an array of files paths <String>
        NANOCLR_CHECK_HRESULT(
            CLR_RT_HeapBlock_Array::CreateInstance(folderPaths, directoryCount, g_CLR_RT_WellKnownTypes.m_String));

        // get a pointer to the first object in the array (which is of type <String>)
        folderPathEntry = (CLR_RT_HeapBlock *)folderPaths.DereferenceArray()->GetFirstElement();

        if (directoryCount > 0)
        {
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
            f_readdir(&currentDirectory, NULL);

            for (;;)
            {
                // read next directory item
                operationResult = f_readdir(&currentDirectory, &fileInfo);

                // break on error or at end of dir
                if (operationResult != FR_OK || fileInfo.fname[0] == 0)
                {
                    break;
                }

                // check if this is a directory
                // but skip if:
                // - has system attribute set
                // - has hidden attribute set
                if ((fileInfo.fattrib & AM_DIR) && !(fileInfo.fattrib & AM_SYS) && !(fileInfo.fattrib & AM_HID))
                {
                    // clear working buffer
                    memset(workingBuffer, 0, 2 * FF_LFN_BUF + 1);
                    // compose directory path
                    CombinePathAndName(workingBuffer, folderPath, fileInfo.fname);
                    // set directory full path in array of strings
                    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(*folderPathEntry, workingBuffer));

                    // move the folder array pointer to the next item in the array
                    folderPathEntry++;
                }
            }
        }
    }

    NANOCLR_CLEANUP();

    if (stringBuffer == NULL)
    {
        platform_free(stringBuffer);
    }
    if (workingBuffer == NULL)
    {
        platform_free(workingBuffer);
    }

    NANOCLR_CLEANUP_END();
}

// Enumerate drives in system
// if array == null then only count drives
// Return number of drives
HRESULT EnumerateDrives(CLR_RT_HeapBlock *array, int &count)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *storageFolder = NULL;
        FRESULT operationResult;
        NANO_DIR currentDirectory;
        char workingDrive[] = INDEX0_DRIVE_PATH;

        if (array)
        {
            // get a pointer to the first object in the array (which is of type <String>)
            storageFolder = (CLR_RT_HeapBlock *)array->DereferenceArray()->GetFirstElement();
        }

        count = 0;
        for (char drive = INDEX0_DRIVE_LETTER[0]; drive <= INTERNAL_DRIVE0_LETTER[0]; drive++)
        {
            workingDrive[0] = drive;

            operationResult = f_opendir(&currentDirectory, workingDrive);
            if (operationResult == FR_OK)
            {
                count++;
                f_closedir(&currentDirectory);

                if (array)
                {
                    // Add entry to array
                    // set the drive letter in string array
                    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(*storageFolder, workingDrive));

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
    {
        SYSTEMTIME fileInfoTime;
        CLR_RT_TypeDescriptor dtType;
        FILINFO fileInfo;

        const char *folderPath = stack.Arg0().RecoverString();
        FAULT_ON_NULL_ARG(folderPath);

        int operationResult = f_stat(folderPath, &fileInfo);
        if (operationResult != FR_OK)
        {
            // folder doesn't exist
            NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_FOUND);
        }

        // get the date time details and return it on Stack as DateTime object
        fileInfoTime = GetDateTime(fileInfo.fdate, fileInfo.ftime);

        CLR_RT_HeapBlock &ref = stack.PushValue();

        // initialize <DateTime> type descriptor
        NANOCLR_CHECK_HRESULT(dtType.InitializeFromType(g_CLR_RT_WellKnownTypes.m_DateTime));

        // create an instance of <DateTime>
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObject(ref, dtType.m_handlerCls));

        CLR_INT64 *pRes = Library_corlib_native_System_DateTime::GetValuePtr(ref);
        *pRes = HAL_Time_ConvertFromSystemTime(&fileInfoTime);
    }
    NANOCLR_NOCLEANUP();
}
