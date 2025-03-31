//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#ifndef NANOHAL_SYS_IO_FILESYSTEM_H
#define NANOHAL_SYS_IO_FILESYSTEM_H

#include <Target_System_IO_FileSystem.h>

// driver letters and paths
// drive letter for SD Card
#define INDEX0_DRIVE_LETTER "D:"
#define INDEX0_DRIVE_PATH   INDEX0_DRIVE_LETTER "\\"
// drive letter for USB mass storage device
#define INDEX1_DRIVE_LETTER "E:"
#define INDEX1_DRIVE_PATH   INDEX1_DRIVE_LETTER "\\"
// spare drive letter
#define INDEX2_DRIVE_LETTER "F:"
#define INDEX2_DRIVE_PATH   INDEX2_DRIVE_LETTER "\\"
// drive letter for internal drive (SPIFFS)
#define INTERNAL_DRIVE0_LETTER "I:"
#define INTERNAL_DRIVE0_PATH   INTERNAL_DRIVE0_LETTER "\\"
// drive letter for second internal drive (SPIFFS)
#define INTERNAL_DRIVE1_LETTER "J:"
#define INTERNAL_DRIVE1_PATH   INTERNAL_DRIVE1_LETTER "\\"

//////////////////////////////////////////////////////////////////////////
// Keep in sync with StorageEventManager.StorageEventType in managed code
// Storage events sub-categories
//////////////////////////////////////////////////////////////////////////
typedef enum StorageEventType
{
    // INVALID
    StorageEventType_Invalid = 0x00,

    // removable device inserted
    StorageEventType_RemovableDeviceInsertion = 0x01,

    // removable device removed
    StorageEventType_RemovableDeviceRemoval = 0x02,

    // Card detect changed
    StorageEventType_CardDetectChanged = 0x03

} StorageEventType;

// constants to be used throughout the code
#define DRIVE_LETTER_LENGTH sizeof(INDEX0_DRIVE_LETTER)
#define DRIVE_PATH_LENGTH   sizeof(INDEX0_DRIVE_PATH)

#ifdef __cplusplus
extern "C"
{
#endif

    void Target_FileSystemInit(void);

#ifdef __cplusplus
}
#endif

#endif // NANOHAL_SYS_IO_FILESYSTEM_H
