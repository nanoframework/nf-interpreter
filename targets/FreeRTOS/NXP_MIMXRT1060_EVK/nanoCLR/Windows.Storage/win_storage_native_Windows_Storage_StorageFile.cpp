//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ff.h>
#include "win_storage_native.h"

extern SYSTEMTIME GetDateTime(uint16_t date, uint16_t time);

HRESULT Library_win_storage_native_Windows_Storage_StorageFile::DeleteFileNative___VOID(CLR_RT_StackFrame& stack)
{
	NANOCLR_HEADER();

	const char* workingPath;
	FRESULT     operationResult;

	// get a pointer to the managed object instance and check that it's not NULL
	CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

	// get a pointer to the path in managed field
	workingPath = pThis[Library_win_storage_native_Windows_Storage_StorageFile::FIELD___path].DereferenceString()->StringText();

	// Delete folder
	operationResult = f_unlink(workingPath);
	if (operationResult != FR_OK)
	{
		// folder doesn't exist
		NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
	}

	NANOCLR_CLEANUP();
	NANOCLR_CLEANUP_END();
}

HRESULT Library_win_storage_native_Windows_Storage_StorageFile::RenameFileNative___VOID__STRING(CLR_RT_StackFrame& stack)
{
	NANOCLR_HEADER();

	const char* workingPath;
	const char* desiredPath;
	FRESULT     operationResult;

	// get a pointer to the managed object instance and check that it's not NULL
	CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

	// get a pointer to the path in managed field
	workingPath = pThis[Library_win_storage_native_Windows_Storage_StorageFile::FIELD___path].DereferenceString()->StringText();

	// get a pointer to the desired folder path
	desiredPath = stack.Arg1().DereferenceString()->StringText();

	// rename folder
	operationResult = f_rename(workingPath, desiredPath);
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

	NANOCLR_CLEANUP();
	NANOCLR_CLEANUP_END();
}

HRESULT Library_win_storage_native_Windows_Storage_StorageFile::GetFileFromPathNative___STATIC__WindowsStorageStorageFile__STRING__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

	CLR_RT_TypeDef_Index    storageFileTypeDef;
	CLR_RT_HeapBlock*       storageFile;

    const char* filePath;
	const char* fileName;

    FRESULT fr;
    FILINFO fileInfo;
	SYSTEMTIME  fileInfoTime;

    // get a pointer to the file path
    filePath = stack.Arg0().DereferenceString()->StringText();

	// get a pointer to the file name
	fileName = stack.Arg1().DereferenceString()->StringText();

    fr = f_stat(filePath, &fileInfo);
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

    NANOCLR_NOCLEANUP();
}
