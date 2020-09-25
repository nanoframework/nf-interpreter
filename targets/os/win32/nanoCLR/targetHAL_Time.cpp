//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "stdafx.h"


//using namespace Microsoft::SPOT::Emulator;

void HAL_Time_Sleep_MicroSeconds( unsigned int uSec )
{
    // UNDONE: FIXME: return EmulatorNative::GetITimeDriver()->Sleep_MicroSeconds( uSec );    
}

void HAL_Time_Sleep_MicroSeconds_InterruptEnabled( unsigned int uSec )
{
    // UNDONE: FIXME: return EmulatorNative::GetITimeDriver()->Sleep_MicroSecondsInterruptsEnabled( uSec );    
}

unsigned int HAL_Time_CurrentSysTicks()
{
	// TODO need to check if using the Win32 100ns ticks works
    return 0; // UNDONE: FIXME: EmulatorNative::GetITimeDriver()->CurrentTicks();
}

uint64_t HAL_Time_SysTicksToTime( unsigned int sysTicks )
{
    _ASSERTE(sysTicks <= 0x7FFFFFFF);
    
    //No need to go to managed code just to return Time.

	// TODO need to convert from whatever ticks are these to Win32 100ns ticks
    return sysTicks;
}
       
uint64_t HAL_Time_CurrentTime()
{
    return 0; // UNDONE: FIXME: EmulatorNative::GetITimeDriver()->CurrentTime();
}

uint64_t HAL_Time_CurrentDateTime(bool datePartOnly)
{
	if (datePartOnly)
	{
		SYSTEMTIME st;
		HAL_Time_ToSystemTime(HAL_Time_CurrentTime(), &st);

		st.wHour = 0;
		st.wMinute = 0;
		st.wSecond = 0;
		st.wMilliseconds = 0;

		return HAL_Time_ConvertFromSystemTime(&st);
	}
	else
	{
		// on Windows we'll just return what is the current time from HAL
		return HAL_Time_CurrentTime();
	}
}

void HAL_Time_GetDriftParameters  ( signed int* a, signed int* b, signed __int64* c )
{
    *a = 1;
    *b = 1;
    *c = 0;
}

unsigned int CPU_SystemClock()
{
    return 0; // UNDONE: FIXME: EmulatorNative::GetITimeDriver()->SystemClock;
}

unsigned int CPU_TicksPerSecond()
{
    return 0; // UNDONE: FIXME: EmulatorNative::GetITimeDriver()->TicksPerSecond;
}

//Completions

void HAL_COMPLETION::InitializeList()
{
}

void HAL_COMPLETION::EnqueueDelta( unsigned int uSecFromNow )
{
    // UNDONE: FIXME: EmulatorNative::GetITimeDriver()->EnqueueCompletion( (IntPtr)this, uSecFromNow ); 
}

void HAL_COMPLETION::EnqueueTicks( uint64_t EventTimeTicks )
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
void HAL_CONTINUATION::InitializeList()
{
}

bool HAL_CONTINUATION::IsLinked()
{
    return false; // UNDONE: FIXME: EmulatorNative::GetITimeDriver()->IsLinked( (IntPtr)this );
}

bool HAL_CONTINUATION::Dequeue_And_Execute()
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

