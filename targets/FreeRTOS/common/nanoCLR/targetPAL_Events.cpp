//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_Events.h>
#include <nanoCLR_Runtime.h>
#include <nanoPAL.h>

#include "FreeRTOS.h"
#include "timers.h"

uint64_t CPU_MillisecondsToTicks(uint64_t ticks);

// timer for bool events
static TimerHandle_t boolEventsTimer;

static void local_Events_SetBoolTimer_Callback(TimerHandle_t xTimer);

bool Events_Initialize_Platform()
{
    boolEventsTimer = xTimerCreate("boolEventsTimer", 10, pdFALSE, (void *)0, local_Events_SetBoolTimer_Callback);

    return true;
}

bool Events_Uninitialize_Platform()
{
    xTimerDelete(boolEventsTimer, 0);

    return true;
}

static void local_Events_SetBoolTimer_Callback(TimerHandle_t xTimer)
{
    NATIVE_PROFILE_PAL_EVENTS();
    bool *timerCompleteFlag = (bool *)pvTimerGetTimerID(xTimer);
    *timerCompleteFlag = true;
}

void Events_SetBoolTimer(bool *timerCompleteFlag, uint32_t millisecondsFromNow)
{
    NATIVE_PROFILE_PAL_EVENTS();

    // we assume only 1 can be active, abort previous just in case
    xTimerStop(boolEventsTimer, 0);

    if (timerCompleteFlag != NULL)
    {
        vTimerSetTimerID(boolEventsTimer, (void *)timerCompleteFlag);
        xTimerChangePeriod(boolEventsTimer, millisecondsFromNow / portTICK_PERIOD_MS, 0);
    }
}

uint32_t Events_WaitForEvents(uint32_t powerLevel, uint32_t wakeupSystemEvents, uint32_t timeoutMilliseconds)
{
    // schedule an interrupt for this far in the future
    // timeout is in milliseconds, need to convert to ticks
    uint64_t countsRemaining = CPU_MillisecondsToTicks(timeoutMilliseconds);

#if defined(HAL_PROFILE_ENABLED)
    Events_WaitForEvents_Calls++;
#endif

    uint64_t expireTimeInTicks = HAL_Time_CurrentSysTicks() + countsRemaining;
    bool runContinuations = true;

    while (true)
    {
        EVENTS_HEART_BEAT;

        uint32_t events = Events_MaskedRead(wakeupSystemEvents);
        if (events)
        {
            return events;
        }

        if (expireTimeInTicks <= HAL_Time_CurrentSysTicks())
        {
            break;
        }

        // first check and possibly run any continuations
        // but only if we have slept after stalling
        if (runContinuations && !SystemState_QueryNoLock(SYSTEM_STATE_NO_CONTINUATIONS))
        {
            // if we stall on time, don't check again until after we sleep
            runContinuations = HAL_CONTINUATION::Dequeue_And_Execute();
        }
        else
        {
            // try stalled continuations again after sleeping
            runContinuations = true;

            HAL_COMPLETION::WaitForInterrupts(expireTimeInTicks, powerLevel, wakeupSystemEvents);
        }

        // no events, pass control to the OS
        taskYIELD();

        // check if reboot or exit flags were set when the other OS threads executed
        if (CLR_EE_DBG_IS(RebootPending) || CLR_EE_DBG_IS(ExitPending))
        {
            break;
        }

        // feed the watchdog...
        Watchdog_Reset();
    }

    return 0;
}

void FreeManagedEvent(uint8_t category, uint8_t subCategory, uint16_t data1, uint32_t data2)
{
    (void)category;
    (void)subCategory;
    (void)data1;
    (void)data2;

    NATIVE_PROFILE_PAL_EVENTS();
}
