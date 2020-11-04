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
    Library_nf_sys_io_filesystem_System_IO_File::ExistsNative___STATIC__BOOLEAN__STRING__STRING,
    Library_nf_sys_io_filesystem_System_IO_File::MoveNative___STATIC__VOID__STRING__STRING,
    Library_nf_sys_io_filesystem_System_IO_File::DeleteNative___STATIC__VOID__STRING,
    Library_nf_sys_io_filesystem_System_IO_File::GetAttributesNative___STATIC__U1__STRING,
    Library_nf_sys_io_filesystem_System_IO_File::SetAttributesNative___STATIC__VOID__STRING__U1,
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
    Library_nf_sys_io_filesystem_System_IO_FileStream::OpenFileNative___VOID__STRING__STRING__I4,
    Library_nf_sys_io_filesystem_System_IO_FileStream::ReadNative___I4__STRING__STRING__I8__SZARRAY_U1__I4,
    Library_nf_sys_io_filesystem_System_IO_FileStream::WriteNative___VOID__STRING__STRING__I8__SZARRAY_U1__I4,
    Library_nf_sys_io_filesystem_System_IO_FileStream::GetLengthNative___I8__STRING__STRING,
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
    0x54569E09,
    method_lookup,
    { 1, 0, 0, 0 }
};

// clang-format on
