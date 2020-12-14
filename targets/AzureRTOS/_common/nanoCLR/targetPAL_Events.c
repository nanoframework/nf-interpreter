//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <tx_api.h>

#include <nanoCLR_Headers.h>
#include <target_platform.h>

uint64_t CPU_MillisecondsToTicks(uint64_t ticks);

// timer for bool events
TX_TIMER boolEventsTimer;
static bool *saveTimerCompleteFlag = 0;

static void local_Events_SetBoolTimer_Callback()
{
    *saveTimerCompleteFlag = true;
}

bool Events_Initialize_Platform()
{
    return tx_timer_create(
        &boolEventsTimer,
        "BoolEvents Timer",
        local_Events_SetBoolTimer_Callback,
        0,
        0,
        1,
        TX_NO_ACTIVATE) == TX_SUCCESS;
}

bool Events_Uninitialize_Platform()
{
    tx_timer_delete(&boolEventsTimer);

    return true;
}

void Events_SetBoolTimer(bool *timerCompleteFlag, uint32_t millisecondsFromNow)
{
    if (timerCompleteFlag != NULL)
    {
        // As only one timer running at a time we will just save it
        saveTimerCompleteFlag = timerCompleteFlag;

        // need to stop the timer first
        tx_timer_deactivate(&boolEventsTimer);
        tx_timer_change(&boolEventsTimer, 0, millisecondsFromNow / 10);
        tx_timer_activate(&boolEventsTimer);
    }
}

