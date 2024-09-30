//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "stdafx.h"
#include <Win32TimerQueue.h>
#include <semaphore>

static std::unique_ptr<Microsoft::Win32::Timer> boolEventsTimer = nullptr;
static bool *saveTimerCompleteFlag = nullptr;

void local_Events_SetBoolTimer_Callback()
{
    NATIVE_PROFILE_PAL_EVENTS();

    *saveTimerCompleteFlag = true;
}

bool Events_Initialize_Platform()
{
    boolEventsTimer = nullptr;

    return true;
}

void Events_SetBoolTimer(bool *timerCompleteFlag, uint32_t millisecondsFromNow)
{
    NATIVE_PROFILE_PAL_EVENTS();

    // we assume only 1 can be active, abort previous just in case
    if (boolEventsTimer != nullptr)
    {
        boolEventsTimer.reset();
    }

    if (timerCompleteFlag)
    {
        // As only one timer running at a time we will just save it
        saveTimerCompleteFlag = timerCompleteFlag;

        boolEventsTimer =
            std::make_unique<Microsoft::Win32::Timer>(millisecondsFromNow, local_Events_SetBoolTimer_Callback);
    }
}

// semaphore, condition variable and flags for CLR's global events state
static std::binary_semaphore EventsSemaphore(1);
static std::condition_variable EventsConditionVar;
static uint32_t SystemEvents;

bool Events_Initialize()
{
    Events_Initialize_Platform();

    EventsSemaphore.acquire();

    SystemEvents = 0;

    EventsSemaphore.release();

    return TRUE;
}

bool Events_Uninitialize()
{
    EventsSemaphore.acquire();

    SystemEvents = 0;

    EventsSemaphore.release();

    return TRUE;
}

void Events_Set(UINT32 Events)
{
    EventsSemaphore.acquire();

    SystemEvents |= Events;

    EventsSemaphore.release();

    EventsConditionVar.notify_all();
}

uint32_t Events_Get(UINT32 EventsOfInterest)
{
    EventsSemaphore.acquire();

    auto retVal = SystemEvents & EventsOfInterest;
    SystemEvents &= ~EventsOfInterest;

    EventsSemaphore.release();

    return retVal;
}

void Events_Clear(UINT32 Events)
{
    EventsSemaphore.acquire();

    SystemEvents &= ~Events;

    EventsSemaphore.release();

    EventsConditionVar.notify_all();
}

uint32_t Events_MaskedRead(uint32_t Events)
{
    return SystemEvents & Events;
}

// block this thread and wake up when at least one of the requested events is set or a timeout occurs...
uint32_t Events_WaitForEvents(uint32_t powerLevel, uint32_t wakeupSystemEvents, uint32_t timeoutMilliseconds)
{
    (void)powerLevel; // Assuming powerLevel is not used in this context

    auto startTime = std::chrono::steady_clock::now();
    auto endTime = startTime + std::chrono::milliseconds(timeoutMilliseconds);
    bool timeout = false;

    while (true)
    {
        // Attempt to acquire the semaphore with a short wait to poll the condition
        if (EventsSemaphore.try_acquire_for(std::chrono::milliseconds(10)))
        {
            if (CLR_EE_DBG_IS(RebootPending) || CLR_EE_DBG_IS(ExitPending) || (wakeupSystemEvents & SystemEvents) != 0)
            {
                // Condition met or special case encountered, break the loop
                // Release the semaphore before breaking
                EventsSemaphore.release();
                break;
            }

            // Release the semaphore if condition not met
            EventsSemaphore.release();
        }

        if (std::chrono::steady_clock::now() >= endTime)
        {
            timeout = true;

            // Exit the loop if timeout reached
            break;
        }
    }

    return timeout ? 0 : SystemEvents & wakeupSystemEvents;
}

void Events_SetCallback(set_Event_Callback pfn, void *arg)
{
    (void)pfn;
    (void)arg;

    _ASSERTE(FALSE);
}

void FreeManagedEvent(uint8_t category, uint8_t subCategory, uint16_t data1, uint32_t data2)
{
    (void)category;
    (void)subCategory;
    (void)data1;
    (void)data2;

    // nothing to release in this platform
}
