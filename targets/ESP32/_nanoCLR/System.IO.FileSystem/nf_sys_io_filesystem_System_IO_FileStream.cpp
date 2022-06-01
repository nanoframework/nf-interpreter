//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

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

//
//  Converts from windows type path       "c:\folder\folder\file.ext"
//  to linux type path used in ESP32 VFS  "/c/folder/folder/file.exe
//  where /c is the mount point
////////////////////////////////////////////
// MAKE SURE TO FREE THE RETURNED POINTER //
////////////////////////////////////////////
//
char *ConvertToVfsPath(const char *filepath)
{
    char *startPath = NULL;
    char *path = NULL;

    int pathlen = hal_strlen_s(filepath);

    /////////////////////////////////
    // MAKE SURE TO FREE THIS POINTER
    startPath = (char *)platform_malloc(pathlen + 1);

    // sanity check for successfull malloc
    if (startPath == NULL)
    {
        // failed to allocate memory
        return NULL;
    }

    path = startPath;
    hal_strcpy_s(path, pathlen + 1, filepath);

    if (hal_strlen_s(path) >= 2)
    {
        // Map  Drive: -> /C
        char drive = *path;
        *path++ = '/';
        *path++ = drive;

        // Convert '\' to '/'
        while (*path)
        {
            if (*path == '\\')
                *path++ = '/';
            else
                path++;
        }
    }

    return startPath;
}

HRESULT Library_nf_sys_io_filesystem_System_IO_FileStream::OpenFileNative___VOID__STRING__STRING__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    const char *workingPath = stack.Arg1().RecoverString();
    const char *fileName = stack.Arg2().RecoverString();
    FileMode mode = (FileMode)(stack.Arg3().NumericByRef().s4);

    FILE *file;
    char modeFlags[4] = {0};
    char *filePath = NULL;
    char *vfsPath = NULL;
    bool fileExists = false;

    FAULT_ON_NULL(workingPath);
    FAULT_ON_NULL(fileName);

    // setup file path
    filePath = (char *)platform_malloc(2 * FF_LFN_BUF + 1);

    // sanity check for successfull malloc
    if (filePath == NULL)
    {
        // failed to allocate memory
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    // clear working buffer
    filePath[0] = 0;

    // compose file path
    CombinePathAndName(filePath, workingPath, fileName);

    // Convert path to ESP32 VFS format
    vfsPath = ConvertToVfsPath(filePath);

    // Check if file exists
    file = fopen(vfsPath, "r");
    if (file)
    {
        fclose(file);
        fileExists = true;
    }

    // compute mode flags from FileMode
    switch (mode)
    {
        // Create new file. If already exist give  IOException exception.
        case FileMode_CreateNew:
            if (fileExists)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_PATH_ALREADY_EXISTS);
            }
            modeFlags[0] = 'w';
            break;

        // Create new file. If already exists then overwrite.
        case FileMode_Create:
            modeFlags[0] = 'w';
            break;

        // Open existing file. If not found then FileNotFoundException exception
        case FileMode_Open:
            if (!fileExists)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
            }
            modeFlags[0] = 'r';
            break;

        case FileMode_OpenOrCreate:
            if (fileExists)
            {
                modeFlags[0] = 'r';
            }
            else
            {
                modeFlags[0] = 'w';
            }
            break;

        // Open existing file, if not found then FileNotFoundException exception
        case FileMode_Truncate:
            if (!fileExists)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
            }
            modeFlags[0] = 'w';
            break;

        // Opens the file if it exists and seeks to the end of the file, or creates a new file.
        case FileMode_Append:
            modeFlags[0] = 'a';
            break;

        default:
            NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    // open file
    file = fopen(vfsPath, modeFlags);
    if (file != NULL)
    {
        // file created (or opened) successfully
        // OK to close it
        fclose(file);
    }
    else
    {
        // failed to create the file
        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
    }

    NANOCLR_CLEANUP();

    // free memory buffers, if allocated
    if (filePath != NULL)
    {
        platform_free(filePath);
    }

    if (vfsPath != NULL)
    {
        platform_free(vfsPath);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_FileStream::ReadNative___I4__STRING__STRING__I8__SZARRAY_U1__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    const char *workingPath = stack.Arg1().RecoverString();
    const char *fileName = stack.Arg2().RecoverString();
    CLR_INT64 position = stack.Arg3().NumericByRef().s8;
    CLR_RT_HeapBlock_Array *pArray = stack.Arg4().DereferenceArray();
    CLR_INT32 length = stack.Arg5().NumericByRef().s4;

    int operationResult;
    CLR_UINT32 readCount = 0;
    FILE *file;
    char *filePath = NULL;
    char *vfsPath = NULL;
    unsigned char *buffer = NULL;

    FAULT_ON_NULL(workingPath);
    FAULT_ON_NULL(fileName);
    FAULT_ON_NULL_ARG(pArray);

    buffer = pArray->GetFirstElement();

    // setup file path
    filePath = (char *)platform_malloc(2 * FF_LFN_BUF + 1);

    // sanity check for successfull malloc
    if (filePath == NULL)
    {
        // failed to allocate memory
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    // clear working buffer
    filePath[0] = 0;

    // combine file path & file name
    CombinePathAndName(filePath, workingPath, fileName);

    // Convert to ESP32 VFS path
    vfsPath = ConvertToVfsPath(filePath);

    // open file for read
    file = fopen(vfsPath, "r");
    if (file != NULL)
    {
        operationResult = fseek(file, position, SEEK_SET); // Change to actual position
        if (operationResult != 0)
        {
            // failed to change position
            NANOCLR_SET_AND_LEAVE(CLR_E_INDEX_OUT_OF_RANGE);
        }

        // file opened successfully
        readCount = fread(buffer, 1, length, file);
        if (readCount == 0)
        {
            // Error or eof
            // failed to read the file
            if (ferror(file))
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
            }
        }

        // OK to close it
        fclose(file);
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

    // free buffer memory, if allocated
    if (vfsPath != NULL)
    {
        platform_free(vfsPath);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_FileStream::WriteNative___VOID__STRING__STRING__I8__SZARRAY_U1__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    const char *workingPath = stack.Arg1().RecoverString();
    const char *fileName = stack.Arg2().RecoverString();
    CLR_INT64 position = stack.Arg3().NumericByRef().s8;
    CLR_RT_HeapBlock_Array *pArray = stack.Arg4().DereferenceArray();
    const CLR_INT32 length = stack.Arg5().NumericByRef().s4;

    FILE *file;
    char *filePath = NULL;
    char *vfsPath = NULL;
    unsigned char *buffer = NULL;

    FAULT_ON_NULL(workingPath);
    FAULT_ON_NULL(fileName);
    FAULT_ON_NULL_ARG(pArray);

    buffer = pArray->GetFirstElement();

    // setup file path
    filePath = (char *)platform_malloc(2 * FF_LFN_BUF + 1);

    // sanity check for successfull mallocs
    if (filePath == NULL)
    {
        // failed to allocate memory
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    // clear working buffer
    filePath[0] = 0;

    // compose file path
    CombinePathAndName(filePath, workingPath, fileName);

    // Convert to ESP32 VFS path
    vfsPath = ConvertToVfsPath(filePath);

    // open file for write / read
    file = fopen(vfsPath, "a+");
    if (file != NULL)
    {
        // Change to actual position in file to start write
        if (fseek(file, position, SEEK_SET) != 0)
        {
            // failed to change position
            NANOCLR_SET_AND_LEAVE(CLR_E_INDEX_OUT_OF_RANGE);
        }

        // file opened successfully
        CLR_UINT32 written = 0;
        written = fwrite(buffer, 1, length, file);
        if (written != (CLR_UINT32)length)
        {
            // Failed to write to file
            NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
        }

        // OK to close it
        fclose(file);
    }
    else
    {
        // failed to create the file
        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
    }

    NANOCLR_CLEANUP();

    // free buffer memory, if allocated
    if (filePath != NULL)
    {
        platform_free(filePath);
    }

    // free buffer memory, if allocated
    if (vfsPath != NULL)
    {
        platform_free(vfsPath);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_FileStream::GetLengthNative___I8__STRING__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_INT64 length = 0;

    const char *workingPath = stack.Arg1().RecoverString();
    const char *fileName = stack.Arg2().RecoverString();

    char *filePath = NULL;
    char *vfsPath = NULL;
    struct stat fileInfo;

    FAULT_ON_NULL(workingPath);
    FAULT_ON_NULL(fileName);

    // setup file path
    filePath = (char *)platform_malloc(2 * FF_LFN_BUF + 1);

    // sanity check for successfull malloc
    if (filePath == NULL)
    {
        // failed to allocate memory
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    // clear working buffer
    filePath[0] = 0;

    // compose file path
    CombinePathAndName(filePath, workingPath, fileName);

    // Convert to ESP32 VFS path
    vfsPath = ConvertToVfsPath(filePath);

    if (stat(vfsPath, &fileInfo) == 0)
    {
        if (S_ISREG(fileInfo.st_mode))
        {
            length = fileInfo.st_size;
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
        }
    }
    else
    {
        // failed to create the file
        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
    }

    stack.SetResult_I8(length);

    NANOCLR_CLEANUP();

    // free buffers memory, if allocated
    if (filePath != NULL)
    {
        platform_free(filePath);
    }
    if (vfsPath != NULL)
    {
        platform_free(vfsPath);
    }
    NANOCLR_CLEANUP_END();
}
