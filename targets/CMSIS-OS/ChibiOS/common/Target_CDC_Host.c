//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <hal.h>
#include <cmsis_os.h>
#include <nanoHAL_v2.h>

#if HAL_USBH_USE_FTDI
#include "usbh/dev/ftdi.h"

__attribute__((noreturn))
void UsbFtdiWorkingThread(void const * argument)
{
    // start USB host
    usbhStart(&USB_FTDI_DRIVER);


    for(;;)
    {
        osDelay(USB_FTDI_POLLING_INTERVAL);

        usbhMainLoop(&USB_FTDI_DRIVER);
    }
}

#endif