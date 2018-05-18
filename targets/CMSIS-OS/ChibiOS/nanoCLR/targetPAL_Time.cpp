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
    (bool*)arg;

    chSysLock();

    // this call also schedules the next one, if there is one
    HAL_COMPLETION::DequeueAndExec();
    
    chSysUnlock();
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
    if(compareValueTicks == 0)
    {
        // compare value is 0 so dequeue and schedule immediately 
        NextEventTimer_Callback(nextEventCallbackDummyArg);
    }
    else
    {
        // need to convert from ticks to milliseconds
        // no need to stop the time if it's running because the API does it anyway
        chVTSet(&nextEventTimer, TIME_MS2I(compareValueTicks * TIME_CONVERSION__TO_MILLISECONDS), NextEventTimer_Callback, nextEventCallbackDummyArg);
    }
}
