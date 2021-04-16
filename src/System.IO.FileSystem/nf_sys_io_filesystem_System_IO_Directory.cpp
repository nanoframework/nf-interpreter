//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_sys_io_filesystem.h"
#include <ff.h>

extern void CombinePathAndName2(char *outpath, const char *path1, const char *path2);
extern void CombinePath(char *outpath, const char *path1, const char *path2);

HRESULT Library_nf_sys_io_filesystem_System_IO_Directory::ExistsNative___STATIC__BOOLEAN__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    const char *workingPath = stack.Arg0().RecoverString();
    const char *folderName = stack.Arg1().RecoverString();

    bool exists = false;
    FRESULT operationResult;
    char *folderPath = NULL;

    FAULT_ON_NULL(workingPath);
    FAULT_ON_NULL(folderName);

    // setup folder path
    folderPath = (char *)platform_malloc(2 * FF_LFN_BUF + 1);

    // sanity check for successfull malloc
    if (folderPath == NULL)
    {
        // failed to allocate memory
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    // clear working buffer
    memset(folderPath, 0, 2 * FF_LFN_BUF + 1);

    // compose file path
    CombinePathAndName2(folderPath, workingPath, folderName);

    // change directory
    operationResult = f_chdir(workingPath);

    if (operationResult != FR_OK)
    {
        if (operationResult == FR_INVALID_DRIVE)
        {
            // invalid drive
            NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
        }
        else
        {
            // error opening the directory
            NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_FOUND);
        }
    }
    else
    {
        FILINFO fno;

        operationResult = f_stat(folderPath, &fno);

        if (operationResult == FR_OK)
        {
            exists = true;
        }
        else if (operationResult == FR_NO_FILE)
        {
            exists = false;
        }
        else
        {
            exists = false;
        }
    }
    stack.SetResult_Boolean(exists);

    NANOCLR_CLEANUP();

    // free buffer memory, if allocated
    if (folderPath != NULL)
    {
        platform_free(folderPath);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_Directory::MoveNative___STATIC__VOID__STRING__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        const char *filePathSrc = stack.Arg0().RecoverString();
        FAULT_ON_NULL(filePathSrc);
        const char *filePathDest = stack.Arg1().RecoverString();
        FAULT_ON_NULL(filePathDest);

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
        FAULT_ON_NULL(folderPath);

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
        FAULT_ON_NULL(folderPath);

        int operationResult = f_mkdir(folderPath);
        if (operationResult != FR_OK)
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
        CLR_RT_HeapBlock *firstFolderPath;
        const char *folderPath = stack.Arg0().RecoverString();
        CLR_RT_HeapBlock &filePaths = stack.PushValue();

        uint16_t directoryCount = 0;
        DIR currentDirectory;
        FRESULT operationResult;
        static FILINFO fileInfo;

        FAULT_ON_NULL(folderPath);

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
                CLR_RT_HeapBlock_Array::CreateInstance(filePaths, directoryCount, g_CLR_RT_WellKnownTypes.m_String));
        }
        else
        {
            // need to perform this in two steps
            // 1st: count the directory objects
            // 2nd: create the array items with each file object

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
            CLR_RT_HeapBlock_Array::CreateInstance(filePaths, directoryCount, g_CLR_RT_WellKnownTypes.m_String));

        // get a pointer to the first object in the array (which is of type <String>)
        firstFolderPath = (CLR_RT_HeapBlock *)filePaths.DereferenceArray()->GetFirstElement();

        if (directoryCount > 0)
        {
            // allocate memory for buffers
            stringBuffer = (char *)malloc(FF_LFN_BUF + 1);
            workingBuffer = (char *)malloc(2 * FF_LFN_BUF + 1);

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
                    CombinePath(workingBuffer, folderPath, fileInfo.fname);
                    // set directory full path in array of strings
                    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(*firstFolderPath, workingBuffer));

                    // move the storage folder pointer to the next item in the array
                    firstFolderPath++;
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

HRESULT Library_nf_sys_io_filesystem_System_IO_Directory::GetDirectoriesNative___STATIC__SZARRAY_STRING__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_Directory::GetLogicalDrivesNative___STATIC__SZARRAY_STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_Directory::GetCreationTimeNative___STATIC__SystemDateTime__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_Directory::GetLastAccessTimeNative___STATIC__SystemDateTime__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_Directory::GetLastWriteTimeNative___STATIC__SystemDateTime__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}
