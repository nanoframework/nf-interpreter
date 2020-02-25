//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL.h>
#include <target_platform.h>
#include <hal.h>
#include <ch.h>


// timer for next event
static virtual_timer_t nextEventTimer;
void*  nextEventCallbackDummyArg = NULL;

static void NextEventTimer_Callback( void* arg )
{
    (void)arg;

    // this call also schedules the next one, if there is one
    HAL_COMPLETION::DequeueAndExec();
}

HRESULT Time_Initialize()
{
    // need to setup the timer at boot, but stoped
    chVTSet(&nextEventTimer, TIME_INFINITE, NextEventTimer_Callback, nextEventCallbackDummyArg);

    return S_OK;
}

HRESULT Time_Uninitialize()
{
    chVTReset(&nextEventTimer);

    // nothing to do here has time management is handled by ChibiOS
    return S_OK;
}

void Time_SetCompare ( uint64_t compareValueTicks )
{
    if(compareValueTicks == 0)
    {
        // compare value is 0 so dequeue and schedule immediately
        // can't call chVTSet with 'immidiate delay value', so use value 1 to get it executed ASAP
        chVTSet(&nextEventTimer, 1, NextEventTimer_Callback, nextEventCallbackDummyArg);
    }
    else if(compareValueTicks == HAL_COMPLETION_IDLE_VALUE)
    {
        // wait for infinity, don't need to do anything here
    }
    else
    {
        if (HAL_Time_CurrentSysTicks() >= compareValueTicks)
        { 
            // already missed the event, dequeue and execute immediately
            // can't call chVTSet with 'immidiate delay value', so use value 1 to get it executed ASAP
            chVTSet(&nextEventTimer, 1, NextEventTimer_Callback, nextEventCallbackDummyArg);
        }
        else
        {
            // compareValueTicks is the time (in sys ticks) that is being requested to fire an HAL_COMPLETION::DequeueAndExec()
            // need to subtract the current system time to set when the timer will fire
        	// compareValueTicks is in CMSIS ticks (which equals to ms), so we use TIME_MS2I only to round
        	compareValueTicks -= HAL_Time_CurrentSysTicks();
            uint64_t delay = TIME_MS2I(compareValueTicks);
            // make sure that chVTSet does not get called with zero delay
            if (delay == 0)
            {
            	delay = 1;
            }

            // no need to stop the timer if it's running because the API does it anyway
            chVTSet(&nextEventTimer, delay, NextEventTimer_Callback, nextEventCallbackDummyArg);
        }
    }
}
