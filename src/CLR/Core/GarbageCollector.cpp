//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "Core.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

CLR_RT_ProtectFromGC* CLR_RT_ProtectFromGC::s_first = NULL;

void CLR_RT_ProtectFromGC::Initialize( CLR_RT_HeapBlock& ref )
{
    NATIVE_PROFILE_CLR_CORE();
    m_next = s_first; s_first = this;

    m_data = (void**)&ref;
    m_fpn  =          NULL;

    if(ref.IsForcedAlive())
    {
        m_flags = c_HeapBlock;
    }
    else
    {
        m_flags = c_HeapBlock | c_ResetKeepAlive;

        ref.MarkForcedAlive();
    }
}

void CLR_RT_ProtectFromGC::Initialize( void** data, Callback fpn )
{
    NATIVE_PROFILE_CLR_CORE();
    m_next = s_first; s_first = this;

    m_data  = data;
    m_fpn   = fpn;
    m_flags = c_Generic;
}

void CLR_RT_ProtectFromGC::Cleanup()
{
    NATIVE_PROFILE_CLR_CORE();
    s_first = m_next;

    if(m_flags & c_ResetKeepAlive)
    {
        CLR_RT_HeapBlock* obj = (CLR_RT_HeapBlock*)m_data;

        obj->UnmarkForcedAlive();
    }
}

void CLR_RT_ProtectFromGC::Invoke()
{
    NATIVE_PROFILE_CLR_CORE();
    if(m_fpn)
    {
        m_fpn( *m_data );
    }
    else if(m_data)
    {
        g_CLR_RT_GarbageCollector.CheckMultipleBlocks( (CLR_RT_HeapBlock*)m_data, 1 );
    }
}

void CLR_RT_ProtectFromGC::InvokeAll()
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RT_ProtectFromGC* ptr;

    ptr = s_first;
    while(ptr)
    {
        ptr->Invoke();

        ptr = ptr->m_next;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(NANOCLR_TRACE_EARLYCOLLECTION)

CLR_RT_AssertEarlyCollection* CLR_RT_AssertEarlyCollection::s_first = NULL;

CLR_RT_AssertEarlyCollection::CLR_RT_AssertEarlyCollection( CLR_RT_HeapBlock* ptr )
{
    NATIVE_PROFILE_CLR_CORE();
    m_next = s_first; s_first = this;

    m_ptr = ptr;
}

CLR_RT_AssertEarlyCollection::~CLR_RT_AssertEarlyCollection()
{
    NATIVE_PROFILE_CLR_CORE();
    s_first = m_next;
}

void CLR_RT_AssertEarlyCollection::Cancel()
{
    NATIVE_PROFILE_CLR_CORE();
    m_ptr = NULL;
}

void CLR_RT_AssertEarlyCollection::CheckAll( CLR_RT_HeapBlock* ptr )
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RT_AssertEarlyCollection* node;

    node = s_first;
    while(node)
    {
        if(node->m_ptr == ptr)
        {
            CLR_Debug::Printf( "INTERNAL ERROR: %08x retired early!!!\r\n", ptr );

#if defined(_WIN32)
            ::Sleep( 1000 );
            ::DebugBreak();
#endif
        }

        node = node->m_next;
    }
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

CLR_UINT32 CLR_RT_GarbageCollector::ExecuteGarbageCollection()
{
    NATIVE_PROFILE_CLR_CORE();
#if defined(NANOCLR_PROFILE_NEW_ALLOCATIONS)
    g_CLR_PRF_Profiler.RecordGarbageCollectionBegin();
#endif

#if defined(NANOCLR_GC_VERBOSE)
    if(s_CLR_RT_fTrace_GC >= c_CLR_RT_Trace_Info)
    {        
        CLR_Debug::Printf( "    Memory: Start %s\r\n", HAL_Time_CurrentDateTimeToString() );
    }
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////

    CLR_RT_ExecutionEngine::ExecutionConstraint_Suspend();

#if defined(NANOCLR_TRACE_MEMORY_STATS)

    CLR_UINT32 stats_start = HAL_Time_CurrentSysTicks();

#endif

    g_CLR_RT_EventCache.EventCache_Cleanup();

    Mark    ();
    MarkWeak();
    Sweep   ();

    Heap_ComputeAliveVsDeadRatio();

    CheckMemoryPressure();

#if defined(NANOCLR_TRACE_MEMORY_STATS)
    if(s_CLR_RT_fTrace_MemoryStats >= c_CLR_RT_Trace_Info)
    {
        int milliSec = ((int)::HAL_Time_SysTicksToTime( HAL_Time_CurrentSysTicks() - stats_start ) + TIME_CONVERSION__TICKUNITS - 1) / TIME_CONVERSION__TICKUNITS;

        CLR_Debug::Printf( "GC: %dmsec %d bytes used, %d bytes available\r\n", milliSec, m_totalBytes - m_freeBytes, m_freeBytes );
    }

    if(s_CLR_RT_fTrace_MemoryStats >= c_CLR_RT_Trace_Info)
    {
        int countBlocks[ DATATYPE_FIRST_INVALID ]; NANOCLR_CLEAR(countBlocks);
        int countArryBlocks[ DATATYPE_FIRST_INVALID ]; NANOCLR_CLEAR(countArryBlocks);
        int dt;

        NANOCLR_FOREACH_NODE(CLR_RT_HeapCluster,hc,g_CLR_RT_ExecutionEngine.m_heap)
        {
            CLR_RT_HeapBlock_Node* ptr = hc->m_payloadStart;
            CLR_RT_HeapBlock_Node* end = hc->m_payloadEnd;

            while(ptr < end)
            {
                dt = ptr->DataType();
                if(dt < DATATYPE_FIRST_INVALID)
                {
                    countBlocks[ dt ] += ptr->DataSize();

                    if(dt == DATATYPE_SZARRAY)
                    {
                        CLR_RT_HeapBlock_Array* arr = (CLR_RT_HeapBlock_Array*)ptr;

                        if(arr != NULL)
                        {
                            dt = arr->m_typeOfElement;

                            if(dt < DATATYPE_FIRST_INVALID)
                            {
                                countArryBlocks[ dt ] += ptr->DataSize();
                            }
                            else
                            {
                                CLR_Debug::Printf("!!!!Unknown array type: %d\r\n", dt);
                            }
                        }

                    }
                }

                ptr += ptr->DataSize();
            }
        }
        NANOCLR_FOREACH_NODE_END();

        for(dt = DATATYPE_VOID; dt < DATATYPE_FIRST_INVALID; dt++)
        {
            if(countBlocks[ dt ])
            {
                CLR_Debug::Printf( "Type %02X (%-20s): %6d bytes\r\n", dt, c_CLR_RT_DataTypeLookup[ dt ].m_name, countBlocks[ dt ] * sizeof(CLR_RT_HeapBlock) );

                if(dt == DATATYPE_SZARRAY)
                {
                    for(int dt2 = DATATYPE_VOID; dt2 < DATATYPE_FIRST_INVALID; dt2++)
                    {
                        if(countArryBlocks[ dt2 ])
                        {
                            CLR_Debug::Printf( "   Type %02X (%-17s): %6d bytes\r\n", dt2, c_CLR_RT_DataTypeLookup[ dt2 ].m_name, countArryBlocks[ dt2 ] * sizeof(CLR_RT_HeapBlock) );
                        }
                    }
                }
            }
        }
    }
#endif

    CLR_RT_ExecutionEngine::ExecutionConstraint_Resume();

    m_numberOfGarbageCollections++;

    ////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(NANOCLR_GC_VERBOSE)
    if(s_CLR_RT_fTrace_GC >= c_CLR_RT_Trace_Info)
    {
        CLR_Debug::Printf( "    Memory: End %s\r\n", HAL_Time_CurrentDateTimeToString() );
    }
#endif

#if defined(NANOCLR_PROFILE_NEW_ALLOCATIONS)
    g_CLR_PRF_Profiler.RecordGarbageCollectionEnd();
#endif

    return m_freeBytes;
}

//--//

void CLR_RT_GarbageCollector::MarkSlow()
{
    NATIVE_PROFILE_CLR_CORE();
    while(m_fOutOfStackSpaceForGC)
    {       
        m_fOutOfStackSpaceForGC = false;

        //go through managed heap.
        //find all objects that are not alive but are pointed to by a live object
        //put them on the markstack.
        for(CLR_RT_HeapCluster* hc = (CLR_RT_HeapCluster*)g_CLR_RT_ExecutionEngine.m_heap.FirstNode();
            hc->Next();
            hc = (CLR_RT_HeapCluster*)hc->Next()
            )
        {
            CLR_RT_HeapBlock* ptr = hc->m_payloadStart;
            CLR_RT_HeapBlock* end = hc->m_payloadEnd;

            while(ptr < end)
            {
                if(ptr->IsAlive() && !ptr->IsEvent())
                {       
                    CheckSingleBlock_Force( ptr );
                }

                ptr += ptr->DataSize();
            }
        }        
    }
}

void CLR_RT_GarbageCollector::Mark()
{
    NATIVE_PROFILE_CLR_CORE();
#if defined(NANOCLR_VALIDATE_APPDOMAIN_ISOLATION)
    CLR_RT_AppDomain* appDomainSav = g_CLR_RT_ExecutionEngine.GetCurrentAppDomain();
#endif

    //
    // Let's setup the proper workers.
    //
    m_funcSingleBlock    = ComputeReachabilityGraphForSingleBlock;
    m_funcMultipleBlocks = ComputeReachabilityGraphForMultipleBlocks;

    m_fOutOfStackSpaceForGC = false;

    ////////////////////////////////////////////////////////////////////////////
    //
    // Prepare the helper buffers.
    //
    m_weakDelegates_Reachable.DblLinkedList_Initialize();
    
    CLR_RT_DblLinkedList       markStackList;
    MarkStack markStack;
    MarkStackElement           markStackBuffer[ c_minimumSpaceForGC ];    

    m_markStackList = &markStackList;
    m_markStack     = &markStack;
    
    m_markStack    ->Initialize( markStackBuffer, ARRAYSIZE(markStackBuffer) );
    m_markStackList->DblLinkedList_Initialize();
    m_markStackList->LinkAtFront( m_markStack );


    ////////////////////////////////////////////////////////////////////////////
    //
    // Call global markers.
    //
#if defined(NANOCLR_VALIDATE_APPDOMAIN_ISOLATION)                    
     (void)g_CLR_RT_ExecutionEngine.SetCurrentAppDomain( NULL );
#endif

    CLR_RT_ProtectFromGC::InvokeAll();

    g_CLR_HW_Hardware.PrepareForGC();


    ////////////////////////////////////////////////////////////////////////////
    //
    // Do the recursive marking!
    //
    {
        //
        // Mark all the events, so we keep the related threads/objects alive.
        //
        {
            NANOCLR_FOREACH_NODE(CLR_RT_HeapBlock_Finalizer,fin,g_CLR_RT_ExecutionEngine.m_finalizersPending)
            {
#if defined(NANOCLR_VALIDATE_APPDOMAIN_ISOLATION)
                (void)g_CLR_RT_ExecutionEngine.SetCurrentAppDomain( fin->m_appDomain );
#endif
                CheckSingleBlock( &fin->m_object );
            }
            NANOCLR_FOREACH_NODE_END();
        }

        //
        // Mark all the static fields.
        //

#if defined(NANOCLR_APPDOMAINS)
        AppDomain_Mark();
#endif

        Assembly_Mark();

        //
        // Walk through all the stack frames, marking the objects as we dig down.
        //
        Thread_Mark( g_CLR_RT_ExecutionEngine.m_threadsReady   );
        Thread_Mark( g_CLR_RT_ExecutionEngine.m_threadsWaiting );

#if !defined(NANOCLR_APPDOMAINS)
        CheckSingleBlock_Force( g_CLR_RT_ExecutionEngine.m_globalLock );
#endif

        CheckSingleBlock_Force( g_CLR_RT_ExecutionEngine.m_currentUICulture );


#if defined(NANOCLR_VALIDATE_APPDOMAIN_ISOLATION)                    
        (void)g_CLR_RT_ExecutionEngine.SetCurrentAppDomain( NULL );
#endif //NANOCLR_VALIDATE_APPDOMAIN_ISOLATION

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
        CheckSingleBlock_Force( g_CLR_RT_ExecutionEngine.m_scratchPadArray );
#endif //#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
    
        if(m_fOutOfStackSpaceForGC)
        {
            MarkSlow();
            _ASSERTE(!m_fOutOfStackSpaceForGC);
        }            
    }
    //
    //
    //
    ////////////////////////////////////////////////////////////////////////////


    ////////////////////////////////////////////////////////////////////////////
    //
    // Prepare finalization of objects
    //
    {
        NANOCLR_FOREACH_NODE(CLR_RT_HeapBlock_Finalizer,fin,g_CLR_RT_ExecutionEngine.m_finalizersAlive)
        {
            //
            // If the object is dead, make it alive one last time and put it in the pending finalizers list.
            //
            if(fin->m_object->IsAlive() == false)
            {
#if defined(NANOCLR_VALIDATE_APPDOMAIN_ISOLATION)                    
                (void)g_CLR_RT_ExecutionEngine.SetCurrentAppDomain( fin->m_appDomain );
#endif
                CheckSingleBlock( &fin->m_object );

                g_CLR_RT_ExecutionEngine.m_finalizersPending.LinkAtBack( fin );
            }
        }
        NANOCLR_FOREACH_NODE_END();
    }
    //
    //
    //
    ////////////////////////////////////////////////////////////////////////////
            
    if(m_fOutOfStackSpaceForGC)
    {
        MarkSlow();
    } 

    _ASSERTE(m_markStackList == &markStackList);
    _ASSERTE(m_markStack     == &markStack    );

    while((MarkStack*)m_markStackList->LastValidNode() != m_markStack)
    {
        MarkStack* markStackT = (MarkStack*)m_markStackList->LastValidNode();
        
        markStackT->RemoveFromList();
        
        CLR_RT_Memory::Release( markStackT );
    }

    m_markStackList = NULL;
    m_markStack     = NULL;

#if defined(NANOCLR_VALIDATE_APPDOMAIN_ISOLATION)
    (void)g_CLR_RT_ExecutionEngine.SetCurrentAppDomain( appDomainSav );
#endif
}

void CLR_RT_GarbageCollector::MarkWeak()
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_FOREACH_NODE(CLR_RT_HeapBlock_WeakReference,weak,g_CLR_RT_ExecutionEngine.m_weakReferences)
    {
        if(weak->m_identity.m_flags & CLR_RT_HeapBlock_WeakReference::WR_Restored)
        {
            if(weak->m_targetSerialized)
            {
                weak->MarkAlive();
            }
        }

        if(weak->IsAlive())
        {
            if(weak->m_targetDirect)
            {
                if(weak->m_targetDirect->IsAlive())
                {
                    //
                    // If the direct object is alive, keep the serialized version around too!!
                    //
                    if(weak->m_targetSerialized && weak->m_targetSerialized->IsAlive() == false)
                    {
                        weak->m_targetSerialized->MarkAlive();
                    }
                }
                else
                {
                    weak->m_targetDirect = NULL;
                }
            }
        }
        else
        {
            weak->Unlink();
        }
    }
    NANOCLR_FOREACH_NODE_END();
}

//--//

void CLR_RT_GarbageCollector::Sweep()
{
    NATIVE_PROFILE_CLR_CORE();

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //                                                                                            //
    // At this point all the alive objects have been reached, let's start the clean up.           //
    //                                                                                            //
    ////////////////////////////////////////////////////////////////////////////////////////////////

    NANOCLR_FOREACH_NODE(CLR_RT_HeapBlock_Delegate_List,weak,m_weakDelegates_Reachable)
    {
        CLR_UINT32        num  = weak->m_length;
        CLR_RT_HeapBlock* dlgs = weak->GetDelegates();

        for(;num--; dlgs++)
        {
            CLR_RT_HeapBlock_Delegate* dlg = dlgs->DereferenceDelegate();
            if(dlg)
            {
                CLR_RT_HeapBlock* obj = dlg->m_object.Dereference();
                if(obj && obj->IsAlive() == false)
                {
                    //
                    // Found a dead delegate, kill it!!
                    //
                    dlgs->SetObjectReference( NULL );
                    continue;
                }

                dlg->MarkAlive();
            }
        }
    }
    NANOCLR_FOREACH_NODE_END();

    RecoverEventsFromGC();

    //--//

    //
    // All the dead objects are marked as such, it's time to reclaim them.
    //
    {
        NANOCLR_FOREACH_NODE(CLR_RT_HeapCluster,hc,g_CLR_RT_ExecutionEngine.m_heap)
        {
            hc->RecoverFromGC();
        }
        NANOCLR_FOREACH_NODE_END();
    }
}

//--//

void CLR_RT_GarbageCollector::CheckMemoryPressure()
{
    NATIVE_PROFILE_CLR_CORE();
    if(m_freeBytes > c_memoryThreshold2)
    {
        m_pressureCounter = 0;
    }
    else
    {
        bool fExit = false;

        //
        // Drop all the restored objects that haven't been reclaimed by an application.
        //
        NANOCLR_FOREACH_NODE_BACKWARD(CLR_RT_HeapBlock_WeakReference,weak,g_CLR_RT_ExecutionEngine.m_weakReferences)
        {
            if(weak->m_identity.m_flags & CLR_RT_HeapBlock_WeakReference::WR_Restored)
            {
                if(weak->m_targetSerialized)
                {
                    fExit = true;
                }
            }
        }
        NANOCLR_FOREACH_NODE_BACKWARD_END();

        if(fExit || m_freeBytes > c_memoryThreshold)
        {
            m_pressureCounter = 0;
        }
        else
        {
            m_pressureCounter++;

            if(m_pressureCounter > c_pressureThreshold)
            {
                NANOCLR_FOREACH_NODE_BACKWARD(CLR_RT_HeapBlock_WeakReference,weak,g_CLR_RT_ExecutionEngine.m_weakReferences)
                {
                    if(weak->m_targetSerialized && weak->m_targetDirect == NULL)
                    {
#if !defined(BUILD_RTM)
                        CLR_RT_ReflectionDef_Index val;
                        CLR_RT_TypeDef_Instance    inst;
                        char                       rgBuffer[ 512 ];
                        char*                      szBuffer = rgBuffer;
                        size_t                     iBuffer  = MAXSTRLEN(rgBuffer);

                        CLR_Debug::Printf( "DROPPING OBJECT " );

                        val.InitializeFromHash( weak->m_identity.m_selectorHash );

                        if(inst.InitializeFromReflection( val, NULL ))
                        {
                            g_CLR_RT_TypeSystem.BuildTypeName( inst, szBuffer, iBuffer ); rgBuffer[ MAXSTRLEN(rgBuffer) ] = 0;

                            CLR_Debug::Printf( "%s:%d ", rgBuffer, weak->m_identity.m_id );
                        }

                        CLR_Debug::Printf( " [%d bytes] %s\r\n", weak->m_targetSerialized->m_numOfElements, (weak->m_targetDirect ? "DIRECT" : "") );
#endif

                        break;
                    }
                }
                NANOCLR_FOREACH_NODE_BACKWARD_END();
            }
        }
    }
}

//--//

#if defined(NANOCLR_APPDOMAINS)

void CLR_RT_GarbageCollector::AppDomain_Mark()
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_FOREACH_NODE(CLR_RT_AppDomain,appDomain,g_CLR_RT_ExecutionEngine.m_appDomains)
    {
#if defined(NANOCLR_VALIDATE_APPDOMAIN_ISOLATION)                            
        (void)g_CLR_RT_ExecutionEngine.SetCurrentAppDomain( appDomain );
#endif

        NANOCLR_FOREACH_NODE(CLR_RT_AppDomainAssembly,appDomainAssembly,appDomain->m_appDomainAssemblies)
        {
            CheckMultipleBlocks( appDomainAssembly->m_pStaticFields, appDomainAssembly->m_assembly->m_iStaticFields );            
        }
        NANOCLR_FOREACH_NODE_END();

        CheckSingleBlock_Force( appDomain->m_globalLock           );
        CheckSingleBlock_Force( appDomain->m_strName              );
        CheckSingleBlock_Force( appDomain->m_outOfMemoryException );
    }
    NANOCLR_FOREACH_NODE_END();
}

#endif

void CLR_RT_GarbageCollector::Assembly_Mark()
{
    NATIVE_PROFILE_CLR_CORE();
#if defined(NANOCLR_VALIDATE_APPDOMAIN_ISOLATION)                            
    (void)g_CLR_RT_ExecutionEngine.SetCurrentAppDomain( NULL );
#endif

    NANOCLR_FOREACH_ASSEMBLY(g_CLR_RT_TypeSystem)
    {

#if !defined(NANOCLR_APPDOMAINS)
        CheckMultipleBlocks( pASSM->m_pStaticFields, pASSM->m_iStaticFields );
#endif

        CheckSingleBlock( &pASSM->m_pFile );
    }
    NANOCLR_FOREACH_ASSEMBLY_END();
}

//--//

void CLR_RT_GarbageCollector::Thread_Mark( CLR_RT_Thread* th )
{
    NATIVE_PROFILE_CLR_CORE();
#if defined(NANOCLR_VALIDATE_APPDOMAIN_ISOLATION)
    (void)g_CLR_RT_ExecutionEngine.SetCurrentAppDomain( th->m_dlg->m_appDomain );
#endif
    CheckSingleBlock      ( &th->m_dlg              );

#if defined(NANOCLR_VALIDATE_APPDOMAIN_ISOLATION)
    (void)g_CLR_RT_ExecutionEngine.SetCurrentAppDomain( th->CurrentAppDomain() );
#endif    
    CheckSingleBlock_Force( &th->m_currentException );

    for(int i=0; i<th->m_nestedExceptionsPos; i++)
    {
        CLR_RT_Thread::UnwindStack& us = th->m_nestedExceptions[ i ];

#if defined(NANOCLR_VALIDATE_APPDOMAIN_ISOLATION)                    
        (void)g_CLR_RT_ExecutionEngine.SetCurrentAppDomain( us.m_stack->m_appDomain );
#endif

        CheckSingleBlock_Force( us.m_exception );
    }

    //
    // Mark all the objects on the stack frames.
    //
    NANOCLR_FOREACH_NODE(CLR_RT_StackFrame,stack,th->m_stackFrames)
    {
#if defined(NANOCLR_VALIDATE_APPDOMAIN_ISOLATION)                    
        (void)g_CLR_RT_ExecutionEngine.SetCurrentAppDomain( stack->m_appDomain );
#endif
#ifndef CLR_NO_IL_INLINE
        if(stack->m_inlineFrame)
        {
            CheckMultipleBlocks( stack->m_inlineFrame->m_frame.m_args     , stack->m_inlineFrame->m_frame.m_call.m_target->numArgs   );
            CheckMultipleBlocks( stack->m_inlineFrame->m_frame.m_locals   , stack->m_inlineFrame->m_frame.m_call.m_target->numLocals );
            CheckMultipleBlocks( stack->m_inlineFrame->m_frame.m_evalStack, (int)(stack->m_inlineFrame->m_frame.m_evalPos - stack->m_inlineFrame->m_frame.m_evalStack));
        }
#endif
        CheckMultipleBlocks( stack->m_arguments, stack->m_call.m_target->numArgs   );
        CheckMultipleBlocks( stack->m_locals   , stack->m_call.m_target->numLocals );
        CheckMultipleBlocks( stack->m_evalStack, stack->TopValuePosition()         );
    }
    NANOCLR_FOREACH_NODE_END();

    //
    // Mark locked objects.
    //
    NANOCLR_FOREACH_NODE(CLR_RT_HeapBlock_Lock,lock,th->m_locks)
    {
#if defined(NANOCLR_VALIDATE_APPDOMAIN_ISOLATION)                    
        (void)g_CLR_RT_ExecutionEngine.SetCurrentAppDomain( lock->m_appDomain );
#endif
        CheckSingleBlock_Force( &lock->m_resource );
    }
    NANOCLR_FOREACH_NODE_END();

    //
    // Mark the objects this thread is waiting for.
    //
#if defined(NANOCLR_VALIDATE_APPDOMAIN_ISOLATION)                    
    (void)g_CLR_RT_ExecutionEngine.SetCurrentAppDomain( th->CurrentAppDomain() );
#endif

    {
        CLR_RT_HeapBlock_WaitForObject* wait = th->m_waitForObject;

        if(wait)
        {
            CheckMultipleBlocks( wait->GetWaitForObjects(), wait->m_cObjects );
        }
    }
}

void CLR_RT_GarbageCollector::Thread_Mark( CLR_RT_DblLinkedList& threads )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_FOREACH_NODE(CLR_RT_Thread,th,threads)
    {
        Thread_Mark( th );
    }
    NANOCLR_FOREACH_NODE_END();
}

//--//

void CLR_RT_GarbageCollector::RecoverEventsFromGC()
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_HeapBlock_EndPoint::HandlerMethod_RecoverFromGC(); 
    CLR_RT_HeapBlock_NativeEventDispatcher::HandlerMethod_RecoverFromGC();

    NANOCLR_FOREACH_NODE(CLR_RT_HeapBlock_Timer,timer,g_CLR_RT_ExecutionEngine.m_timers)
    {
        timer->RecoverFromGC();
    }
    NANOCLR_FOREACH_NODE_END();

    //--//

    NANOCLR_FOREACH_NODE(CLR_RT_Thread,th,g_CLR_RT_ExecutionEngine.m_threadsReady)
    {
        th->RecoverFromGC();
    }
    NANOCLR_FOREACH_NODE_END();

    NANOCLR_FOREACH_NODE(CLR_RT_Thread,th,g_CLR_RT_ExecutionEngine.m_threadsWaiting)
    {
        th->RecoverFromGC();
    }
    NANOCLR_FOREACH_NODE_END();

    NANOCLR_FOREACH_NODE(CLR_RT_Thread,th,g_CLR_RT_ExecutionEngine.m_threadsZombie)
    {
        th->RecoverFromGC();
        th->ReleaseWhenDeadEx(); // After the GC has happened, we have to check if any zombie thread can be reclaimed.
    }
    NANOCLR_FOREACH_NODE_END();

#if defined(NANOCLR_APPDOMAINS)
    NANOCLR_FOREACH_NODE(CLR_RT_AppDomain,appDomain,g_CLR_RT_ExecutionEngine.m_appDomains)
    {
        appDomain->RecoverFromGC();
    }
    NANOCLR_FOREACH_NODE_END();
#endif
}

//--//

CLR_UINT32 CLR_RT_GarbageCollector::Heap_ComputeAliveVsDeadRatio()
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_UINT32 totalBytes = 0;
    CLR_UINT32 freeBlocks = 0;

    NANOCLR_FOREACH_NODE(CLR_RT_HeapCluster,hc,g_CLR_RT_ExecutionEngine.m_heap)
    {
        CLR_RT_HeapBlock_Node* ptr = hc->m_payloadStart;
        CLR_RT_HeapBlock_Node* end = hc->m_payloadEnd;

        totalBytes += (CLR_UINT32)((CLR_UINT8*)end - (CLR_UINT8*)ptr);

        NANOCLR_FOREACH_NODE_BACKWARD(CLR_RT_HeapBlock_Node,ptrFree,hc->m_freeList)
        {
            freeBlocks += ptrFree->DataSize();
        }
        NANOCLR_FOREACH_NODE_BACKWARD_END();
    }
    NANOCLR_FOREACH_NODE_END();

    m_totalBytes = totalBytes;
    m_freeBytes  = freeBlocks * sizeof(CLR_RT_HeapBlock);

    return m_freeBytes;
}

