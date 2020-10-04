//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


#include "nf_rt_native.h"



HRESULT Library_nf_rt_native_nanoFramework_Runtime_Native_GC::Run___STATIC__U4__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();

    NANOCLR_HEADER();

#if defined(NANOCLR_GC_VERBOSE)
    if(s_CLR_RT_fTrace_GC >= c_CLR_RT_Trace_Info)
    {
        CLR_Debug::Printf( "    Memory: Debug.GC.\r\n" );
    }
#endif

    stack.SetResult_I4( g_CLR_RT_ExecutionEngine.PerformGarbageCollection() );

    if(stack.Arg0().NumericByRefConst().u1)
    {
        //
        // Decrement the number of GC, otherwise the outer loop may request another compaction.
        //
        g_CLR_RT_GarbageCollector.m_numberOfGarbageCollections--;

        g_CLR_RT_ExecutionEngine.PerformHeapCompaction();
    }

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_rt_native_nanoFramework_Runtime_Native_GC::EnableGCMessages___STATIC__VOID__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();

    NANOCLR_HEADER();

  #if defined(NANOCLR_TRACE_MEMORY_STATS)
    s_CLR_RT_fTrace_MemoryStats = stack.Arg0().NumericByRefConst().u1 != 0 ? c_CLR_RT_Trace_Info : c_CLR_RT_Trace_None;
  #else
    (void)stack;
  #endif

    NANOCLR_NOCLEANUP_NOLABEL();
}
