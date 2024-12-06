//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NF_SYS_IO_FILESYSTEM_H
#define NF_SYS_IO_FILESYSTEM_H

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <target_platform.h>
#include <nanoCLR_FileStream.h>

/////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH managed System.IO.NativeIO.EmptyAttribute !!! //
/////////////////////////////////////////////////////////////////////////
#define EMPTY_ATTRIBUTE 0xFFFFFFFF
#define ATTRIBUTE_SET_MASK                                                                                             \
    (FileAttributes::FileAttributes_ReadOnly | FileAttributes::FileAttributes_Hidden |                                 \
     FileAttributes::FileAttributes_System | FileAttributes::FileAttributes_Directory |                                \
     FileAttributes::FileAttributes_Archive)

typedef enum __nfpack CardDetectState
{
    CardDetectState_Inserted = 0,
    CardDetectState_Removed = 1,
} CardDetectState;

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

typedef enum __nfpack RemovableDriveEventArgs_RemovableDeviceEvent
{
    RemovableDriveEventArgs_RemovableDeviceEvent_Inserted = 0,
    RemovableDriveEventArgs_RemovableDeviceEvent_Removed = 1,
} RemovableDriveEventArgs_RemovableDeviceEvent;

typedef enum __nfpack StorageEventManager_StorageEventType
{
    StorageEventManager_StorageEventType_Invalid = 0,
    StorageEventManager_StorageEventType_RemovableDeviceInsertion = 1,
    StorageEventManager_StorageEventType_RemovableDeviceRemoval = 2,
    StorageEventManager_StorageEventType_CardDetectChanged = 3,
} StorageEventManager_StorageEventType;

typedef enum __nfpack DriveType
{
    DriveType_Unknown = 0,
    DriveType_NoRootDirectory = 1,
    DriveType_Removable = 2,
    DriveType_Fixed = 3,
    DriveType_Ram = 4,
} DriveType;

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
    FileAttributes_Normal = 128,
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

struct Library_nf_sys_io_filesystem_nanoFramework_System_IO_CardDetectChangedEventArgs
{
    static const int FIELD___cardState = 1;
    static const int FIELD___slotIndex = 2;

    //--//
};

struct Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard
{
    static const int FIELD___mounted = 1;
    static const int FIELD___disposed = 2;
    static const int FIELD___sdCardType = 3;
    static const int FIELD___enableCardDetectPin = 4;
    static const int FIELD___cardDetectedState = 5;
    static const int FIELD___cardDetectPin = 6;
    static const int FIELD___slotIndex = 7;
    static const int FIELD___autoMount = 8;
    static const int FIELD___dataWidth = 9;
    static const int FIELD___spiBus = 10;
    static const int FIELD___chipSelectPin = 11;
    static const int FIELD__CardDetectChanged = 12;

    NANOCLR_NATIVE_DECLARE(InitNative___VOID);
    NANOCLR_NATIVE_DECLARE(NativeDispose___VOID);
    NANOCLR_NATIVE_DECLARE(MountNative___VOID);
    NANOCLR_NATIVE_DECLARE(UnmountNative___VOID);
    NANOCLR_NATIVE_DECLARE(PollCardDetectNative___BOOLEAN);

    //--//
};

struct Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCardMmcParameters
{
    static const int FIELD__slotIndex = 1;
    static const int FIELD__dataWidth = 2;

    //--//
};

struct Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCardSpiParameters
{
    static const int FIELD__slotIndex = 1;
    static const int FIELD__spiBus = 2;
    static const int FIELD__chipSelectPin = 3;

    //--//
};

struct Library_nf_sys_io_filesystem_nanoFramework_System_IO_RemovableDriveEventArgs
{
    static const int FIELD___drive = 1;
    static const int FIELD___event = 2;

    //--//
};

struct Library_nf_sys_io_filesystem_System_IO_DriveInfo
{
    static const int FIELD___driveType = 1;
    static const int FIELD___name = 2;
    static const int FIELD___totalSize = 3;
    static const int FIELD___volumeIndex = 4;

    NANOCLR_NATIVE_DECLARE(Refresh___VOID);
    NANOCLR_NATIVE_DECLARE(DriveInfoNative___VOID__STRING);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__U4);
    NANOCLR_NATIVE_DECLARE(GetFileSystems___STATIC__SZARRAY_STRING);
    NANOCLR_NATIVE_DECLARE(MountRemovableVolumes___STATIC__VOID);
    NANOCLR_NATIVE_DECLARE(GetDrivesNative___STATIC__SZARRAY_SystemIODriveInfo);

    //--//
    static HRESULT UpdateVolumeInfo(CLR_RT_HeapBlock *hbVolume, FileSystemVolume *volume);
};

struct Library_nf_sys_io_filesystem_nanoFramework_System_IO_StorageEventManager
{
    static const int FIELD_STATIC__RemovableDeviceInserted = 0;
    static const int FIELD_STATIC__RemovableDeviceRemoved = 1;
    static const int FIELD_STATIC___drives = 2;
    static const int FIELD_STATIC___sdCardList = 3;

    //--//
};

struct Library_nf_sys_io_filesystem_nanoFramework_System_IO_StorageEventManager__StorageEvent
{
    static const int FIELD__EventType = 3;
    static const int FIELD__VolumeIndex = 4;
    static const int FIELD__Time = 5;
    static const int FIELD__state = 6;

    //--//
};

struct Library_nf_sys_io_filesystem_System_IO_Directory
{
    NANOCLR_NATIVE_DECLARE(NativeGetChildren___STATIC__SZARRAY_STRING__STRING__BOOLEAN);

    //--//

    static void CombinePaths(char *outpath, const char *path1, const char *path2);
};

struct Library_nf_sys_io_filesystem_System_IO_FileSystemManager__FileRecord
{
    static const int FIELD__FullName = 1;
    static const int FIELD__NativeFileStream = 2;
    static const int FIELD__Share = 3;

    //--//
};

struct Library_nf_sys_io_filesystem_System_IO_FileSystemManager
{
    static const int FIELD_STATIC___openFiles = 4;
    static const int FIELD_STATIC___lockedDirs = 5;
    static const int FIELD_STATIC___currentDirectoryRecord = 6;
    static const int FIELD_STATIC__CurrentDirectory = 7;

    //--//
};

struct Library_nf_sys_io_filesystem_System_IO_FileStream
{
    static const int FIELD___canRead = 1;
    static const int FIELD___canWrite = 2;
    static const int FIELD___canSeek = 3;
    static const int FIELD___seekLimit = 4;
    static const int FIELD___disposed = 5;
    static const int FIELD___fileName = 6;
    static const int FIELD___nativeFileStream = 7;
    static const int FIELD___fileRecord = 8;

    //--//
};

struct Library_nf_sys_io_filesystem_System_IO_File
{
    static const int FIELD_STATIC__EmptyBytes = 8;

    //--//
};

struct Library_nf_sys_io_filesystem_System_IO_NativeFileInfo
{
    static const int FIELD__Attributes = 1;
    static const int FIELD__Size = 2;
    static const int FIELD__FileName = 3;

    //--//
};

struct Library_nf_sys_io_filesystem_System_IO_FileSystemInfo
{
    static const int FIELD___nativeFileInfo = 1;
    static const int FIELD___fullPath = 2;

    //--//
};

struct Library_nf_sys_io_filesystem_System_IO_NativeFileStream
{
    static const int FIELD___fs = 1;

    NANOCLR_NATIVE_DECLARE(_ctor___VOID__STRING__I4);
    NANOCLR_NATIVE_DECLARE(Read___I4__SZARRAY_U1__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(Write___I4__SZARRAY_U1__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(Seek___I8__I8__U4);
    NANOCLR_NATIVE_DECLARE(Flush___VOID);
    NANOCLR_NATIVE_DECLARE(GetLength___I8);
    NANOCLR_NATIVE_DECLARE(SetLength___VOID__I8);
    NANOCLR_NATIVE_DECLARE(GetStreamProperties___VOID__BYREF_BOOLEAN__BYREF_BOOLEAN__BYREF_BOOLEAN);
    NANOCLR_NATIVE_DECLARE(Close___VOID);

    //--//
    static HRESULT ReadWriteHelper(CLR_RT_StackFrame &stack, bool isRead);
    static HRESULT GetFileStream(CLR_RT_StackFrame &stack, CLR_RT_FileStream *&fs);
};

struct Library_nf_sys_io_filesystem_System_IO_NativeFindFile
{
    NANOCLR_NATIVE_DECLARE(GetFileInfo___STATIC__SystemIONativeFileInfo__STRING);

    //--//
};

struct Library_nf_sys_io_filesystem_System_IO_NativeIO
{
    NANOCLR_NATIVE_DECLARE(Delete___STATIC__VOID__STRING__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(Move___STATIC__BOOLEAN__STRING__STRING);
    NANOCLR_NATIVE_DECLARE(CreateDirectory___STATIC__VOID__STRING);
    NANOCLR_NATIVE_DECLARE(GetAttributes___STATIC__U4__STRING);
    NANOCLR_NATIVE_DECLARE(SetAttributes___STATIC__VOID__STRING__U4);
    NANOCLR_NATIVE_DECLARE(Format___STATIC__VOID__STRING__STRING__U4);

    //--//
    static HRESULT FindVolume(CLR_RT_HeapBlock &hbNamespaceRef, FileSystemVolume *&volume);
    static HRESULT FindVolume(CLR_RT_HeapBlock &hbPathRef, FileSystemVolume *&volume, char *&relativePath);
};

struct Library_nf_sys_io_filesystem_System_IO_Path
{
    static const int FIELD_STATIC__DirectorySeparatorChar = 9;
    static const int FIELD_STATIC__AltDirectorySeparatorChar = 10;
    static const int FIELD_STATIC__VolumeSeparatorChar = 11;
    static const int FIELD_STATIC__PathSeparator = 12;

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_IO_FileSystem;

#endif // NF_SYS_IO_FILESYSTEM_H
