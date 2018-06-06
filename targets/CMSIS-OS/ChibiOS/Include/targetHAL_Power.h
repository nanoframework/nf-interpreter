//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_HAL_POWER_H_
#define _TARGET_HAL_POWER_H_ 1

#include <cmsis_os.h>
#include <nanoHAL_v2.h>

inline void CPU_Reset(){ NVIC_SystemReset(); };

// CPU sleep is not currently implemented in this target
inline void CPU_Sleep(SLEEP_LEVEL_type level, uint64_t wakeEvents){ (void)level; (void)wakeEvents;  };

inline bool CPU_IsSoftRebootSupported() { return true; };

#endif //_TARGET_HAL_POWER_H_
