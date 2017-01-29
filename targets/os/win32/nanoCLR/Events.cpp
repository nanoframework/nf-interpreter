//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "stdafx.h"

//using namespace Microsoft::SPOT::Emulator;

////////////////////////////////////////////////////////////////////////////////////////////////////

//--//

BOOL Events_Initialize()
{
    return TRUE; // UNDONE: FIXME: EmulatorNative::GetIEventsDriver()->Initialize();
}

BOOL Events_Uninitialize()
{
    return TRUE; // UNDONE: FIXME: EmulatorNative::GetIEventsDriver()->Uninitialize();
}

void Events_Set( UINT32 Events )
{
    // UNDONE: FIXME: EmulatorNative::GetIEventsDriver()->Set( Events );
}

UINT32 Events_Get( UINT32 EventsOfInterest )
{
    return 0; // UNDONE: FIXME: EmulatorNative::GetIEventsDriver()->Get( EventsOfInterest ); 
}

void Events_Clear( UINT32 Events )
{
    // UNDONE: FIXME: return EmulatorNative::GetIEventsDriver()->Clear( Events );
}

UINT32 Events_MaskedRead( UINT32 Events )
{
    return 0; // UNDONE: FIXME: EmulatorNative::GetIEventsDriver()->MaskedRead( Events );
}

UINT32 Events_WaitForEvents( UINT32 powerLevel, UINT32 WakeupSystemEvents, UINT32 Timeout_Milliseconds )
{
    return 0; // UNDONE: FIXME: EmulatorNative::GetIEventsDriver()->WaitForEvents( powerLevel, WakeupSystemEvents, Timeout_Milliseconds ); 
}

void Events_SetBoolTimer( BOOL* TimerCompleteFlag, UINT32 MillisecondsFromNow )
{
    // UNDONE: FIXME: return EmulatorNative::GetIEventsDriver()->SetBoolTimer( (IntPtr)TimerCompleteFlag, MillisecondsFromNow ); 
}

//void Events_SetCallback( set_Event_Callback pfn, void* arg )
//{
//    _ASSERTE(FALSE);
//}
