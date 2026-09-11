//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Board LED for the MCUboot heartbeat — ORGPAL_PALX.
//
// LED2 on PG7, the same LED the legacy nanoBooter blinked. halInit() applies the
// PIN_MODE_OUTPUT config from board.h, so no further setup is needed here.
//
// Note board.h also defines LINE_LED2 as PAL_LINE(GPIOC, 13U); that macro is stale
// (PC13 is the RTC pin on this board). Use the GPIOG_LED2 pad, as nanoBooter does.

#include "hal.h"

#include "mcuboot_board_iface.h"

void mcuboot_heartbeat_led_toggle(void)
{
    palTogglePad(GPIOG, GPIOG_LED2);
}
