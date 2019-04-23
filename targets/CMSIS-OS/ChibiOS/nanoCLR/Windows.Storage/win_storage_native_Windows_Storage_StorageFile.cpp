//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ff.h>
#include "win_storage_native.h"

// defining these types here to make it shorter and improve code readability
typedef Library_win_storage_native_Windows_Storage_StorageFile StorageFile;

HRESULT Library_win_storage_native_Windows_Storage_StorageFile::CheckFileNative___STATIC__VOID__STRING(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER();

    const char* filePath;
    FRESULT fr;
    FILINFO fileInfo;

    // get a pointer to the file path
    filePath = stack.Arg1().DereferenceString()->StringText();

    fr = f_stat(filePath, &fileInfo);

    if(fr == FR_OK)
    {
        // file exists, we are good
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


HRESULT Library_win_storage_native_Windows_Storage_StorageFile::DeleteFileNative___VOID(CLR_RT_StackFrame& stack)
{
	NANOCLR_HEADER();

	const char* workingPath;
	FRESULT     operationResult;

	// get a pointer to the managed object instance and check that it's not NULL
	CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

	// get a pointer to the path in managed field
	workingPath = pThis[StorageFile::FIELD___path].DereferenceString()->StringText();

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
	workingPath = pThis[StorageFile::FIELD___path].DereferenceString()->StringText();

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

