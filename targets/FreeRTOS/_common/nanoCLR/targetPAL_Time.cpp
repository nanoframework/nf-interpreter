//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL.h>

#include "FreeRTOS.h"
#include <timers.h>

static TimerHandle_t nextEventTimer;

static void NextEventTimer_Callback( TimerHandle_t xTimer )
{
    (void)xTimer;
    
    // this call also schedules the next one, if there is one
    HAL_COMPLETION::DequeueAndExec();
}

HRESULT Time_Initialize()
{
    nextEventTimer = xTimerCreate( "NextEventTimer", 10, pdFALSE, (void *)0, NextEventTimer_Callback);

    return S_OK;
}

HRESULT Time_Uninitialize()
{
    xTimerDelete(nextEventTimer, 0);

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
    }    
    else
    {
        if (HAL_Time_CurrentTime() >= compareValueTicks) 
        { 
            // already missed the event, dequeue and execute immediately 
            HAL_COMPLETION::DequeueAndExec();
        }
        else
        {
            xTimerStop( nextEventTimer, 0 );

            // compareValueTicks is the time (in sys ticks) that is being requested to fire an HAL_COMPLETION::DequeueAndExec()
            // need to subtract the current system time to set when the timer will fire
            compareValueTicks -= HAL_Time_CurrentTime();
            
            if (compareValueTicks == 0) 
            {
                // compare value is 0 so dequeue and execute immediately
                // no need to call the timer
                HAL_COMPLETION::DequeueAndExec();
                return;
            }

            // no need to stop the timer even if it's running because the API does it anyway
            // need to convert from nF ticks to milliseconds and then to FreeRTOS sys ticks to load the timer
            xTimerChangePeriod(nextEventTimer, compareValueTicks, 0);
        }
    }
}
