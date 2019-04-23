//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ff.h>
#include "win_storage_native.h"
#include <target_windows_storage_config.h>
#include <nanoHAL_Windows_Storage.h>
#include <target_platform.h>


// defining these types here to make it shorter and improve code readability
typedef Library_win_storage_native_Windows_Storage_StorageFolder StorageFolder;
typedef Library_win_storage_native_Windows_Storage_StorageFile StorageFile;

extern "C"
{
    bool Storage_InitSDCardMMC(char * vfsName, int maxFiles, bool bit1Mode);
    bool Storage_InitSDCardSPI(char * vfsName, int maxFiles, int pin_Miso, int pin_Mosi, int pin_Clk, int pin_Cs);
};

// Queue length for IO task
#define StorageQueueLength	4


//////////////////////////////////////////

enum  FileOperationType { READ_TEXT, WRITE_TEXT, READ_BINARY, WRITE_BINARY, EXIT };

struct FileOperation
{
    FileOperationType	Operation;
    FILE*				File;
    char*				Content;
    uint32_t			ContentLength;
};

struct OperationResult
{
    FRESULT    result;
};


// Queues for passing Storage IO operations to thread abd returning results
static QueueHandle_t StorageInputQueue;
static QueueHandle_t StorageResultQueue;


char * ConvertToESP32Path(const char * filepath);


// Send IO operation to Queue
BaseType_t SendIOtoQueue(FileOperationType type, FILE *file, char * content, uint32_t contentLength)
{
    FileOperation      fileIoOperation;

    // setup FileIO operation
    fileIoOperation.Operation = type;
    fileIoOperation.File = file;
    fileIoOperation.Content = content;
    fileIoOperation.ContentLength = contentLength;

    return xQueueSendToBack(StorageInputQueue, &fileIoOperation, portMAX_DELAY);
}

// Wait for Result of IO operation
FRESULT WaitIOResult()
{
    OperationResult result;

    xQueueReceive(StorageResultQueue, &result, portMAX_DELAY);

    return result.result;
}

// All file IO is serialised by queue job to this IO thread.
//

// this is the FileIO working thread
// because FatFS is supposed to be atomic we won't have any concurrent threads

// ReadText working thread
static FRESULT StorageReadText(FileOperation*  fileIoOperation )
{
    FRESULT         operationResult;

    // need an extra one for the terminator
    uint32_t readLength = fileIoOperation->ContentLength + 1;
    
    // read string
    if (fgets(fileIoOperation->Content, readLength, fileIoOperation->File) )
    {
        operationResult = FR_OK;
    }
    else
    {
        operationResult = FR_DISK_ERR;
    }

    // close file
    fclose(fileIoOperation->File);

    return operationResult;
}


// WriteBinary working thread
static FRESULT StorageWriteBinary(FileOperation*  fileIoOperation)
{
    FRESULT     operationResult = FR_DISK_ERR;
    size_t      bytesWritten;

    bytesWritten = fwrite(fileIoOperation->Content, 1, fileIoOperation->ContentLength, fileIoOperation->File);
    if( bytesWritten == fileIoOperation->ContentLength )
    {
        // expected number of bytes written
        operationResult = FR_OK;
    }

    // close file
    fclose(fileIoOperation->File);

    return operationResult;
}

// ReadBinary working thread
static FRESULT StorageReadBinary(FileOperation*  fileIoOperation)
{
    FRESULT     operationResult = FR_DISK_ERR;
    UINT        bytesRead;

    bytesRead = fread( fileIoOperation->Content, 1, fileIoOperation->ContentLength, fileIoOperation->File);
    

    if( bytesRead == fileIoOperation->ContentLength)
    {
        // expected number of bytes read
        operationResult = FR_OK;
    }

    // close file
    fclose(fileIoOperation->File);

    return operationResult;
}


void StorageIOTask(void *pvParameters)
{
    (void)pvParameters;

    bool bExit = false;
    struct FileOperation  fileOpMessage;
    struct OperationResult operationResult;

    while (!bExit)
    {
        // Wait for message on Storage Queue
        if (xQueueReceive(StorageInputQueue, &fileOpMessage, (TickType_t)10))
        {
            switch (fileOpMessage.Operation)
            {
            case EXIT:
                bExit = true;
                operationResult.result = FR_OK;
                break;

            case READ_TEXT:
                operationResult.result = StorageReadText(&fileOpMessage);
                break;

            case READ_BINARY:
                operationResult.result = StorageReadBinary(&fileOpMessage);
                break;

            case WRITE_TEXT:
            case WRITE_BINARY:
                operationResult.result = StorageWriteBinary(&fileOpMessage);
                break;

            default:
                operationResult.result = FR_OK;
                break;
            };

            // Queue result and fire event
			xQueueSend(StorageResultQueue, &operationResult, portMAX_DELAY);

            // fire event for FileIO operation complete
			if (fileOpMessage.Operation != EXIT)
				Events_Set(SYSTEM_EVENT_FLAG_STORAGE_IO);
        }
    }

	vTaskDelete(NULL);
}

void CreateStorageIOTask()
{
    StorageInputQueue = xQueueCreate(StorageQueueLength, sizeof(FileOperation));
    StorageResultQueue = xQueueCreate(StorageQueueLength, sizeof(OperationResult));

    xTaskCreate(StorageIOTask, "StorageIOTask", 1500, 0, 6, NULL);
}

void DeleteQueues()
{
    vQueueDelete(StorageInputQueue);
    vQueueDelete(StorageResultQueue);
}

void Storage_Initialize()
{
    // Start IO Task
    CreateStorageIOTask();
}

void Storage_Uninitialize()
{
    // Close down IO task
    SendIOtoQueue(EXIT, 0, 0, 0);
    WaitIOResult();

    DeleteQueues();
}

////////////////////////////////////////////////
// Developer notes:
// Depending on the content size these operations have the potential to be a long running ones as the string or buffer is written to the storage.
// Despite we are not (yet!) async this is better handled by spawning a thread where the actual data transfer occurs and not blocking the execution.
// The underlying RTOS inheritably takes care of making this happen "in the background".
// When the operation is completed a CLR event is fired and the thread execution resumes.
// Being hard to estimate the expected duration of the operation (depends on storage hardware, CPU clock, transfer speed, etc)
// the timeout is set to an infinite timeout
// the catch is that the working thread MUST ALWAYS return at some point
////////////////////////////////////////////////

HRESULT Library_win_storage_native_Windows_Storage_FileIO::WriteBytes___STATIC__VOID__WindowsStorageIStorageFile__SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array* bufferArray;
    CLR_RT_HeapBlock    hbTimeout;
    CLR_INT64*          timeout;
    bool                eventResult = true;

    char*               buffer;
    uint32_t            bufferLength;

    const TCHAR*        filePath;
    char *              workingPath = NULL;
    FILE*               file;
    FRESULT             operationResult;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
        
    // get a pointer to the buffer
    bufferArray = stack.Arg1().DereferenceArray();

    buffer = (char*)bufferArray->GetFirstElement();

    bufferLength = bufferArray->m_numOfElements;

    // get a pointer to the file path
    filePath = (TCHAR*)pThis[ StorageFile::FIELD___path ].DereferenceString()->StringText();

    // !! need to cast to CLR_INT64 otherwise it wont setup a proper timeout infinite
    hbTimeout.SetInteger((CLR_INT64)-1);

    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks( hbTimeout, timeout ));
    
    if(stack.m_customState == 1)
    {   
        // open file (which is supposed to already exist)
        // need to use FA_OPEN_ALWAYS("w+") because we are writting the file content from start
        workingPath = ConvertToESP32Path(filePath);
        file = fopen(workingPath, "w+");
        if(file != NULL)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
        }
        else
        {
            // protect the StorageFile and the content buffer from GC so the working thread can access those
            CLR_RT_ProtectFromGC gcStorageFile( *pThis );
            CLR_RT_ProtectFromGC gcContent( *bufferArray );

            // setup FileIO operation
            if ( SendIOtoQueue(WRITE_BINARY, file, buffer, bufferLength) == errQUEUE_FULL)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
            }

            // bump custom state
            stack.m_customState = 2;
        }
    }

    while(eventResult)
    {
        // non-blocking wait allowing other threads to run while we wait for the write operation to complete
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.WaitEvents( stack.m_owningThread, *timeout, CLR_RT_ExecutionEngine::c_Event_StorageIo, eventResult ));

        if(eventResult)
        {
            // event occurred
            operationResult = WaitIOResult();

            if(operationResult == FR_DISK_ERR)
            {
                NANOCLR_SET_AND_LEAVE( CLR_E_FILE_IO );
            }
            else if(operationResult == FR_NO_FILE)
            {
                NANOCLR_SET_AND_LEAVE( CLR_E_FILE_NOT_FOUND );
            }
            else if(operationResult == FR_INVALID_DRIVE)
            {
                // failed to change drive
                NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
            }

            // done here
            break;
        }
        else
        {
            NANOCLR_SET_AND_LEAVE( CLR_E_TIMEOUT );
        }
    }

    // pop timeout heap block from stack
    stack.PopValue();

    NANOCLR_CLEANUP();

    // free buffer memory, if allocated
    if (workingPath != NULL)
    {
        platform_free(workingPath);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_win_storage_native_Windows_Storage_FileIO::WriteText___STATIC__VOID__WindowsStorageIStorageFile__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_String* content;
    CLR_RT_HeapBlock    hbTimeout;
    CLR_INT64*          timeout;
    bool                eventResult = true;

    const TCHAR*        filePath;
    char *              workingPath = NULL;
    FILE *              file;
    FRESULT             operationResult;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
        
    // get a pointer to the content
    content = stack.Arg1().DereferenceString();

    // get a pointer to the file path
    filePath = (TCHAR*)pThis[ StorageFile::FIELD___path ].DereferenceString()->StringText();

    // !! need to cast to CLR_INT64 otherwise it wont setup a proper timeout infinite
    hbTimeout.SetInteger((CLR_INT64)-1);

    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks( hbTimeout, timeout ));
    
    if(stack.m_customState == 1)
    {   

        // open file (which is supposed to already exist)
        // need to use FA_OPEN_ALWAYS("w+") because we are writting the file content from start
        workingPath = ConvertToESP32Path(filePath);
        file = fopen(workingPath, "w+" );
        if(file == NULL)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
        }
        else
        {
            // protect the StorageFile and the content buffer from GC so the working thread can access those
            CLR_RT_ProtectFromGC gcStorageFile( *pThis );
            CLR_RT_ProtectFromGC gcContent( *content );

            if (SendIOtoQueue(WRITE_TEXT, file, (char*)content->StringText(), hal_strlen_s(content->StringText())) == errQUEUE_FULL)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
            }

            // bump custom state
            stack.m_customState = 2;
        }
    }

    while(eventResult)
    {
        // non-blocking wait allowing other threads to run while we wait for the write operation to complete
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.WaitEvents( stack.m_owningThread, *timeout, CLR_RT_ExecutionEngine::c_Event_StorageIo, eventResult ));

        if(eventResult)
        {
            // event occurred
            operationResult = WaitIOResult();

            if(operationResult == FR_DISK_ERR)
            {
                NANOCLR_SET_AND_LEAVE( CLR_E_FILE_IO );
            }
            else if(operationResult == FR_NO_FILE)
            {
                NANOCLR_SET_AND_LEAVE( CLR_E_FILE_NOT_FOUND );
            }
            else if(operationResult == FR_INVALID_DRIVE)
            {
                // failed to change drive
                NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
            }

            // done here
            break;
        }
        else
        {
            NANOCLR_SET_AND_LEAVE( CLR_E_TIMEOUT );
        }
    }

    // pop timeout heap block from stack
    stack.PopValue();

    NANOCLR_CLEANUP();

    // free buffer memory, if allocated
    if (workingPath != NULL)
    {
        platform_free(workingPath);
    }

    NANOCLR_CLEANUP_END(); 
}

HRESULT Library_win_storage_native_Windows_Storage_FileIO::ReadBufferNative___STATIC__VOID__WindowsStorageIStorageFile__BYREF_SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    
    CLR_RT_HeapBlock    hbTimeout;
    CLR_INT64*          timeout;
    bool                eventResult = true;

    const TCHAR*        filePath;
    char *              workingPath = NULL;

    FILE*               file;
    struct stat         fileStat;
    FRESULT             operationResult;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    // !! need to cast to CLR_INT64 otherwise it wont setup a proper timeout infinite
    hbTimeout.SetInteger((CLR_INT64)-1);

    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks( hbTimeout, timeout ));

    // get a pointer to the file path
    filePath = (TCHAR*)pThis[ StorageFile::FIELD___path ].DereferenceString()->StringText();
   
    if(stack.m_customState == 1)
    {
        // protect the StorageFile from GC
        CLR_RT_ProtectFromGC gcStorageFile( *pThis );

        // open file (which is supposed to already exist)
        // need to use FA_OPEN_EXISTING("r") because we are reading an existing file content from start
        workingPath = ConvertToESP32Path(filePath);
        file = fopen(workingPath, "r");
        if(file == NULL)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
        }
        else
        {
            // get file details
            stat(workingPath, &fileStat);

            CLR_RT_HeapBlock buffer;
            buffer.SetObjectReference( NULL );
            CLR_RT_ProtectFromGC gc2( buffer );

            // create a new byte array with the appropriate size (and type)
            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(buffer, (CLR_INT32)fileStat.st_size, g_CLR_RT_WellKnownTypes.m_UInt8));

            // store this to the argument passed byref
            NANOCLR_CHECK_HRESULT(buffer.StoreToReference( stack.Arg1(), 0 ));

            // get a pointer to the buffer array to improve readability on the code ahead
            CLR_RT_HeapBlock_Array* bufferArray = buffer.DereferenceArray();

            if (SendIOtoQueue(READ_BINARY, file, (char*)bufferArray->GetFirstElement(), bufferArray->m_numOfElements) == errQUEUE_FULL)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
            }
 
            // bump custom state
            stack.m_customState = 2;
        }
    }

    while(eventResult)
    {
        // non-blocking wait allowing other threads to run while we wait for the write operation to complete
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.WaitEvents( stack.m_owningThread, *timeout, CLR_RT_ExecutionEngine::c_Event_StorageIo, eventResult ));

        if(eventResult)
        {
            // event occurred
            operationResult = WaitIOResult();
            
            if(operationResult == FR_DISK_ERR)
            {
                NANOCLR_SET_AND_LEAVE( CLR_E_FILE_IO );
            }
            else if(operationResult == FR_NO_FILE)
            {
                NANOCLR_SET_AND_LEAVE( CLR_E_FILE_NOT_FOUND );
            }
            else if(operationResult == FR_INVALID_DRIVE)
            {
                // failed to change drive
                NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
            }

            // done here
            break;
        }
        else
        {
            NANOCLR_SET_AND_LEAVE( CLR_E_TIMEOUT );
        }
    }

    // pop timeout heap block from stack
    stack.PopValue();

    NANOCLR_CLEANUP();

    // free buffer memory, if allocated
    if (workingPath != NULL)
    {
        platform_free(workingPath);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_win_storage_native_Windows_Storage_FileIO::ReadTextNative___STATIC__VOID__WindowsStorageIStorageFile__BYREF_STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock    hbTimeout;
    CLR_INT64*          timeout;
    bool                eventResult = true;

    const TCHAR*        filePath;
    char *              workingPath = NULL;

    FILE *              file;
    struct stat         fileStat;
    FRESULT             operationResult;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    // !! need to cast to CLR_INT64 otherwise it wont setup a proper timeout infinite
    hbTimeout.SetInteger((CLR_INT64)-1);

    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks( hbTimeout, timeout ));

    // get a pointer to the file path
    filePath = (TCHAR*)pThis[ StorageFile::FIELD___path ].DereferenceString()->StringText();
   
    if(stack.m_customState == 1)
    {
        // protect the StorageFile and the content buffer from GC so the working thread can access those
        CLR_RT_ProtectFromGC gcStorageFile( *pThis );

        // open file (which is supposed to already exist)
        // need to use FA_OPEN_EXISTING("r") because we are reading an existing file content from start
        workingPath = ConvertToESP32Path(filePath);
        file = fopen(workingPath, "r");
        if(file == NULL)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
        }
        else
        {
            // get file details
            stat(workingPath, &fileStat);

            // create a new string object with the appropriate size
            CLR_RT_HeapBlock  hbText;
            hbText.SetObjectReference( NULL );
            CLR_RT_ProtectFromGC gc( hbText );

            CLR_RT_HeapBlock_String* textString = CLR_RT_HeapBlock_String::CreateInstance( hbText, (CLR_UINT32)fileStat.st_size );
            FAULT_ON_NULL(textString);

            // store this to the argument passed byref
            NANOCLR_CHECK_HRESULT(hbText.StoreToReference( stack.Arg1(), 0 ));

            // get a pointer to the buffer array to improve readability on the code ahead
            hbText.DereferenceString();

            if ( SendIOtoQueue(READ_TEXT, file, (char*)textString->StringText(), fileStat.st_size) == errQUEUE_FULL)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
            }

            // bump custom state
            stack.m_customState = 2;
        }
    }

    while(eventResult)
    {
        // non-blocking wait allowing other threads to run while we wait for the write operation to complete
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.WaitEvents( stack.m_owningThread, *timeout, CLR_RT_ExecutionEngine::c_Event_StorageIo, eventResult ));

        if(eventResult)
        {
            // event occurred
            operationResult = WaitIOResult();

            if(operationResult == FR_DISK_ERR)
            {
                NANOCLR_SET_AND_LEAVE( CLR_E_FILE_IO );
            }
            else if(operationResult == FR_NO_FILE)
            {
                NANOCLR_SET_AND_LEAVE( CLR_E_FILE_NOT_FOUND );
            }
            else if(operationResult == FR_INVALID_DRIVE)
            {
                // failed to change drive
                NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
            }

            // done here
            break;
        }
        else
        {
            NANOCLR_SET_AND_LEAVE( CLR_E_TIMEOUT );
        }
    }

    // pop timeout heap block from stack
    stack.PopValue();

    NANOCLR_CLEANUP();

    // free buffer memory, if allocated
    if (workingPath != NULL)
    {
        platform_free(workingPath);
    }

    NANOCLR_CLEANUP_END();
}
