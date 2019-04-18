//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "win_storage_native.h"

extern char * ConvertToESP32Path(const char * filepath);

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
