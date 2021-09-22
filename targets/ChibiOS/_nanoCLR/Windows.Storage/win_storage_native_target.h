//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef WIN_STORAGE_NATIVE_TARGET_H
#define WIN_STORAGE_NATIVE_TARGET_H

#include <ch.h>
#include <hal.h>
#include <win_storage_native.h>
#include <target_windows_storage_config.h>
#include <nanoHAL_Windows_Storage.h>
#include <Target_Windows_Storage.h>
#include <nanoHAL_Windows_Storage.h>

#define WORKING_DRIVE_IS_INTERNAL_DRIVE                                                                                \
    (memcmp(workingDrive, INTERNAL_DRIVE0_LETTER, sizeof(INTERNAL_DRIVE0_LETTER) - 1) == 0 ||                          \
     memcmp(workingDrive, INTERNAL_DRIVE1_LETTER, sizeof(INTERNAL_DRIVE1_LETTER) - 1) == 0)

extern int32_t GetInternalDriveIndex(char *drive);

#endif // WIN_STORAGE_NATIVE_TARGET_H
