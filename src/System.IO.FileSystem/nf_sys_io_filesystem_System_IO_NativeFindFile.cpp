//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_sys_io_filesystem.h"

typedef Library_nf_sys_io_filesystem_System_IO_NativeFileInfo NativeFileInfo;

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeFindFile::GetFileInfo___STATIC__SystemIONativeFileInfo__STRING(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *managedFindFile;
    CLR_RT_TypeDef_Index nativeFindFileTypeDef;

    FS_FILEINFO fileInfo;
    bool found;
    char *path = NULL;
    FileSystemVolume *driver = NULL;

    CLR_RT_HeapBlock &top = stack.PushValueAndClear();

    NANOCLR_CHECK_HRESULT(Library_nf_sys_io_filesystem_System_IO_NativeIO::FindVolume(stack.Arg0(), driver, path));

    // zero initialize the fileInfo
    memset(&fileInfo, 0, sizeof(FS_FILEINFO));

    NANOCLR_CHECK_HRESULT(driver->GetFileInfo(path, &fileInfo, &found));

    if (found)
    {
        // find <NativeFindFile> type definition, don't bother checking the result as it exists for sure
        g_CLR_RT_TypeSystem.FindTypeDef("NativeFileInfo", "System.IO", nativeFindFileTypeDef);

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
