//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_v2.h>

#define SL_STOP_TIMEOUT         (200)
extern uint16_t sl_Stop(const uint16_t Timeout);

void CPU_Reset()
{
    // stop network processor activities before reseting the MCU
    sl_Stop(SL_STOP_TIMEOUT);

    // Reset the MCU
    PRCMHibernateCycleTrigger();    
};
