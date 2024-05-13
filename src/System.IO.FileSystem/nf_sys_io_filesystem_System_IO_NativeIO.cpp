//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_sys_io_filesystem.h"

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeIO::Delete___STATIC__VOID__STRING__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    FileSystemVolume *driver;
    char *path;
    bool recursive = stack.Arg1().NumericByRef().u1 != 0;

    NANOCLR_CHECK_HRESULT(FindVolume(stack.Arg0(), driver, path));

    NANOCLR_CHECK_HRESULT(driver->Delete(path, recursive));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeIO::Move___STATIC__BOOLEAN__STRING__STRING(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    char *path1;
    char *path2;
    FileSystemVolume *driver1;
    FileSystemVolume *driver2;

    NANOCLR_CHECK_HRESULT(FindVolume(stack.Arg0(), driver1, path1));
    NANOCLR_CHECK_HRESULT(FindVolume(stack.Arg1(), driver2, path2));

    // check if the two volumes are the same
    if (driver1 != driver2)
    {
        // need cross-volume move, so return false and let managed code deal with this
        stack.SetResult_Boolean(false);
    }
    else
    {
        NANOCLR_CHECK_HRESULT(driver1->Move(path1, path2));

        stack.SetResult_Boolean(true);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeIO::CreateDirectory___STATIC__VOID__STRING(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    char *path;
    FileSystemVolume *driver;

    NANOCLR_CHECK_HRESULT(FindVolume(stack.Arg0(), driver, path));

    if (hal_strlen_s(path) >= FS_MAX_DIRECTORY_LENGTH)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_PATH_TOO_LONG);
    }

    NANOCLR_CHECK_HRESULT(driver->CreateDirectory(path));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeIO::GetAttributes___STATIC__U4__STRING(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    uint32_t attributes;
    char *path;
    FileSystemVolume *driver;

    NANOCLR_CHECK_HRESULT(FindVolume(stack.Arg0(), driver, path));

    NANOCLR_CHECK_HRESULT(driver->GetAttributes(path, &attributes));

    stack.SetResult_U4(attributes);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeIO::SetAttributes___STATIC__VOID__STRING__U4(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    uint32_t attributes;
    char *path;
    FileSystemVolume *driver;

    NANOCLR_CHECK_HRESULT(FindVolume(stack.Arg0(), driver, path));

    attributes = stack.Arg1().NumericByRef().u4;

    NANOCLR_CHECK_HRESULT(driver->SetAttributes(path, attributes));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeIO::FindVolume(
    CLR_RT_HeapBlock &hbPathRef,
    FileSystemVolume *&volume,
    char *&relativePath)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_String *hbName;
    char *fullPath;
    char *rootName;
    uint32_t rootNameLength = 0;
    uint32_t *rootNameLengthP = &rootNameLength;

    hbName = hbPathRef.DereferenceString();
    FAULT_ON_NULL(hbName);

    fullPath = (char *)hbName->StringText();

    NANOCLR_CHECK_HRESULT(CLR_RT_FileStream::SplitFilePath(fullPath, rootName, rootNameLengthP, relativePath));

    // Retrieve appropriate driver that handles this root name
    if ((volume = FileSystemVolumeList::FindVolume(rootName, rootNameLength)) == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_DRIVER);
    }

    NANOCLR_NOCLEANUP();
}
