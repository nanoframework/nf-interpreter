//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#if (HAL_USE_SDC == TRUE) || (HAL_USBH_USE_MSD == TRUE)
#include <ff.h>
#endif
#include "win_storage_native_target.h"
#include <target_windows_storage_config.h>
#include <nanoHAL_Windows_Storage.h>

#if (HAL_USBH_USE_MSD == TRUE)
#include "usbh/dev/msd.h"
#endif
#if (USE_SPIFFS_FOR_STORAGE == TRUE)
#include <hal_spiffs.h>
#endif

// defining these types here to make it shorter and improve code readability
typedef Library_win_storage_native_Windows_Storage_StorageFolder StorageFolder;
typedef Library_win_storage_native_Windows_Storage_StorageFile StorageFile;

//////////////////////////////////////////
enum FileOperationResult
{
    FileOperationResult_OK              = 0,
    FileOperationResult_Error           = 10,
    FileOperationResult_NoFile          = 20,
    FileOperationResult_InvalidDrive    = 30,
};

//////////////////////////////////////////

struct FileOperation
{
  #if (HAL_USE_SDC == TRUE) || (HAL_USBH_USE_MSD == TRUE)
    FIL*            FatFile;
  #endif
  #if USE_SPIFFS_FOR_STORAGE
    spiffs_file*    SpiffsFile;
  #endif
    char*           Content;
    uint32_t        ContentLength;
};

// this is the FileIO working thread
// because FatFS is supposed to be atomic we won't have any concurrent threads
static thread_t* fileIoWorkingThread;

// ReadText working thread
static THD_FUNCTION(ReadTextWorkingThread, arg)
{
    FileOperationResult opResult = FileOperationResult_OK;

    FileOperation*  fileIoOperation = (FileOperation*)arg;

    // need an extra one for the terminator
    uint32_t readLength = fileIoOperation->ContentLength + 1;

  #if ((HAL_USE_SDC == TRUE) || (HAL_USBH_USE_MSD == TRUE))
    if(fileIoOperation->FatFile != NULL)
    {
        // read string
        if(f_gets((TCHAR*)fileIoOperation->Content, readLength, fileIoOperation->FatFile))
        {
            // operation succesfull
        }
        else
        {
            switch((FRESULT)f_error(fileIoOperation->FatFile))
            {
                case FR_NO_FILE:
                    opResult = FileOperationResult_NoFile;
                    break;

                case FR_INVALID_DRIVE:
                    opResult = FileOperationResult_InvalidDrive;
                    break;
                
                default:
                    opResult = FileOperationResult_Error;
                    break;
            }
        }

        // close file
        f_close(fileIoOperation->FatFile);

        // free memory
        platform_free(fileIoOperation->FatFile);

    }
  #endif
  #if USE_SPIFFS_FOR_STORAGE
    if(fileIoOperation->SpiffsFile != NULL)
    {
        uint32_t bytesRead = SPIFFS_read(&fs, *fileIoOperation->SpiffsFile, fileIoOperation->Content, fileIoOperation->ContentLength);

        if (bytesRead == fileIoOperation->ContentLength)
        {
            // expected number of bytes read
        }
        else
        {
            // failed to read expected number of bytes
            opResult = FileOperationResult_Error;
        }

        // close file
        SPIFFS_close(&fs, *fileIoOperation->SpiffsFile);

        // free memory
        platform_free(fileIoOperation->SpiffsFile);
    }
  #endif

    // free memory
    platform_free(fileIoOperation);

    // fire event for FileIO operation complete
    Events_Set(SYSTEM_EVENT_FLAG_STORAGE_IO);

    chThdExit((msg_t)opResult);
}

// WriteText working thread
static THD_FUNCTION(WriteTextWorkingThread, arg)
{
    FileOperationResult opResult = FileOperationResult_OK;

    FileOperation*  fileIoOperation = (FileOperation*)arg;

  #if ((HAL_USE_SDC == TRUE) || (HAL_USBH_USE_MSD == TRUE))
    if(fileIoOperation->FatFile != NULL)
    {
        if(f_puts(fileIoOperation->Content, fileIoOperation->FatFile) == (int)fileIoOperation->ContentLength)
        {
            // expected number of bytes written
        }
        else
        {
            switch((FRESULT)f_error(fileIoOperation->FatFile))
            {
                case FR_NO_FILE:
                    opResult = FileOperationResult_NoFile;
                    break;

                case FR_INVALID_DRIVE:
                    opResult = FileOperationResult_InvalidDrive;
                    break;
                
                default:
                    opResult = FileOperationResult_Error;
                    break;
            }
        }

        // close file
        f_close(fileIoOperation->FatFile);

        // free memory
        platform_free(fileIoOperation->FatFile);

    }
  #endif
  #if USE_SPIFFS_FOR_STORAGE
    if(fileIoOperation->SpiffsFile != NULL)
    {
        if (SPIFFS_write(&fs, *fileIoOperation->SpiffsFile, fileIoOperation->Content, fileIoOperation->ContentLength) < 0)
        {
            // failed to write expected number of bytes
            opResult = FileOperationResult_Error;
        }

        // close file
        SPIFFS_close(&fs, *fileIoOperation->SpiffsFile);

        // free memory
        platform_free(fileIoOperation->SpiffsFile);
    }
  #endif

    platform_free(fileIoOperation);

    // fire event for FileIO operation complete
    Events_Set(SYSTEM_EVENT_FLAG_STORAGE_IO);
  
    chThdExit((msg_t)opResult);
}

// WriteBinary working thread
static THD_FUNCTION(WriteBinaryWorkingThread, arg)
{
    FileOperationResult opResult = FileOperationResult_OK;

    FileOperation*  fileIoOperation = (FileOperation*)arg;

  #if ((HAL_USE_SDC == TRUE) || (HAL_USBH_USE_MSD == TRUE))
    if(fileIoOperation->FatFile != NULL)
    {
        UINT        bytesWritten;

        FRESULT operationResult = f_write(fileIoOperation->FatFile, fileIoOperation->Content, fileIoOperation->ContentLength, &bytesWritten);

        if( (operationResult == FR_OK) && 
            (bytesWritten == fileIoOperation->ContentLength))
        {
            // expected number of bytes written
        }
        else
        {
            switch((FRESULT)f_error(fileIoOperation->FatFile))
            {
                case FR_NO_FILE:
                    opResult = FileOperationResult_NoFile;
                    break;

                case FR_INVALID_DRIVE:
                    opResult = FileOperationResult_InvalidDrive;
                    break;
                
                default:
                    opResult = FileOperationResult_Error;
                    break;
            }
        }

        // close file
        f_close(fileIoOperation->FatFile);

        // free memory
        platform_free(fileIoOperation->FatFile);

    }
  #endif
  #if USE_SPIFFS_FOR_STORAGE
    if(fileIoOperation->SpiffsFile != NULL)
    {
        if (SPIFFS_write(&fs, *fileIoOperation->SpiffsFile, fileIoOperation->Content, fileIoOperation->ContentLength) < 0)
        {
            // failed to write expected number of bytes
            opResult = FileOperationResult_Error;
        }

        // close file
        SPIFFS_close(&fs, *fileIoOperation->SpiffsFile);

        // free memory
        platform_free(fileIoOperation->SpiffsFile);
    }
  #endif

    // free memory
    platform_free(fileIoOperation);

    // fire event for FileIO operation complete
    Events_Set(SYSTEM_EVENT_FLAG_STORAGE_IO);
  
    chThdExit((msg_t)opResult);
}

// ReadBinary working thread
static THD_FUNCTION(ReadBinaryWorkingThread, arg)
{
    FileOperationResult opResult = FileOperationResult_OK;

    FileOperation*  fileIoOperation = (FileOperation*)arg;

  #if ((HAL_USE_SDC == TRUE) || (HAL_USBH_USE_MSD == TRUE))
    if(fileIoOperation->FatFile != NULL)
    {
        UINT bytesRead;

        FileOperation*  fileIoOperation = (FileOperation*)arg;

        FRESULT operationResult = f_read(fileIoOperation->FatFile, fileIoOperation->Content, fileIoOperation->ContentLength, &bytesRead);

        if( (operationResult == FR_OK) && 
            (bytesRead == fileIoOperation->ContentLength))
        {
            // expected number of bytes read
        }
        else
        {
            switch((FRESULT)f_error(fileIoOperation->FatFile))
            {
                case FR_NO_FILE:
                    opResult = FileOperationResult_NoFile;
                    break;

                case FR_INVALID_DRIVE:
                    opResult = FileOperationResult_InvalidDrive;
                    break;
                
                default:
                    opResult = FileOperationResult_Error;
                    break;
            }
        }

        // close file
        f_close(fileIoOperation->FatFile);

        // free memory
        platform_free(fileIoOperation->FatFile);

    }
  #endif
  #if USE_SPIFFS_FOR_STORAGE
    if(fileIoOperation->SpiffsFile != NULL)
    {
        uint32_t bytesRead = SPIFFS_read(&fs, *fileIoOperation->SpiffsFile, fileIoOperation->Content, fileIoOperation->ContentLength);

        if (bytesRead == fileIoOperation->ContentLength)
        {
            // expected number of bytes read
        }
        else
        {
            // failed to read expected number of bytes
            opResult = FileOperationResult_Error;
        }

        // close file
        SPIFFS_close(&fs, *fileIoOperation->SpiffsFile);

        // free memory
        platform_free(fileIoOperation->SpiffsFile);
    }
  #endif

    // free memory
    platform_free(fileIoOperation);

    // fire event for FileIO operation complete
    Events_Set(SYSTEM_EVENT_FLAG_STORAGE_IO);
  
    chThdExit((msg_t)opResult);
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

    char workingDrive[DRIVE_LETTER_LENGTH];
    const char*         filePath;
    char*               workingBuffer = NULL;
    FileOperation*      fileIoOperation;

    char*               buffer;
    uint32_t            bufferLength;

  #if ((HAL_USE_SDC == TRUE) || (HAL_USBH_USE_MSD == TRUE))
    FIL*                fatFile = NULL;
    FRESULT             operationResult;
  #endif
  #if (USE_SPIFFS_FOR_STORAGE == TRUE)
    spiffs_file*        spiffsFile = NULL;
  #endif

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
        
    // get a pointer to the buffer
    bufferArray = stack.Arg1().DereferenceArray();

    buffer = (char*)bufferArray->GetFirstElement();

    bufferLength = bufferArray->m_numOfElements;

    // get a pointer to the file path
    filePath = pThis[ StorageFile::FIELD___path ].DereferenceString()->StringText();

    // !! need to cast to CLR_INT64 otherwise it wont setup a proper timeout infinite
    hbTimeout.SetInteger((CLR_INT64)-1);

    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks( hbTimeout, timeout ));
    
    if(stack.m_customState == 1)
    {   
        // copy the first 2 letters of the path for the drive
        // path is 'D:\folder\file.txt', so we need 'D:'
        memcpy(workingDrive, filePath, DRIVE_LETTER_LENGTH);

        if(WORKING_DRIVE_IS_INTERNAL_DRIVE)
        {
          #if (USE_SPIFFS_FOR_STORAGE == TRUE)
            // SPIFFS drive workflow

            // create file struct
            spiffsFile = (spiffs_file*)platform_malloc(sizeof(spiffs_file));
            // check allocation
            if(spiffsFile == NULL)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
            }

            // get file name removing the drive letter
            workingBuffer = (char*)platform_malloc(SPIFFS_OBJ_NAME_LEN);
            // sanity check for successfull malloc
            if(workingBuffer == NULL)
            {
                // failed to allocate memory
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
            }
            memcpy(workingBuffer, (filePath + 3), SPIFFS_OBJ_NAME_LEN - 3);

            *spiffsFile = SPIFFS_open(&fs, workingBuffer, SPIFFS_TRUNC | SPIFFS_RDWR, 0);
            if(*spiffsFile < 0)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
            }

          #endif
        }
        else
        {
          #if ((HAL_USE_SDC == TRUE) || (HAL_USBH_USE_MSD == TRUE))
            // FatFS workflow

            // create file struct
            fatFile = (FIL*)platform_malloc(sizeof(FIL));
            // check allocation
            if(fatFile == NULL)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
            }

            // open file (which is supposed to already exist)
            // need to use FA_OPEN_ALWAYS because we are writting the file content from start
            operationResult = f_open(fatFile, filePath, FA_OPEN_ALWAYS | FA_WRITE);
            
            if(operationResult != FR_OK)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
            }

          #endif
        }

        // protect the content buffer from GC so the working thread can access those
        CLR_RT_ProtectFromGC gcContent( *bufferArray );

        // setup FileIO operation
        fileIoOperation = (FileOperation*)platform_malloc(sizeof(FileOperation));

      #if ((HAL_USE_SDC == TRUE) || (HAL_USBH_USE_MSD == TRUE))
        fileIoOperation->FatFile = fatFile;
      #endif
      #if USE_SPIFFS_FOR_STORAGE
        fileIoOperation->SpiffsFile = spiffsFile;
      #endif

        fileIoOperation->Content = buffer;
        fileIoOperation->ContentLength = bufferLength;

        // spawn working thread to perform the write transaction
        fileIoWorkingThread = chThdCreateFromHeap(NULL, THD_WORKING_AREA_SIZE(2048),
                                    "STWT", NORMALPRIO, WriteBinaryWorkingThread, fileIoOperation);

        if(fileIoWorkingThread == NULL)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
        }

        // bump custom state
        stack.m_customState = 2;

    }

    while(eventResult)
    {
        // non-blocking wait allowing other threads to run while we wait for the write operation to complete
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.WaitEvents( stack.m_owningThread, *timeout, CLR_RT_ExecutionEngine::c_Event_StorageIo, eventResult ));

        if(eventResult)
        {
            // event occurred

            // ChibiOS requirement: need to call chThdWait on working thread in order to have it's memory released to the heap, otherwise it won't be returned
            FileOperationResult result = (FileOperationResult)chThdWait(fileIoWorkingThread);

            if(result == FileOperationResult_Error)
            {
                NANOCLR_SET_AND_LEAVE( CLR_E_FILE_IO );
            }
            else if(result == FileOperationResult_NoFile)
            {
                NANOCLR_SET_AND_LEAVE( CLR_E_FILE_NOT_FOUND );
            }
            else if(result == FileOperationResult_InvalidDrive)
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

    if(workingBuffer != NULL)
    {
        platform_free(workingBuffer);
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

    char workingDrive[DRIVE_LETTER_LENGTH];
    const char*         filePath;
    char*               workingBuffer = NULL;
    FileOperation*      fileIoOperation;

  #if ((HAL_USE_SDC == TRUE) || (HAL_USBH_USE_MSD == TRUE))
    FIL*                fatFile = NULL;
    FRESULT             operationResult;
  #endif
  #if (USE_SPIFFS_FOR_STORAGE == TRUE)
    spiffs_file*        spiffsFile = NULL;
  #endif

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
        
    // get a pointer to the content
    content = stack.Arg1().DereferenceString();

    // get a pointer to the file path
    filePath = pThis[ StorageFile::FIELD___path ].DereferenceString()->StringText();

    // !! need to cast to CLR_INT64 otherwise it wont setup a proper timeout infinite
    hbTimeout.SetInteger((CLR_INT64)-1);

    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks( hbTimeout, timeout ));
    
    if(stack.m_customState == 1)
    {   
        // copy the first 2 letters of the path for the drive
        // path is 'D:\folder\file.txt', so we need 'D:'
        memcpy(workingDrive, filePath, DRIVE_LETTER_LENGTH);

        if(WORKING_DRIVE_IS_INTERNAL_DRIVE)
        {
          #if (USE_SPIFFS_FOR_STORAGE == TRUE)
            // SPIFFS drive workflow

            // create file struct
            spiffsFile = (spiffs_file*)platform_malloc(sizeof(spiffs_file));
            // check allocation
            if(spiffsFile == NULL)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
            }

            // get file name removing the drive letter
            workingBuffer = (char*)platform_malloc(SPIFFS_OBJ_NAME_LEN);
            // sanity check for successfull malloc
            if(workingBuffer == NULL)
            {
                // failed to allocate memory
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
            }
            memcpy(workingBuffer, (filePath + 3), SPIFFS_OBJ_NAME_LEN - 3);

            *spiffsFile = SPIFFS_open(&fs, workingBuffer, SPIFFS_TRUNC | SPIFFS_RDWR, 0);
            if(*spiffsFile < 0)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
            }

          #endif
        }
        else
        {
          #if ((HAL_USE_SDC == TRUE) || (HAL_USBH_USE_MSD == TRUE))
            // FatFS workflow

            // create file struct
            fatFile = (FIL*)platform_malloc(sizeof(FIL));
            // check allocation
            if(fatFile == NULL)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
            }

            // open file (which is supposed to already exist)
            // need to use FA_OPEN_ALWAYS because we are writting the file content from start
            operationResult = f_open(fatFile, filePath, FA_OPEN_ALWAYS | FA_WRITE);
            
            if(operationResult != FR_OK)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
            }

          #endif
        }

        // protect the content buffer from GC so the working thread can access those
        CLR_RT_ProtectFromGC gcContent( *content );

        // setup FileIO operation
        fileIoOperation = (FileOperation*)platform_malloc(sizeof(FileOperation));

      #if ((HAL_USE_SDC == TRUE) || (HAL_USBH_USE_MSD == TRUE))
        fileIoOperation->FatFile = fatFile;
      #endif
      #if USE_SPIFFS_FOR_STORAGE
        fileIoOperation->SpiffsFile = spiffsFile;
      #endif

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

    while(eventResult)
    {
        // non-blocking wait allowing other threads to run while we wait for the write operation to complete
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.WaitEvents( stack.m_owningThread, *timeout, CLR_RT_ExecutionEngine::c_Event_StorageIo, eventResult ));

        if(eventResult)
        {
            // event occurred

            // ChibiOS requirement: need to call chThdWait on working thread in order to have it's memory released to the heap, otherwise it won't be returned
            FileOperationResult result = (FileOperationResult)chThdWait(fileIoWorkingThread);

            if(result == FileOperationResult_Error)
            {
                NANOCLR_SET_AND_LEAVE( CLR_E_FILE_IO );
            }
            else if(result == FileOperationResult_NoFile)
            {
                NANOCLR_SET_AND_LEAVE( CLR_E_FILE_NOT_FOUND );
            }
            else if(result == FileOperationResult_InvalidDrive)
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

    if(workingBuffer != NULL)
    {
        platform_free(workingBuffer);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_win_storage_native_Windows_Storage_FileIO::ReadBufferNative___STATIC__VOID__WindowsStorageIStorageFile__BYREF_SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    
    CLR_RT_HeapBlock    hbTimeout;
    CLR_INT64*          timeout;
    bool                eventResult = true;
    uint32_t            fileSize = 0;

    char workingDrive[DRIVE_LETTER_LENGTH];
    const char*         filePath;
    char*               workingBuffer = NULL;
    FileOperation*      fileIoOperation;

  #if ((HAL_USE_SDC == TRUE) || (HAL_USBH_USE_MSD == TRUE))
    FIL*                fatFile = NULL;
    FRESULT             operationResult;
  #endif
  #if (USE_SPIFFS_FOR_STORAGE == TRUE)
    spiffs_file*        spiffsFile = NULL;
  #endif

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    // !! need to cast to CLR_INT64 otherwise it wont setup a proper timeout infinite
    hbTimeout.SetInteger((CLR_INT64)-1);

    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks( hbTimeout, timeout ));

    // get a pointer to the file path
    filePath = pThis[ StorageFile::FIELD___path ].DereferenceString()->StringText();
   
    if(stack.m_customState == 1)
    {
        // copy the first 2 letters of the path for the drive
        // path is 'D:\folder\file.txt', so we need 'D:'
        memcpy(workingDrive, filePath, DRIVE_LETTER_LENGTH);

        if(WORKING_DRIVE_IS_INTERNAL_DRIVE)
        {
          #if (USE_SPIFFS_FOR_STORAGE == TRUE)
            // SPIFFS drive workflow

            // create file struct
            spiffsFile = (spiffs_file*)platform_malloc(sizeof(spiffs_file));
            // check allocation
            if(spiffsFile == NULL)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
            }

            // get file name removing the drive letter
            workingBuffer = (char*)platform_malloc(SPIFFS_OBJ_NAME_LEN);
            // sanity check for successfull malloc
            if(workingBuffer == NULL)
            {
                // failed to allocate memory
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
            }
            memcpy(workingBuffer, (filePath + 3), SPIFFS_OBJ_NAME_LEN - 3);

            *spiffsFile = SPIFFS_open(&fs, workingBuffer, SPIFFS_RDONLY, 0);
            if(*spiffsFile < 0)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
            }

            // get file details
            spiffs_stat fileInfo;
            SPIFFS_stat(&fs, workingBuffer, &fileInfo);

            // we are only interested in the file size
            fileSize = fileInfo.size;

          #endif
        }
        else
        {
          #if ((HAL_USE_SDC == TRUE) || (HAL_USBH_USE_MSD == TRUE))
            // FatFS workflow

            // create file struct
            fatFile = (FIL*)platform_malloc(sizeof(FIL));
            // check allocation
            if(fatFile == NULL)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
            }

            // open file (which is supposed to already exist)
            operationResult = f_open(fatFile, filePath, FA_OPEN_EXISTING | FA_READ);
            
            if(operationResult != FR_OK)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
            }

            // get file details
            FILINFO fileInfo; 
            f_stat(filePath, &fileInfo);

            // we are only interested in the file size
            fileSize = fileInfo.fsize;

          #endif
        }

        CLR_RT_HeapBlock buffer;
        buffer.SetObjectReference( NULL );
        CLR_RT_ProtectFromGC gc2( buffer );

        // create a new byte array with the appropriate size (and type)
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( buffer, (CLR_INT32)fileSize, g_CLR_RT_WellKnownTypes.m_UInt8 ));

        // store this to the argument passed byref
        NANOCLR_CHECK_HRESULT(buffer.StoreToReference( stack.Arg1(), 0 ));

        // get a pointer to the buffer array to improve readability on the code ahead
        CLR_RT_HeapBlock_Array* bufferArray = buffer.DereferenceArray();

        // setup FileIO operation
        fileIoOperation = (FileOperation*)platform_malloc(sizeof(FileOperation));

      #if ((HAL_USE_SDC == TRUE) || (HAL_USBH_USE_MSD == TRUE))
        fileIoOperation->FatFile = fatFile;
      #endif
      #if USE_SPIFFS_FOR_STORAGE
        fileIoOperation->SpiffsFile = spiffsFile;
      #endif

        fileIoOperation->Content = (char*)bufferArray->GetFirstElement();
        fileIoOperation->ContentLength = bufferArray->m_numOfElements;

        // spawn working thread to perform the read transaction
        fileIoWorkingThread = chThdCreateFromHeap(NULL, THD_WORKING_AREA_SIZE(2048),
                                    "STRB", NORMALPRIO, ReadBinaryWorkingThread, fileIoOperation);

        if(fileIoWorkingThread == NULL)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
        }

        // bump custom state
        stack.m_customState = 2;
    }

    while(eventResult)
    {
        // non-blocking wait allowing other threads to run while we wait for the write operation to complete
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.WaitEvents( stack.m_owningThread, *timeout, CLR_RT_ExecutionEngine::c_Event_StorageIo, eventResult ));

        if(eventResult)
        {
            // event occurred

            // ChibiOS requirement: need to call chThdWait on working thread in order to have it's memory released to the heap, otherwise it won't be returned
            FileOperationResult result = (FileOperationResult)chThdWait(fileIoWorkingThread);

            if(result == FileOperationResult_Error)
            {
                NANOCLR_SET_AND_LEAVE( CLR_E_FILE_IO );
            }
            else if(result == FileOperationResult_NoFile)
            {
                NANOCLR_SET_AND_LEAVE( CLR_E_FILE_NOT_FOUND );
            }
            else if(result == FileOperationResult_InvalidDrive)
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

    if(workingBuffer != NULL)
    {
        platform_free(workingBuffer);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_win_storage_native_Windows_Storage_FileIO::ReadTextNative___STATIC__VOID__WindowsStorageIStorageFile__BYREF_STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock    hbTimeout;
    CLR_INT64*          timeout;
    bool                eventResult = true;
    uint32_t            fileSize = 0;

    char workingDrive[DRIVE_LETTER_LENGTH];
    const char*         filePath;
    char*               workingBuffer = NULL;
    FileOperation*      fileIoOperation;

  #if ((HAL_USE_SDC == TRUE) || (HAL_USBH_USE_MSD == TRUE))
    FIL*                fatFile = NULL;
    FRESULT             operationResult;
  #endif
  #if (USE_SPIFFS_FOR_STORAGE == TRUE)
    spiffs_file*        spiffsFile = NULL;
  #endif

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    // !! need to cast to CLR_INT64 otherwise it wont setup a proper timeout infinite
    hbTimeout.SetInteger((CLR_INT64)-1);

    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks( hbTimeout, timeout ));

    // get a pointer to the file path
    filePath = pThis[ StorageFile::FIELD___path ].DereferenceString()->StringText();
   
    if(stack.m_customState == 1)
    {
        // copy the first 2 letters of the path for the drive
        // path is 'D:\folder\file.txt', so we need 'D:'
        memcpy(workingDrive, filePath, DRIVE_LETTER_LENGTH);

        if(WORKING_DRIVE_IS_INTERNAL_DRIVE)
        {
          #if (USE_SPIFFS_FOR_STORAGE == TRUE)
            // SPIFFS drive workflow

            // create file struct
            spiffsFile = (spiffs_file*)platform_malloc(sizeof(spiffs_file));
            // check allocation
            if(spiffsFile == NULL)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
            }

            // get file name removing the drive letter
            workingBuffer = (char*)platform_malloc(SPIFFS_OBJ_NAME_LEN);
            // sanity check for successfull malloc
            if(workingBuffer == NULL)
            {
                // failed to allocate memory
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
            }
            memcpy(workingBuffer, (filePath + 3), SPIFFS_OBJ_NAME_LEN - 3);

            *spiffsFile = SPIFFS_open(&fs, workingBuffer, SPIFFS_RDONLY, 0);
            if(*spiffsFile < 0)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
            }

            // get file details
            spiffs_stat fileInfo;
            SPIFFS_stat(&fs, workingBuffer, &fileInfo);

            // we are only interested in the file size
            fileSize = fileInfo.size;

          #endif
        }
        else
        {
          #if ((HAL_USE_SDC == TRUE) || (HAL_USBH_USE_MSD == TRUE))
            // FatFS workflow

            // create file struct
            fatFile = (FIL*)platform_malloc(sizeof(FIL));
            // check allocation
            if(fatFile == NULL)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
            }

            // open file (which is supposed to already exist)
            operationResult = f_open(fatFile, filePath, FA_OPEN_EXISTING | FA_READ);
            
            if(operationResult != FR_OK)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
            }

            // get file details
            FILINFO fileInfo; 
            f_stat(filePath, &fileInfo);

            // we are only interested in the file size
            fileSize = fileInfo.fsize;

          #endif
        }

        // create a new string object with the appropriate size
        CLR_RT_HeapBlock  hbText;
        hbText.SetObjectReference( NULL );
        CLR_RT_ProtectFromGC gc( hbText );

        CLR_RT_HeapBlock_String* textString = CLR_RT_HeapBlock_String::CreateInstance( hbText, (CLR_UINT32)fileSize );
        FAULT_ON_NULL(textString);

        // store this to the argument passed byref
        NANOCLR_CHECK_HRESULT(hbText.StoreToReference( stack.Arg1(), 0 ));

        // get a pointer to the buffer array to improve readability on the code ahead
        hbText.DereferenceString();

        // setup FileIO operation
        fileIoOperation = (FileOperation*)platform_malloc(sizeof(FileOperation));

      #if ((HAL_USE_SDC == TRUE) || (HAL_USBH_USE_MSD == TRUE))
        fileIoOperation->FatFile = fatFile;
      #endif
      #if USE_SPIFFS_FOR_STORAGE
        fileIoOperation->SpiffsFile = spiffsFile;
      #endif

        fileIoOperation->Content = (char*)textString->StringText();
        fileIoOperation->ContentLength = fileSize;

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

    while(eventResult)
    {
        // non-blocking wait allowing other threads to run while we wait for the write operation to complete
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.WaitEvents( stack.m_owningThread, *timeout, CLR_RT_ExecutionEngine::c_Event_StorageIo, eventResult ));

        if(eventResult)
        {
            // event occurred

            // ChibiOS requirement: need to call chThdWait on working thread in order to have it's memory released to the heap, otherwise it won't be returned
            FileOperationResult result = (FileOperationResult)chThdWait(fileIoWorkingThread);

            if(result == FileOperationResult_Error)
            {
                NANOCLR_SET_AND_LEAVE( CLR_E_FILE_IO );
            }
            else if(result == FileOperationResult_NoFile)
            {
                NANOCLR_SET_AND_LEAVE( CLR_E_FILE_NOT_FOUND );
            }
            else if(result == FileOperationResult_InvalidDrive)
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

    if(workingBuffer != NULL)
    {
        platform_free(workingBuffer);
    }

    NANOCLR_CLEANUP_END();
}
