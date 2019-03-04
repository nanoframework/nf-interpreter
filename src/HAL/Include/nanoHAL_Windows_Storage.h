//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//
#ifndef _NANOHAL_WINDOWS_STORAGE_H_
#define _NANOHAL_WINDOWS_STORAGE_H_ 1

// FatFs define for size of file name members
// ANSI/OEM at DBCS
#define FF_LFN_BUF  255

// driver letters and paths
#define SDCARD_DRIVE_LETTER     "D:"
#define SDCARD_DRIVE_PATH       SDCARD_DRIVE_LETTER"\\"
#define USB_MSD_DRIVE_LETTER    "E:"
#define USB_MSD_DRIVE_PATH      USB_MSD_DRIVE_LETTER"\\"

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
#define DRIVE_LETTER_LENGTH     sizeof(SDCARD_DRIVE_LETTER)
#define DRIVE_PATH_LENGTH       sizeof(SDCARD_DRIVE_PATH)

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
