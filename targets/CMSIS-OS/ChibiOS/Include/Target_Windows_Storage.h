//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_WINDOWS_STORAGE_H_
#define _TARGET_WINDOWS_STORAGE_H_ 1

#define SDCARD_POLLING_INTERVAL                (1000)
#define SDCARD_POLLING_DELAY                   (2)


#define USB_MSD_POLLING_INTERVAL               (1000)
#define USB_MSD_POLLING_DELAY                  (2)

// void RemoveHandler(eventid_t id);
// void InsertHandler(eventid_t id);

#ifdef __cplusplus
extern "C" {
#endif

// declaration of RTOS thread
void UsbMsdWorkingThread(void const * argument);

#ifdef __cplusplus
}
#endif

#endif  //_TARGET_WINDOWS_STORAGE_H_
