//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "win_storage_native.h"


static const CLR_RT_MethodHandler method_lookup[] =
{
    NULL,
    Library_win_storage_native_Windows_Storage_FileIO::ReadText___STATIC__STRING__WindowsStorageIStorageFile,
    Library_win_storage_native_Windows_Storage_FileIO::WriteText___STATIC__VOID__WindowsStorageIStorageFile__STRING,
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
    Library_win_storage_native_Windows_Storage_StorageFolder::CreateFileNative___WindowsStorageStorageFile__STRING__U4,
    Library_win_storage_native_Windows_Storage_StorageFolder::CreateFolderNative___WindowsStorageStorageFolder__STRING__U4,
    NULL,
    NULL,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Storage =
{
    "Windows.Storage", 
    0x7EF84375,
    method_lookup,
    { 1, 0, 0, 0 }
};
