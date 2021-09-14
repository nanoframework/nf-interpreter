//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoWeak.h>
#include <nanoHAL_v2.h>
#include <esp32_idf.h>

inline void HAL_AssertEx()
{
    asm("break.n 1");

    while (true)
    { // nop
    }
}

#if !defined(BUILD_RTM)

inline void HARD_Breakpoint()
{
    asm("break.n 1");

    while (true)
    { // nop
    }
};

#endif // !defined(BUILD_RTM)

// ESP32 stores configuration blocks using its non-volatile storage therefore no erase is required.
inline TARGET_CONFIG_UPDATE_REQUIRES_ERASE(false);

// ESP32 targets don't declare any target capability
inline GET_TARGET_CAPABILITIES(0);

// ESP32 has a proprietatry bootloader
inline TARGET_HAS_PROPRIETARY_BOOTER(true);

inline TARGET_IFU_CAPABLE(false);

// Mutex for GLOBAL_LOCK / GLOBAL_UNLOCK
portMUX_TYPE globalLockMutex = portMUX_INITIALIZER_UNLOCKED;
