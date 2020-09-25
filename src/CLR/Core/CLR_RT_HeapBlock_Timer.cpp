//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "Core.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

 HRESULT CLR_RT_HeapBlock_Timer::CreateInstance( CLR_UINT32 flags, CLR_RT_HeapBlock& owner, CLR_RT_HeapBlock& tmRef )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Timer* timer = NULL;

    //
    // Create a request and stop the calling thread.
    //
    timer = EVENTCACHE_EXTRACT_NODE(g_CLR_RT_EventCache,CLR_RT_HeapBlock_Timer,DATATYPE_TIMER_HEAD); CHECK_ALLOCATION(timer);

    {
        CLR_RT_ProtectFromGC gc( *timer );

        timer->Initialize();

        timer->m_flags               = flags;
        timer->m_timeExpire          = TIMEOUT_INFINITE;
        timer->m_timeFrequency       = TIMEOUT_INFINITE;
        timer->m_timeLastExpiration  = 0;
        timer->m_ticksLastExpiration = 0;

        g_CLR_RT_ExecutionEngine.m_timers.LinkAtBack( timer );

        NANOCLR_SET_AND_LEAVE(CLR_RT_ObjectToEvent_Source::CreateInstance( timer, owner, tmRef ));
    }

    NANOCLR_CLEANUP();

    if(FAILED(hr))
    {
        if(timer) timer->ReleaseWhenDead();
    }

    NANOCLR_CLEANUP_END();
}

HRESULT CLR_RT_HeapBlock_Timer::ExtractInstance( CLR_RT_HeapBlock& ref, CLR_RT_HeapBlock_Timer*& timer )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_ObjectToEvent_Source* src = CLR_RT_ObjectToEvent_Source::ExtractInstance( ref ); FAULT_ON_NULL(src);

    timer = (CLR_RT_HeapBlock_Timer*)src->m_eventPtr;

    NANOCLR_NOCLEANUP();
}

//--//

void CLR_RT_HeapBlock_Timer::RecoverFromGC()
{
    NATIVE_PROFILE_CLR_CORE();
    CheckAll();

    ReleaseWhenDead();  
}

//--//

void CLR_RT_HeapBlock_Timer::Trigger()
{
    NATIVE_PROFILE_CLR_CORE();
    if(m_references.IsEmpty()) return;
    
    if(m_flags & CLR_RT_HeapBlock_Timer::c_Executing)
    {        
        return;
    }

    m_flags |= CLR_RT_HeapBlock_Timer::c_Triggered;
}

void CLR_RT_HeapBlock_Timer::SpawnTimer( CLR_RT_Thread* th )
{   
    NATIVE_PROFILE_CLR_CORE();
     //Only one managed timer max
    _ASSERTE(m_references.NumOfNodes() == 1);

    CLR_RT_ObjectToEvent_Source* ref          = (CLR_RT_ObjectToEvent_Source*)m_references.FirstValidNode();
    CLR_RT_HeapBlock*            managedTimer = ref->m_objectPtr;
    CLR_RT_HeapBlock*            callback     = &managedTimer[ Library_corlib_native_System_Threading_Timer::FIELD___callback ];
    CLR_RT_HeapBlock*            state        = &managedTimer[ Library_corlib_native_System_Threading_Timer::FIELD___state    ];
    CLR_RT_HeapBlock_Delegate*   delegate     = callback->DereferenceDelegate();
    CLR_RT_ProtectFromGC         gc( *managedTimer );

    _ASSERTE(delegate != NULL);  if(delegate == NULL) return;
    _ASSERTE(delegate->DataType() == DATATYPE_DELEGATE_HEAD);

    m_ticksLastExpiration = HAL_Time_CurrentTime();
    m_timeLastExpiration  = m_timeExpire;
    m_timeExpire          = TIMEOUT_INFINITE;

    if(SUCCEEDED(th->PushThreadProcDelegate( delegate )))
    {
        CLR_RT_StackFrame* stack = th->FirstFrame();
        if(stack->Next() != NULL)
        {
            int numArgs = stack->m_call.m_target->numArgs;
            if(numArgs > 0)
            {
                stack->m_arguments[ numArgs-1 ].Assign( *state );
            }
        }

        //
        // Associate the timer with the thread.
        //
        m_flags |=  CLR_RT_HeapBlock_Timer::c_Executing;
        m_flags &= ~CLR_RT_HeapBlock_Timer::c_Triggered;

        th->m_terminationCallback  = CLR_RT_HeapBlock_Timer::ThreadTerminationCallback;
        th->m_terminationParameter = this;
    }
}

void CLR_RT_HeapBlock_Timer::ThreadTerminationCallback( void* param )
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RT_HeapBlock_Timer* pThis = (CLR_RT_HeapBlock_Timer*)param;

    pThis->m_flags &= ~CLR_RT_HeapBlock_Timer::c_Executing;

    pThis->Reschedule();

    g_CLR_RT_ExecutionEngine.SpawnTimer();
}

void CLR_RT_HeapBlock_Timer::Reschedule()
{
    NATIVE_PROFILE_CLR_CORE();
    if((m_flags & CLR_RT_HeapBlock_Timer::c_Recurring   ) &&
       (m_flags & CLR_RT_HeapBlock_Timer::c_EnabledTimer)  )
    {
        CLR_UINT64 expire = m_timeLastExpiration + m_timeFrequency;

        //
        // Normally, adding the 'timeFrequency' will put the expiration in the future.
        //
        // If we fall back too much, we need to compute the next expiration in the future, to avoid an avalanche effect.
        //
        if(expire < HAL_Time_CurrentTime())
        {
            expire = HAL_Time_CurrentTime() - ((HAL_Time_CurrentTime() - expire) % m_timeFrequency) + m_timeFrequency;
        }

        m_timeExpire = expire; CLR_RT_ExecutionEngine::InvalidateTimerCache();
    }
}

//--//

void CLR_RT_HeapBlock_Timer::AdjustNextFixedExpire( const SYSTEMTIME& systemTime, bool fNext )
{
    NATIVE_PROFILE_CLR_CORE();
    SYSTEMTIME st = systemTime;
    CLR_INT64  baseTime;
    CLR_INT64  add;

    CLR_RT_ExecutionEngine::InvalidateTimerCache();

    switch(m_flags & CLR_RT_HeapBlock_Timer::c_AnyChange)
    {
    case CLR_RT_HeapBlock_Timer::c_SecondChange: add = TIME_CONVERSION__ONESECOND * (CLR_INT64)TIME_CONVERSION__TO_SECONDS;                                               break;
    case CLR_RT_HeapBlock_Timer::c_MinuteChange: add = TIME_CONVERSION__ONEMINUTE * (CLR_INT64)TIME_CONVERSION__TO_SECONDS; st.wSecond = 0;                               break;
    case CLR_RT_HeapBlock_Timer::c_HourChange  : add = TIME_CONVERSION__ONEHOUR   * (CLR_INT64)TIME_CONVERSION__TO_SECONDS; st.wSecond = 0; st.wMinute = 0;               break;
    case CLR_RT_HeapBlock_Timer::c_DayChange   : add = TIME_CONVERSION__ONEDAY    * (CLR_INT64)TIME_CONVERSION__TO_SECONDS; st.wSecond = 0; st.wMinute = 0; st.wHour = 0; break;

    default                                    : return;
    }

    st.wMilliseconds = 0;
    baseTime = HAL_Time_ConvertFromSystemTime( &st );

    m_timeExpire     = fNext ? baseTime + add : baseTime;
    m_timeFrequency  = add;
    m_flags         |= CLR_RT_HeapBlock_Timer::c_Recurring;
}

bool CLR_RT_HeapBlock_Timer::CheckDisposed( CLR_RT_StackFrame& stack )
{
    CLR_RT_HeapBlock*       pThis =   stack.This();
    CLR_RT_HeapBlock_Timer* timer;
    
    if(!FAILED(CLR_RT_HeapBlock_Timer::ExtractInstance( pThis[ Library_corlib_native_System_Threading_Timer::FIELD___timer ], timer )))
    {
        if((timer->m_flags & CLR_RT_HeapBlock_Timer::c_ACTION_Destroy) == 0)
        {
            return false;
        }
    }

    return true;
}

HRESULT CLR_RT_HeapBlock_Timer::ConfigureObject( CLR_RT_StackFrame& stack, CLR_UINT32 flags )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock*       pThis =   stack.This();
    CLR_RT_HeapBlock*       args  = &(stack.Arg1());
    CLR_RT_HeapBlock_Timer* timer;
    
    const CLR_INT64 maxTime = (CLR_INT64)0x7FFFFFFF * (CLR_INT64)TIME_CONVERSION__TO_MILLISECONDS;
    const CLR_INT64 minTime = 0;

    if(flags & CLR_RT_HeapBlock_Timer::c_ACTION_Create)
    {
        FAULT_ON_NULL(args[ 0 ].DereferenceDelegate());

        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Timer::CreateInstance( 0, *pThis, pThis[ Library_corlib_native_System_Threading_Timer::FIELD___timer ] ));

        pThis[ Library_corlib_native_System_Threading_Timer::FIELD___callback ].Assign( args[ 0 ] );
        pThis[ Library_corlib_native_System_Threading_Timer::FIELD___state    ].Assign( args[ 1 ] );

        args += 2;
    }

    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Timer::ExtractInstance( pThis[ Library_corlib_native_System_Threading_Timer::FIELD___timer ], timer ));

    if(flags & CLR_RT_HeapBlock_Timer::c_ACTION_Create)
    {
        CLR_UINT32 anyChange = (flags & CLR_RT_HeapBlock_Timer::c_AnyChange);

        if(anyChange)
        {
            SYSTEMTIME systemTime; HAL_Time_ToSystemTime( HAL_Time_CurrentTime(), &systemTime );

            timer->m_flags |= anyChange | CLR_RT_HeapBlock_Timer::c_EnabledTimer;

            timer->AdjustNextFixedExpire( systemTime, true );
        }
        else
        {
            flags |= CLR_RT_HeapBlock_Timer::c_ACTION_Change;
        }
    }

    if(flags & CLR_RT_HeapBlock_Timer::c_ACTION_Destroy)
    {
        timer->m_flags &= ~CLR_RT_HeapBlock_Timer::c_EnabledTimer;
    }

    if(flags & CLR_RT_HeapBlock_Timer::c_ACTION_Change)
    {
        //
        // You cannot change a fixed timer after creation.
        //
        if(timer->m_flags & CLR_RT_HeapBlock_Timer::c_AnyChange) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);

        if(flags & CLR_RT_HeapBlock_Timer::c_INPUT_Int32)
        {
            timer->m_timeExpire    = args[ 0 ].NumericByRef().s4;
            timer->m_timeFrequency = args[ 1 ].NumericByRef().s4;

            if (timer->m_timeExpire    == -1) timer->m_timeExpire    = TIMEOUT_INFINITE;
            if (timer->m_timeFrequency == -1) timer->m_timeFrequency = TIMEOUT_INFINITE;
        }
        else if(flags & CLR_RT_HeapBlock_Timer::c_INPUT_TimeSpan)
        {
            CLR_INT64* pVal;

            pVal = Library_corlib_native_System_TimeSpan::GetValuePtr( args[ 0 ] ); FAULT_ON_NULL(pVal); 
            if (*pVal == -c_TickPerMillisecond) timer->m_timeExpire = TIMEOUT_INFINITE;
            else timer->m_timeExpire    = *pVal;
            
            pVal = Library_corlib_native_System_TimeSpan::GetValuePtr( args[ 1 ] ); FAULT_ON_NULL(pVal); 
            if (*pVal == -c_TickPerMillisecond) timer->m_timeFrequency = TIMEOUT_INFINITE;
            else timer->m_timeFrequency    = *pVal;
        }

        if(timer->m_timeExpire == TIMEOUT_INFINITE) 
        {
            timer->m_flags &= ~CLR_RT_HeapBlock_Timer::c_EnabledTimer;
        }
        else
        {
            if(flags & CLR_RT_HeapBlock_Timer::c_INPUT_Int32) timer->m_timeExpire *= TIME_CONVERSION__TO_MILLISECONDS;

            if (minTime <= timer->m_timeExpire && timer->m_timeExpire <= maxTime)
            {
                timer->m_timeExpire += HAL_Time_CurrentTime();

                timer->m_flags |= CLR_RT_HeapBlock_Timer::c_EnabledTimer;
            }
            else
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
            }
        }

        if(timer->m_timeFrequency == 0 || timer->m_timeFrequency == TIMEOUT_INFINITE) 
        {
            timer->m_flags &= ~CLR_RT_HeapBlock_Timer::c_Recurring;
        }
        else
        {
            if(flags & CLR_RT_HeapBlock_Timer::c_INPUT_Int32) timer->m_timeFrequency *= TIME_CONVERSION__TO_MILLISECONDS;
            
            if(minTime <= timer->m_timeFrequency && timer->m_timeFrequency <= maxTime )
            {
                timer->m_flags |= CLR_RT_HeapBlock_Timer::c_Recurring;
            }
            else
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
            }
        }
    }

    CLR_RT_ExecutionEngine::InvalidateTimerCache();

    NANOCLR_NOCLEANUP();
}

