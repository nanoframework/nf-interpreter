//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <chrono>
#include <cstdint>
#include <thread>
#include <time.h>

void HAL_Time_Sleep_MicroSeconds(unsigned int uSec)
{
    std::this_thread::sleep_for(std::chrono::microseconds(uSec));
}

void HAL_Time_Sleep_MicroSeconds_InterruptEnabled(unsigned int uSec)
{
    std::this_thread::sleep_for(std::chrono::microseconds(uSec));
}

uint64_t HAL_Time_CurrentSysTicks()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    // Convert to .NET ticks (100ns units).
    return (static_cast<uint64_t>(ts.tv_sec) * 10000000ULL) + (static_cast<uint64_t>(ts.tv_nsec) / 100ULL);
}
