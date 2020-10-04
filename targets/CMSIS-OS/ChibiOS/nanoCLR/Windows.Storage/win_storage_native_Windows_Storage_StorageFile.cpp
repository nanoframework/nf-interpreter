//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

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

extern SYSTEMTIME GetDateTime(uint16_t date, uint16_t time);

HRESULT Library_win_storage_native_Windows_Storage_StorageFile::DeleteFileNative___VOID(CLR_RT_StackFrame& stack)
{
		NANOCLR_HEADER();

    char workingDrive[DRIVE_LETTER_LENGTH];
    const char* workingPath;
    char*       workingBuffer = NULL;

		// get a pointer to the managed object instance and check that it's not NULL
		CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

		// get a pointer to the path in managed field
		workingPath = pThis[Library_win_storage_native_Windows_Storage_StorageFile::FIELD___path].DereferenceString()->StringText();

		// copy the first 2 letters of the path for the drive
		// path is 'D:\folder\file.txt', so we need 'D:'
		memcpy(workingDrive, workingPath, DRIVE_LETTER_LENGTH);

		if(WORKING_DRIVE_IS_INTERNAL_DRIVE)
		{
			#if (USE_SPIFFS_FOR_STORAGE == TRUE)
				// SPIFFS drive workflow

				// get file name removing the drive letter
				workingBuffer = (char*)platform_malloc(SPIFFS_OBJ_NAME_LEN);
				// sanity check for successfull malloc
				if(workingBuffer == NULL)
				{
					// failed to allocate memory
					NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
				}
				memcpy(workingBuffer, (workingPath + 3), SPIFFS_OBJ_NAME_LEN - 3);

				if(SPIFFS_remove(&fs, workingBuffer) < 0)
				{
					NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
				}

			#endif
		}
		else
		{	
			#if ((HAL_USE_SDC == TRUE) || (HAL_USBH_USE_MSD == TRUE))
				// FatFS workflow

				// Delete folder
				FRESULT operationResult = f_unlink(workingPath);
				if (operationResult != FR_OK)
				{
					// folder doesn't exist
					NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
				}
			#endif
		}

    NANOCLR_CLEANUP();

    if(workingBuffer != NULL)
    {
        platform_free(workingBuffer);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_win_storage_native_Windows_Storage_StorageFile::RenameFileNative___VOID__STRING(CLR_RT_StackFrame& stack)
{
		NANOCLR_HEADER();

    char workingDrive[DRIVE_LETTER_LENGTH];
    const char* workingPath;
		const char* desiredPath;
    char*       workingBuffer = NULL;
    char*       workingBuffer1 = NULL;

		// get a pointer to the managed object instance and check that it's not NULL
		CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

		// get a pointer to the path in managed field
		workingPath = pThis[Library_win_storage_native_Windows_Storage_StorageFile::FIELD___path].DereferenceString()->StringText();

		// get a pointer to the desired folder path
		desiredPath = stack.Arg1().DereferenceString()->StringText();

		// copy the first 2 letters of the path for the drive
		// path is 'D:\folder\file.txt', so we need 'D:'
		memcpy(workingDrive, workingPath, DRIVE_LETTER_LENGTH);

		if(WORKING_DRIVE_IS_INTERNAL_DRIVE)
		{
			#if (USE_SPIFFS_FOR_STORAGE == TRUE)
				// SPIFFS drive workflow

				// get origin and desired file names removing the drive letter
				// need to malloc these
				workingBuffer = (char*)platform_malloc(SPIFFS_OBJ_NAME_LEN);
				workingBuffer1 = (char*)platform_malloc(SPIFFS_OBJ_NAME_LEN);
				// sanity check for successfull malloc
				if( (workingBuffer == NULL) ||
					(workingBuffer1 == NULL))
				{
					// failed to allocate memory
					NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
				}
				memcpy(workingBuffer, (workingPath + 3), SPIFFS_OBJ_NAME_LEN - 3);
				memcpy(workingBuffer1, (desiredPath + 3), SPIFFS_OBJ_NAME_LEN - 3);

				if(SPIFFS_rename(&fs, workingBuffer, workingBuffer1) < 0)
				{
					switch(SPIFFS_errno(&fs))
					{
						case SPIFFS_ERR_NOT_FOUND:
							NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
							break;

						default:
							NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
							break;
					}
				}

			#endif
		}
		else
		{	
			#if ((HAL_USE_SDC == TRUE) || (HAL_USBH_USE_MSD == TRUE))
				// FatFS workflow

				// rename folder
				FRESULT operationResult = f_rename(workingPath, desiredPath);
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
			#endif
		}

    NANOCLR_CLEANUP();

    if(workingBuffer != NULL)
    {
        platform_free(workingBuffer);
    }
    if(workingBuffer1 != NULL)
    {
        platform_free(workingBuffer1);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_win_storage_native_Windows_Storage_StorageFile::GetFileFromPathNative___STATIC__WindowsStorageStorageFile__STRING__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Index    storageFileTypeDef;
    CLR_RT_HeapBlock*       storageFile;

    char workingDrive[DRIVE_LETTER_LENGTH];
    char*       workingBuffer = NULL;
    const char* filePath;
    const char* fileName;

    // get a pointer to the file path
    filePath = stack.Arg0().DereferenceString()->StringText();

    // get a pointer to the file name
    fileName = stack.Arg1().DereferenceString()->StringText();

    // copy the first 2 letters of the path for the drive
    // path is 'D:\folder\file.txt', so we need 'D:'
    memcpy(workingDrive, filePath, DRIVE_LETTER_LENGTH);

    if(WORKING_DRIVE_IS_INTERNAL_DRIVE)
    {
      #if (USE_SPIFFS_FOR_STORAGE == TRUE)
        // SPIFFS drive workflow

        // get file name removing the drive letter
        // need to malloc this
        workingBuffer = (char*)platform_malloc(SPIFFS_OBJ_NAME_LEN);
        // sanity check for successfull malloc
        if(workingBuffer == NULL)
        {
            // failed to allocate memory
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }
        memcpy(workingBuffer, (filePath + 3), SPIFFS_OBJ_NAME_LEN - 3);

        spiffs_file file = SPIFFS_open(&fs, workingBuffer, SPIFFS_RDONLY, 0);
        if(file < 0)
        {
            switch(SPIFFS_errno(&fs))
            {
                case SPIFFS_ERR_NOT_FOUND:
                    SPIFFS_close(&fs, file);
                    NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
                    break;

                default:
                    SPIFFS_close(&fs, file);
                    NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
                    break;
            }
        }
        
        SPIFFS_close(&fs, file);

        // compose return object
        // find <StorageFile> type, don't bother checking the result as it exists for sure
        g_CLR_RT_TypeSystem.FindTypeDef("StorageFile", "Windows.Storage", storageFileTypeDef);

        // create a <StorageFile>
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(stack.PushValue(), storageFileTypeDef));

        // get a handle to the storage file
        storageFile = stack.TopValue().Dereference();
        
        // file name
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(storageFile[Library_win_storage_native_Windows_Storage_StorageFile::FIELD___name], fileName));

        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(storageFile[Library_win_storage_native_Windows_Storage_StorageFile::FIELD___path], filePath));

        // SPIFFS files don't have date information so we are done here

      #endif
    }
    else
    {	
      #if ((HAL_USE_SDC == TRUE) || (HAL_USBH_USE_MSD == TRUE))
        // FatFS workflow

        FILINFO fileInfo;
        SYSTEMTIME  fileInfoTime;

        FRESULT fr = f_stat(filePath, &fileInfo);
        if(fr == FR_OK)
        {
            // is this a file?
            if ( !(fileInfo.fattrib & AM_DIR) )
            {
                // path represents a file, we are good

                // compose return object
                // find <StorageFile> type, don't bother checking the result as it exists for sure
                g_CLR_RT_TypeSystem.FindTypeDef("StorageFile", "Windows.Storage", storageFileTypeDef);

                // create a <StorageFile>
                NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(stack.PushValue(), storageFileTypeDef));

                // get a handle to the storage file
                storageFile = stack.TopValue().Dereference();
                
                // file name
                NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(storageFile[Library_win_storage_native_Windows_Storage_StorageFile::FIELD___name], fileName));

                NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(storageFile[Library_win_storage_native_Windows_Storage_StorageFile::FIELD___path], filePath));

                // get the date time details and fill in the managed field
                // compute directory date
                fileInfoTime = GetDateTime(fileInfo.fdate, fileInfo.ftime);

                // get a reference to the dateCreated managed field...
                CLR_RT_HeapBlock& dateFieldRef = storageFile[Library_win_storage_native_Windows_Storage_StorageFile::FIELD___dateCreated];
                CLR_INT64* pRes = (CLR_INT64*)&dateFieldRef.NumericByRef().s8;
                // ...and set it with the fileInfoTime
                *pRes = HAL_Time_ConvertFromSystemTime( &fileInfoTime );
            }
            else
            {
                // path exists but it's a folder
                NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
            }
        }
        else if(fr == FR_NO_FILE)
        {
            // file doesn't exist
            NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
        }
        else
        {
            // IO error
            NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
        }

      #endif
    }

    NANOCLR_CLEANUP();

    if(workingBuffer != NULL)
    {
        platform_free(workingBuffer);
    }

    NANOCLR_CLEANUP_END();
}
