//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "win_storage_native_target.h"

// defining these types here to make it shorter and improve code readability
typedef Library_win_storage_native_Windows_Storage_StorageFolder StorageFolder;
typedef Library_win_storage_native_Windows_Storage_StorageFile StorageFile;

//////////////////////////////////////////

struct FileIOFileOperation
{
    const char *FileName;
    char *Content;
    uint32_t ContentLength;
};

// this is the FileIO working thread
static volatile FRESULT threadOperationResult;

// ReadText working thread
static void ReadTextWorkingThread(void *arg)
{

    FileIOFileOperation *fileIoOperation = reinterpret_cast<FileIOFileOperation *>(arg);

    FIL file;

    // need an extra one for the terminator
    uint32_t readLength = fileIoOperation->ContentLength + 1;

    // open file (which is supposed to already exist)
    // need to use FA_OPEN_EXISTING because we are reading an existing file content from start
    threadOperationResult = f_open(&file, fileIoOperation->FileName, FA_OPEN_EXISTING | FA_READ);

    if (threadOperationResult != FR_OK)
    {
        // free memory
        platform_free(fileIoOperation);
        Events_Set(SYSTEM_EVENT_FLAG_STORAGE_IO);
        vTaskDelete(NULL);
    }

    // read string
    if (!(f_gets((TCHAR *)fileIoOperation->Content, readLength, &file)))
    {
        threadOperationResult = (FRESULT)f_error(&file);
    }

    // close file
    f_close(&file);

    // free memory
    platform_free(fileIoOperation);

    // fire event for FileIO operation complete
    Events_Set(SYSTEM_EVENT_FLAG_STORAGE_IO);

    vTaskDelete(NULL);
}

// WriteText working thread
static void WriteTextWorkingThread(void *arg)
{

    FileIOFileOperation *fileIoOperation = reinterpret_cast<FileIOFileOperation *>(arg);

    FIL file;

    // open file (which is supposed to already exist)
    // need to use FA_OPEN_ALWAYS because we are writting the file content from start
    threadOperationResult = f_open(&file, fileIoOperation->FileName, FA_OPEN_ALWAYS | FA_WRITE);

    if (threadOperationResult != FR_OK)
    {
        // free memory
        platform_free(fileIoOperation);
        Events_Set(SYSTEM_EVENT_FLAG_STORAGE_IO);
        vTaskDelete(NULL);
    }

    if (f_puts(fileIoOperation->Content, &file) != (int)fileIoOperation->ContentLength)
    {
        threadOperationResult = FR_DISK_ERR;
    }

    // close file
    f_close(&file);

    // free memory
    platform_free(fileIoOperation);

    // fire event for FileIO operation complete
    Events_Set(SYSTEM_EVENT_FLAG_STORAGE_IO);

    vTaskDelete(NULL);
}

// WriteBinary working thread
static void WriteBinaryWorkingThread(void *arg)
{
    UINT bytesWritten;

    FileIOFileOperation *fileIoOperation = reinterpret_cast<FileIOFileOperation *>(arg);

    FIL file;

    // open file (which is supposed to already exist)
    // need to use FA_OPEN_ALWAYS because we are writting the file content from start
    threadOperationResult = f_open(&file, fileIoOperation->FileName, FA_OPEN_ALWAYS | FA_WRITE);

    if (threadOperationResult != FR_OK)
    {
        // free memory
        platform_free(fileIoOperation);
        Events_Set(SYSTEM_EVENT_FLAG_STORAGE_IO);
        vTaskDelete(NULL);
    }

    threadOperationResult = f_write(&file, fileIoOperation->Content, fileIoOperation->ContentLength, &bytesWritten);
    if (bytesWritten != fileIoOperation->ContentLength)
    {
        threadOperationResult = FR_DISK_ERR;
    }
    // close file
    f_close(&file);

    // free memory
    platform_free(fileIoOperation);

    // fire event for FileIO operation complete
    Events_Set(SYSTEM_EVENT_FLAG_STORAGE_IO);

    vTaskDelete(NULL);
}

// ReadBinary working thread
static void ReadBinaryWorkingThread(void *arg)
{

    FileIOFileOperation *fileIoOperation = reinterpret_cast<FileIOFileOperation *>(arg);

    FIL file;

    // need an extra one for the terminator
    UINT bytesRead = fileIoOperation->ContentLength + 1;

    // open file (which is supposed to already exist)
    // need to use FA_OPEN_EXISTING because we are reading an existing file content from start
    threadOperationResult = f_open(&file, fileIoOperation->FileName, FA_OPEN_EXISTING | FA_READ);

    if (threadOperationResult != FR_OK)
    {
        // free memory
        platform_free(fileIoOperation);
        Events_Set(SYSTEM_EVENT_FLAG_STORAGE_IO);
        vTaskDelete(NULL);
    }

    threadOperationResult = f_read(&file, fileIoOperation->Content, fileIoOperation->ContentLength, &bytesRead);

    if (bytesRead != fileIoOperation->ContentLength)
    {
        threadOperationResult = FR_DISK_ERR;
    }

    // close file
    f_close(&file);

    platform_free(fileIoOperation);

    // fire event for FileIO operation complete
    Events_Set(SYSTEM_EVENT_FLAG_STORAGE_IO);

    vTaskDelete(NULL);
}

////////////////////////////////////////////////
// Developer notes:
// Depending on the content size these operations have the potential to be a long running ones as the string or buffer
// is written to the storage. Despite we are not (yet!) async this is better handled by spawning a thread where the
// actual data transfer occurs and not blocking the execution. The underlying RTOS inheritably takes care of making this
// happen "in the background". When the operation is completed a CLR event is fired and the thread execution resumes.
// Being hard to estimate the expected duration of the operation (depends on storage hardware, CPU clock, transfer
// speed, etc) the timeout is set to an infinite timeout the catch is that the working thread MUST ALWAYS return at some
// point
////////////////////////////////////////////////

HRESULT Library_win_storage_native_Windows_Storage_FileIO::
    WriteBytes___STATIC__VOID__WindowsStorageIStorageFile__SZARRAY_U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array *bufferArray;

    CLR_RT_HeapBlock hbTimeout;
    CLR_INT64 *timeout;
    bool eventResult = true;

    const TCHAR *filePath;

    char *buffer;
    uint32_t bufferLength;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get a pointer to the buffer
    bufferArray = stack.Arg1().DereferenceArray();

    buffer = (char *)bufferArray->GetFirstElement();

    bufferLength = bufferArray->m_numOfElements;

    // get a pointer to the file path
    filePath = (TCHAR *)pThis[StorageFile::FIELD___path].DereferenceString()->StringText();

    // !! need to cast to CLR_INT64 otherwise it wont setup a proper timeout infinite
    hbTimeout.SetInteger((CLR_INT64)-1);

    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));

    if (stack.m_customState == 1)
    {
        // protect the content buffer from GC so the working thread can access those
        CLR_RT_ProtectFromGC gcContent(*bufferArray);

        // setup FileIO operation
        FileIOFileOperation *fileIoOperation =
            reinterpret_cast<FileIOFileOperation *>(platform_malloc(sizeof(FileIOFileOperation)));

        // fileIoOperation->File = file;
        fileIoOperation->FileName = filePath;
        fileIoOperation->Content = buffer;
        fileIoOperation->ContentLength = bufferLength;

        // spawn working thread to perform the write transaction
        BaseType_t ret;
        ret = xTaskCreate(
            WriteBinaryWorkingThread,
            "WriteBin",
            configMINIMAL_STACK_SIZE + 600,
            fileIoOperation,
            configMAX_PRIORITIES - 2,
            NULL);

        if (ret != pdPASS)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
        }

        // bump custom state
        stack.m_customState = 2;
    }

    while (eventResult)
    {
        // non-blocking wait allowing other threads to run while we wait for the write operation to complete
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_StorageIo, eventResult));

        if (eventResult)
        {
            // event occurred

            if (threadOperationResult == FR_DISK_ERR)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
            }
            else if (threadOperationResult == FR_NO_FILE)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
            }
            else if (threadOperationResult == FR_INVALID_DRIVE)
            {
                // failed to change drive
                NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
            }

            // done here
            break;
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
        }
    }
    // pop timeout heap block from stack
    stack.PopValue();

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_storage_native_Windows_Storage_FileIO::WriteText___STATIC__VOID__WindowsStorageIStorageFile__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_String *content;

    CLR_RT_HeapBlock hbTimeout;
    CLR_INT64 *timeout;
    bool eventResult = true;

    const TCHAR *filePath;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get a pointer to the content
    content = stack.Arg1().DereferenceString();

    // get a pointer to the file path
    filePath = (TCHAR *)pThis[StorageFile::FIELD___path].DereferenceString()->StringText();

    // !! need to cast to CLR_INT64 otherwise it wont setup a proper timeout infinite
    hbTimeout.SetInteger((CLR_INT64)-1);

    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));

    if (stack.m_customState == 1)
    {
        // protect the StorageFile and the content buffer from GC so the working thread can access those
        CLR_RT_ProtectFromGC gcStorageFile(*pThis);
        CLR_RT_ProtectFromGC gcContent(*content);

        // setup FileIO operation
        FileIOFileOperation *fileIoOperation =
            reinterpret_cast<FileIOFileOperation *>(platform_malloc(sizeof(FileIOFileOperation)));

        fileIoOperation->FileName = filePath;
        fileIoOperation->Content = (char *)content->StringText();
        fileIoOperation->ContentLength = hal_strlen_s(fileIoOperation->Content);

        // spawn working thread to perform the write transaction
        BaseType_t ret;
        ret = xTaskCreate(
            WriteTextWorkingThread,
            "WriteText",
            configMINIMAL_STACK_SIZE + 400,
            fileIoOperation,
            configMAX_PRIORITIES - 2,
            NULL);

        if (ret != pdPASS)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
        }

        // bump custom state
        stack.m_customState = 2;
    }

    while (eventResult)
    {
        // non-blocking wait allowing other threads to run while we wait for the write operation to complete
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_StorageIo, eventResult));

        if (eventResult)
        {
            // event occurred
            if (threadOperationResult == FR_DISK_ERR)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
            }
            else if (threadOperationResult == FR_NO_FILE)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
            }
            else if (threadOperationResult == FR_INVALID_DRIVE)
            {
                // failed to change drive
                NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
            }

            // done here
            break;
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
        }
    }
    // pop timeout heap block from stack
    stack.PopValue();

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_storage_native_Windows_Storage_FileIO::
    ReadBufferNative___STATIC__VOID__WindowsStorageIStorageFile__BYREF_SZARRAY_U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock hbTimeout;
    CLR_INT64 *timeout;
    bool eventResult = true;

    const TCHAR *filePath;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // !! need to cast to CLR_INT64 otherwise it wont setup a proper timeout infinite
    hbTimeout.SetInteger((CLR_INT64)-1);

    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));

    // get a pointer to the file path
    filePath = (TCHAR *)pThis[StorageFile::FIELD___path].DereferenceString()->StringText();

    if (stack.m_customState == 1)
    {
        // get file details
        static FILINFO fileInfo;
        f_stat(filePath, &fileInfo);

        CLR_RT_HeapBlock buffer;
        buffer.SetObjectReference(NULL);
        CLR_RT_ProtectFromGC gc2(buffer);

        // create a new byte array with the appropriate size (and type)
        NANOCLR_CHECK_HRESULT(
            CLR_RT_HeapBlock_Array::CreateInstance(buffer, (CLR_INT32)fileInfo.fsize, g_CLR_RT_WellKnownTypes.m_UInt8));

        // store this to the argument passed byref
        NANOCLR_CHECK_HRESULT(buffer.StoreToReference(stack.Arg1(), 0));

        // get a pointer to the buffer array to improve readability on the code ahead
        CLR_RT_HeapBlock_Array *bufferArray = buffer.DereferenceArray();

        // setup FileIO operation
        FileIOFileOperation *fileIoOperation =
            reinterpret_cast<FileIOFileOperation *>(platform_malloc(sizeof(FileIOFileOperation)));

        fileIoOperation->FileName = filePath;
        fileIoOperation->Content = (char *)bufferArray->GetFirstElement();
        fileIoOperation->ContentLength = bufferArray->m_numOfElements;

        // spawn working thread to perform the read transaction
        BaseType_t ret;
        ret = xTaskCreate(
            ReadBinaryWorkingThread,
            "ReadBin",
            configMINIMAL_STACK_SIZE + 600,
            fileIoOperation,
            configMAX_PRIORITIES - 2,
            NULL);

        if (ret != pdPASS)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
        }

        // bump custom state
        stack.m_customState = 2;
    }

    while (eventResult)
    {
        // non-blocking wait allowing other threads to run while we wait for the write operation to complete
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_StorageIo, eventResult));

        if (eventResult)
        {
            // event occurred

            if (threadOperationResult == FR_DISK_ERR)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
            }
            else if (threadOperationResult == FR_NO_FILE)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
            }
            else if (threadOperationResult == FR_INVALID_DRIVE)
            {
                // failed to change drive
                NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
            }
            // done here
            break;
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
        }
    }
    // pop timeout heap block from stack
    stack.PopValue();

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_storage_native_Windows_Storage_FileIO::
    ReadTextNative___STATIC__VOID__WindowsStorageIStorageFile__BYREF_STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock hbTimeout;
    CLR_INT64 *timeout;
    bool eventResult = true;

    const TCHAR *filePath;

    static FILINFO fileInfo;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // !! need to cast to CLR_INT64 otherwise it wont setup a proper timeout infinite
    hbTimeout.SetInteger((CLR_INT64)-1);

    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));

    // get a pointer to the file path
    filePath = (TCHAR *)pThis[StorageFile::FIELD___path].DereferenceString()->StringText();

    if (stack.m_customState == 1)
    {
        // protect the StorageFile and the content buffer from GC so the working thread can access those
        CLR_RT_ProtectFromGC gcStorageFile(*pThis);

        // get file details
        f_stat(filePath, &fileInfo);

        // create a new string object with the appropriate size
        CLR_RT_HeapBlock hbText;
        hbText.SetObjectReference(NULL);
        CLR_RT_ProtectFromGC gc(hbText);

        CLR_RT_HeapBlock_String *textString =
            CLR_RT_HeapBlock_String::CreateInstance(hbText, (CLR_UINT32)fileInfo.fsize);
        FAULT_ON_NULL(textString);

        // store this to the argument passed byref
        NANOCLR_CHECK_HRESULT(hbText.StoreToReference(stack.Arg1(), 0));

        // setup FileIO operation
        FileIOFileOperation *fileIoOperation = (FileIOFileOperation *)platform_malloc(sizeof(FileIOFileOperation));

        fileIoOperation->FileName = filePath;
        fileIoOperation->Content = (char *)textString->StringText();
        fileIoOperation->ContentLength = fileInfo.fsize;

        // spawn working thread to perform the read transaction
        BaseType_t ret;
        ret = xTaskCreate(
            ReadTextWorkingThread,
            "ReadText",
            configMINIMAL_STACK_SIZE + 300,
            fileIoOperation,
            configMAX_PRIORITIES - 2,
            NULL);

        if (ret != pdPASS)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
        }

        // bump custom state
        stack.m_customState = 2;
    }

    while (eventResult)
    {
        // non-blocking wait allowing other threads to run while we wait for the write operation to complete
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_StorageIo, eventResult));

        if (eventResult)
        {
            // event occurred

            if (threadOperationResult == FR_DISK_ERR)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
            }
            else if (threadOperationResult == FR_NO_FILE)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
            }
            else if (threadOperationResult == FR_INVALID_DRIVE)
            {
                // failed to change drive
                NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
            }

            // done here
            break;
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
        }
    }
    // pop timeout heap block from stack
    stack.PopValue();

    NANOCLR_NOCLEANUP();
}
