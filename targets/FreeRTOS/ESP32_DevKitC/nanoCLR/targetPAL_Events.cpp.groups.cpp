//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_Events.h>
#include <nanoPAL.h>
#include <target_platform.h>
#include <Esp32_os.h>

#define ESP32_AVAILABLE_EVENTBITS  0x00ffffff

static void local_Events_SetBoolTimer_Callback(  TimerHandle_t xTimer  );

// events timer
static TimerHandle_t boolEventsTimer;
static bool*  saveTimerCompleteFlag = 0;

EventGroupHandle_t systemEventsHigh;
EventGroupHandle_t systemEventsLow;

set_Event_Callback g_Event_Callback     = NULL;
void*              g_Event_Callback_Arg = NULL;

bool Events_Initialize()
{
    NATIVE_PROFILE_PAL_EVENTS();

    // init events
    // The Eps32 Event group is 24 bits, top 8 bits used by system
    // Currently Nanoframework only uses 21 bits, see nanoHal.h
    systemEventsHigh = xEventGroupCreate();
    systemEventsLow  = xEventGroupCreate();
    
    boolEventsTimer = xTimerCreate( "boolEventsTimer", 10, pdFALSE, (void *)0, local_Events_SetBoolTimer_Callback);

    return true;
}

bool Events_Uninitialize()
{
    NATIVE_PROFILE_PAL_EVENTS();

    vEventGroupDelete( systemEventsHigh );
    vEventGroupDelete( systemEventsLow);
    
    return true;
}

void Events_Set( uint32_t events )
{
    NATIVE_PROFILE_PAL_EVENTS();
ets_printf( "Events_set %X\n", events);
    // set events
    xEventGroupSetBits( systemEventsHigh,  events >> 16 );
    xEventGroupSetBits( systemEventsLow,  events & 0x00ffff );
    
    if( g_Event_Callback != NULL )
    {
        g_Event_Callback( g_Event_Callback_Arg );
    }
}

uint32_t Events_Get( uint32_t eventsOfInterest )
{
    NATIVE_PROFILE_PAL_EVENTS();

    // get the requested flags from system events state and...

    uint32_t returnEvents = (xEventGroupGetBits(systemEventsHigh) << 16) & xEventGroupGetBits(systemEventsLow)

    // Make off top 8 bits otherwise get exception when calling xEventGroupClearBits
    uint32_t eventstoClear = ~eventsOfInterest;

    // ... clear the requested flags atomically
    xEventGroupClearBits(systemEventsHigh, eventstoClear >> 16 );
    xEventGroupClearBits(systemEventsLow,  eventstoClear & 0x00ffff );
    
    // give the caller notice of just the events they asked for ( and were cleared already )
    return returnEvents;
}

uint32_t Events_MaskedRead( uint32_t eventsOfInterest )
{
    NATIVE_PROFILE_PAL_EVENTS();

    uint32_t returnEvents = (xEventGroupGetBits(systemEventsHigh) << 16) & xEventGroupGetBits(systemEventsLow)
    
    return ( returnEvents & eventsOfInterest);
}

static void local_Events_SetBoolTimer_Callback(  TimerHandle_t xTimer  )
{
    NATIVE_PROFILE_PAL_EVENTS();

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
    xTimerStop( boolEventsTimer, 0 );

    if(timerCompleteFlag != NULL)
    {

        xTimerChangePeriod( boolEventsTimer, millisecondsFromNow / portTICK_PERIOD_MS,  0 );

// Was going to just change existing timer but vTimerSetTimerID() does not exist in this version of FreeRTOS
// As only one timer running at a time we will just save it in global memory
        saveTimerCompleteFlag = timerCompleteFlag;
        //        vTimerSetTimerID( boolEventsTimer, (void *)timerCompleteFlag );
        xTimerStart(boolEventsTimer, 0);
    }
}

uint32_t Events_WaitForEvents( uint32_t powerLevel, uint32_t wakeupSystemEvents, uint32_t timeoutMilliseconds )
{

#if defined(HAL_PROFILE_ENABLED)
    Events_WaitForEvents_Calls++;
#endif

    EventBits_t events = (xEventGroupGetBits(systemEventsHigh) << 16) & xEventGroupGetBits(systemEventsLow)
    if( events == 0) {
        // no events, wait for timeoutMilliseconds
        vTaskDelay( timeoutMilliseconds / portTICK_PERIOD_MS );
    }

    return (uint32_t)events;
}

void FreeManagedEvent(uint8_t category, uint8_t subCategory, uint16_t data1, uint32_t data2)
{
    NATIVE_PROFILE_PAL_EVENTS();
}
