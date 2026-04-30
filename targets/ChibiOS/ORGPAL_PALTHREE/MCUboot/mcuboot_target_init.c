//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Board-specific MCUboot hardware initialisation for ORGPAL_PALTHREE.
// Mirrors the USB CDC startup sequence used in the ORGPAL_PALTHREE nanoBooter.

#include "hal.h"
#include "usbcfg.h"

void mcuboot_target_init(void)
{
    //  Initializes a serial-over-USB CDC driver.
    sduObjectInit(&SERIAL_DRIVER);
    sduStart(&SERIAL_DRIVER, &serusbcfg);

    // Activates the USB driver and then the USB bus pull-up on D+.
    // Note, a delay is inserted in order to not have to disconnect the cable after a reset.
    usbDisconnectBus(serusbcfg.usbp);
    chThdSleepMilliseconds(100);
    usbStart(serusbcfg.usbp, &usbcfg);
    usbConnectBus(serusbcfg.usbp);
}
