//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "win_storage_native.h"


static const CLR_RT_MethodHandler method_lookup[] =
{
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_win_storage_native_Windows_Storage_StorageFolder::GetRemovableStorageFoldersNative___SZARRAY_WindowsStorageStorageFolder,
    Library_win_storage_native_Windows_Storage_StorageFolder::GetStorageFoldersNative___SZARRAY_WindowsStorageStorageFolder,
    Library_win_storage_native_Windows_Storage_StorageFolder::GetStorageFilesNative___SZARRAY_WindowsStorageStorageFile__U4__U4,
    NULL,
    NULL,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Storage =
{
    "Windows.Storage", 
    0xC266E15B,
    method_lookup,
    { 1, 0, 0, 18 }
};

