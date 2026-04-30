//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Board-specific MCUboot hardware initialisation for ST_STM32F769I_DISCOVERY.
// Mirrors the serial startup used in the ST_STM32F769I_DISCOVERY nanoBooter.
// USART1/SD1 is routed via the on-board ST-Link USB bridge.

#include "hal.h"

void mcuboot_target_init(void)
{
    // Starts the serial driver.
    sdStart(&SD1, NULL);
}
