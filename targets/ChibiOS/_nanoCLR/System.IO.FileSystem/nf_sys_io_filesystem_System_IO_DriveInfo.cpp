//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nf_sys_io_filesystem.h>

#if defined(NF_FEATURE_USE_LITTLEFS) && (NF_FEATURE_USE_LITTLEFS == TRUE)
#include <hal_littlefs.h>
#endif

extern int32_t GetInternalDriveIndex(char *drive);

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

        lfs_t *fsDrive = NULL;

        // get littlefs drive index...
        driveIndex = GetInternalDriveIndex(workingDrive);
        //... and pointer to the littlefs instance
        fsDrive = hal_lfs_get_fs_from_index(driveIndex);

        // get the drive info from littlefs
        lfs_fsinfo fsinfo;
        lfs_fs_stat(fsDrive, &fsinfo);

        // fill in the drive info namaged fields
        driveInfo[FIELD___driveType].SetInteger(DriveType_Fixed);
        CLR_RT_HeapBlock_String::CreateInstance(driveInfo[FIELD___name], workingDrive);
        driveInfo[FIELD___totalSize].SetInteger((CLR_UINT32)(fsinfo.block_size * fsinfo.block_count));

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
#if defined(NF_FEATURE_USE_LITTLEFS) && (NF_FEATURE_USE_LITTLEFS == TRUE)

        lfs_t *fsDrive = NULL;

        // get littlefs drive index...
        driveIndex = GetInternalDriveIndex(workingDrive);
        //... and pointer to the littlefs instance
        fsDrive = hal_lfs_get_fs_from_index(driveIndex);

        // looks like littlefs is not formated (occuring at 1st boot)
        lfs_format(fsDrive, &lfsConfig[driveIndex]);

        // try mounting again
        lfs_mount(fsDrive, &lfsConfig[driveIndex]);

#endif
    }
    else
    {
        stack.NotImplementedStub();
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_DriveInfo::GetDrivesNative___STATIC__SZARRAY_SystemIODriveInfo(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    char workingDrive[sizeof(DRIVE_PATH_LENGTH)];

    uint32_t driveCount = 0;

    CLR_RT_HeapBlock *volume = NULL;

    CLR_RT_HeapBlock &top = stack.PushValueAndClear();

#if defined(NF_FEATURE_USE_LITTLEFS) && (NF_FEATURE_USE_LITTLEFS == TRUE)
    // // is littlefs file system available and mounted?
    // if (lfsFileSystemReady)
    // {
    //     // get SPIFFS instances count
    //     driveCount += hal_lfs_get_instances_count();
    // }

#endif

    // create an array of files paths <String> for count drives
    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(top, driveCount, g_CLR_RT_WellKnownTypes.m_String));
    // get a pointer to the first object in the array (which is of type <String>)
    volume = (CLR_RT_HeapBlock *)top.DereferenceArray()->GetFirstElement();

    (void)workingDrive;
    
#if defined(NF_FEATURE_USE_LITTLEFS) && (NF_FEATURE_USE_LITTLEFS == TRUE)

    for (uint16_t driveIterator = 0; driveIterator < driveCount; driveIterator++)
    {
        // fill the folder name and path
        switch (driveIterator)
        {
            case 0:
                memcpy(workingDrive, INTERNAL_DRIVE0_PATH, DRIVE_PATH_LENGTH);
                break;

            case 1:
                memcpy(workingDrive, INTERNAL_DRIVE1_PATH, DRIVE_PATH_LENGTH);
                break;

            default:
                HAL_AssertEx();
                NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }

        // set the drive letter in string array
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(*volume, workingDrive));

        // Next element in array
        volume++;
    }

#endif

    NANOCLR_NOCLEANUP();
}
