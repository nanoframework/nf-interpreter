//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//


#ifndef _WIN_STORAGE_NATIVE_H_
#define _WIN_STORAGE_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>

struct Library_win_storage_native_Windows_Storage_Devices_SDCard
{
    static const int FIELD_STATIC___mounted = 0;

    NANOCLR_NATIVE_DECLARE(MountMMCNative___STATIC__VOID__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(MountSpiNative___STATIC__VOID__I4__I4);
    NANOCLR_NATIVE_DECLARE(UnmountNative___STATIC__VOID);

    //--//

};

struct Library_win_storage_native_Windows_Storage_FileIO
{
    NANOCLR_NATIVE_DECLARE(WriteBytes___STATIC__VOID__WindowsStorageIStorageFile__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(WriteText___STATIC__VOID__WindowsStorageIStorageFile__STRING);
    NANOCLR_NATIVE_DECLARE(ReadBufferNative___STATIC__VOID__WindowsStorageIStorageFile__BYREF_SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(ReadTextNative___STATIC__VOID__WindowsStorageIStorageFile__BYREF_STRING);

    //--//

};

struct Library_win_storage_native_Windows_Storage_RemovableDeviceEventArgs
{
    static const int FIELD___path = 1;
    static const int FIELD___event = 2;

    //--//

};

struct Library_win_storage_native_Windows_Storage_StorageEventManager
{
    static const int FIELD_STATIC__RemovableDeviceInserted = 1;
    static const int FIELD_STATIC__RemovableDeviceRemoved = 2;

    //--//

};

struct Library_win_storage_native_Windows_Storage_StorageEventManager__StorageEvent
{
    static const int FIELD__EventType = 1;
    static const int FIELD__DriveIndex = 2;
    static const int FIELD__Time = 3;

    //--//

};

struct Library_win_storage_native_Windows_Storage_StorageFile
{
    static const int FIELD___dateCreated = 1;
    static const int FIELD___name = 2;
    static const int FIELD___path = 3;

    NANOCLR_NATIVE_DECLARE(DeleteFileNative___VOID);
    NANOCLR_NATIVE_DECLARE(RenameFileNative___VOID__STRING);
    NANOCLR_NATIVE_DECLARE(GetFileFromPathNative___STATIC__WindowsStorageStorageFile__STRING__STRING);

    //--//

};

struct Library_win_storage_native_Windows_Storage_StorageFolder
{
    static const int FIELD___knownFolderId = 1;
    static const int FIELD___dateCreated = 2;
    static const int FIELD___name = 3;
    static const int FIELD___path = 4;

    NANOCLR_NATIVE_DECLARE(GetRemovableStorageFoldersNative___SZARRAY_WindowsStorageStorageFolder);
    NANOCLR_NATIVE_DECLARE(GetInternalStorageFoldersNative___SZARRAY_WindowsStorageStorageFolder);
    NANOCLR_NATIVE_DECLARE(GetStorageFoldersNative___SZARRAY_WindowsStorageStorageFolder);
    NANOCLR_NATIVE_DECLARE(GetStorageFilesNative___SZARRAY_WindowsStorageStorageFile__U4__U4);
    NANOCLR_NATIVE_DECLARE(CreateFileNative___WindowsStorageStorageFile__STRING__U4);
    NANOCLR_NATIVE_DECLARE(CreateFolderNative___WindowsStorageStorageFolder__STRING__U4);
    NANOCLR_NATIVE_DECLARE(DeleteFolderNative___VOID);
    NANOCLR_NATIVE_DECLARE(RenameFolderNative___VOID__STRING);
    NANOCLR_NATIVE_DECLARE(GetFolderNative___WindowsStorageStorageFolder__STRING);

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
