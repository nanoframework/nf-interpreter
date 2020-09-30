//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//


#ifndef _WIN_STORAGE_NATIVE_TARGET_H_
#define _WIN_STORAGE_NATIVE_TARGET_H_

#include <win_storage_native.h>
#include <hal.h>

#define WORKING_DRIVE_IS_INTERNAL_DRIVE         memcmp(workingDrive, INTERNAL_DRIVE_LETTER, sizeof(INTERNAL_DRIVE_LETTER) - 1) == 0

#endif  //_WIN_STORAGE_NATIVE_TARGET_H_
