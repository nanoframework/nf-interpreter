//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <thread>

namespace
{
std::mutex s_eventsMutex;
std::condition_variable s_eventsCondition;
uint32_t s_systemEvents = 0;
} // namespace

bool Events_Initialize_Platform()
{
    return true;
}

void Events_SetBoolTimer(bool *timerCompleteFlag, uint32_t millisecondsFromNow)
{
    // TODO: Placeholder timer implementation. Make this path thread-safe when runtime integration is wired.
    if (timerCompleteFlag == nullptr)
    {
        return;
    }

    *timerCompleteFlag = false;

    std::thread([timerCompleteFlag, millisecondsFromNow]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(millisecondsFromNow));
        *timerCompleteFlag = true;
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

    if (timeoutMilliseconds == static_cast<uint32_t>(-1))
    {
        s_eventsCondition.wait(lock, [wakeupSystemEvents]() { return (s_systemEvents & wakeupSystemEvents) != 0; });
        return s_systemEvents & wakeupSystemEvents;
    }

    const bool signaled =
        s_eventsCondition.wait_for(lock, std::chrono::milliseconds(timeoutMilliseconds), [wakeupSystemEvents]() {
            return (s_systemEvents & wakeupSystemEvents) != 0;
        });

    return signaled ? (s_systemEvents & wakeupSystemEvents) : 0;
}

using set_Event_Callback = void (*)(void *, uint32_t);

void Events_SetCallback(set_Event_Callback pfn, void *arg)
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
