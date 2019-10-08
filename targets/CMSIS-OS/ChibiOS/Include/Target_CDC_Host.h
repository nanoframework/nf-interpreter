//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_USB_CDC_H_
#define _TARGET_USB_CDC_H_ 1


#define USB_FTDI_POLLING_INTERVAL               (1000)
#define USB_FTDI_POLLING_DELAY                  (1000)

#ifdef __cplusplus
extern "C" {
#endif

    // declaration of CDC working threads
    void UsbFtdiWorkingThread(void const * argument);

#ifdef __cplusplus
}
#endif

#endif  //_TARGET_USB_CDC_H_