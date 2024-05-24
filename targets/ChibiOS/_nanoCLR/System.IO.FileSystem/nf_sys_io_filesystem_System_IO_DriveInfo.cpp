//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nf_sys_io_filesystem.h>

#if defined(NF_FEATURE_USE_LITTLEFS) && (NF_FEATURE_USE_LITTLEFS == TRUE)
#include <hal_littlefs.h>
#endif

#define WORKING_DRIVE_IS_INTERNAL_DRIVE                                                                                \
    (memcmp(workingDrive, INTERNAL_DRIVE0_LETTER, sizeof(INTERNAL_DRIVE0_LETTER) - 1) == 0 ||                          \
     memcmp(workingDrive, INTERNAL_DRIVE1_LETTER, sizeof(INTERNAL_DRIVE1_LETTER) - 1) == 0)

HRESULT Library_nf_sys_io_filesystem_System_IO_DriveInfo::DriveInfoNative___VOID__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    char workingDrive[DRIVE_LETTER_LENGTH];

    int32_t driveIndex;

    CLR_RT_TypeDef_Index driveInfoTypeDef;
    CLR_RT_HeapBlock *driveInfo;

    CLR_RT_HeapBlock &top = stack.PushValue();

    const char *workingPath = stack.Arg0().RecoverString();
    // check for null argument
    FAULT_ON_NULL_ARG(workingPath);

    // copy the first 2 letters of the path for the drive
    // path is 'D:\folder\file.txt', so we need 'D:'
    memcpy(workingDrive, workingPath, DRIVE_LETTER_LENGTH);

    // start composing the reply
    // find <DriveInfo> type definition, don't bother checking the result as it exists for sure
    g_CLR_RT_TypeSystem.FindTypeDef("DriveInfo", "System.IO", driveInfoTypeDef);

    // create an instance of <DriveInfo>
    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, driveInfoTypeDef));

    // get a pointer to DriveInfo
    driveInfo = (CLR_RT_HeapBlock *)top.Dereference();

    (void)driveIndex;
    (void)driveInfo;

    if (WORKING_DRIVE_IS_INTERNAL_DRIVE)
    {
#if defined(NF_FEATURE_USE_LITTLEFS) && (NF_FEATURE_USE_LITTLEFS == TRUE)

#endif
    }
    else
    {
        stack.NotImplementedStub();
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_DriveInfo::Format___STATIC__VOID__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    char workingDrive[DRIVE_LETTER_LENGTH];

    int32_t driveIndex;

    const char *workingPath = stack.Arg0().RecoverString();
    // check for null argument
    FAULT_ON_NULL_ARG(workingPath);

    // copy the first 2 letters of the path for the drive
    // path is 'D:\folder\file.txt', so we need 'D:'
    memcpy(workingDrive, workingPath, DRIVE_LETTER_LENGTH);

    (void)driveIndex;
    (void)workingDrive;

    if (WORKING_DRIVE_IS_INTERNAL_DRIVE)
    {

    }
    else
    {
        stack.NotImplementedStub();
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

    NANOCLR_NOCLEANUP();
}
