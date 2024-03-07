//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#ifndef NANOHAL_PERSISTENT_STORAGE_H
#define NANOHAL_PERSISTENT_STORAGE_H

#include <Target_Windows_Storage.h>

// FatFs define for size of file name members
// ANSI/OEM at DBCS
#ifndef FF_LFN_BUF
#define FF_LFN_BUF 255
#endif

// driver letters and paths

#define DRIVE_PATH_STR "\\"

// SD Card
#define INDEX0_DRIVE_LETTER "D:"
#define INDEX0_DRIVE_PATH   INDEX0_DRIVE_LETTER DRIVE_PATH_STR

// USB mass storage device
#define INDEX1_DRIVE_LETTER "E:"
#define INDEX1_DRIVE_PATH   INDEX1_DRIVE_LETTER DRIVE_PATH_STR

// USB mass storage device 2
#define INDEX2_DRIVE_LETTER "F:"
#define INDEX2_DRIVE_PATH   INDEX2_DRIVE_LETTER DRIVE_PATH_STR

// Internal drive 0 (SPIFFS)
#define INDEX3_DRIVE_LETTER "I:"
#define INDEX3_DRIVE_PATH   INDEX3_DRIVE_LETTER DRIVE_PATH_STR

// Internal drive 1 (SPIFFS)
#define INDEX4_DRIVE_LETTER "J:"
#define INDEX4_DRIVE_PATH   INDEX4_DRIVE_LETTER DRIVE_PATH_STR

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

} StorageEventType;

// constants to be used throughout the code
#define SUPPORTED_DRIVES_COUNT (2)
#define DRIVE_LETTER_LENGTH    (2)
#define DRIVE_PATH_LENGTH      (DRIVE_LETTER_LENGTH + sizeof(DRIVE_PATH_STR))

/////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Storage.CreationCollisionOption (in managed code) !!! //
/////////////////////////////////////////////////////////////////////////////////////////

enum CreationCollisionOption
{
    CreationCollisionOption_GenerateUniqueName = 0,
    CreationCollisionOption_ReplaceExisting,
    CreationCollisionOption_FailIfExists,
    CreationCollisionOption_OpenIfExists
};

#ifdef __cplusplus
extern "C"
{
#endif

    void Target_FileSystemInit(void);

#ifdef __cplusplus
}
#endif

#endif // NANOHAL_PERSISTENT_STORAGE_H
