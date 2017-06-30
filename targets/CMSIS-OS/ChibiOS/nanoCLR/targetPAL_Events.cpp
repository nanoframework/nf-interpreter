//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_Events.h>
#include <nanoPAL.h>
#include <target_platform.h>
#include <hal.h>

// events timer
static virtual_timer_t eventsBoolTimer;

bool Events_Initialize()
{
    NATIVE_PROFILE_PAL_EVENTS();
    return true;
}

bool Events_Uninitialize()
{
    NATIVE_PROFILE_PAL_EVENTS();

    chVTResetI(&eventsBoolTimer);

    return true;
}

static void local_Events_SetBoolTimer_Callback( void* arg )
{
    NATIVE_PROFILE_PAL_EVENTS();
    bool* timerCompleteFlag = (bool*)arg;

    *timerCompleteFlag = true;
}

void Events_SetBoolTimer( bool* timerCompleteFlag, unsigned int millisecondsFromNow )
{
    NATIVE_PROFILE_PAL_EVENTS();

    // we assume only 1 can be active, abort previous just in case
    chVTResetI(&eventsBoolTimer);

    if(timerCompleteFlag != NULL)
    {
        chVTSetI(&eventsBoolTimer, MS2ST(millisecondsFromNow), local_Events_SetBoolTimer_Callback, timerCompleteFlag);
    }
}
