//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_sys_io_filesystem.h"

#include <ff.h>
#include <nanoHAL_Windows_Storage.h>

void CombinePathAndName2(char *outpath, const char *path1, const char *path2)
{
    strcat(outpath, path1);

    // Add "\" to path if required
    if (outpath[hal_strlen_s(outpath) - 1] != '\\')
    {
        strcat(outpath, "\\");
    }
    strcat(outpath, path2);
}

HRESULT Library_nf_sys_io_filesystem_System_IO_File::ExistsNative___STATIC__BOOLEAN__STRING__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        const char *workingPath = stack.Arg0().RecoverString();
        FAULT_ON_NULL(workingPath);
        const char *fileName = stack.Arg1().RecoverString();
        FAULT_ON_NULL(fileName);

        bool exists = false;

        FRESULT operationResult;
        char *filePath = NULL;

        // setup file path
        filePath = (char *)platform_malloc(2 * FF_LFN_BUF + 1);

        // sanity check for successfull malloc
        if (filePath == NULL)
        {
            // failed to allocate memory
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }

        // clear working buffer
        memset(filePath, 0, 2 * FF_LFN_BUF + 1);

        // compose file path
        CombinePathAndName2(filePath, workingPath, fileName);

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
                // error opening the directoty
                NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_FOUND);
            }
        }
        else
        {
            FILINFO fno;

            operationResult = f_stat(filePath, &fno);

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

        // free buffer memory, if allocated
        if (filePath != NULL)
        {
            platform_free(filePath);
        }

        stack.SetResult_Boolean(exists);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_File::MoveNative___STATIC__VOID__STRING__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        const char *filePathSrc = stack.Arg0().RecoverString();
        FAULT_ON_NULL(filePathSrc);
        const char *filePathDest = stack.Arg1().RecoverString();
        FAULT_ON_NULL(filePathDest);

        // rename file
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

HRESULT Library_nf_sys_io_filesystem_System_IO_File::DeleteNative___STATIC__VOID__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        const char *filePath = stack.Arg0().RecoverString();
        FAULT_ON_NULL(filePath);

        // Delete file
        FRESULT operationResult = f_unlink(filePath);

        if (operationResult != FR_OK)
        {
            // file doesn't exist
            NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_File::GetAttributesNative___STATIC__U1__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        const char *filePath = stack.Arg0().RecoverString();
        FAULT_ON_NULL(filePath);
        CLR_UINT8 attributes = 0xFF;

        FILINFO fno;

        // Get infos about file
        FRESULT operationResult = f_stat(filePath, &fno);

        if (operationResult == FR_OK)
        {
            attributes = fno.fattrib;
        }
        else if (operationResult == FR_NO_FILE)
        {
            // File/Directory not found
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
        }

        stack.SetResult_U1(attributes);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_File::SetAttributesNative___STATIC__VOID__STRING__U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        const char *filePath = stack.Arg0().RecoverString();
        FAULT_ON_NULL(filePath);
        CLR_UINT8 attributes = stack.Arg1().NumericByRef().u1;

        // Get infos about file
        FRESULT operationResult = f_chmod(filePath, attributes, 0xFF);

        if (operationResult != FR_OK)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }
    }
    NANOCLR_NOCLEANUP();
}
