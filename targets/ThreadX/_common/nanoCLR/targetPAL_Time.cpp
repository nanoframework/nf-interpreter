//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <tx_api.h>

#include <nanoPAL.h>
#include <target_platform.h>

// timer for next event
static TX_TIMER nextEventTimer;
void *nextEventCallbackDummyArg = NULL;

static void NextEventTimer_Callback(uint32_t id)
{
    (void)id;

    // this call also schedules the next one, if there is one
    HAL_COMPLETION::DequeueAndExec();
}

HRESULT Time_Initialize()
{
    // create timer without activating it
    if (tx_timer_create(
            &nextEventTimer,
            (char *)"PAL Events Timer",
            NextEventTimer_Callback,
            0,
            1,
            0,
            TX_NO_ACTIVATE) == TX_SUCCESS)
    {
        return S_OK;
    }

    return S_FALSE;
}

HRESULT Time_Uninitialize()
{
    tx_timer_delete(&nextEventTimer);

    return S_OK;
}

void Time_SetCompare(uint64_t compareValueTicks)
{
    if (compareValueTicks == 0)
    {
        // compare value is 0 so dequeue and schedule immediately
        // need to stop the timer first
        tx_timer_deactivate(&nextEventTimer);
        // can't call tx_timer_change with 'immediate delay value', so use value 1 to get it executed ASAP
        tx_timer_change(&nextEventTimer, 1, 0);
        tx_timer_activate(&nextEventTimer);
    }
    else if (compareValueTicks == HAL_COMPLETION_IDLE_VALUE)
    {
        // wait for infinity, don't need to do anything here
    }
    else
    {
        if (HAL_Time_CurrentSysTicks() >= compareValueTicks)
        {
            // already missed the event, dequeue and execute immediately
            // need to stop the timer first
            tx_timer_deactivate(&nextEventTimer);
            // can't call tx_timer_change with 'immediate delay value', so use value 1 to get it executed ASAP
            tx_timer_change(&nextEventTimer, 1, 0);
            tx_timer_activate(&nextEventTimer);
        }
        else
        {
            // compareValueTicks is the time (in sys ticks) that is being requested to fire an
            // HAL_COMPLETION::DequeueAndExec() need to subtract the current system time to set when the timer will fire
            // compareValueTicks is in CMSIS ticks (which equals to ms)
            compareValueTicks -= HAL_Time_CurrentSysTicks();

            // make sure that chVTSet does not get called with zero delay
            if (compareValueTicks == 0)
            {
                // compare value is 0 so dequeue and execute immediately
                // no need to call the timer
                HAL_COMPLETION::DequeueAndExec();
                return;
            }

            // need to stop the timer first
            tx_timer_deactivate(&nextEventTimer);
            tx_timer_change(&nextEventTimer, compareValueTicks, 0);
            tx_timer_activate(&nextEventTimer);
        }
    }
}
