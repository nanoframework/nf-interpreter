//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"


HRESULT Library_corlib_native_System_WeakReference::get_IsAlive___BOOLEAN( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_WeakReference* weak;

    weak = (CLR_RT_HeapBlock_WeakReference*)stack.This(); FAULT_ON_NULL(weak);

    stack.SetResult_Boolean( weak->m_targetDirect != NULL );

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_WeakReference::get_Target___OBJECT( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_WeakReference* weak;

    weak = (CLR_RT_HeapBlock_WeakReference*)stack.This(); FAULT_ON_NULL(weak);

    NANOCLR_SET_AND_LEAVE(weak->GetTarget( stack.PushValue() ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_WeakReference::set_Target___VOID__OBJECT( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_WeakReference* weak;

    weak = (CLR_RT_HeapBlock_WeakReference*)stack.This(); FAULT_ON_NULL(weak);

    NANOCLR_SET_AND_LEAVE(weak->SetTarget( stack.Arg1() ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_WeakReference::_ctor___VOID__OBJECT( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_WeakReference* weak = (CLR_RT_HeapBlock_WeakReference*)stack.This(); FAULT_ON_NULL(weak);

    NANOCLR_SET_AND_LEAVE(weak->SetTarget( stack.Arg1() ));

    NANOCLR_NOCLEANUP();
}
