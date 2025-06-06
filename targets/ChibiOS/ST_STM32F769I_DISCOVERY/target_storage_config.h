//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_STORAGE_CONFIG_H_
#define _TARGET_STORAGE_CONFIG_H_

// the following macro maps the SD card driver
#define SD_CARD_DRIVER SDCD2

// maps the SD Card detect GPIO definition (target storage config) to board GPIO line (in board.h)
#define SDCARD_LINE_DETECT LINE_SD_CARD_DETECT

// the following macro maps the USB mass storage device driver
// #define USB_MSD_DRIVER USBHD2

#endif // _TARGET_STORAGE_CONFIG_H_
