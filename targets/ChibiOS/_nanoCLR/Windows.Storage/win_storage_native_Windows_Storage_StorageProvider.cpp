//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <win_storage_native_target.h>

#if (NF_FEATURE_USE_SPIFFS == TRUE)
#include <hal_spiffs.h>
#endif

HRESULT Library_win_storage_native_Windows_Storage_StorageProvider::FormatVolume___VOID__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    char workingDrive[DRIVE_LETTER_LENGTH];
    const char *driveName;

#if (NF_FEATURE_USE_SPIFFS == TRUE)
    int32_t driveIndex;
#endif

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get a pointer to the desired folder path
    driveName = stack.Arg1().DereferenceString()->StringText();

    // copy the first 2 letters of the path for the drive
    // path is 'D:\folder\file.txt', so we need 'D:'
    memcpy(workingDrive, driveName, DRIVE_LETTER_LENGTH);

    if (WORKING_DRIVE_IS_INTERNAL_DRIVE)
    {
#if (NF_FEATURE_USE_SPIFFS == TRUE)
        // SPIFFS drive workflow
        driveIndex = GetInternalDriveIndex(workingDrive);

        if (hal_spiffs_format(driveIndex) != SPIFFS_OK)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO)
        }

#endif
    }
    else
    {
        // unsupported drive
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    NANOCLR_NOCLEANUP();
}
