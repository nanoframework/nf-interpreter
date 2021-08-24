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
    static const int FIELD___canRead = 1;
    static const int FIELD___canWrite = 2;
    static const int FIELD___canSeek = 3;
    static const int FIELD___seekLimit = 4;
    static const int FIELD___position = 5;
    static const int FIELD___disposed = 6;
    static const int FIELD___name = 7;
    static const int FIELD___path = 8;

    NANOCLR_NATIVE_DECLARE(OpenFileNative___VOID__STRING__STRING__I4);
    NANOCLR_NATIVE_DECLARE(ReadNative___I4__STRING__STRING__I8__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(WriteNative___VOID__STRING__STRING__I8__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(GetLengthNative___I8__STRING__STRING);

    //--//
};

struct Library_nf_sys_io_filesystem_System_IO_MemoryStream
{
    static const int FIELD___buffer = 1;
    static const int FIELD___position = 2;
    static const int FIELD___length = 3;
    static const int FIELD___capacity = 4;
    static const int FIELD___expandable = 5;
    static const int FIELD___isOpen = 6;

    //--//
};

struct Library_nf_sys_io_filesystem_System_IO_Path
{
    static const int FIELD_STATIC__DirectorySeparatorChar = 0;
    static const int FIELD_STATIC__InvalidPathChars = 1;
    static const int FIELD_STATIC__m_illegalCharacters = 2;

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_IO_FileSystem;

#endif //NF_SYS_IO_FILESYSTEM_H