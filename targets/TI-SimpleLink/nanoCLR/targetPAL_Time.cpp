//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL.h>
#include <target_platform.h>
#include <ti/sysbios/knl/Clock.h>
#include <xdc/runtime/Error.h>

static Clock_Handle nextEventTimer;

static void NextEventTimer_Callback( UArg arg )
{
    (void)arg;

    // this call also schedules the next one, if there is one
    HAL_COMPLETION::DequeueAndExec();
}

HRESULT Time_Initialize()
{
    Clock_Params params;

    Clock_Params_init(&params);
    params.arg = NULL;
    params.startFlag = FALSE;
    params.period = 0;

    nextEventTimer = Clock_create(NextEventTimer_Callback, 0, &params, Error_IGNORE);

    return S_OK;
}

HRESULT Time_Uninitialize()
{
    Clock_stop(nextEventTimer);

    return S_OK;
}

void Time_SetCompare ( uint64_t compareValueTicks )
{
    if(compareValueTicks == 0)
    {
        // compare value is 0 so dequeue and schedule immediately 
        HAL_COMPLETION::DequeueAndExec();
    }
    else if(compareValueTicks == HAL_COMPLETION_IDLE_VALUE)
    {
        // wait for infinity, don't need to do anything here
        return;
    }    
    else
    {
        if (HAL_Time_CurrentSysTicks() >= compareValueTicks) 
        { 
            // already missed the event, dequeue and execute immediately 
            HAL_COMPLETION::DequeueAndExec();
        }
        else
        {
            // need to stop the timer, in case it's running
            Clock_stop(nextEventTimer);

            // compareValueTicks is the time (in sys ticks) that is being requested to fire an HAL_COMPLETION::DequeueAndExec()
            // need to subtract the current system time to set when the timer will fire
            compareValueTicks -= HAL_Time_CurrentSysTicks();
            
            if (compareValueTicks == 0) 
            {
                // compare value is 0 so dequeue and execute immediately
                // no need to call the timer
                HAL_COMPLETION::DequeueAndExec();
                return;
            }

            Clock_setPeriod(nextEventTimer, compareValueTicks);
            Clock_start(nextEventTimer);
        }
    }
}
