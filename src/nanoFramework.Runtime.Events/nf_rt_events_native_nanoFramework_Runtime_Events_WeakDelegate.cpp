//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


#include "nf_rt_events_native.h"


HRESULT Library_nf_rt_events_native_nanoFramework_Runtime_Events_WeakDelegate::Combine___STATIC__SystemDelegate__SystemDelegate__SystemDelegate( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();

    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(CLR_RT_HeapBlock_Delegate_List::Combine( stack.PushValue(), stack.Arg0(), stack.Arg1(), true ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_rt_events_native_nanoFramework_Runtime_Events_WeakDelegate::Remove___STATIC__SystemDelegate__SystemDelegate__SystemDelegate( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();

    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(CLR_RT_HeapBlock_Delegate_List::Remove( stack.PushValue(), stack.Arg0(), stack.Arg1() ));

    NANOCLR_NOCLEANUP();
}
