//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_v2.h>

inline void CPU_Reset(){
    //TODO reset
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
    //TODO power modes support
    (void)powerLevel;
}
