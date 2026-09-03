//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <atomic>
#include <chrono>
#include <cstdint>
#include <thread>

#include <nanoHAL.h>
#include <nanoHAL_Time.h>
#include <nanoPAL_Events.h>

// Generation counter: each new Time_SetCompare call invalidates the previous timer thread.
static std::atomic<uint64_t> s_timerGeneration{0};

extern "C" void Time_SetCompare(uint64_t compareValue)
{
    // Increment generation so any previously spawned timer thread exits without firing.
    uint64_t myGen = s_timerGeneration.fetch_add(1, std::memory_order_acq_rel) + 1;

    std::thread([compareValue, myGen]() {
        // Convert compare value (100ns ticks) to current time delta.
        uint64_t nowTicks = HAL_Time_CurrentTime();

        if (compareValue > nowTicks)
        {
            uint64_t deltaTicks = compareValue - nowTicks;
            // Each tick is 100ns; convert to microseconds then sleep.
            auto sleepUs = std::chrono::microseconds(deltaTicks / 10ULL);

            // Sleep in small increments so we notice generation changes quickly.
            const auto slice = std::chrono::milliseconds(1);
            auto remaining = sleepUs;

            while (remaining > slice)
            {
                std::this_thread::sleep_for(slice);
                remaining -= slice;

                if (s_timerGeneration.load(std::memory_order_acquire) != myGen)
                    return; // superseded — do not fire
            }

            if (remaining.count() > 0)
                std::this_thread::sleep_for(remaining);
        }

        if (s_timerGeneration.load(std::memory_order_acquire) != myGen)
            return; // superseded after final sleep

        // Fire the HAL completion queue and wake up any Events_WaitForEvents call.
        HAL_COMPLETION::DequeueAndExec();
        Events_Set(SYSTEM_EVENT_FLAG_SYSTEM_TIMER);
    }).detach();
}
