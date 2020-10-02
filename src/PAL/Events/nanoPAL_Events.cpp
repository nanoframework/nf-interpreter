//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_events.h>
// #include <stdbool.h>

static uint32_t systemEvents;

set_Event_Callback g_Event_Callback = NULL;
void *g_Event_Callback_Arg = NULL;

// functions below declared as weak can be replaced at target/platform level if needed

__nfweak bool Events_Initialize_Platform()
{
    return true;
}

__nfweak bool Events_Uninitialize_Platform()
{
    return true;
}

__nfweak void Events_Set(uint32_t events)
{
    NATIVE_PROFILE_PAL_EVENTS();

    // set events atomically
#ifdef __CM0_CMSIS_VERSION
    GLOBAL_LOCK();
    systemEvents |= events;
    GLOBAL_UNLOCK();
#else
    __atomic_fetch_or(&systemEvents, events, __ATOMIC_RELAXED);
#endif

    if (g_Event_Callback != NULL)
    {
        g_Event_Callback(g_Event_Callback_Arg);
    }
}

__nfweak uint32_t Events_Get(uint32_t eventsOfInterest)
{
    NATIVE_PROFILE_PAL_EVENTS();

    // ... clear the requested flags atomically
    // give the caller notice of just the events they asked for ( and were cleared already )
#ifdef __CM0_CMSIS_VERSION
    // get the requested flags from system events state and...
    uint32_t returnEvents = (systemEvents & eventsOfInterest);

    GLOBAL_LOCK();
    systemEvents &= ~eventsOfInterest;
    GLOBAL_UNLOCK();

    return returnEvents;
#else
    return __atomic_fetch_and(&systemEvents, ~eventsOfInterest, __ATOMIC_RELAXED) & eventsOfInterest;
#endif
}

__nfweak uint32_t Events_MaskedRead(uint32_t eventsOfInterest)
{
    NATIVE_PROFILE_PAL_EVENTS();
    return (systemEvents & eventsOfInterest);
}

__nfweak void Events_SetCallback(set_Event_Callback pfn, void *arg)
{
    NATIVE_PROFILE_PAL_EVENTS();

    g_Event_Callback = pfn;
    g_Event_Callback_Arg = arg;
}

bool Events_Initialize()
{
    NATIVE_PROFILE_PAL_EVENTS();

    // init events atomically
    __atomic_clear(&systemEvents, __ATOMIC_RELAXED);

    return Events_Initialize_Platform();
}

bool Events_Uninitialize()
{
    NATIVE_PROFILE_PAL_EVENTS();

    return Events_Uninitialize_Platform();
}
