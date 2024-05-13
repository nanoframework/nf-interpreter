//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_sys_io_filesystem.h"

typedef Library_nf_sys_io_filesystem_System_IO_NativeFileInfo NativeFileInfo;

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeFindFile::_ctor___VOID__STRING__STRING(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_String *hbPath;
    CLR_RT_HeapBlock_String *hbPattern;

    CLR_RT_HeapBlock *pThis = stack.This();
    CLR_RT_HeapBlock *pArgs = &(stack.Arg1());

    hbPath = pArgs[0].DereferenceString();
    FAULT_ON_NULL(hbPath);

    hbPattern = pArgs[1].DereferenceString();
    FAULT_ON_NULL(hbPattern);

    NANOCLR_CHECK_HRESULT(
        CLR_RT_FindFile::CreateInstance(pThis[FIELD___ff], hbPath->StringText(), hbPattern->StringText()));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeFindFile::GetNext___SystemIONativeFileInfo(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *managedFindFile;
    CLR_RT_TypeDef_Index nativeFindFileTypeDef;

    CLR_RT_FindFile *ff;
    FS_FILEINFO *fi;
    bool found;

    CLR_RT_HeapBlock &top = stack.PushValueAndClear();

    NANOCLR_CHECK_HRESULT(GetFindFile(stack, ff));

    NANOCLR_CHECK_HRESULT(ff->GetNext(&fi, &found));

    if (found)
    {
        // find <NativeFindFile> type definition, don't bother checking the result as it exists for sure
        g_CLR_RT_TypeSystem.FindTypeDef("NativeFindFile", "System.IO", nativeFindFileTypeDef);

        // create an instance of <NativeFindFile>
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, nativeFindFileTypeDef));

        managedFindFile = top.Dereference();

        managedFindFile[NativeFileInfo::FIELD__Attributes].SetInteger((CLR_INT32)fi->Attributes);
        managedFindFile[NativeFileInfo::FIELD__Size].SetInteger((CLR_INT64)fi->Size);

        NANOCLR_CHECK_HRESULT(ff->CreateFilenameString(managedFindFile[NativeFileInfo::FIELD__FileName]));
    }
    else
    {
        top.SetObjectReference(NULL);
    }

    NANOCLR_CLEANUP();

    if (fi->FileName != NULL)
    {
        platform_free(fi->FileName);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeFindFile::Close___VOID(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    CLR_RT_FindFile *ff;

    NANOCLR_CHECK_HRESULT(GetFindFile(stack, ff));

    NANOCLR_CHECK_HRESULT(ff->Close());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeFindFile::GetFileInfo___STATIC__SystemIONativeFileInfo__STRING(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *managedFindFile;
    CLR_RT_TypeDef_Index nativeFindFileTypeDef;

    FS_FILEINFO fileInfo;
    bool found;
    char *path;
    FileSystemVolume *driver;

    CLR_RT_HeapBlock &top = stack.PushValueAndClear();

    NANOCLR_CHECK_HRESULT(Library_nf_sys_io_filesystem_System_IO_NativeIO::FindVolume(stack.Arg0(), driver, path));

    NANOCLR_CHECK_HRESULT(driver->GetFileInfo(path, &fileInfo, &found));

    if (found)
    {
        // find <NativeFindFile> type definition, don't bother checking the result as it exists for sure
        g_CLR_RT_TypeSystem.FindTypeDef("NativeFindFile", "System.IO", nativeFindFileTypeDef);

        // create an instance of <NativeFindFile>
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, nativeFindFileTypeDef));

        managedFindFile = top.Dereference();

        managedFindFile[NativeFileInfo::FIELD__Attributes].SetInteger((CLR_UINT32)fileInfo.Attributes);
        managedFindFile[NativeFileInfo::FIELD__Size].SetInteger((CLR_INT64)fileInfo.Size);
        managedFindFile[NativeFileInfo::FIELD__FileName].SetObjectReference(NULL);
    }
    else
    {
        top.SetObjectReference(NULL);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeFindFile::GetFindFile(
    CLR_RT_StackFrame &stack,
    CLR_RT_FindFile *&ff)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_BinaryBlob *blob;
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    blob = pThis[FIELD___ff].DereferenceBinaryBlob();

    if (!blob || blob->DataType() != DATATYPE_BINARY_BLOB_HEAD)
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);

    ff = (CLR_RT_FindFile *)blob->GetData();

    NANOCLR_NOCLEANUP();
}
