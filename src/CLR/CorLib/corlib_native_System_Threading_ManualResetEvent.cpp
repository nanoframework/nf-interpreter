//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"


HRESULT Library_corlib_native_System_Threading_ManualResetEvent::_ctor___VOID__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock* pThis = stack.This();

    if(stack.Arg1().NumericByRef().s4)
    {
        pThis->SetFlags( CLR_RT_HeapBlock::HB_Signaled );
    }

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_corlib_native_System_Threading_ManualResetEvent::Reset___BOOLEAN( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    Library_corlib_native_System_Threading_WaitHandle::Reset( stack );

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_corlib_native_System_Threading_ManualResetEvent::Set___BOOLEAN( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    Library_corlib_native_System_Threading_WaitHandle::Set( stack );

    NANOCLR_NOCLEANUP_NOLABEL();    
}
