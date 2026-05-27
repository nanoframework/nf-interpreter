//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"


HRESULT Library_corlib_native_System_Diagnostics_Debugger::get_IsAttached___STATIC__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

    stack.SetResult_Boolean( CLR_EE_DBG_IS( SourceLevelDebugging ));     

#else

    stack.SetResult_Boolean( false );

#endif

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_corlib_native_System_Diagnostics_Debugger::Break___STATIC__VOID( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();
    
#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

    if(stack.m_customState == 0)
    {
        stack.m_customState++;
        g_CLR_RT_ExecutionEngine.Breakpoint_StackFrame_Break( &stack );    
        
        hr = CLR_E_RESCHEDULE;  NANOCLR_LEAVE();
    }

    NANOCLR_NOCLEANUP();
    
#else
    (void)stack;
    NANOCLR_NOCLEANUP_NOLABEL();

#endif

}
