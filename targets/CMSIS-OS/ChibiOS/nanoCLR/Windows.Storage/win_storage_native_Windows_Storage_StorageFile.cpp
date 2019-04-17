//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "win_storage_native_native.h"


HRESULT Library_win_storage_native_Windows_Storage_StorageFile::CheckFileNative___STATIC__VOID__STRING(CLR_RT_StackFrame& stack)
{
	NANOCLR_HEADER();

	const char* filePath;

	// get a pointer to the file path
	filePath = stack.Arg1().DereferenceString()->StringText();

	// TODO
	NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());


	NANOCLR_NOCLEANUP();
}
