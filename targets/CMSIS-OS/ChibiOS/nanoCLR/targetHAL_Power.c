//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_Power.h>
#include <nanoHAL_v2.h>
#include <target_platform.h>
#include <cmsis_os.h>
#include <hal.h>
#include <ch.h>

inline void CPU_Reset(){ NVIC_SystemReset(); };

inline bool CPU_IsSoftRebootSupported() { return true; };

// CPU sleep is not currently implemented in this target
inline void CPU_Sleep(SLEEP_LEVEL_type level, uint64_t wakeEvents) { (void)level; (void)wakeEvents;  };

void CPU_SetPowerMode(PowerLevel_type powerLevel)
{
    (void)powerLevel;
}
