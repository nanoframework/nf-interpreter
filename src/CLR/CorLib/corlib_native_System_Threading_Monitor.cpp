//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"


HRESULT Library_corlib_native_System_Threading_Monitor::Enter___STATIC__VOID__OBJECT( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_StackFrame* caller = NULL;
        
    if(stack.Arg0().Dereference() == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_ARGUMENT_NULL);
    }
    
    switch(stack.m_customState)
    {
    case 0:
        {
            caller = stack.Caller(); FAULT_ON_NULL(caller); // We need to set the constraint on the caller, not on us...

            hr = g_CLR_RT_ExecutionEngine.LockObject( stack.Arg0(), caller->m_owningSubThread, TIMEOUT_INFINITE, false );
            if(hr == CLR_E_THREAD_WAITING)
            {
                stack.m_customState = 1;
            }

            NANOCLR_LEAVE();
        }
        break;

    case 1:
        NANOCLR_SET_AND_LEAVE(S_OK);
        break;

    default:
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Threading_Monitor::Exit___STATIC__VOID__OBJECT( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();
    
    CLR_RT_StackFrame* caller = NULL;

    if(stack.Arg0().Dereference() == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_ARGUMENT_NULL);
    }
    
    caller = stack.Caller(); FAULT_ON_NULL(caller); // We need to set the constraint on the caller, not on us...

    NANOCLR_SET_AND_LEAVE(g_CLR_RT_ExecutionEngine.UnlockObject( stack.Arg0(), caller->m_owningSubThread ));

    NANOCLR_NOCLEANUP();
}
