//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// maps the SD Card driver
#define SD_CARD_DRIVER      SDCD2

// maps the SD Card detect GPIO definition (in Target_Windows_Storage.c) to board GPIO line (in board.h)
#define SDCARD_LINE_DETECT  LINE_SD_DETECT

// includes SPIFFS in storage
#define USE_SPIFFS_FOR_STORAGE  FALSE
