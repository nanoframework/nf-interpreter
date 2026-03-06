//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGET_HAL_H
#define TARGET_HAL_H

#include <chrono>
#include <csignal>
#include <cstdint>
#include <thread>

// Keep the same macro shape used by other targets.
#define PLATFORM_DELAY(milliSecs) std::this_thread::sleep_for(std::chrono::milliseconds((milliSecs)))

// Start with the same minimum socket count used by win32 host.
#define PLATFORM_DEPENDENT__SOCKETS_MAX_COUNT 1

#define NANOCLR_STOP() std::raise(SIGTRAP)

inline bool Target_ConfigUpdateRequiresErase()
{
    return true;
}

inline bool Target_HasNanoBooter()
{
    return false;
}

inline bool Target_CanChangeMacAddress()
{
    return false;
}

inline bool Target_IFUCapable()
{
    return false;
}

inline bool Target_HasProprietaryBooter()
{
    return false;
}

inline uint32_t GetPlatformCapabilities()
{
    return 0;
}

inline uint32_t GetTargetCapabilities()
{
    return 0;
}

inline bool RequestToLaunchProprietaryBootloader()
{
    return false;
}

inline bool RequestToLaunchNanoBooter(int32_t errorCode)
{
    (void)errorCode;
    return false;
}

inline uint32_t CPU_TicksPerSecond()
{
    // 100ns ticks to align with nanoCLR time usage in virtual-device mode.
    return 10000000U;
}

inline uint64_t CPU_MicrosecondsToTicks(uint64_t uSec)
{
    return uSec * 10ULL;
}

inline uint64_t CPU_MillisecondsToTicks(uint64_t mSec)
{
    return mSec * 10000ULL;
}

#endif // TARGET_HAL_H
