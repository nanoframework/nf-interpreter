//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_Events.h>
#include <nanoPAL.h>
#include <target_platform.h>
#include <Esp32_os.h>

// events timer
static TimerHandle_t eventsBoolTimer;
static bool*  saveTimerCompleteFlag = 0;

volatile uint32_t systemEvents;


static void local_Events_SetBoolTimer_Callback(  TimerHandle_t xTimer  );

#define chSysLock() 
#define chSysUnlock() 

set_Event_Callback g_Event_Callback     = NULL;
void*              g_Event_Callback_Arg = NULL;

bool Events_Initialize()
{
    NATIVE_PROFILE_PAL_EVENTS();

    // init events
 //   chSysLock();
    systemEvents = 0;
 //   chSysUnlock();

    eventsBoolTimer = xTimerCreate( "eventsTimer", 10, pdFALSE, (void *)0, local_Events_SetBoolTimer_Callback);
 
    return true;
}

bool Events_Uninitialize()
{
    NATIVE_PROFILE_PAL_EVENTS();

    xTimerDelete(eventsBoolTimer,0);
 
    return true;
}

void Events_Set( uint32_t events )
{
    NATIVE_PROFILE_PAL_EVENTS();

    // set events
    chSysLock();
    systemEvents |= events;
    chSysUnlock();

    if( g_Event_Callback != NULL )
    {
        g_Event_Callback( g_Event_Callback_Arg );
    }
}

uint32_t Events_Get( uint32_t eventsOfInterest )
{
    NATIVE_PROFILE_PAL_EVENTS();

    // get the requested flags from system events state and...
    uint32_t returnEvents = (systemEvents & eventsOfInterest);

    // ... clear the requested flags atomically
    chSysLock();
    systemEvents &= ~eventsOfInterest;
    chSysUnlock();
    
    // give the caller notice of just the events they asked for ( and were cleared already )
    return returnEvents;
}

uint32_t Events_MaskedRead( uint32_t eventsOfInterest )
{
    NATIVE_PROFILE_PAL_EVENTS();
    return (systemEvents & eventsOfInterest);
}

static void local_Events_SetBoolTimer_Callback(  TimerHandle_t xTimer  )
{
    NATIVE_PROFILE_PAL_EVENTS();

//   bool* timerCompleteFlag = (bool*)pvTimerGetTimerID( xTimer );
// *timerCompleteFlag = true;

    *saveTimerCompleteFlag = true;
}

void Events_SetCallback( set_Event_Callback pfn, void* arg )
{
    NATIVE_PROFILE_PAL_EVENTS();

    g_Event_Callback     = pfn;
    g_Event_Callback_Arg = arg;
}

void Events_SetBoolTimer( bool* timerCompleteFlag, uint32_t millisecondsFromNow )
{
    NATIVE_PROFILE_PAL_EVENTS();

    // we assume only 1 can be active, abort previous just in case
    xTimerStop( eventsBoolTimer, 0 );

    if(timerCompleteFlag != NULL)
    {

        xTimerChangePeriod( eventsBoolTimer, millisecondsFromNow / portTICK_PERIOD_MS,  0 );

// Was going to just change existing timer but vTimerSetTimerID() does not exist in this version of FreeRtos
// As only one timer running at a time we will just save it in global memory
        saveTimerCompleteFlag = timerCompleteFlag;
        //        vTimerSetTimerID( eventsBoolTimer, (void *)timerCompleteFlag );
        xTimerStart(eventsBoolTimer, 0);
    }
}

uint32_t Events_WaitForEvents( uint32_t powerLevel, uint32_t wakeupSystemEvents, uint32_t timeout_Milliseconds )
{

#if defined(HAL_PROFILE_ENABLED)
    Events_WaitForEvents_Calls++;
#endif

   if( systemEvents == 0) {
        // no events, wait for timeout_Milliseconds
        vTaskDelay( timeout_Milliseconds / portTICK_PERIOD_MS );
    }

    return  systemEvents;
}

void FreeManagedEvent(uint8_t category, uint8_t subCategory, uint16_t data1, uint32_t data2)
{
    NATIVE_PROFILE_PAL_EVENTS();

    // TODO: not sure if this is really needed here... just kept it for further investigation
    // switch(category)
    // {
    //     //case EVENT_GESTURE:
    //     case EVENT_TOUCH:
    //         break;
    //     default:
    //         break;
    // }
}
