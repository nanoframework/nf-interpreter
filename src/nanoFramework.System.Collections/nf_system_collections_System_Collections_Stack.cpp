//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nf_system_collections.h"


HRESULT Library_nf_system_collections_System_Collections_Stack::Clear___VOID( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Stack* pThis = (CLR_RT_HeapBlock_Stack*)stack.This(); FAULT_ON_NULL(pThis);

    NANOCLR_SET_AND_LEAVE(pThis->Clear());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_system_collections_System_Collections_Stack::Peek___OBJECT( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Stack* pThis = (CLR_RT_HeapBlock_Stack*)stack.This(); FAULT_ON_NULL(pThis);
    CLR_RT_HeapBlock*       value;

    NANOCLR_CHECK_HRESULT(pThis->Peek( value ));

    stack.SetResult_Object( value );

    NANOCLR_NOCLEANUP();

}

HRESULT Library_nf_system_collections_System_Collections_Stack::Pop___OBJECT( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Stack* pThis = (CLR_RT_HeapBlock_Stack*)stack.This(); FAULT_ON_NULL(pThis);
    CLR_RT_HeapBlock*       value;

    NANOCLR_CHECK_HRESULT(pThis->Pop( value ));

    stack.SetResult_Object( value );

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_system_collections_System_Collections_Stack::Push___VOID__OBJECT( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Stack* pThis = (CLR_RT_HeapBlock_Stack*)stack.This(); FAULT_ON_NULL(pThis);

    NANOCLR_SET_AND_LEAVE(pThis->Push( stack.Arg1().Dereference() ));

    NANOCLR_NOCLEANUP();
}
