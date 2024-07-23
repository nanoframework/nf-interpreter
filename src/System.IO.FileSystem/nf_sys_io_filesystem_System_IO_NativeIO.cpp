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

    FileSystemVolume *driver = NULL;
    char pathBuffer[FS_MAX_PATH_LENGTH + 1];
    char *path = pathBuffer;
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

    char path1Buffer[FS_MAX_PATH_LENGTH + 1];
    char path2Buffer[FS_MAX_PATH_LENGTH + 1];
    char *path1 = path1Buffer;
    char *path2 = path2Buffer;
    bool moveResult = false;

    FileSystemVolume *driver1 = NULL;
    FileSystemVolume *driver2 = NULL;

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
        moveResult = driver1->Move(path1, path2);
        NANOCLR_CHECK_HRESULT(moveResult);

        stack.SetResult_Boolean(moveResult == S_OK);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeIO::CreateDirectory___STATIC__VOID__STRING(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    char pathBuffer[FS_MAX_PATH_LENGTH + 1];
    char *path = pathBuffer;

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
    char pathBuffer[FS_MAX_PATH_LENGTH + 1];
    char *path = pathBuffer;
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
    char pathBuffer[FS_MAX_PATH_LENGTH + 1];
    char *path = pathBuffer;
    FileSystemVolume *driver;

    NANOCLR_CHECK_HRESULT(FindVolume(stack.Arg0(), driver, path));

    attributes = stack.Arg1().NumericByRef().u4;

    NANOCLR_CHECK_HRESULT(driver->SetAttributes(path, attributes));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeIO::Format___STATIC__VOID__STRING__STRING__U4(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_String *hbFileSystemName;
    const char *fileSystemName;
    uint32_t parameters;
    char pathBuffer[FS_MAX_PATH_LENGTH + 1];
    char *path = pathBuffer;
    FileSystemVolume *driver;
    FILESYSTEM_DRIVER_INTERFACE *originalFS = NULL;
    STREAM_DRIVER_INTERFACE *originalStream = NULL;
    bool needInitialize = FALSE;

    NANOCLR_CHECK_HRESULT(FindVolume(stack.Arg0(), driver, path));
    hbFileSystemName = stack.Arg1().DereferenceString();
    parameters = stack.Arg2().NumericByRef().u4;

    originalFS = driver->m_fsDriver;
    originalStream = driver->m_streamDriver;

    if (!hbFileSystemName)
    {
        needInitialize = TRUE;

        NANOCLR_SET_AND_LEAVE(driver->Format("", parameters));
    }

    fileSystemName = hbFileSystemName->StringText();

    for (uint32_t i = 0; i < g_InstalledFSCount; i++)
    {
        if (hal_strncmp_s(g_AvailableFSInterfaces[i].fsDriver->Name, fileSystemName, FS_NAME_DEFAULT_LENGTH) == 0)
        {
            if (originalFS)
            {
                driver->UninitializeVolume();
            }

            // From this point on, even if we fail, we should try to re-initialize before returning
            needInitialize = TRUE;

            driver->m_fsDriver = g_AvailableFSInterfaces[i].fsDriver;
            driver->m_streamDriver = g_AvailableFSInterfaces[i].streamDriver;

            NANOCLR_SET_AND_LEAVE(driver->Format("", parameters));
        }
    }

    NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);

    NANOCLR_CLEANUP();

    if (needInitialize)
    {
        // format did't succeed, restore the original drivers
        if (FAILED(hr))
        {
            driver->m_fsDriver = originalFS;
            driver->m_streamDriver = originalStream;
        }

        if (!(driver->InitializeVolume()))
        {
            // if we were successful up to this point, fail, otherwise leave the original hresult
            if (SUCCEEDED(hr))
            {
                hr = CLR_E_FILE_IO;
            }
        }
        else
        {
            driver->m_fsDriver->GetVolumeLabel(&driver->m_volumeId, driver->m_label, ARRAYSIZE(driver->m_label));
        }
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeIO::FindVolume(
    CLR_RT_HeapBlock &hbNamespaceRef,
    FileSystemVolume *&volume)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_String *hbName;
    char *rootName;
    uint32_t rootNameLength = -1;

    hbName = hbNamespaceRef.DereferenceString();
    FAULT_ON_NULL(hbName);

    rootName = (char *)hbName->StringText();

    if (rootName[0] == '\\')
    {
        rootName++;
    }

    rootNameLength = hal_strlen_s(rootName);

    // Retrieve appropriate driver that handles this namespace
    if ((volume = FileSystemVolumeList::FindVolume(rootName, rootNameLength)) == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_VOLUME_NOT_FOUND);
    }

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
    char rootNameBuffer[FS_MAX_PATH_LENGTH + 1];
    char *rootName = rootNameBuffer;
    uint32_t rootNameLength = -1;

    memset(rootNameBuffer, 0, FS_MAX_PATH_LENGTH + 1);

    hbName = hbPathRef.DereferenceString();
    FAULT_ON_NULL(hbName);

    fullPath = (char *)hbName->StringText();

    NANOCLR_CHECK_HRESULT(CLR_RT_FileStream::SplitFilePath(fullPath, rootName, rootNameLength, relativePath));

    // Retrieve appropriate driver that handles this root name
    if ((volume = FileSystemVolumeList::FindVolume(rootName, rootNameLength)) == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_DRIVER);
    }

    NANOCLR_NOCLEANUP();
}
