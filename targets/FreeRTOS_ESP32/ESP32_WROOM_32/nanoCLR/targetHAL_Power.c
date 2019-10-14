//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <esp32_os.h>
#include <nanoHAL_v2.h>

inline void CPU_Reset()
{ 
    esp_restart(); 
};

// CPU sleep is not currently implemented in this target
inline void CPU_Sleep(SLEEP_LEVEL_type level, uint64_t wakeEvents)
{ 
    (void)level;
    (void)wakeEvents;

};

inline bool CPU_IsSoftRebootSupported()
{ 
    return true;
};

void CPU_SetPowerMode(PowerLevel_type powerLevel)
{
    switch(powerLevel)
    {
        case PowerLevel__Off:
            // gracefully shutdown everything
            nanoHAL_Uninitialize_C();

            esp_deep_sleep_start();

            break;

        default:
            // all the other power modes are unsupported here
            break;
    }
}
