//
// Copyright (c) 2017 The nanoFramework project contributors
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

void Events_Set( unsigned int Events )
{
    // UNDONE: FIXME: EmulatorNative::GetIEventsDriver()->Set( Events );
}

unsigned int Events_Get( unsigned int EventsOfInterest )
{
    return 0; // UNDONE: FIXME: EmulatorNative::GetIEventsDriver()->Get( EventsOfInterest ); 
}

void Events_Clear( unsigned int Events )
{
    // UNDONE: FIXME: return EmulatorNative::GetIEventsDriver()->Clear( Events );
}

unsigned int Events_MaskedRead( unsigned int Events )
{
    return 0; // UNDONE: FIXME: EmulatorNative::GetIEventsDriver()->MaskedRead( Events );
}

unsigned int Events_WaitForEvents( unsigned int powerLevel, unsigned int WakeupSystemEvents, unsigned int Timeout_Milliseconds )
{
    return 0; // UNDONE: FIXME: EmulatorNative::GetIEventsDriver()->WaitForEvents( powerLevel, WakeupSystemEvents, Timeout_Milliseconds ); 
}

void Events_SetBoolTimer( bool* TimerCompleteFlag, unsigned int MillisecondsFromNow )
{
    // UNDONE: FIXME: return EmulatorNative::GetIEventsDriver()->SetBoolTimer( (IntPtr)TimerCompleteFlag, MillisecondsFromNow ); 
}

//void Events_SetCallback( set_Event_Callback pfn, void* arg )
//{
//    _ASSERTE(FALSE);
//}

