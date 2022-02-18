//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NF_SYS_IO_FILESYSTEM_H
#define NF_SYS_IO_FILESYSTEM_H

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>
#include <target_platform.h>

typedef enum __nfpack RemovableDeviceEventArgs_RemovableDeviceEvent
{
    RemovableDeviceEventArgs_RemovableDeviceEvent_Inserted = 0,
    RemovableDeviceEventArgs_RemovableDeviceEvent_Removed = 1,
} RemovableDeviceEventArgs_RemovableDeviceEvent;

typedef enum __nfpack SDCard_SDDataWidth
{
    SDCard_SDDataWidth__1_bit = 1,
    SDCard_SDDataWidth__4_bit = 2,
} SDCard_SDDataWidth;

typedef enum __nfpack SDCard_SDInterfaceType
{
    SDCard_SDInterfaceType_System = 0,
    SDCard_SDInterfaceType_Mmc = 1,
    SDCard_SDInterfaceType_Spi = 2,
} SDCard_SDInterfaceType;

typedef enum __nfpack StorageEventManager_StorageEventType
{
    StorageEventManager_StorageEventType_Invalid = 0,
    StorageEventManager_StorageEventType_RemovableDeviceInsertion = 1,
    StorageEventManager_StorageEventType_RemovableDeviceRemoval = 2,
} StorageEventManager_StorageEventType;

typedef enum __nfpack FileAccess
{
    FileAccess_Read = 1,
    FileAccess_Write = 2,
    FileAccess_ReadWrite = 3,
} FileAccess;

typedef enum __nfpack FileAttributes
{
    FileAttributes_ReadOnly = 1,
    FileAttributes_Hidden = 2,
    FileAttributes_System = 4,
    FileAttributes_Directory = 16,
    FileAttributes_Archive = 32,
} FileAttributes;

typedef enum __nfpack FileMode
{
    FileMode_CreateNew = 1,
    FileMode_Create = 2,
    FileMode_Open = 3,
    FileMode_OpenOrCreate = 4,
    FileMode_Truncate = 5,
    FileMode_Append = 6,
} FileMode;

typedef enum __nfpack FileShare
{
    FileShare_None = 0,
    FileShare_Read = 1,
    FileShare_Write = 2,
    FileShare_ReadWrite = 3,
} FileShare;

struct Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_RemovableDeviceEventArgs
{
    static const int FIELD___path = 1;
    static const int FIELD___event = 2;

    //--//
};

struct Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard
{
    static const int FIELD___mounted = 1;
    static const int FIELD___disposed = 2;
    static const int FIELD___sdCardType = 3;
    static const int FIELD___enableCardDetectPin = 4;
    static const int FIELD___cardDetectPin = 5;
    static const int FIELD___dataWidth = 6;
    static const int FIELD___spiBus = 7;
    static const int FIELD___chipSelectPin = 8;

    NANOCLR_NATIVE_DECLARE(InitNative___VOID);
    NANOCLR_NATIVE_DECLARE(NativeDispose___VOID);
    NANOCLR_NATIVE_DECLARE(MountNative___VOID);
    NANOCLR_NATIVE_DECLARE(UnmountNative___VOID);
    NANOCLR_NATIVE_DECLARE(PollCardDetectNative___BOOLEAN);

    //--//
};

struct Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard__SDCardMmcParameters
{
    static const int FIELD__dataWidth = 1;
    static const int FIELD__enableCardDetectPin = 2;
    static const int FIELD__cardDetectPin = 3;

    //--//
};

struct Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard__SDCardSpiParameters
{
    static const int FIELD__spiBus = 1;
    static const int FIELD__chipSelectPin = 2;
    static const int FIELD__enableCardDetectPin = 3;
    static const int FIELD__cardDetectPin = 4;

    //--//
};

struct Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_StorageEventManager
{
    static const int FIELD_STATIC__RemovableDeviceInserted = 0;
    static const int FIELD_STATIC__RemovableDeviceRemoved = 1;

    //--//
};

struct Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_StorageEventManager__StorageEvent
{
    static const int FIELD__EventType = 3;
    static const int FIELD__DriveIndex = 4;
    static const int FIELD__Time = 5;

    //--//
};

struct Library_nf_sys_io_filesystem_System_IO_Directory
{
    NANOCLR_NATIVE_DECLARE(ExistsNative___STATIC__BOOLEAN__STRING);
    NANOCLR_NATIVE_DECLARE(MoveNative___STATIC__VOID__STRING__STRING);
    NANOCLR_NATIVE_DECLARE(DeleteNative___STATIC__VOID__STRING);
    NANOCLR_NATIVE_DECLARE(CreateNative___STATIC__VOID__STRING);
    NANOCLR_NATIVE_DECLARE(GetFilesNative___STATIC__SZARRAY_STRING__STRING);
    NANOCLR_NATIVE_DECLARE(GetDirectoriesNative___STATIC__SZARRAY_STRING__STRING);
    NANOCLR_NATIVE_DECLARE(GetLogicalDrivesNative___STATIC__SZARRAY_STRING);
    NANOCLR_NATIVE_DECLARE(GetLastWriteTimeNative___STATIC__SystemDateTime__STRING);

    //--//
};

struct Library_nf_sys_io_filesystem_System_IO_File
{
    NANOCLR_NATIVE_DECLARE(ExistsNative___STATIC__BOOLEAN__STRING__STRING);
    NANOCLR_NATIVE_DECLARE(MoveNative___STATIC__VOID__STRING__STRING);
    NANOCLR_NATIVE_DECLARE(DeleteNative___STATIC__VOID__STRING);
    NANOCLR_NATIVE_DECLARE(GetAttributesNative___STATIC__U1__STRING);
    NANOCLR_NATIVE_DECLARE(SetAttributesNative___STATIC__VOID__STRING__U1);
    NANOCLR_NATIVE_DECLARE(GetLastWriteTimeNative___STATIC__SystemDateTime__STRING);

    //--//
};

struct Library_nf_sys_io_filesystem_System_IO_FileStream
{
    static const int FIELD___canRead = 2;
    static const int FIELD___canWrite = 3;
    static const int FIELD___canSeek = 4;
    static const int FIELD___seekLimit = 5;
    static const int FIELD___position = 6;
    static const int FIELD___disposed = 7;
    static const int FIELD___name = 8;
    static const int FIELD___path = 9;

    NANOCLR_NATIVE_DECLARE(OpenFileNative___VOID__STRING__STRING__I4);
    NANOCLR_NATIVE_DECLARE(ReadNative___I4__STRING__STRING__I8__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(WriteNative___VOID__STRING__STRING__I8__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(GetLengthNative___I8__STRING__STRING);

    //--//
};

struct Library_nf_sys_io_filesystem_System_IO_Path
{
    static const int FIELD_STATIC__DirectorySeparatorChar = 2;
    static const int FIELD_STATIC__InvalidPathChars = 3;
    static const int FIELD_STATIC__m_illegalCharacters = 4;

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_IO_FileSystem;

#endif //_NF_SYS_IO_FILESYSTEM_H_
