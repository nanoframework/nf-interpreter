//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "stdafx.h"

//using namespace Microsoft::SPOT::Emulator;

////////////////////////////////////////////////////////////////////////////////////////////////////

//--//

bool Events_Initialize()
{
    return TRUE; // UNDONE: FIXME: EmulatorNative::GetIEventsDriver()->Initialize();
}

bool Events_Uninitialize()
{
    return TRUE; // UNDONE: FIXME: EmulatorNative::GetIEventsDriver()->Uninitialize();
}

void Events_Set( uint32_t events )
{
    // UNDONE: FIXME: EmulatorNative::GetIEventsDriver()->Set( Events );
}

uint32_t Events_Get( uint32_t eventsOfInterest )
{
    return 0; // UNDONE: FIXME: EmulatorNative::GetIEventsDriver()->Get( EventsOfInterest ); 
}

void Events_Clear( uint32_t Events )
{
    // UNDONE: FIXME: return EmulatorNative::GetIEventsDriver()->Clear( Events );
}

uint32_t Events_MaskedRead( uint32_t Events )
{
    return 0; // UNDONE: FIXME: EmulatorNative::GetIEventsDriver()->MaskedRead( Events );
}

uint32_t Events_WaitForEvents( uint32_t powerLevel, uint32_t WakeupSystemEvents, uint32_t TimeoutMilliseconds )
{
    return 0; // UNDONE: FIXME: EmulatorNative::GetIEventsDriver()->WaitForEvents( powerLevel, WakeupSystemEvents, TimeoutMilliseconds ); 
}

void Events_SetBoolTimer( bool* TimerCompleteFlag, uint32_t MillisecondsFromNow )
{
    // UNDONE: FIXME: return EmulatorNative::GetIEventsDriver()->SetBoolTimer( (IntPtr)TimerCompleteFlag, MillisecondsFromNow ); 
}

void Events_SetCallback( set_Event_Callback pfn, void* arg )
{
   _ASSERTE(FALSE);
}

void FreeManagedEvent(UINT8 category, UINT8 subCategory, UINT16 data1, UINT32 data2)
{
    
}


