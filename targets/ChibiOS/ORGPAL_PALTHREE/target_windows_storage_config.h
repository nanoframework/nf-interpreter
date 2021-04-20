//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// the following macro maps the SD card driver
#define SD_CARD_DRIVER      SDCD1

// maps the SD Card detect GPIO definition (in Target_Windows_Storage.c) to board GPIO line (in board.h)
#define SDCARD_LINE_DETECT  LINE_SD_CARD_DETECT

// the following macro maps the USB mass storage device driver
#define USB_MSD_DRIVER      USBHD2

// includes SPIFFS in storage
#define USE_SPIFFS_FOR_STORAGE  TRUE
