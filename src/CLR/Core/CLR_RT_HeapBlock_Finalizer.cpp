//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "Core.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT CLR_RT_HeapBlock_Finalizer::CreateInstance( CLR_RT_HeapBlock* object, const CLR_RT_TypeDef_Instance& inst )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Instance inst2 = inst;

    do
    {
        if(inst2.m_target->flags & CLR_RECORD_TYPEDEF::TD_HasFinalizer)
        {
            int                         pos =       inst2.m_target->methods_First;
            int                         end = pos + inst2.m_target->vMethods_Num + inst2.m_target->iMethods_Num;
            const CLR_RECORD_METHODDEF* md  = inst2.m_assm->GetMethodDef( pos );

            for(; pos<end; pos++, md++)
            {
                if(md->flags & CLR_RECORD_METHODDEF::MD_Finalizer) break;
            }

            if(pos < end)
            {
                CLR_RT_HeapBlock_Finalizer* fin = EVENTCACHE_EXTRACT_NODE_NOALLOC(g_CLR_RT_EventCache,CLR_RT_HeapBlock_Finalizer,DATATYPE_FINALIZER_HEAD); CHECK_ALLOCATION(fin);

                fin->m_object = object;

                fin->m_md.Set( inst2.Assembly(), pos );

#if defined(NANOCLR_APPDOMAINS)
                fin->m_appDomain = g_CLR_RT_ExecutionEngine.GetCurrentAppDomain();
#endif

                g_CLR_RT_ExecutionEngine.m_finalizersAlive.LinkAtBack( fin );

                break;
            }
        }
    }
    while(inst2.SwitchToParent());

    NANOCLR_NOCLEANUP();
}

void CLR_RT_HeapBlock_Finalizer::Relocate()
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RT_GarbageCollector::Heap_Relocate( (void**)&m_object );
}

void CLR_RT_HeapBlock_Finalizer::SuppressFinalize( CLR_RT_HeapBlock* object )
{
    NATIVE_PROFILE_CLR_CORE();
    RemoveInstance( object, g_CLR_RT_ExecutionEngine.m_finalizersAlive   );
    RemoveInstance( object, g_CLR_RT_ExecutionEngine.m_finalizersPending );
}

void CLR_RT_HeapBlock_Finalizer::RemoveInstance( CLR_RT_HeapBlock* object, CLR_RT_DblLinkedList& lst )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_FOREACH_NODE(CLR_RT_HeapBlock_Finalizer,fin,lst)
    {
        if(fin->m_object == object)
        {
            g_CLR_RT_EventCache.Append_Node( fin );

            break;
        }
    }
    NANOCLR_FOREACH_NODE_END();
}

