//-----------------------------------------------------------------------------
//
//                   ** WARNING! ** 
//    This file was generated automatically by a tool.
//    Re-running the tool will overwrite this file.
//    You should copy this file to a custom location
//    before adding any customization in the copy to
//    prevent loss of your changes when the tool is
//    re-run.
//
//-----------------------------------------------------------------------------

#include "nf_sys_io_filesystem.h"

#include <ff.h>
#include <nanoHAL_Windows_Storage.h>

void CombinePathAndName(char *outpath, const char *path1, const char *path2)
{
    strcat(outpath, path1);

    // Add "\" to path if required
    if (outpath[hal_strlen_s(outpath) - 1] != '\\')
    {
        strcat(outpath, "\\");
    }
    strcat(outpath, path2);
}

HRESULT Library_nf_sys_io_filesystem_System_IO_FileStream::OpenFileNative___VOID__STRING__STRING__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        const char *workingPath = stack.Arg0().RecoverString();
        FAULT_ON_NULL(workingPath);
        const char *fileName = stack.Arg1().RecoverString();
        FAULT_ON_NULL(fileName);
        CLR_INT32 mode = stack.Arg2().NumericByRef().s4;

        FIL file;
        FRESULT operationResult;
        CLR_UINT8 modeFlags = 0;
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
        CombinePathAndName(filePath, workingPath, fileName);

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
            // compute mode flags from FileMode
            switch (mode)
            {
                case FileMode_CreateNew:
                    modeFlags = FA_CREATE_NEW;
                    break;

                case FileMode_Create:
                    modeFlags = FA_CREATE_ALWAYS;
                    break;

                case FileMode_Open:
                    modeFlags = FA_OPEN_EXISTING;
                    break;

                case FileMode_OpenOrCreate:
                    modeFlags = FA_OPEN_ALWAYS;
                    break;

                case FileMode_Truncate:
                    modeFlags = FA_CREATE_ALWAYS;
                    break;

                case FileMode_Append:
                    modeFlags = FA_OPEN_APPEND;
                    break;

                default:
                    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
            }

            // open file
            operationResult = f_open(&file, filePath, modeFlags);

            // process operation result according to creation options
            if ((operationResult == FR_EXIST) && (mode == FileMode_CreateNew))
            {
                // file already exists
                NANOCLR_SET_AND_LEAVE(CLR_E_PATH_ALREADY_EXISTS);
            }

            if ((operationResult == FR_NO_FILE) && ((mode == FileMode_Open) || (mode == FileMode_Truncate)))
            {
                // file doesn't exist
                NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
            }

            if (operationResult == FR_OK)
            {
                // file created (or opened) succesfully
                // OK to close it
                f_close(&file);
            }
            else
            {
                // failed to create the file
                NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
            }
        }

        // free buffer memory, if allocated
        if (filePath != NULL)
        {
            platform_free(filePath);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_FileStream::ReadNative___I4__STRING__STRING__I8__SZARRAY_U1__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        const char *workingPath = stack.Arg0().RecoverString();
        FAULT_ON_NULL(workingPath);
        const char *fileName = stack.Arg1().RecoverString();
        FAULT_ON_NULL(fileName);
        CLR_INT64 position = stack.Arg2().NumericByRef().s8;
        CLR_RT_HeapBlock_Array* pArray = stack.Arg3().DereferenceArray();
        FAULT_ON_NULL_ARG(pArray);
        unsigned char *buffer = pArray->GetFirstElement();
        CLR_INT32 length = stack.Arg4().NumericByRef().s4;

        CLR_UINT32 readCount = 0;

        FIL file;
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
        CombinePathAndName(filePath, workingPath, fileName);

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
            // open file
            operationResult = f_open(&file, filePath, FA_READ);

            if (operationResult == FR_OK)
            {
                operationResult = f_lseek(&file, position); // Change to actual position
                if (operationResult != FR_OK)
                {
                    // failed to change position
                    NANOCLR_SET_AND_LEAVE(CLR_E_INDEX_OUT_OF_RANGE);
                }

                // If there is not as much data to read as asked, shorten the length
                if ((f_size(&file) - position) < length)
                {
                    length = f_size(&file) - position;
                }

                // file opened succesfully
                operationResult = f_read(&file, buffer, length, &readCount);
                if (operationResult != FR_OK)
                {
                    // Failed to write to file
                    NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
                }

                // OK to close it
                f_close(&file);
            }
            else
            {
                // failed to create the file
                NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
            }
        }

        // free buffer memory, if allocated
        if (filePath != NULL)
        {
            platform_free(filePath);
        }

        stack.SetResult_I4(readCount);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_FileStream::WriteNative___VOID__STRING__STRING__I8__SZARRAY_U1__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        const char *workingPath = stack.Arg0().RecoverString();
        FAULT_ON_NULL(workingPath);
        const char *fileName = stack.Arg1().RecoverString();
        FAULT_ON_NULL(fileName);
        CLR_INT64 position = stack.Arg2().NumericByRef().s8;
        CLR_RT_HeapBlock_Array* pArray = stack.Arg3().DereferenceArray();
        FAULT_ON_NULL_ARG(pArray);
        unsigned char *buffer = pArray->GetFirstElement();
        const CLR_INT32 length = stack.Arg4().NumericByRef().s4;

        FIL file;
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
        CombinePathAndName(filePath, workingPath, fileName);

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
            // open file
            operationResult = f_open(&file, filePath, FA_WRITE);

            if (operationResult == FR_OK)
            {
                operationResult = f_lseek(&file, position); // Change to actual position
                if (operationResult != FR_OK)
                {
                    // failed to change position
                    NANOCLR_SET_AND_LEAVE(CLR_E_INDEX_OUT_OF_RANGE);
                }

                // file opened succesfully
                CLR_UINT32 written = 0;
                operationResult = f_write(&file, buffer, length, &written);
                if (operationResult != FR_OK)
                {
                    // Failed to write to file
                    NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
                }

                // OK to close it
                f_close(&file);
            }
            else
            {
                // failed to create the file
                NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
            }
        }

        // free buffer memory, if allocated
        if (filePath != NULL)
        {
            platform_free(filePath);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_FileStream::GetLengthNative___I8__STRING__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_INT64 length = 0;

        const char *workingPath = stack.Arg0().RecoverString();
        FAULT_ON_NULL(workingPath);
        const char *fileName = stack.Arg1().RecoverString();
        FAULT_ON_NULL(fileName);

        FIL file;
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
        CombinePathAndName(filePath, workingPath, fileName);

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
            // open file
            operationResult = f_open(&file, filePath, FA_OPEN_EXISTING);

            if (operationResult == FR_OK)
            {
                // file opened succesfully
                length = f_size(&file);

                // OK to close it
                f_close(&file);
            }
            else
            {
                // failed to create the file
                NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
            }
        }

        // free buffer memory, if allocated
        if (filePath != NULL)
        {
            platform_free(filePath);
        }

        stack.SetResult_I8(length);
    }
    NANOCLR_NOCLEANUP();
}
