//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_v2.h>

#define SL_STOP_TIMEOUT         (200)
extern uint16_t sl_Stop(const uint16_t Timeout);

// FIXME TODO
void CPU_Reset()
{
    // stop network processor activities before reseting the MCU
    sl_Stop(SL_STOP_TIMEOUT);

    // Reset the MCU
    PRCMHibernateCycleTrigger();    
};

// CPU sleep is not currently implemented in this target
inline void CPU_Sleep(SLEEP_LEVEL_type level, uint64_t wakeEvents){ (void)level; (void)wakeEvents; };

inline bool CPU_IsSoftRebootSupported() { return true; };

void CPU_SetPowerMode(PowerLevel_type powerLevel)
{
    switch(powerLevel)
    {
        case PowerLevel__Off:
            // gracefully shutdown everything
            // FIXME TODO
            break;

        default:
            // all the other power modes are unsupported here
            break;
    }
}
