//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nf_sys_io_filesystem.h>
#include <esp_vfs_fat.h>
#include <driver/sdmmc_host.h>
#include <diskio_impl.h>
#include <targetHAL_FileOperation.h>
#include <targetHAL_ConfigStorage.h>

typedef Library_nf_sys_io_filesystem_System_IO_DriveInfo DriveInfo;
static HRESULT EnumerateDrives(CLR_RT_HeapBlock *array, int &count);

extern bool IsInternalFilePath(const char *filePath);
extern "C" sdmmc_card_t *card;

HRESULT Library_nf_sys_io_filesystem_System_IO_DriveInfo::DriveInfoNative___VOID__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    char workingDrive[DRIVE_LETTER_LENGTH];

#if (HAL_USE_SDC == TRUE)
    char *vfsPath = NULL;
    int operationResult;
    struct stat fno;
#endif

    CLR_RT_TypeDef_Index driveInfoTypeDef;
    CLR_RT_HeapBlock *driveInfo;

    CLR_RT_HeapBlock &top = stack.PushValue();

    const char *drivePath = stack.Arg1().RecoverString();
    FAULT_ON_NULL_ARG(drivePath);

    // copy the first 2 letters of the path for the drive
    // path is 'D:\folder\file.txt', so we need 'D:'
    memcpy(workingDrive, drivePath, DRIVE_LETTER_LENGTH);

    // start composing the reply
    // find <DriveInfo> type definition, don't bother checking the result as it exists for sure
    g_CLR_RT_TypeSystem.FindTypeDef("DriveInfo", "System.IO", driveInfoTypeDef);

    // create an instance of <DriveInfo>
    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, driveInfoTypeDef));

    // get a pointer to DriveInfo
    driveInfo = (CLR_RT_HeapBlock *)top.Dereference();

    if (IsInternalFilePath(workingDrive))
    {
        // get the drive info from SPIFFs
        size_t total = 0, used = 0;
        esp_spiffs_info(SPIFFS_PARTITION_LABEL, &total, &used);

        // fill in the drive info namaged fields
        driveInfo[FIELD___driveType].SetInteger(DriveType_Fixed);
        CLR_RT_HeapBlock_String::CreateInstance(driveInfo[FIELD___name], drivePath);
        driveInfo[FIELD___totalSize].SetInteger((CLR_UINT32)total);
    }
    else

#if (HAL_USE_SDC == TRUE)

    {
        // assume "0:"" as we're always mounting 1 FATFS drive

        operationResult = stat(vfsPath, &fno);
        if (operationResult == 0)
        {
            // fill in the drive info namaged fields
            driveInfo[FIELD___driveType].SetInteger(DriveType_Removable);
            CLR_RT_HeapBlock_String::CreateInstance(driveInfo[FIELD___name], drivePath);
            driveInfo[FIELD___totalSize].SetInteger((CLR_UINT32)fno.st_size);
        }
        else
        {
            // failed to get drive info
            NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
        }
    }

#endif // HAL_USE_SDC

    {
        // unsupported drive
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_DriveInfo::Format___STATIC__VOID__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    const char *drivePath = stack.Arg1().RecoverString();
    FAULT_ON_NULL_ARG(drivePath);

#if (HAL_USE_SDC == TRUE)

    // check if SD card is mounted
    if (card != NULL)
    {
        // format it
        const size_t workbuf_size = 4096;

        void *workbuf = NULL;
        BYTE pdrv;
        DWORD plist[] = {100, 0, 0, 0};

        workbuf = ff_memalloc(workbuf_size);
        if (workbuf == NULL)
        {
            return ESP_ERR_NO_MEM;
        }

        ff_diskio_get_drive(&pdrv);

        f_fdisk(pdrv, plist, workbuf);
    }
    else
    {
        // SD card is not mounted
        // return error
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }
#endif // HAL_USE_SDC

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_DriveInfo::GetDrivesNative___STATIC__SZARRAY_SystemIODriveInfo(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int count = 0;

    CLR_RT_HeapBlock &top = stack.PushValueAndClear();

    // 1st pass find number of drives
    NANOCLR_CHECK_HRESULT(EnumerateDrives(NULL, count));

    // create an array of files paths <String> for count drives
    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(top, count, g_CLR_RT_WellKnownTypes.m_String));

    // 2nd pass fill array with drives
    NANOCLR_CHECK_HRESULT(EnumerateDrives(&top, count));

    NANOCLR_NOCLEANUP();
}

// Enumerate drives in system
// if array == null then only count drives
// Return number of drives
static HRESULT EnumerateDrives(CLR_RT_HeapBlock *array, int &count)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *storageFolder = NULL;
        DIR *currentDirectory;
        char outputDrive[] = INDEX0_DRIVE_PATH;
        char workingDrive[] = "/D/";

        if (array)
        {
            // get a pointer to the first object in the array (which is of type <String>)
            storageFolder = (CLR_RT_HeapBlock *)array->DereferenceArray()->GetFirstElement();
        }

        count = 0;
        for (char drive = INDEX0_DRIVE_LETTER[0]; drive <= INTERNAL_DRIVE0_LETTER[0]; drive++)
        {
            workingDrive[1] = drive;
            currentDirectory = opendir(workingDrive);
            if (currentDirectory != NULL)
            {
                count++;
                closedir(currentDirectory);

                if (array)
                {
                    // Add entry to array
                    outputDrive[0] = drive;

                    // set the drive letter in string array
                    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(*storageFolder, outputDrive));

                    // Next element in array
                    storageFolder++;
                }
            }
        }
    }
    NANOCLR_NOCLEANUP();
}
