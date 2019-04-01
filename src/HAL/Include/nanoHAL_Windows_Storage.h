//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//
#ifndef _NANOHAL_WINDOWS_STORAGE_H_
#define _NANOHAL_WINDOWS_STORAGE_H_ 1

#include <Target_Windows_Storage.h>

// FatFs define for size of file name members
// ANSI/OEM at DBCS
#define FF_LFN_BUF  255

// driver letters and paths
#define INDEX0_DRIVE_LETTER     "D:"
#define INDEX0_DRIVE_PATH       INDEX0_DRIVE_LETTER"\\"
#define INDEX1_DRIVE_LETTER     "E:"
#define INDEX1_DRIVE_PATH       INDEX1_DRIVE_LETTER"\\"
#define INDEX2_DRIVE_LETTER     "F:"
#define INDEX2_DRIVE_PATH       INDEX2_DRIVE_LETTER"\\"

// Storage events sub-categories
#define EVENT_STORAGE_DEVICE_INSERTION      0x01
#define EVENT_STORAGE_DEVICE_REMOVAL        0x02

// enum with the supported drives in Windows.Storage
typedef enum Storage_Drives
{
    // SD Card
    Storage_Drives_SDCard    = 0,

    // USB mass storage device
    Storage_Drives_UsbMsd = 1,

}Storage_Drives;

// constants to be used throughout the code
#define SUPPORTED_DRIVES_COUNT  (2)
#define DRIVE_LETTER_LENGTH     sizeof(INDEX0_DRIVE_LETTER)
#define DRIVE_PATH_LENGTH       sizeof(INDEX0_DRIVE_PATH)

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
extern "C" {
#endif

void Target_FileSystemInit(void);

#ifdef __cplusplus
}
#endif

#endif // _NANOHAL_WINDOWS_STORAGE_H_
