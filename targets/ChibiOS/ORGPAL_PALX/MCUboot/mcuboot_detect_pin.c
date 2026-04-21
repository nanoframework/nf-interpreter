//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// mcuboot_detect_pin.c — Serial recovery GPIO detection for ORGPAL_PALX
// (STM32F769NI, button GPIOK7, external pull-up, active-LOW).
//
// boot_serial_detect_pin() mirrors the IsToRemainInBooter() check in
// targets/ChibiOS/ORGPAL_PALX/nanoBooter/main.c:
//   palReadPad(GPIOK, GPIOK_BUTTON_BOOT)
// The same GPIOK7 pin with external pull-up is used; button press = LOW.
//
// MCUboot calls this function after MCUBOOT_SERIAL_DETECT_DELAY (100 ms) so
// no additional debounce delay is needed here.
//
// No UART is initialised for the PAL boards: wire protocol uses USB CDC (SDU1)
// and the USB OTG peripheral occupies PA9/PA10, leaving no accessible USART1
// pins for serial recovery.

#include "mcuboot_serial_port.h"

#if defined(MCUBOOT_SERIAL)

#include "hal.h"

bool boot_serial_detect_pin(void)
{
    // GPIOK7: input with external pull-up, active-LOW.
    // board.c / boardInit() already configures this pin as input.
    // palReadPad() returns 0 when the button is pressed (line pulled LOW).
    return palReadPad(GPIOK, GPIOK_BUTTON_BOOT) == 0U;
}

#endif // MCUBOOT_SERIAL
