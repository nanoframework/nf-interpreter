//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_sys_io_filesystem.h"

#include <ff.h>
#include <nanoHAL_Windows_Storage.h>

extern void CombinePathAndName(char *outpath, const char *path1, const char *path2);
extern char *ConvertToVfsPath(const char *filepath);

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

    const char *workingPath = stack.Arg0().RecoverString();
    const char *fileName = stack.Arg1().RecoverString();

    bool exists = false;
    int operationResult;
    char *filePath = NULL;
    char *vfsPath = NULL;
    struct stat fno;

    FAULT_ON_NULL(workingPath);
    FAULT_ON_NULL(fileName);

    // TODO don't really need to pass in path & filename, whole path would have been OK for ESP32

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

    vfsPath = ConvertToVfsPath(filePath);

    operationResult = stat(vfsPath, &fno);
    if (operationResult == 0)
    {
        if (S_ISREG(fno.st_mode))
        {
            exists = true;
        }
    }

    stack.SetResult_Boolean(exists);

    NANOCLR_CLEANUP();

    // free buffer memory, if allocated
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

HRESULT Library_nf_sys_io_filesystem_System_IO_File::MoveNative___STATIC__VOID__STRING__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    char *vfsPathSrc = NULL;
    char *vfsPathDest = NULL;

    const char *filePathSrc = stack.Arg0().RecoverString();
    const char *filePathDest = stack.Arg1().RecoverString();

    FAULT_ON_NULL(filePathSrc);
    FAULT_ON_NULL(filePathDest);

    vfsPathSrc = ConvertToVfsPath(filePathSrc);
    vfsPathDest = ConvertToVfsPath(filePathDest);

    // rename/move file
    if (rename(vfsPathSrc, vfsPathDest) != 0)
    {
        if (errno == ENOENT)
        {
            // Invalid path/file not found
            NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
        }
        else if (errno == EEXIST)
        {
            // file already exists
            NANOCLR_SET_AND_LEAVE(CLR_E_PATH_ALREADY_EXISTS);
        }
        else
        {
            // Some other error
            NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
        }
    }

    NANOCLR_CLEANUP();

    // free memory buffers , if allocated
    if (vfsPathSrc != NULL)
    {
        platform_free(vfsPathSrc);
    }

    if (vfsPathDest != NULL)
    {
        platform_free(vfsPathDest);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_File::DeleteNative___STATIC__VOID__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    char *vfsPath = NULL;
    const char *filePath = stack.Arg0().RecoverString();
    FAULT_ON_NULL(filePath);

    vfsPath = ConvertToVfsPath(filePath);

    // Delete file
    if (unlink(vfsPath) != 0)
    {
        // file doesn't exist
        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
    }

    NANOCLR_CLEANUP();

    if (vfsPath != NULL)
    {
        platform_free(vfsPath);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_File::GetAttributesNative___STATIC__U1__STRING(CLR_RT_StackFrame &stack)
{
    char *workingPath = NULL;

    NANOCLR_HEADER();
    {
        CLR_UINT8 attributes = 0xFF;

        struct stat fileInfo;

        const char *filePath = stack.Arg0().RecoverString();
        FAULT_ON_NULL(filePath);

        // For non FATFS drives use VFS stat() method (Internal drive)
        // For FATFS drives use f_stat() method
        if (IsInternalFilePath(filePath))
        {
            workingPath = ConvertToVfsPath(filePath);

            // Get infos about file
            if (stat(workingPath, &fileInfo) == 0)
            {
                // Convert attributes
                attributes = 0;
                if (fileInfo.st_mode & S_IFDIR)
                {
                    attributes |= FileAttributes::FileAttributes_Directory;
                }
            }
            else
            {
                if (errno == ENOENT)
                {
                    // Invalid path/file not found
                    NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
                }
                else
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
                }
            }
        }
        else
        {
            // We don't know the fatfs drive used by VFS 0:,  1: etc
            // So for the moment assume "0:"" as that ok for 1 mounted FATFS drive

            // Take copy of filepath, use vfspath string
            int srcLen = hal_strlen_s(filePath);
            workingPath = (char *)platform_malloc(srcLen + 1);
            if (workingPath == NULL)
            {
                // failed to allocate memory
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
            }

            hal_strcpy_s(workingPath, srcLen + 1, filePath);

            // Make drive 0:
            workingPath[0] = '0';

            FILINFO fno;

            // Get infos about file
            FRESULT operationResult = f_stat(workingPath, &fno);
            if (operationResult == FR_OK)
            {
                attributes = fno.fattrib;
            }
            else if (operationResult == FR_NO_FILE)
            {
                // File/Directory not found
                NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
            }
            else
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
            }
        }
        stack.SetResult_U1(attributes);
    }
    NANOCLR_CLEANUP();

    if (workingPath != NULL)
    {
        platform_free(workingPath);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_File::SetAttributesNative___STATIC__VOID__STRING__U1(
    CLR_RT_StackFrame &stack)
{
    char *workingPath = NULL;

    NANOCLR_HEADER();
    {
        FRESULT operationResult;
        const char *filePath = stack.Arg0().RecoverString();
        FAULT_ON_NULL(filePath);

        CLR_UINT8 attributes = stack.Arg1().NumericByRef().u1;

        // TODO fix
        // No way to change attribute with ESP32 VFS so need to call FATFS if driver is FAT drive, anything other than
        // internal drive But we don't know the fatfs drive used by VFS 0:,  1: etc For the moment just use 0: as that
        // ok for 1 mounted FATFS drive For internal drive just ignore
        if (!IsInternalFilePath(filePath))
        {
            // Take copy of filepath
            int srcLen = hal_strlen_s(filePath);
            workingPath = (char *)platform_malloc(srcLen + 1);
            if (workingPath == NULL)
            {
                // failed to allocate memory
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
            }

            hal_strcpy_s(workingPath, srcLen + 1, filePath);

            // Make drive 0:
            workingPath[0] = '0';

            // Get infos about file
            operationResult = f_chmod(workingPath, attributes, 0xFF);
            if (operationResult != FR_OK)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
            }
        }
    }
    NANOCLR_CLEANUP();

    if (workingPath != NULL)
    {
        platform_free(workingPath);
    }

    NANOCLR_CLEANUP_END();
}

SYSTEMTIME GetDateTimeFromStat(time_t *time)
{
    SYSTEMTIME fileTime;

    memset(&fileTime, 0, sizeof(SYSTEMTIME));

    tm *ptm = gmtime(time);

    // Year from 1601
    fileTime.wYear = ptm->tm_year + 1900; // from Year based 1900
    // Month 1-12
    fileTime.wMonth = ptm->tm_mon + 1; // from Month 0-11
    // Day of month 1 - 31
    fileTime.wDay = ptm->tm_mday;
    // Day of week 0 = Sunday
    fileTime.wDayOfWeek = ptm->tm_wday; // 0 = Sunday
    // Hour 0 to 23.
    fileTime.wHour = ptm->tm_hour;
    // Minutes 0 to 59.
    fileTime.wMinute = ptm->tm_min;
    // seconds 0 to 59
    fileTime.wSecond = ptm->tm_sec;

    return fileTime;
}

HRESULT Library_nf_sys_io_filesystem_System_IO_File::GetLastWriteTimeNative___STATIC__SystemDateTime__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    SYSTEMTIME fileInfoTime;
    CLR_RT_TypeDescriptor dtType;
    struct stat fileInfo;
    char *vfsPath = NULL;
    CLR_INT64 *pRes;
    CLR_RT_HeapBlock &ref = stack.PushValue();

    const char *filePath = stack.Arg0().RecoverString();
    FAULT_ON_NULL(filePath);

    vfsPath = ConvertToVfsPath(filePath);

    if (stat(vfsPath, &fileInfo) != 0)
    {
        // file doesn't exist
        NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_FOUND);
    }

    // get the date time details and return it on Stack as DateTime object
    fileInfoTime = GetDateTimeFromStat(&(fileInfo.st_mtime));

    // initialize <DateTime> type descriptor
    NANOCLR_CHECK_HRESULT(dtType.InitializeFromType(g_CLR_RT_WellKnownTypes.m_DateTime));

    // create an instance of <DateTime>
    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObject(ref, dtType.m_handlerCls));

    pRes = Library_corlib_native_System_DateTime::GetValuePtr(ref);
    *pRes = HAL_Time_ConvertFromSystemTime(&fileInfoTime);

    NANOCLR_CLEANUP();

    if (vfsPath != NULL)
    {
        platform_free(vfsPath);
    }

    NANOCLR_CLEANUP_END();
}
