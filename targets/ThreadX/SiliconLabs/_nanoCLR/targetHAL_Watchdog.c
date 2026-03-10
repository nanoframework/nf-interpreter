//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <em_wdog.h>

#include <target_common.h>

//  Method to initialize the watchdog.
void Watchdog_Init()
{
    // Watchdog Initialize settings
    // 8193 clock cycles of a 1kHz clock  ~8 seconds period
    WDOG_Init_TypeDef wdogInit = WDOG_INIT_DEFAULT;
    wdogInit.clkSel = wdogClkSelULFRCO;
    wdogInit.perSel = wdogPeriod_8k;

    // Initializing watchdog with chosen settings
    WDOGn_Init(DEFAULT_WDOG, &wdogInit);
}

//  Reset the watchdog. If not done within the timout period,
//  the watchdog will trigger and MCU will reset
void Watchdog_Reset()
{
    WDOGn_Feed(DEFAULT_WDOG);
}
