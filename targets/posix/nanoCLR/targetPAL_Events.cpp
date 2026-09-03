//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <atomic>
#include <mutex>
#include <thread>

#include <nanoCLR_Runtime.h>
#include <nanoHAL_v2.h>

namespace
{
    std::mutex s_eventsMutex;
    std::condition_variable s_eventsCondition;
    uint32_t s_systemEvents = 0;
    std::atomic<uint64_t> s_boolTimerGeneration{0};
} // namespace

bool Events_Initialize_Platform()
{
    return true;
}

// All functions below are declared extern "C" in nanoPAL_Events.h.
// We match that linkage explicitly here so the linker resolves them as C symbols.
extern "C"
{

// Forward declaration — Events_Set is defined later in this block.
void Events_Set(uint32_t events);

void Events_SetBoolTimer(bool *timerCompleteFlag, uint32_t millisecondsFromNow)
{
    // Always invalidate any previously spawned timer thread by bumping the generation,
    // even when timerCompleteFlag is NULL (cancellation).  The Win32 implementation
    // explicitly cancels the old timer in this case; we achieve the same by making the
    // old thread's generation stale so it exits without firing.
    s_boolTimerGeneration.fetch_add(1, std::memory_order_acq_rel);

    if (timerCompleteFlag == nullptr)
    {
        return;
    }

    *timerCompleteFlag = false;
    const uint64_t myGen = s_boolTimerGeneration.load(std::memory_order_acquire);

    // Capture timerCompleteFlag directly in the lambda — never store it in a shared global.
    // A shared global causes a data race: concurrent calls from different threads (each thread
    // sets its own quantum timer) would overwrite the pointer before the previous timer fires,
    // so the old timer writes 'true' to the wrong memory location and the quantum never expires.
    std::thread([timerCompleteFlag, millisecondsFromNow, myGen]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(millisecondsFromNow));

        if (s_boolTimerGeneration.load(std::memory_order_acquire) != myGen)
        {
            return;
        }

        *timerCompleteFlag = true;

        // Wake up any Events_WaitForEvents call so the CLR can process the timer.
        Events_Set(SYSTEM_EVENT_FLAG_SYSTEM_TIMER);
    }).detach();
}

bool Events_Initialize()
{
    std::lock_guard<std::mutex> lock(s_eventsMutex);
    s_systemEvents = 0;
    return Events_Initialize_Platform();
}

bool Events_Uninitialize()
{
    std::lock_guard<std::mutex> lock(s_eventsMutex);
    s_systemEvents = 0;
    return true;
}

void Events_Set(uint32_t events)
{
    {
        std::lock_guard<std::mutex> lock(s_eventsMutex);
        s_systemEvents |= events;
    }

    s_eventsCondition.notify_all();
}

uint32_t Events_Get(uint32_t eventsOfInterest)
{
    std::lock_guard<std::mutex> lock(s_eventsMutex);
    const uint32_t result = s_systemEvents & eventsOfInterest;
    s_systemEvents &= ~eventsOfInterest;
    return result;
}

void Events_Clear(uint32_t events)
{
    {
        std::lock_guard<std::mutex> lock(s_eventsMutex);
        s_systemEvents &= ~events;
    }

    s_eventsCondition.notify_all();
}

uint32_t Events_MaskedRead(uint32_t events)
{
    std::lock_guard<std::mutex> lock(s_eventsMutex);
    return s_systemEvents & events;
}

uint32_t Events_WaitForEvents(uint32_t powerLevel, uint32_t wakeupSystemEvents, uint32_t timeoutMilliseconds)
{
    (void)powerLevel;

    std::unique_lock<std::mutex> lock(s_eventsMutex);

    if (timeoutMilliseconds == 0)
    {
        return s_systemEvents & wakeupSystemEvents;
    }

    // Use a polling approach similar to the Win32 implementation: wait in short
    // slices (10 ms) so we periodically re-check the event flags and the
    // RebootPending / ExitPending conditions.  This avoids relying solely on
    // condition_variable::notify_all() which, on some Linux libstdc++ versions,
    // can exhibit missed-wakeup behaviour when detached timer threads race with
    // the main CLR thread.
    auto startTime = std::chrono::steady_clock::now();
    auto endTime = (timeoutMilliseconds == static_cast<uint32_t>(-1))
                       ? std::chrono::steady_clock::time_point::max()
                       : startTime + std::chrono::milliseconds(timeoutMilliseconds);

    while (true)
    {
        // Check if requested events are already set.
        if ((s_systemEvents & wakeupSystemEvents) != 0)
        {
            return s_systemEvents & wakeupSystemEvents;
        }

        // Check if the CLR is shutting down — matches the Win32 implementation.
        if (CLR_EE_DBG_IS(RebootPending) || CLR_EE_DBG_IS(ExitPending))
        {
            return s_systemEvents & wakeupSystemEvents;
        }

        // Check for timeout expiry.
        if (std::chrono::steady_clock::now() >= endTime)
        {
            return 0;
        }

        // Wait for at most 10 ms before re-checking.
        s_eventsCondition.wait_for(lock, std::chrono::milliseconds(10));
    }
}

// set_Event_Callback matches the typedef in nanoPAL_Events.h: void (*)(void *)
void Events_SetCallback(void (*pfn)(void *), void *arg)
{
    // TODO: callback registration support for host runtime integration.
    (void)pfn;
    (void)arg;
}

void FreeManagedEvent(uint8_t category, uint8_t subCategory, uint16_t data1, uint32_t data2)
{
    (void)category;
    (void)subCategory;
    (void)data1;
    (void)data2;
}

} // extern "C"

