//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"

//--//

HRESULT Library_corlib_native_System_Threading_Thread::_ctor___VOID__SystemThreadingThreadStart( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock* pThis = stack.This();

    pThis[ FIELD___delegate ].Assign            ( stack.Arg1() );
    
    // Thread is always constructed with normal priority.
    pThis[ FIELD___priority ].NumericByRef().s4 = ThreadPriority::Normal;
    
    // Book a Thread ID
    pThis[ FIELD___id       ].NumericByRef().s4 = g_CLR_RT_ExecutionEngine.GetNextThreadId();
    
           
#if defined(NANOCLR_APPDOMAINS)
    NANOCLR_CHECK_HRESULT(CLR_RT_ObjectToEvent_Source::CreateInstance( g_CLR_RT_ExecutionEngine.GetCurrentAppDomain(), *pThis, pThis[ FIELD___appDomain ] ));
    NANOCLR_NOCLEANUP();
#else
    NANOCLR_NOCLEANUP_NOLABEL();
#endif

}

HRESULT Library_corlib_native_System_Threading_Thread::Start___VOID( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_Thread*             th;
    CLR_RT_HeapBlock*          pThis;
    CLR_RT_HeapBlock_Delegate* dlg;
    int                        pri;

    //
    // Don't start twice...
    //
    NANOCLR_CHECK_HRESULT(GetThread( stack, th, false, false ));
    if(th != NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    pThis = stack.This();                                     FAULT_ON_NULL(pThis);
    dlg   = pThis[ FIELD___delegate ].DereferenceDelegate(); FAULT_ON_NULL(dlg  );
    
    // Set priority of new sub-thread to the priority stored in the thread C# object.
    // The new sub-thread becames the current one, it should always match priority stored in Thread C# object 

    pri   = pThis[ FIELD___priority ].NumericByRef().s4;

    pThis->ResetFlags( CLR_RT_HeapBlock::HB_Signaled );

    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewThread( th, dlg, pri, pThis[ FIELD___id ].NumericByRef().s4 ));

    NANOCLR_SET_AND_LEAVE(SetThread( stack, th ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Threading_Thread::Abort___VOID( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_Thread* th;

    NANOCLR_CHECK_HRESULT(GetThread( stack, th, true, true ));

    if(th == stack.m_owningThread)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    th->Abort();

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Threading_Thread::Suspend___VOID( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();
    
    // Test if this the call from managed Thread.Suspend() or second call on resume.
    switch( stack.m_customState )
    {
        // This is the normal call from managed code.
        case 0: 
            {
                CLR_RT_Thread* th;

                NANOCLR_CHECK_HRESULT(GetThread( stack, th, true, true ));

                NANOCLR_CHECK_HRESULT(th->Suspend());
                
                // If the thread suspends itself, then we need to re-schedule.
                if ( stack.m_owningThread == th )
                {   // Set flag in thread stack that thread is re-scheduled.
                    stack.m_customState = 1;
                    
                    // Call to th->Suspend() was successful and moved thread to suspended list. 
                    // Now the threads should be re-scheduled to make suspension effective immidiately. 
                    // In order to do it we return CLR_E_RESCHEDULE from this function - Suspend___VOID
                    NANOCLR_SET_AND_LEAVE( CLR_E_RESCHEDULE );
                }
            }
            break;
        
        // This is the case when thread is resumed, brought back from suspended state.
        case 1:
            {   
                stack.m_customState = 0;
                NANOCLR_SET_AND_LEAVE(S_OK);
            }
            break;

        default:
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
            break;
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Threading_Thread::Resume___VOID( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_Thread* th;

    NANOCLR_CHECK_HRESULT(GetThread( stack, th, true, true ));

    NANOCLR_SET_AND_LEAVE(th->Resume());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Threading_Thread::get_Priority___SystemThreadingThreadPriority( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_Thread* th;
    int pri;

    // Get C# thread object
    CLR_RT_HeapBlock* pThis = stack.This(); FAULT_ON_NULL(pThis);
    
    // Reads priority stored in C# object. 
    pri = pThis[ FIELD___priority ].NumericByRef().s4;

    // Here we check consistency of values stored in C# and internal thread objects.
    // Get thread associated with C# thread object. It might be NULL if thread was not started.
    NANOCLR_CHECK_HRESULT(GetThread( stack, th, false, false ));
    
    // If thread was started, then we use priority from the CLR_RT_Thread.
    // There are 2 reasons it might be different from priority from C# object:
    // 1. The manager C# object was garbage collected and then re-created.
    // 2. Sub-thread with different priority may have been created in Library_spot_native_Microsoft_SPOT_ExecutionConstraint::Install___STATIC__VOID__I4__I4

    if ( th != NULL )
    {
       pri = th->GetThreadPriority();
    
       // We store it back to managed object to keep values consistent.
       pThis[ FIELD___priority ].NumericByRef().s4 = pri;
    }

    // Zero priority correspond is equal to managed. ThreadPriority.Lowest.
    // The highest value should be 4 ( ThreadPriority.highest ), but we allow higher values.
    // Users cannot create threads with values outside the Lowest-Highest range; we exceed that
    // range internally, but we can't expose those values to managed code.
    if      (pri < ThreadPriority::Lowest ) pri = ThreadPriority::Lowest;
    else if (pri > ThreadPriority::Highest) pri = ThreadPriority::Highest;
    
    // Return value back to C# code.
    stack.SetResult_I4( pri );
    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Threading_Thread::set_Priority___VOID__SystemThreadingThreadPriority( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    if ( stack.m_customState == 0 )
    {
        CLR_RT_Thread* th;

        CLR_RT_HeapBlock* pThis = stack.This(); FAULT_ON_NULL(pThis);

        // Get argument for priority. Check the range - should be between ThreadPriority::Highest and ThreadPriority::Lowest ( 0 - 4 )  
        int pri = stack.Arg1().NumericByRef().s4;
        if ( pri < ThreadPriority::Lowest || pri > ThreadPriority::Highest )
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        
        // Get CLR thread. The object exists if thread was started.
        NANOCLR_CHECK_HRESULT(GetThread( stack, th, false, true ));

        // Save priority to managed C# object
        pThis[ FIELD___priority ].NumericByRef().s4 = pri;

        if ( th )
        {
            // Priority of current sub-thread is set the thread priority. 
            th->SetThreadPriority( pri );

            stack.m_customState = 1;

            // If we set high priority to another thread, then we need to swtich to another thread. 
            if ( pri > stack.m_owningThread->GetThreadPriority() ) 
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_RESCHEDULE);
            }
        }
    }

    NANOCLR_NOCLEANUP();

}

HRESULT Library_corlib_native_System_Threading_Thread::get_ManagedThreadId___I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    int id;

    // Get C# thread object
    CLR_RT_HeapBlock* pThis = stack.This(); FAULT_ON_NULL(pThis);
    
    // Reads priority stored in C# object. 
    id = pThis[ FIELD___id ].NumericByRef().s4;

    // Return value back to C# code.
    stack.SetResult_I4( id );
    NANOCLR_NOCLEANUP();
}


HRESULT Library_corlib_native_System_Threading_Thread::get_IsAlive___BOOLEAN( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_Thread* th;
    
    GetThread( stack, th, false, false );

    stack.SetResult_Boolean( th != NULL && th->m_status != CLR_RT_Thread::TH_S_Terminated );

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_corlib_native_System_Threading_Thread::Join___VOID( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(Join( stack, TIMEOUT_INFINITE ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Threading_Thread::Join___BOOLEAN__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_INT64 timeExpire;

    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.InitTimeout( timeExpire, stack.Arg1().NumericByRef().s4 ));

    NANOCLR_SET_AND_LEAVE(Join( stack, timeExpire ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Threading_Thread::Join___BOOLEAN__SystemTimeSpan( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_INT64* timeExpireIn;
    CLR_INT64  timeExpireOut;

    timeExpireIn = Library_corlib_native_System_TimeSpan::GetValuePtr( stack.Arg1() ); FAULT_ON_NULL(timeExpireIn);

    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.InitTimeout( timeExpireOut, *timeExpireIn ));

    NANOCLR_SET_AND_LEAVE(Join( stack, timeExpireOut ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Threading_Thread::get_ThreadState___SystemThreadingThreadState( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();

    NANOCLR_HEADER();

    CLR_RT_Thread* th;
    int            val = 0;

    NANOCLR_CHECK_HRESULT(GetThread( stack, th, false, false ));

    if(th == NULL)
    {
        val = 8; //Unstarted
    }
    else
    {
        switch(th->m_status)
        {
        case CLR_RT_Thread::TH_S_Ready     : val =  0; break; // Running
        case CLR_RT_Thread::TH_S_Waiting   : val = 32; break; // WaitSleepJoin
        case CLR_RT_Thread::TH_S_Terminated: val = 16; break; // Stopped
        default                            : _ASSERTE(false); break;
        }
        
        // Suspended thread. Add ThreadState.Suspended to the return value
        if(th->m_flags & CLR_RT_Thread::TH_F_Suspended) 
        {
            val |= 64;   // ThreadState.Suspended = 64
        }
        // Aborted thread. Add ThreadState.Aborted to the return value
        if(th->m_flags & CLR_RT_Thread::TH_F_Aborted) 
        {
            val |= 256;  // ThreadState.Aborted = 256
        }

    }

    stack.SetResult_I4( val );

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Threading_Thread::Sleep___STATIC__VOID__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_INT64 timeExpire;
    CLR_INT32 timeSleep;

    switch(stack.m_customState)
    {
    case 0:
        
        timeSleep = stack.m_arguments[ 0 ].NumericByRef().s4;
        if ( timeSleep != 0 )
        {
            NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.InitTimeout( timeExpire, stack.m_arguments[ 0 ].NumericByRef().s4 ));

            hr = g_CLR_RT_ExecutionEngine.Sleep( stack.m_owningThread, timeExpire );
            if(hr == CLR_E_THREAD_WAITING)
            {
                stack.m_customState = 1;
            }
        }
        else
        {
            // In case of Sleep(0) call the threads should be re-scheduled . 
            // This function updates execution counter in pThread and makes it last to execute.
            // Used to Thread.Sleep(0) imlementation. The thread is still ready, but is last to execute.
            g_CLR_RT_ExecutionEngine.UpdateToLowestExecutionCounter( stack.m_owningThread );
            // In order to do it we return CLR_E_RESCHEDULE from this function.
            stack.m_customState = 1;
            NANOCLR_SET_AND_LEAVE( CLR_E_RESCHEDULE );
        }


        NANOCLR_LEAVE();

    case 1:
        NANOCLR_SET_AND_LEAVE(S_OK);

    default:
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Threading_Thread::get_CurrentThread___STATIC__SystemThreadingThread( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_Thread*    thread = stack.m_owningThread;
    CLR_RT_HeapBlock& top    = stack.PushValueAndClear();
    CLR_RT_HeapBlock* pRes;

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
    //If we are a thread spawned by the debugger to perform evaluations,
    //return the thread object that correspond to thread that has focus in debugger.
    thread = thread->m_realThread;
#endif //#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

    //Find an existing managed thread, if it exists
    //making sure to only return the managed object association with the current appdomain
    //to prevent leaking of managed Thread objects across AD boundaries.
    NANOCLR_FOREACH_NODE(CLR_RT_ObjectToEvent_Source,src,thread->m_references)
    {
        CLR_RT_HeapBlock* pManagedThread = src->m_objectPtr;
        bool fFound = false;
        _ASSERTE(pManagedThread != NULL);
        
#if defined(NANOCLR_APPDOMAINS)
        {
            CLR_RT_ObjectToEvent_Source* appDomainSrc =  CLR_RT_ObjectToEvent_Source::ExtractInstance( pManagedThread[ FIELD___appDomain ] );

            FAULT_ON_NULL_HR(appDomainSrc, CLR_E_FAIL);
            
            fFound = (appDomainSrc->m_eventPtr == g_CLR_RT_ExecutionEngine.GetCurrentAppDomain());
        }
#else
        fFound = true;
#endif

        if(fFound)
        {
            top.SetObjectReference( pManagedThread );
            
            NANOCLR_SET_AND_LEAVE(S_OK);
        }
    }
    NANOCLR_FOREACH_NODE_END();    


    //Create the managed thread.
    //This implies that there is no state in the managed object.  This is not exactly true, as the managed thread 
    //contains the priority as well as the delegate to start.  However, that state is really just used as a placeholder for
    //the data before the thread is started.  Once the thread is started, they are copied over to the unmanaged thread object
    //and no longer used.  The managed object is then used simply as a wrapper for the unmanaged thread.  Therefore, it is safe 
    //to simply make another managed thread here.
    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex( top, g_CLR_RT_WellKnownTypes.m_Thread ));
            
    pRes = top.Dereference();

    NANOCLR_CHECK_HRESULT(CLR_RT_ObjectToEvent_Source::CreateInstance( thread, *pRes, pRes[ FIELD___thread ] ));

#if defined(NANOCLR_APPDOMAINS)
    NANOCLR_CHECK_HRESULT(CLR_RT_ObjectToEvent_Source::CreateInstance( g_CLR_RT_ExecutionEngine.GetCurrentAppDomain(), *pRes, pRes[ FIELD___appDomain ] ));
#endif

    NANOCLR_NOCLEANUP();
}

#if (NANOCLR_REFLECTION == TRUE)

HRESULT Library_corlib_native_System_Threading_Thread::GetDomain___STATIC__SystemAppDomain( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

#if !defined(NANOCLR_APPDOMAINS)    
    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());
#else
    CLR_RT_AppDomain* appDomain = g_CLR_RT_ExecutionEngine.GetCurrentAppDomain();
    
    NANOCLR_CHECK_HRESULT(appDomain->GetManagedObject( stack.PushValue() ));
#endif    
        
    NANOCLR_NOCLEANUP();
}
#endif // NANOCLR_REFLECTION

//--//

CLR_RT_ObjectToEvent_Source* Library_corlib_native_System_Threading_Thread::GetThreadReference( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RT_HeapBlock* pThis = stack.This();

    return CLR_RT_ObjectToEvent_Source::ExtractInstance( pThis[ FIELD___thread ] );
}

void Library_corlib_native_System_Threading_Thread::ResetThreadReference( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RT_ObjectToEvent_Source* src = GetThreadReference( stack );
    if(src)
    {
        src->Detach();
    }
}

HRESULT Library_corlib_native_System_Threading_Thread::SetThread( CLR_RT_StackFrame& stack, CLR_RT_Thread* th )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock* pThis = stack.This();

    ResetThreadReference( stack );

    NANOCLR_SET_AND_LEAVE(CLR_RT_ObjectToEvent_Source::CreateInstance( th, *pThis, pThis[ FIELD___thread ] ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Threading_Thread::GetThread( CLR_RT_StackFrame& stack, CLR_RT_Thread*& th, bool mustBeStarted, bool noSystemThreads )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();
    CLR_RT_ObjectToEvent_Source* src = GetThreadReference( stack );

    th = (src == NULL) ? NULL : (CLR_RT_Thread*)src->m_eventPtr;
    if(th == NULL)
    {
        if (mustBeStarted)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
        }
    }
    else if (noSystemThreads && th->m_flags & CLR_RT_Thread::TH_F_System)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Threading_Thread::Join( CLR_RT_StackFrame& stack, const CLR_INT64& timeExpire )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_Thread* th;
    bool fRes = true;

    NANOCLR_CHECK_HRESULT(GetThread( stack, th, true, false ));

    //Don't let programs join from system threads like interrupts or finalizers
    if ( stack.m_owningThread->m_flags & CLR_RT_Thread::TH_F_System )
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    //
    // You cannot join yourself.
    //
    if(th == stack.m_owningThread)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    if(th->m_status != CLR_RT_Thread::TH_S_Terminated)
    {
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_WaitForObject::WaitForSignal( stack, timeExpire, stack.ThisRef() ));

        fRes = (stack.m_owningThread->m_waitForObject_Result != CLR_RT_Thread::TH_WAIT_RESULT_TIMEOUT);
    }

    stack.SetResult_Boolean( fRes );

    NANOCLR_NOCLEANUP();
}

