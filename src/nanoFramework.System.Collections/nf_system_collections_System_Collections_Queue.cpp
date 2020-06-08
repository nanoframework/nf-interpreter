//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nf_system_collections.h"


HRESULT Library_nf_system_collections_System_Collections_Queue::CopyTo___VOID__SystemArray__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Queue* pThis;
    CLR_RT_HeapBlock_Array* array;
    CLR_INT32               index;

    pThis = (CLR_RT_HeapBlock_Queue*)stack.This(); FAULT_ON_NULL(pThis);
    array = stack.Arg1().DereferenceArray(); FAULT_ON_NULL_ARG(array);
    index = stack.Arg2().NumericByRef().s4;

    NANOCLR_SET_AND_LEAVE(pThis->CopyTo( array, index ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_system_collections_System_Collections_Queue::Clear___VOID( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Queue* pThis = (CLR_RT_HeapBlock_Queue*)stack.This(); FAULT_ON_NULL(pThis);

    NANOCLR_SET_AND_LEAVE(pThis->Clear());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_system_collections_System_Collections_Queue::Enqueue___VOID__OBJECT( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Queue* pThis = (CLR_RT_HeapBlock_Queue*)stack.This(); FAULT_ON_NULL(pThis);

    NANOCLR_SET_AND_LEAVE(pThis->Enqueue( stack.Arg1().Dereference() ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_system_collections_System_Collections_Queue::Dequeue___OBJECT( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Queue* pThis = (CLR_RT_HeapBlock_Queue*)stack.This(); FAULT_ON_NULL(pThis);
    CLR_RT_HeapBlock*       value;

    NANOCLR_CHECK_HRESULT(pThis->Dequeue( value ));

    stack.SetResult_Object( value );

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_system_collections_System_Collections_Queue::Peek___OBJECT( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Queue* pThis = (CLR_RT_HeapBlock_Queue*)stack.This(); FAULT_ON_NULL(pThis);
    CLR_RT_HeapBlock*       value;

    NANOCLR_CHECK_HRESULT(pThis->Peek( value ));

    stack.SetResult_Object( value );

    NANOCLR_NOCLEANUP();
}
