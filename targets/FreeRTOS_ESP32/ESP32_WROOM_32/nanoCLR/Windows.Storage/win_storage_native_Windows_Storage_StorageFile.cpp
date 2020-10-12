//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "win_storage_native_target.h"

extern char * ConvertToESP32Path(const char * filepath);
extern uint64_t GetFileTimeFromPath(char * path);

HRESULT Library_win_storage_native_Windows_Storage_StorageFile::DeleteFileNative___VOID(CLR_RT_StackFrame& stack)
{
	NANOCLR_HEADER();

	char*       workingPath = NULL;
	const char* managedPath;
	int         operationResult;

	// get a pointer to the managed object instance and check that it's not NULL
	CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

	// get a pointer to the path in managed field
	managedPath = pThis[Library_win_storage_native_Windows_Storage_StorageFile::FIELD___path].DereferenceString()->StringText();

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
	managedPath = pThis[Library_win_storage_native_Windows_Storage_StorageFile::FIELD___path].DereferenceString()->StringText();

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
		// Invalid path/file not found
		NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
	}
	else if (operationResult == EEXIST)
	{
		// file already exists
		NANOCLR_SET_AND_LEAVE(CLR_E_PATH_ALREADY_EXISTS);
	}
	else if (operationResult != 0)
	{
		// Some other error
		NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
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

HRESULT Library_win_storage_native_Windows_Storage_StorageFile::GetFileFromPathNative___STATIC__WindowsStorageStorageFile__STRING__STRING( CLR_RT_StackFrame& stack )
{
	NANOCLR_HEADER();

	CLR_RT_TypeDef_Index    storageFileTypeDef;
	CLR_RT_HeapBlock*       storageFile;

	const char* fileName;
	const char* managedfilePath;
	char *      workingPath = NULL;

	static struct stat fileInfo;

	int         operationResult;

	// get a pointer to the file path
	managedfilePath = stack.Arg0().DereferenceString()->StringText();

	// get a pointer to the file name
	fileName = stack.Arg1().DereferenceString()->StringText();

	workingPath = ConvertToESP32Path(managedfilePath);

	// check if path exists
	operationResult = stat(workingPath, &fileInfo);
	if(operationResult < 0)
	{
		operationResult = errno;
	}

	if( operationResult == ENOENT )
	{
		// path doesn't exist
		NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
	}


	if( operationResult == 0 )
	{
		// Is this a file
		if (fileInfo.st_mode & S_IFREG)
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

            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(storageFile[Library_win_storage_native_Windows_Storage_StorageFile::FIELD___path], managedfilePath));

			// get the date time details and fill in the managed field
			// get a reference to the dateCreated managed field...
			CLR_RT_HeapBlock& dateFieldRef = storageFile[Library_win_storage_native_Windows_Storage_StorageFile::FIELD___dateCreated];
			CLR_INT64* pRes = (CLR_INT64*)&dateFieldRef.NumericByRef().s8;

			// get the date time details and fill in the managed field
			// compute file date
			// ...and set it 
			*pRes = GetFileTimeFromPath(workingPath);
		}
		else
		{
			 // path exists but it's a folder
			NANOCLR_SET_AND_LEAVE(CLR_E_FILE_NOT_FOUND);
		}
	}


	NANOCLR_CLEANUP();

	// free buffer memory, if allocated
	if (workingPath != NULL)
	{
		platform_free(workingPath);
	}

	NANOCLR_CLEANUP_END();
}
