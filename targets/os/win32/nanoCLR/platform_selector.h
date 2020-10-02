//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef _PLATFORM_WINDOWS_SELECTOR_H_
#define _PLATFORM_WINDOWS_SELECTOR_H_ 1

#define PLATFORM_WINDOWS_EMULATOR 1

/////////////////////////////////////////////////////////
//
// macros
//

#define FAT_FS__VALIDATE_READONLY_CACHELINE     1
//#define FAT_FS__DO_NOT_UPDATE_FILE_ACCESS_TIME  1
#define FAT_FS__CACHE_FLUSH_TIMEOUT_USEC        (5*1000*1000)

//
// macros
//
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
//
// constants
//
#define PRODUCT_WATCH                       1
#define PRODUCT_AUXDISPLAY                  1

// Port definitions
#define TOTAL_USART_PORT                    2
#define COM1                                ConvertCOM_ComHandle(0)
#define COM2                                ConvertCOM_ComHandle(1)

#define TOTAL_USB_CONTROLLER                1
#define USB1                                ConvertCOM_UsbHandle(0)

#define TOTAL_DEBUG_PORT                    1
#define COM_DEBUG                           ConvertCOM_DebugHandle(0)

#define TOTAL_MESSAGING_PORT                1
#define COM_MESSAGING                       ConvertCOM_MessagingHandle(0)

#define DEBUG_TEXT_PORT    COM_DEBUG
#define DEBUGGER_PORT      COM_DEBUG
#define MESSAGING_PORT     COM_DEBUG

#define PLATFORM_DEPENDENT_TX_USART_BUFFER_SIZE    512  // there is one TX for each usart port
#define PLATFORM_DEPENDENT_RX_USART_BUFFER_SIZE    512  // there is one RX for each usart port
#define PLATFORM_DEPENDENT_USB_QUEUE_PACKET_COUNT  2    // there is one queue for each pipe of each endpoint and the size of a single packet is sizeof(USB_PACKET64) == 68 bytes


// These are configuration for FATFS, uncomment if non-default values are needed
//#define PLATFORM_DEPENDENT_FATFS_SECTORCACHE_MAXSIZE  8
//#define PLATFORM_DEPENDENT_FATFS_SECTORCACHE_LINESIZE 2048
//#define PLATFORM_DEPENDENT_FATFS_MAX_OPEN_HANDLES     8
#define PLATFORM_DEPENDENT_FATFS_MAX_VOLUMES          8
//
// constants
//
/////////////////////////////////////////////////////////

//#include <processor_selector.h>

#endif // _PLATFORM_WINDOWS_SELECTOR_H_ 1

