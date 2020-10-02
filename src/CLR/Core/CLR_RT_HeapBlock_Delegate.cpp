//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "Core.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT CLR_RT_HeapBlock_Delegate::CreateInstance( CLR_RT_HeapBlock& reference, const CLR_RT_MethodDef_Index& ftn, CLR_RT_StackFrame* call )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    reference.SetObjectReference( NULL );

    CLR_UINT32 length = 0;

  #if defined(NANOCLR_DELEGATE_PRESERVE_STACK)    
    if(call)
    {
        NANOCLR_FOREACH_NODE_BACKWARD__DIRECT(CLR_RT_StackFrame,ptr,call)
        {
            length++;
        }
        NANOCLR_FOREACH_NODE_BACKWARD_END();
    }

    //
    // Limit depth to three callers.
    //
    if(length > 3) length = 3;
  #else
    (void)call;
  #endif

    CLR_UINT32 totLength = (CLR_UINT32)(sizeof(CLR_RT_HeapBlock_Delegate) + length * sizeof(CLR_RT_MethodDef_Index));

    CLR_RT_HeapBlock_Delegate* dlg = (CLR_RT_HeapBlock_Delegate*)g_CLR_RT_ExecutionEngine.ExtractHeapBytesForObjects( DATATYPE_DELEGATE_HEAD, 0, totLength ); CHECK_ALLOCATION(dlg);

    reference.SetObjectReference( dlg );

    dlg->ClearData();
    dlg->m_cls.Clear();
    dlg->m_ftn              = ftn;
#if defined(NANOCLR_DELEGATE_PRESERVE_STACK)
    dlg->m_numOfStackFrames = length;
#endif
    
    dlg->m_object.SetObjectReference( NULL );

#if defined(NANOCLR_APPDOMAINS)
    dlg->m_appDomain = g_CLR_RT_ExecutionEngine.GetCurrentAppDomain();
#endif

#if defined(NANOCLR_DELEGATE_PRESERVE_STACK)
    if(call)
    {
        CLR_RT_MethodDef_Index* callStack = dlg->GetStackFrames();

        NANOCLR_FOREACH_NODE_BACKWARD__DIRECT(CLR_RT_StackFrame,ptr,call)
        {
            if(length-- == 0) break;

            *callStack++ = ptr->m_call;
        }
        NANOCLR_FOREACH_NODE_BACKWARD_END();
    }
#endif

    NANOCLR_NOCLEANUP();
}

void CLR_RT_HeapBlock_Delegate::Relocate()
{
    NATIVE_PROFILE_CLR_CORE();
    m_object.Relocate__HeapBlock();
}

