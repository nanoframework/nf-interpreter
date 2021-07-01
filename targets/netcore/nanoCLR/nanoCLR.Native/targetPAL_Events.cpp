//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "stdafx.h"
#include <Win32TimerQueue.h>

static std::unique_ptr<Microsoft::Win32::Timer> boolEventsTimer;
static bool *saveTimerCompleteFlag = 0;

void local_Events_SetBoolTimer_Callback()
{
    NATIVE_PROFILE_PAL_EVENTS();

    *saveTimerCompleteFlag = true;
}

bool Events_Initialize_Platform()
{
    boolEventsTimer = NULL;

    return true;
}

void Events_SetBoolTimer(bool *timerCompleteFlag, uint32_t millisecondsFromNow)
{
    NATIVE_PROFILE_PAL_EVENTS();

    // we assume only 1 can be active, abort previous just in case
    if (boolEventsTimer != NULL)
    {
        delete boolEventsTimer.release();
    }

    if (timerCompleteFlag)
    {
        // As only one timer running at a time we will just save it
        saveTimerCompleteFlag = timerCompleteFlag;

        boolEventsTimer =
            std::make_unique<Microsoft::Win32::Timer>(millisecondsFromNow, local_Events_SetBoolTimer_Callback);
    }
}

// mutex, condition variable and flags for CLR's global events state
static std::mutex EventsMutex;
static std::condition_variable EventsConditionVar;
static uint32_t SystemEvents;

bool Events_Initialize()
{
    std::unique_lock<std::mutex> scopeLock(EventsMutex);
    SystemEvents = 0;
    return TRUE;
}

bool Events_Uninitialize()
{
    std::unique_lock<std::mutex> scopeLock(EventsMutex);
    SystemEvents = 0;
    return TRUE;
}

void Events_Set(UINT32 Events)
{
    {
        std::unique_lock<std::mutex> scopeLock(EventsMutex);
        SystemEvents |= Events;
    }
    EventsConditionVar.notify_all();
}

uint32_t Events_Get(UINT32 EventsOfInterest)
{
    std::unique_lock<std::mutex> scopeLock(EventsMutex);
    auto retVal = SystemEvents & EventsOfInterest;
    SystemEvents &= ~EventsOfInterest;
    return retVal;
}

void Events_Clear(UINT32 Events)
{
    {
        std::unique_lock<std::mutex> scopeLock(EventsMutex);
        SystemEvents &= ~Events;
    }
    EventsConditionVar.notify_all();
}

uint32_t Events_MaskedRead(uint32_t Events)
{
    return SystemEvents & Events;
}

// block this thread and wake up when at least one of the requested events is set or a timeout occurs...
uint32_t Events_WaitForEvents(uint32_t powerLevel, uint32_t wakeupSystemEvents, uint32_t timeoutMilliseconds)
{
    std::unique_lock<std::mutex> scopeLock(EventsMutex);

    bool timeout = false;
    // check current condition before waiting as Condition var doesn't do that
    if ((wakeupSystemEvents & SystemEvents) == 0)
    {
        timeout = !EventsConditionVar.wait_for(scopeLock, std::chrono::milliseconds(timeoutMilliseconds), [=]() {
            return (wakeupSystemEvents & SystemEvents) != 0;
        });
    }
    return timeout ? 0 : SystemEvents & wakeupSystemEvents;
}

void Events_SetCallback(set_Event_Callback pfn, void *arg)
{
    _ASSERTE(FALSE);
}

void FreeManagedEvent(uint8_t category, uint8_t subCategory, uint16_t data1, uint32_t data2)
{
}
