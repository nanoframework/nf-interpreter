//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <ff.h>
#include "win_storage_native.h"
#include <target_windows_storage_config.h>
#include <Target_Windows_Storage.h>
#include <nanoHAL_Windows_Storage.h>

#if HAL_USBH_USE_MSD
#include "usbh/dev/msd.h"
#endif

// defining these types here to make it shorter and improve code readability
typedef Library_win_storage_native_Windows_Storage_StorageFolder StorageFolder;
typedef Library_win_storage_native_Windows_Storage_StorageFile StorageFile;

//////////////////////////////////////////

struct ReadTextOperation
{
    FIL*    File;  
    int32_t ContentLength;
};

struct WriteTextOperation
{
    FIL*    File;
    char*   Content;
    int32_t ContentLength;
};

// this is the FileIO working thread
// because FatFS is supposed to be atomic we won't have any concurrent threads
static thread_t* fileIoWorkingThread;

// ReadText working thread
static THD_FUNCTION(ReadTextWorkingThread, arg)
{
    ReadTextOperation*  fileIoOperation = (ReadTextOperation*)arg;

    // need an extra one for the terminator
    uint32_t readLength = fileIoOperation->ContentLength + 1;

    // allocate buffer for reading string
    char* buffer = (char*)platform_malloc(readLength);
    if(buffer)
    {
        // read string
        f_gets((TCHAR*)buffer, readLength, fileIoOperation->File);

        // close file
        f_close(fileIoOperation->File);

        // free memory
        platform_free(fileIoOperation->File);
        platform_free(fileIoOperation);

        // fire event for FileIO operation complete
        Events_Set(SYSTEM_EVENT_FLAG_STORAGE_IO);

        chThdExit((msg_t)buffer);
    }
    else
    {
        chThdExit((msg_t)NULL);
    }
}

// WriteText working thread
static THD_FUNCTION(WriteTextWorkingThread, arg)
{
    FRESULT         operationResult;

    WriteTextOperation*  fileIoOperation = (WriteTextOperation*)arg;
    if(f_puts(fileIoOperation->Content, fileIoOperation->File) == fileIoOperation->ContentLength)
    {
        // expected number of bytes written
        operationResult = FR_OK;
    }

    // close file
    f_close(fileIoOperation->File);

    // free memory
    platform_free(fileIoOperation->File);
    platform_free(fileIoOperation);

    // fire event for FileIO operation complete
    Events_Set(SYSTEM_EVENT_FLAG_STORAGE_IO);
  
    chThdExit(operationResult);
}

//////////////////////////////////////////

HRESULT Library_win_storage_native_Windows_Storage_FileIO::ReadText___STATIC__STRING__WindowsStorageIStorageFile( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock    hbTimeout;
    CLR_INT64*          timeout;
    bool                eventResult = true;

    ReadTextOperation*  fileIoOperation;

    char workingDrive[DRIVE_LETTER_LENGTH];
    const TCHAR*        filePath;
    char*               buffer = NULL;

    FIL*                file;
    static FILINFO      fileInfo;
    FRESULT             operationResult;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    // being hard to estimate the expected duration (depends on storage hardware, CPU clock, transfer speed, etc)
    // we set this to an infinite timeout
    // the catch is that the working thread MUST ALWAYS return at some point
    // !! need to cast to CLR_INT64 otherwise it wont setup a proper timeout infinite
    hbTimeout.SetInteger((CLR_INT64)-1);

    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks( hbTimeout, timeout ));

    // get a pointer to the file path
    filePath = (TCHAR*)pThis[ StorageFile::FIELD___path ].DereferenceString()->StringText();
   
    if(stack.m_customState == 1)
    {
        // protect the StorageFile and the content buffer from GC so the working thread can access those
        CLR_RT_ProtectFromGC gcStorageFile( *pThis );

        // copy the first 2 letters of the path for the drive
        // path is 'D:\folder\file.txt', so we need 'D:'
        memcpy(workingDrive, filePath, DRIVE_LETTER_LENGTH);

        // create file struct
        file = (FIL*)platform_malloc(sizeof(FIL));
        // check allocation
        if(file == NULL)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }

        // open file (which is supposed to already exist)
        // need to use FA_OPEN_EXISTING because we are reading an existing file content from start
        operationResult = f_open(file, filePath, FA_OPEN_EXISTING | FA_READ);
        
        if(operationResult != FR_OK)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
        }
        else
        {
            // get file details
            f_stat(filePath, &fileInfo);

            // setup FileIO operation
            fileIoOperation = (ReadTextOperation*)platform_malloc(sizeof(ReadTextOperation));

            fileIoOperation->File = file;
            fileIoOperation->ContentLength = fileInfo.fsize;

            // spawn working thread to perform the read transaction
            fileIoWorkingThread = chThdCreateFromHeap(NULL, THD_WORKING_AREA_SIZE(2048),
                                        "STRT", NORMALPRIO, ReadTextWorkingThread, fileIoOperation);

            if(fileIoWorkingThread == NULL)
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

            // ChibiOS requirement: need to call chThdWait on working thread in order to have it's memory released to the heap, otherwise it won't be returned
            buffer = (char*)chThdWait(fileIoWorkingThread);

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

    // set return value, if the buffer has content
    if(buffer)
    {
        stack.SetResult_String( buffer );
    }
    else
    {
        // OK to leave without freeing buffer memory as it seems it wasn't allocated anyway
        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
    }
    
    if(buffer)
    {
        platform_free(buffer);
    }

    NANOCLR_NOCLEANUP();
}

////////////////////////////////////////////////
// Developer notes:
// Depending on the content size this operation has the potential to be a long running one as the string is written to the storage.
// Despite we are not (yet!) async this is better handled by spawning a thread where the actual data transfer occurs and not blocking the execution.
// The underlying RTOS inheritably takes care of making this happen "in the background".
// When the operation is completed a CLR event is fired and the thread execution resumes.
////////////////////////////////////////////////
HRESULT Library_win_storage_native_Windows_Storage_FileIO::WriteText___STATIC__VOID__WindowsStorageIStorageFile__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_String* content;

    char workingDrive[DRIVE_LETTER_LENGTH];

    CLR_RT_HeapBlock    hbTimeout;
    CLR_INT64*          timeout;
    bool                eventResult = true;

    WriteTextOperation*  fileIoOperation;

    const TCHAR*         filePath;
    FIL*                file;
    FRESULT             operationResult;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
        
    // get a pointer to the content
    content = stack.Arg1().DereferenceString();

    // get a pointer to the file path
    filePath = (TCHAR*)pThis[ StorageFile::FIELD___path ].DereferenceString()->StringText();

    // being hard to estimate the expected duration (depends on storage hardware, CPU clock, transfer speed, etc)
    // we set this to an infinite timeout
    // the catch is that the working thread MUST ALWAYS return at some point
    // !! need to cast to CLR_INT64 otherwise it wont setup a proper timeout infinite
    hbTimeout.SetInteger((CLR_INT64)-1);

    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks( hbTimeout, timeout ));
    
    if(stack.m_customState == 1)
    {   
        // copy the first 2 letters of the path for the drive
        // path is 'D:\folder\file.txt', so we need 'D:'
        memcpy(workingDrive, filePath, DRIVE_LETTER_LENGTH);

        // create file struct
        file = (FIL*)platform_malloc(sizeof(FIL));
        // check allocation
        if(file == NULL)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }

        // open file (which is supposed to already exist)
        // need to use FA_OPEN_ALWAYS because we are writting the file content from start
        operationResult = f_open(file, filePath, FA_OPEN_ALWAYS | FA_WRITE);
        
        if(operationResult != FR_OK)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
        }
        else
        {
            // protect the StorageFile and the content buffer from GC so the working thread can access those
            CLR_RT_ProtectFromGC gcStorageFile( *pThis );
            CLR_RT_ProtectFromGC gcContent( *content );

            // setup FileIO operation
            fileIoOperation = (WriteTextOperation*)platform_malloc(sizeof(WriteTextOperation));

            fileIoOperation->File = file;
            fileIoOperation->Content = (char*)content->StringText();
            fileIoOperation->ContentLength = hal_strlen_s(fileIoOperation->Content);

            // spawn working thread to perform the write transaction
            fileIoWorkingThread = chThdCreateFromHeap(NULL, THD_WORKING_AREA_SIZE(2048),
                                        "STWT", NORMALPRIO, WriteTextWorkingThread, fileIoOperation);

            if(fileIoWorkingThread == NULL)
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

            // ChibiOS requirement: need to call chThdWait on working thread in order to have it's memory released to the heap, otherwise it won't be returned
            operationResult = (FRESULT)chThdWait(fileIoWorkingThread);

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

    NANOCLR_NOCLEANUP();
}
