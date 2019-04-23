//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "win_storage_native.h"

extern char * ConvertToESP32Path(const char * filepath);

// defining these types here to make it shorter and improve code readability
typedef Library_win_storage_native_Windows_Storage_StorageFile StorageFile;


HRESULT Library_win_storage_native_Windows_Storage_StorageFile::CheckFileNative___STATIC__VOID__STRING(CLR_RT_StackFrame& stack)
{
	NANOCLR_HEADER();

	const char* managedfilePath;
	char *      filePath = NULL;
	FILE *		file;

	// get a pointer to the file path
	managedfilePath = stack.Arg0().DereferenceString()->StringText();

	filePath = ConvertToESP32Path(managedfilePath);

	// Check file exists 
	file = fopen(filePath, "r");

	if (file == NULL)
	{
		NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
	}
	fclose(file);

	NANOCLR_CLEANUP();

	// free buffer memory, if allocated
	if (filePath != NULL)
	{
		platform_free(filePath);
	}

	NANOCLR_CLEANUP_END();
}

HRESULT Library_win_storage_native_Windows_Storage_StorageFile::DeleteFileNative___VOID(CLR_RT_StackFrame& stack)
{
	NANOCLR_HEADER();

	char*       workingPath = NULL;
	const char* managedPath;
	int         operationResult;

	// get a pointer to the managed object instance and check that it's not NULL
	CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

	// get a pointer to the path in managed field
	managedPath = pThis[StorageFile::FIELD___path].DereferenceString()->StringText();

	// Convert to ESP32 form path ( linux like )
	// return allocated converted path, must be freed
	workingPath = ConvertToESP32Path(managedPath);

	// Delete folder, 0=succesfull
	operationResult = unlink(workingPath);
	if (operationResult < 0) operationResult = errno;
	if (operationResult != 0)
	{
		// folder doesn't exist
		NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
	}

	NANOCLR_CLEANUP();

	// free buffer memory, if allocated
	if (workingPath != NULL)
	{
		platform_free(workingPath);
	}

	NANOCLR_CLEANUP_END();

}

HRESULT Library_win_storage_native_Windows_Storage_StorageFile::RenameFileNative___VOID__STRING(CLR_RT_StackFrame& stack)
{
	NANOCLR_HEADER();

	char*       workingPath = NULL;
	char*       desiredWorkingPath = NULL;
	const char* managedPath;
	const char* desiredPath;
	int         operationResult;

	// get a pointer to the managed object instance and check that it's not NULL
	CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

	// get a pointer to the path in managed field
	managedPath = pThis[StorageFile::FIELD___path].DereferenceString()->StringText();

	// Convert to ESP32 form path ( linux like )
	// return allocated converted path, must be freed
	workingPath = ConvertToESP32Path(managedPath);

	// get a pointer to the desired folder path
	desiredPath = stack.Arg1().DereferenceString()->StringText();
	desiredWorkingPath = ConvertToESP32Path(desiredPath);

	// rename folder, 0=succesfull
	operationResult = rename(workingPath, desiredWorkingPath);
	if (operationResult < 0) operationResult = errno;
	if (operationResult == ENOENT)
	{
		// Invalid path
		NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
	}
	else if (operationResult != 0)
	{
		// folder doesn't exist
		NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_FOUND);
	}

	NANOCLR_CLEANUP();

	// free buffer memory, if allocated
	if (workingPath != NULL)
	{
		platform_free(workingPath);
	}
	if (desiredWorkingPath != NULL)
	{
		platform_free(desiredWorkingPath);
	}

	NANOCLR_CLEANUP_END();
}

