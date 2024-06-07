//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_sys_io_filesystem.h"

// clang-format off

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
    Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard::InitNative___VOID,
    Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard::NativeDispose___VOID,
    Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard::MountNative___VOID,
    Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard::UnmountNative___VOID,
    Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard::PollCardDetectNative___BOOLEAN,
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
    Library_nf_sys_io_filesystem_System_IO_DriveInfo::DriveInfoNative___VOID__STRING,
    Library_nf_sys_io_filesystem_System_IO_DriveInfo::_ctor___VOID__U4,
    NULL,
    Library_nf_sys_io_filesystem_System_IO_DriveInfo::Format___STATIC__VOID__STRING,
    Library_nf_sys_io_filesystem_System_IO_DriveInfo::MountRemovableVolumes___STATIC__VOID,
    Library_nf_sys_io_filesystem_System_IO_DriveInfo::GetDrivesNative___STATIC__SZARRAY_SystemIODriveInfo,
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
    Library_nf_sys_io_filesystem_System_IO_Directory::NativeGetChildren___STATIC__SZARRAY_STRING__STRING__BOOLEAN,
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
    Library_nf_sys_io_filesystem_System_IO_NativeFileStream::_ctor___VOID__STRING__I4,
    Library_nf_sys_io_filesystem_System_IO_NativeFileStream::Read___I4__SZARRAY_U1__I4__I4__I4,
    Library_nf_sys_io_filesystem_System_IO_NativeFileStream::Write___I4__SZARRAY_U1__I4__I4__I4,
    Library_nf_sys_io_filesystem_System_IO_NativeFileStream::Seek___I8__I8__U4,
    Library_nf_sys_io_filesystem_System_IO_NativeFileStream::Flush___VOID,
    Library_nf_sys_io_filesystem_System_IO_NativeFileStream::GetLength___I8,
    Library_nf_sys_io_filesystem_System_IO_NativeFileStream::SetLength___VOID__I8,
    Library_nf_sys_io_filesystem_System_IO_NativeFileStream::GetStreamProperties___VOID__BYREF_BOOLEAN__BYREF_BOOLEAN__BYREF_BOOLEAN,
    Library_nf_sys_io_filesystem_System_IO_NativeFileStream::Close___VOID,
    NULL,
    Library_nf_sys_io_filesystem_System_IO_NativeFindFile::GetFileInfo___STATIC__SystemIONativeFileInfo__STRING,
    Library_nf_sys_io_filesystem_System_IO_NativeIO::Delete___STATIC__VOID__STRING__BOOLEAN,
    Library_nf_sys_io_filesystem_System_IO_NativeIO::Move___STATIC__BOOLEAN__STRING__STRING,
    Library_nf_sys_io_filesystem_System_IO_NativeIO::CreateDirectory___STATIC__VOID__STRING,
    Library_nf_sys_io_filesystem_System_IO_NativeIO::GetAttributes___STATIC__U4__STRING,
    Library_nf_sys_io_filesystem_System_IO_NativeIO::SetAttributes___STATIC__VOID__STRING__U4,
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
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_IO_FileSystem =
{
    "System.IO.FileSystem",
    0xBFE26FCD,
    method_lookup,
    { 1, 1, 0, 3 }
};

// clang-format on
