//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Cortex-M7 core initialisation for the MCUboot bootloader.
//
// Overrides the weak __cpu_init() from ChibiOS crt1.c, which enables both the I-cache
// and the D-cache. The bootloader keeps the D-cache OFF.

#include <hal.h>

void __cpu_init(void)
{
#if CORTEX_MODEL == 7
    // D-cache deliberately not enabled for MCUboot
    SCB_EnableICache();
#endif
}
