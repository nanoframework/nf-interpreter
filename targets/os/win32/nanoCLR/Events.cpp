//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "stdafx.h"

HAL_COMPLETION g_Events_BoolTimerCompletion;

static void local_Events_SetBoolTimer_Callback(void *arg)
{
    NATIVE_PROFILE_PAL_EVENTS();
    bool *TimerCompleteFlag = (bool *)arg;

    *TimerCompleteFlag = TRUE;
}

void Events_SetBoolTimer(bool *timerCompleteFlag, uint32_t millisecondsFromNow)
{
    NATIVE_PROFILE_PAL_EVENTS();
    // we assume only 1 can be active, abort previous just in case
    g_Events_BoolTimerCompletion.Abort();

    if (timerCompleteFlag)
    {
        g_Events_BoolTimerCompletion.InitializeForISR(local_Events_SetBoolTimer_Callback, timerCompleteFlag);
        g_Events_BoolTimerCompletion.EnqueueDelta(millisecondsFromNow * 1000);
    }
}

void HAL_COMPLETION::EnqueueDelta64(UINT64 uSecFromNow)
{
    NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();

    // grab time first to be closest to now as possible from when this function was called
    uint64_t now = HAL_Time_CurrentSysTicks();
    uint64_t eventTimeTicks = CPU_MicrosecondsToTicks(uSecFromNow);

    EnqueueTicks(now * CPU_TicksPerSecond() + eventTimeTicks);
}

void HAL_COMPLETION::EnqueueDelta(UINT32 uSecFromNow)
{
    NATIVE_PROFILE_PAL_ASYNC_PROC_CALL();
    EnqueueDelta64((UINT64)uSecFromNow);
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

uint32_t Events_MaskedRead(UINT32 Events)
{
    return SystemEvents & Events;
}

// block this thread and wake up when at least one of the requested events is set or a timeout occurs...
uint32_t Events_WaitForEvents(UINT32 powerLevel, UINT32 WakeupSystemEvents, UINT32 Timeout_Milliseconds)
{
    std::unique_lock<std::mutex> scopeLock(EventsMutex);

    bool timeout = false;
    // check current condition before waiting as Condition var doesn't do that
    if ((WakeupSystemEvents & SystemEvents) == 0)
    {
        timeout = !EventsConditionVar.wait_for(scopeLock, std::chrono::milliseconds(Timeout_Milliseconds), [=]() {
            return (WakeupSystemEvents & SystemEvents) != 0;
        });
    }
    return timeout ? 0 : SystemEvents & WakeupSystemEvents;
}

void Events_SetCallback(set_Event_Callback pfn, void *arg)
{
    _ASSERTE(FALSE);
}

void FreeManagedEvent(UINT8 category, UINT8 subCategory, UINT16 data1, UINT32 data2)
{
}
