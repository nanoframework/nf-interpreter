//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_sys_io_filesystem.h"

typedef Library_nf_sys_io_filesystem_System_IO_NativeIO NativeIO;

HRESULT Library_nf_sys_io_filesystem_System_IO_DriveInfo::Refresh___VOID(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pThis = stack.This();
    FileSystemVolume *volume;

    NANOCLR_CHECK_HRESULT(NativeIO::FindVolume(pThis[FIELD___name], volume));

    NANOCLR_CHECK_HRESULT(UpdateVolumeInfo(pThis, volume));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_DriveInfo::DriveInfoNative___VOID__STRING(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    FileSystemVolume *volume;

    NANOCLR_CHECK_HRESULT(NativeIO::FindVolume(stack.Arg1(), volume));

    NANOCLR_CHECK_HRESULT(UpdateVolumeInfo(stack.This(), volume));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_DriveInfo::_ctor___VOID__U4(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    FileSystemVolume *volume;

    uint32_t driveIndex = stack.Arg1().NumericByRef().u4;

    volume = FileSystemVolumeList::FindVolume(driveIndex);

    if (volume != NULL)
    {
        NANOCLR_SET_AND_LEAVE(UpdateVolumeInfo(stack.This(), volume));
    }

    NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_DriveInfo::GetFileSystems___STATIC__SZARRAY_STRING(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock &ret = stack.PushValueAndClear();
    CLR_RT_HeapBlock *fsNames;

    NANOCLR_CHECK_HRESULT(
        CLR_RT_HeapBlock_Array::CreateInstance(ret, g_InstalledFSCount, g_CLR_RT_WellKnownTypes.m_String));

    fsNames = (CLR_RT_HeapBlock *)ret.DereferenceArray()->GetFirstElement();

    for (uint32_t i = 0; i < g_InstalledFSCount; i++)
    {
        NANOCLR_CHECK_HRESULT(
            CLR_RT_HeapBlock_String::CreateInstance(fsNames[i], g_AvailableFSInterfaces[i].fsDriver->Name));
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_DriveInfo::MountRemovableVolumes___STATIC__VOID(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    (void)stack;

    FS_MountRemovableVolumes();

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_DriveInfo::GetDrivesNative___STATIC__SZARRAY_SystemIODriveInfo(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Index driveInfoTypeDef;

    uint32_t volumeCount = FileSystemVolumeList::GetNumVolumes();
    FileSystemVolume *currentVolume;

    // CLR_RT_HeapBlock *volume = NULL;
    CLR_RT_HeapBlock *hbVolumes;

    CLR_RT_HeapBlock &top = stack.PushValueAndClear();

    // find <DriveInfo> type definition, don't bother checking the result as it exists for sure
    g_CLR_RT_TypeSystem.FindTypeDef("DriveInfo", "System.IO", driveInfoTypeDef);

    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(top, volumeCount, driveInfoTypeDef));

    hbVolumes = (CLR_RT_HeapBlock *)top.DereferenceArray()->GetFirstElement();

    currentVolume = FileSystemVolumeList::GetFirstVolume();

    for (uint8_t i = 0; i < volumeCount; i++)
    {
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(hbVolumes[i], driveInfoTypeDef));

        NANOCLR_CHECK_HRESULT(UpdateVolumeInfo(hbVolumes[i].Dereference(), currentVolume));

        currentVolume = FileSystemVolumeList::GetNextVolume(*currentVolume);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_DriveInfo::UpdateVolumeInfo(
    CLR_RT_HeapBlock *hbVolume,
    FileSystemVolume *volume)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();

    int64_t totalSize;
    int64_t totalFreeSpace;

    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(hbVolume[FIELD___name], volume->m_rootName));

    // map the drive type to the managed enum
    if (memcmp(volume->m_rootName, INTERNAL_DRIVE0_LETTER, sizeof(INTERNAL_DRIVE0_LETTER) - 1) == 0 ||
        memcmp(volume->m_rootName, INTERNAL_DRIVE1_LETTER, sizeof(INTERNAL_DRIVE1_LETTER) - 1) == 0)
    {
        hbVolume[FIELD___driveType].SetInteger(DriveType_Fixed);
    }
    else
    {
        // all other drives are considered removable
        hbVolume[FIELD___driveType].SetInteger(DriveType_Removable);
    }

    if (FAILED(volume->GetSizeInfo(&totalSize, &totalFreeSpace)))
    {
        totalSize = 0;
        totalFreeSpace = 0;
    }

    hbVolume[FIELD___totalSize].SetInteger(totalSize);

    hbVolume[FIELD___volumeIndex].SetInteger((CLR_UINT32)volume->m_volumeId.volumeId);

    NANOCLR_NOCLEANUP();
}
