//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_WINDOWS_STORAGE_H_
#define _TARGET_WINDOWS_STORAGE_H_ 1

#include <ch.h>
#include <cmsis_os.h>
#include <hal.h>
#include <ff.h>

#define SDCARD_POLLING_INTERVAL                1000
#define SDCARD_POLLING_DELAY                   500


#define USB_MSD_POLLING_INTERVAL               1000
#define USB_MSD_POLLING_DELAY                  500


// FatFs define for size of file name members
// ANSI/OEM at DBCS
#define FF_LFN_BUF  255

// driver letters and paths
#define SDCARD_DRIVE_LETTER     "D:"
#define SDCARD_DRIVE_PATH       SDCARD_DRIVE_LETTER"\\"
#define USB_MSD_DRIVE_LETTER    "E:"
#define USB_MSD_DRIVE_PATH      USB_MSD_DRIVE_LETTER"\\"

// void RemoveHandler(eventid_t id);
// void InsertHandler(eventid_t id);

#ifdef __cplusplus
extern "C" {
#endif

void Target_FileSystemInit(void);

#ifdef __cplusplus
}
#endif

#endif  //_TARGET_WINDOWS_STORAGE_H_
