//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "stdafx.h"

//using namespace Microsoft::SPOT::Emulator;

void HAL_Time_Sleep_MicroSeconds( UINT32 uSec )
{
    // UNDONE: FIXME: return EmulatorNative::GetITimeDriver()->Sleep_MicroSeconds( uSec );    
}

void HAL_Time_Sleep_MicroSeconds_InterruptEnabled( UINT32 uSec )
{
    // UNDONE: FIXME: return EmulatorNative::GetITimeDriver()->Sleep_MicroSecondsInterruptsEnabled( uSec );    
}

UINT32 HAL_Time_CurrentSysTicks()
{
	// TODO need to check if using the Win32 100ns ticks works
    return 0; // UNDONE: FIXME: EmulatorNative::GetITimeDriver()->CurrentTicks();
}

INT64 HAL_Time_SysTicksToTime( UINT32 sysTicks )
{
    _ASSERTE(sysTicks <= 0x7FFFFFFF);
    
    //No need to go to managed code just to return Time.

	// TODO need to convert from whatever ticks are these to Win32 100ns ticks
    return sysTicks;
}
       
INT64 HAL_Time_CurrentTime()
{
    return 0; // UNDONE: FIXME: EmulatorNative::GetITimeDriver()->CurrentTime();
}

void HAL_Time_GetDriftParameters  ( INT32* a, INT32* b, INT64* c )
{
    *a = 1;
    *b = 1;
    *c = 0;
}

UINT32 CPU_SystemClock()
{
    return 0; // UNDONE: FIXME: EmulatorNative::GetITimeDriver()->SystemClock;
}

UINT32 CPU_TicksPerSecond()
{
    return 0; // UNDONE: FIXME: EmulatorNative::GetITimeDriver()->TicksPerSecond;
}

//Completions

void HAL_COMPLETION::EnqueueDelta( UINT32 uSecFromNow )
{
    // UNDONE: FIXME: EmulatorNative::GetITimeDriver()->EnqueueCompletion( (IntPtr)this, uSecFromNow ); 
}

void HAL_COMPLETION::EnqueueTicks( UINT64 EventTimeTicks )
{
    _ASSERTE(FALSE);
}

void HAL_COMPLETION::Abort()
{
    // UNDONE: FIXME: EmulatorNative::GetITimeDriver()->AbortCompletion( (IntPtr)this );
}

void HAL_COMPLETION::Execute()
{
    if(this->ExecuteInISR)
    {
        HAL_CONTINUATION* cont = this;

        cont->Execute();
    }
    else
    {
        this->Enqueue();
    }
}

//Continuations

bool HAL_CONTINUATION::IsLinked()
{
    return false; // UNDONE: FIXME: EmulatorNative::GetITimeDriver()->IsLinked( (IntPtr)this );
}

BOOL HAL_CONTINUATION::Dequeue_And_Execute()
{
    return 0; // UNDONE: FIXME: EmulatorNative::GetITimeDriver()->DequeueAndExecuteContinuation();
}

void HAL_CONTINUATION::InitializeCallback( HAL_CALLBACK_FPN EntryPoint, void* Argument )
{
    Initialize();

    Callback.Initialize( EntryPoint, Argument );
}

void HAL_CONTINUATION::Enqueue()
{    
    _ASSERTE(this->GetEntryPoint() != NULL);
    
    // UNDONE: FIXME: EmulatorNative::GetITimeDriver()->EnqueueContinuation( (IntPtr) this );        
}

void HAL_CONTINUATION::Abort()
{
    // UNDONE: FIXME: EmulatorNative::GetITimeDriver()->AbortContinuation( (IntPtr) this );
}

//various

//void CLR_RT_EmulatorHooks::Notify_ExecutionStateChanged()
//{    
//    // UNDONE: FIXME: EmulatorNative::GetITimeDriver()->IsExecutionPaused = ClrIsDebuggerStopped();
//}

