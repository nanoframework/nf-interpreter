//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ff.h>
#include "win_storage_native.h"

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
