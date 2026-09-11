//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Board LED for the MCUboot heartbeat — ST_STM32F769I_DISCOVERY.
//
// LD2 (green) on PJ5, the same LED the legacy nanoBooter blinked. halInit() applies the
// PIN_MODE_OUTPUT config from board.h, so no further setup is needed here.

#include "hal.h"

#include "mcuboot_board_iface.h"

void mcuboot_heartbeat_led_toggle(void)
{
    palToggleLine(LINE_LED2_GREEN);
}
