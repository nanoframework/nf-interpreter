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
    UnicodeString pathW;

    NANOCLR_CHECK_HRESULT(FindVolume(stack.Arg0(), driver, pathW));

    NANOCLR_CHECK_HRESULT(driver->Delete(pathW));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeIO::Move___STATIC__BOOLEAN__STRING__STRING(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeIO::CreateDirectory___STATIC__VOID__STRING(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeIO::GetAttributes___STATIC__U4__STRING(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeIO::SetAttributes___STATIC__VOID__STRING__U4(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeIO::FindVolume(
    CLR_RT_HeapBlock &hbPathRef,
    FileSystemVolume *&volume,
    UnicodeString &relativePathW)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_String *hbName;
    const char *fullPath;
    const char *nameSpace;
    const char *relativePath;
    uint32_t nameSpaceLength;

    hbName = hbPathRef.DereferenceString();
    FAULT_ON_NULL(hbName);

    fullPath = hbName->StringText();

    NANOCLR_CHECK_HRESULT(CLR_RT_FileStream::SplitFilePath(fullPath, &nameSpace, &nameSpaceLength, &relativePath));

    /// Retrieve appropriate driver that handles this namespace.
    if ((volume = FileSystemVolumeList::FindVolume(nameSpace, nameSpaceLength)) == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_DRIVER);
    }

    NANOCLR_CHECK_HRESULT(relativePathW.Assign(relativePath));

    NANOCLR_NOCLEANUP();
}
