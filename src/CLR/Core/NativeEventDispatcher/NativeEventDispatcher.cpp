//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "stdafx.h"
#include <nanoCLR_Hardware.h>
#include <nf_rt_events_native.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

CLR_RT_DblLinkedList CLR_RT_HeapBlock_NativeEventDispatcher::eventList; 

void CLR_RT_HeapBlock_NativeEventDispatcher::HandlerMethod_Initialize()
{
    NATIVE_PROFILE_CLR_IOPORT();

    CLR_RT_HeapBlock_NativeEventDispatcher::eventList.DblLinkedList_Initialize();
}

void CLR_RT_HeapBlock_NativeEventDispatcher::HandlerMethod_RecoverFromGC()
{
    NATIVE_PROFILE_CLR_IOPORT();

    NANOCLR_FOREACH_NODE(CLR_RT_HeapBlock_NativeEventDispatcher, event, CLR_RT_HeapBlock_NativeEventDispatcher::eventList)
    {
        event->RecoverFromGC();
    }
    NANOCLR_FOREACH_NODE_END();
}

void CLR_RT_HeapBlock_NativeEventDispatcher::HandlerMethod_CleanUp()
{
    NATIVE_PROFILE_CLR_IOPORT();

    CLR_RT_HeapBlock_NativeEventDispatcher* event;

    while(NULL != (event = (CLR_RT_HeapBlock_NativeEventDispatcher*)CLR_RT_HeapBlock_NativeEventDispatcher::eventList.FirstValidNode()))
    {
        if(event->driverMethods != NULL)
        {
            event->driverMethods->cleanupProcessor(event);
        }

        event->DetachAll();
        event->ReleaseWhenDeadEx();
    }
}

HRESULT CLR_RT_HeapBlock_NativeEventDispatcher::CreateInstance( CLR_RT_HeapBlock& owner, CLR_RT_HeapBlock& eventRef )
{
    NATIVE_PROFILE_CLR_IOPORT();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_NativeEventDispatcher* event = NULL;
    
    event = EVENTCACHE_EXTRACT_NODE(g_CLR_RT_EventCache,CLR_RT_HeapBlock_NativeEventDispatcher,DATATYPE_IO_PORT); CHECK_ALLOCATION(event);
    
    {
    
        CLR_RT_ProtectFromGC gc( *event );
    
        event->Initialize();

        eventList.LinkAtBack( event );
    
        NANOCLR_CHECK_HRESULT(CLR_RT_ObjectToEvent_Source::CreateInstance( event, owner, eventRef ));
    
    } 

    // Set pointer to driver custom data to NULL. It initialized later by users of CLR_RT_HeapBlock_NativeEventDispatcher
    event->pDrvCustomData = NULL;
    // Set pointers to drivers methods to NULL.
    event->driverMethods  = NULL;

    NANOCLR_CLEANUP();

    if(FAILED(hr))
    {
        if(event)
        {
            event->ReleaseWhenDead();
        }
    }

    NANOCLR_CLEANUP_END();
}

HRESULT CLR_RT_HeapBlock_NativeEventDispatcher::ExtractInstance( CLR_RT_HeapBlock& ref, CLR_RT_HeapBlock_NativeEventDispatcher*& event )
{
    NATIVE_PROFILE_CLR_IOPORT();

    NANOCLR_HEADER();

    CLR_RT_ObjectToEvent_Source* src = CLR_RT_ObjectToEvent_Source::ExtractInstance( ref ); FAULT_ON_NULL(src);

    event = (CLR_RT_HeapBlock_NativeEventDispatcher*)src->m_eventPtr;

    NANOCLR_NOCLEANUP();
}

void CLR_RT_HeapBlock_NativeEventDispatcher::RecoverFromGC()
{
    NATIVE_PROFILE_CLR_IOPORT();

    CheckAll();

    ReleaseWhenDeadEx();
}

bool CLR_RT_HeapBlock_NativeEventDispatcher::ReleaseWhenDeadEx()
{
    NATIVE_PROFILE_CLR_IOPORT();

    if(!IsReadyForRelease()) return false;

    //remove any queued interrupts for this event
    NANOCLR_FOREACH_NODE(CLR_RT_ApplicationInterrupt,interrupt,g_CLR_HW_Hardware.m_interruptData.m_applicationQueue)
    {
        if(this == interrupt->m_interruptPortInterrupt.context)
        {
            interrupt->Unlink();

            --g_CLR_HW_Hardware.m_interruptData.m_queuedInterrupts;

            ThreadTerminationCallback( interrupt );
        }
    }
    NANOCLR_FOREACH_NODE_END();

    return ReleaseWhenDead();
}

/************************************************************************************************************
**  Iterates over HAL queue and remove records that point to this instance of CLR_RT_HeapBlock_NativeEventDispatcher.
**  This operation should be done from Dispose as part of destroying the CLR_RT_HeapBlock_NativeEventDispatcher
**
*************************************************************************************************************/

void CLR_RT_HeapBlock_NativeEventDispatcher::RemoveFromHALQueue()
{
    // Since we are going to analyze and update the queue we need to disable interrupts.
    // Interrupt service routines add records to this queue.
	CLR_UINT32 elemCount = 0;
	GLOBAL_LOCK();
    elemCount = g_CLR_HW_Hardware.m_interruptData.m_HalQueue.NumberOfElements();
    GLOBAL_UNLOCK();

    // For all elements in the queue
    for ( CLR_UINT32 curElem = 0; curElem < elemCount; curElem++ )
    {
        // Retrieve the element ( actually remove it from the queue )
    	CLR_HW_Hardware::HalInterruptRecord* testRec = NULL;
        GLOBAL_LOCK();
        testRec = g_CLR_HW_Hardware.m_interruptData.m_HalQueue.Pop();
        GLOBAL_UNLOCK();
        
        // Check if context of this record points to the instance of CLR_RT_HeapBlock_NativeEventDispatcher
        // If the "context" is the same as "this", then we skip the "Push" and record is removed.
        if ( testRec->m_context != this )
        {
            // If it is different from this instance of CLR_RT_HeapBlock_NativeEventDispatcher, thin push it back
        	CLR_HW_Hardware::HalInterruptRecord* newRec = NULL;
        	GLOBAL_LOCK();
            newRec = g_CLR_HW_Hardware.m_interruptData.m_HalQueue.Push();
            GLOBAL_UNLOCK();

            newRec->AssignFrom( *testRec );
        }
    }
}

void CLR_RT_HeapBlock_NativeEventDispatcher::SaveToHALQueue( uint32_t data1, uint32_t data2 )
{
    NATIVE_PROFILE_CLR_IOPORT();

    CLR_HW_Hardware::HalInterruptRecord* rec = g_CLR_HW_Hardware.m_interruptData.m_HalQueue.Push();

    if(rec == NULL)
    {
        // remove the oldest interrupt to make room for the newest
        g_CLR_HW_Hardware.m_interruptData.m_HalQueue.Pop();
        
        rec = g_CLR_HW_Hardware.m_interruptData.m_HalQueue.Push();
    }
    
    if(rec)
    {
        rec->m_data1   = data1;
        rec->m_data2   = data2;
        rec->m_context = this;
        rec->m_time    = HAL_Time_CurrentTime();
    }

    ::Events_Set( SYSTEM_EVENT_HW_INTERRUPT );
}

void SaveNativeEventToHALQueue( CLR_RT_HeapBlock_NativeEventDispatcher *pContext, uint32_t data1, uint32_t data2 )
{
    pContext->SaveToHALQueue( data1, data2 );
}

void CleanupNativeEventsFromHALQueue( CLR_RT_HeapBlock_NativeEventDispatcher *pContext )
{
    pContext->RemoveFromHALQueue();
}

HRESULT CLR_RT_HeapBlock_NativeEventDispatcher::StartDispatch( CLR_RT_ApplicationInterrupt* appInterrupt, CLR_RT_Thread* th )
{
    NATIVE_PROFILE_CLR_IOPORT();

    NANOCLR_HEADER();

    CLR_RT_StackFrame* stackTop;
    CLR_RT_HeapBlock*  args;
    CLR_RT_HeapBlock_Delegate* dlg;
    CLR_RT_HeapBlock* event;
    const CLR_UINT64 c_UTCMask = 0x8000000000000000ULL;

    InterruptPortInterrupt& interrupt = appInterrupt->m_interruptPortInterrupt;

    NANOCLR_CHECK_HRESULT(RecoverManagedObject( event ));
    dlg = event[ Library_nf_rt_events_native_nanoFramework_Runtime_Events_NativeEventDispatcher::FIELD__threadSpawn ].DereferenceDelegate(); FAULT_ON_NULL(dlg);

    NANOCLR_CHECK_HRESULT(th->PushThreadProcDelegate( dlg ));

    stackTop = th->CurrentFrame();

    args = stackTop->m_arguments;

    if((stackTop->m_call.m_target->flags & CLR_RECORD_METHODDEF::MD_Static) == 0)
    {
        ++args;
    }

    //
    // set values for delegate arguments
    //
    args[0].SetInteger    ( interrupt.data1                        );
    args[1].SetInteger    ( interrupt.data2                        );
    args[2].SetInteger    ( (CLR_UINT64)interrupt.time | c_UTCMask );
    args[2].ChangeDataType( DATATYPE_DATETIME                      );
    
    th->m_terminationCallback  = CLR_RT_HeapBlock_NativeEventDispatcher::ThreadTerminationCallback;
    th->m_terminationParameter = appInterrupt;

    NANOCLR_NOCLEANUP();
}

void CLR_RT_HeapBlock_NativeEventDispatcher::ThreadTerminationCallback( void* arg )
{
    NATIVE_PROFILE_CLR_IOPORT();

    CLR_RT_ApplicationInterrupt* appInterrupt = (CLR_RT_ApplicationInterrupt*)arg;
    CLR_RT_HeapBlock_NativeEventDispatcher::InterruptPortInterrupt& interrupt = appInterrupt->m_interruptPortInterrupt;

    FreeManagedEvent((interrupt.data1 >>  8) & 0xff, //category
                     (interrupt.data1      ) & 0xff, //subCategory
                      interrupt.data1 >> 16        , //data1
                      interrupt.data2              );

    interrupt.data1 = 0;
    interrupt.data2 = 0;

    CLR_RT_Memory::Release( appInterrupt );

    g_CLR_HW_Hardware.SpawnDispatcher();
}

HRESULT CLR_RT_HeapBlock_NativeEventDispatcher::RecoverManagedObject( CLR_RT_HeapBlock*& event )
{
    NATIVE_PROFILE_CLR_IOPORT();

    NANOCLR_HEADER();

    // recover the managed object
    NANOCLR_FOREACH_NODE(CLR_RT_ObjectToEvent_Source,ref,this->m_references)
    {
        if(ref->m_objectPtr)
        {
            event = ref->m_objectPtr;
            NANOCLR_SET_AND_LEAVE(S_OK);
        }
    }
    NANOCLR_FOREACH_NODE_END();

    event = NULL;

    NANOCLR_SET_AND_LEAVE(CLR_E_PIN_DEAD);

    NANOCLR_NOCLEANUP();
}
