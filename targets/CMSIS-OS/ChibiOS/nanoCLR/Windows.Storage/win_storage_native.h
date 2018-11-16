//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#ifndef _WIN_STORAGE_NATIVE_H_
#define _WIN_STORAGE_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <hal.h>

struct Library_win_storage_native_Windows_Storage_StorageFile
{
    static const int FIELD___contentType = 1;
    static const int FIELD___fileType = 2;
    static const int FIELD___name = 3;
    static const int FIELD___path = 4;


    //--//

};

struct Library_win_storage_native_Windows_Storage_StorageFolder
{
    static const int FIELD___knownFolderId = 1;
    static const int FIELD___dateCreated = 2;
    static const int FIELD___name = 3;
    static const int FIELD___path = 4;

    NANOCLR_NATIVE_DECLARE(GetRemovableStorageFoldersNative___SZARRAY_WindowsStorageStorageFolder);
    NANOCLR_NATIVE_DECLARE(GetStorageFoldersNative___SZARRAY_WindowsStorageStorageFolder);

    //--//

};

struct Library_win_storage_native_Windows_Storage_StorageProvider
{
    static const int FIELD___displayName = 1;
    static const int FIELD___id = 2;


    //--//

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Storage;

#endif  //_WIN_STORAGE_NATIVE_H_
