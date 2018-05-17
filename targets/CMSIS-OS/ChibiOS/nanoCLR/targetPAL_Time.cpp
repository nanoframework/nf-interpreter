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


static void NextEventTimer_Callback( void* arg )
{
    (bool*)arg;

    // this call also schedules the next one, if there is one
    HAL_COMPLETION::DequeueAndExec();
}

HRESULT Time_Initialize()
{
    // nothing to do here has time management is handled by ChibiOS
    return S_OK;
}

HRESULT Time_Uninitialize()
{
    // nothing to do here has time management is handled by ChibiOS
    return S_OK;
}

void Time_SetCompare ( uint64_t compareValueTicks )
{
    // can have only one event timer setup, abort previous just in case
    chVTResetI(&nextEventTimer);

    if(compareValueTicks == 0)
    {
        // compare value is 0 so dequeue and schedule immediately 
        NextEventTimer_Callback(NULL);
    }
    else
    {
        // need to convert from ticks to milliseconds
        chVTSetI(&nextEventTimer, TIME_MS2I(compareValueTicks * TIME_CONVERSION__TO_MILLISECONDS), NextEventTimer_Callback, NULL);
    }
}
