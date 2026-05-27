//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"


HRESULT Library_corlib_native_System_MulticastDelegate::op_Equality___STATIC__BOOLEAN__SystemMulticastDelegate__SystemMulticastDelegate( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(Library_corlib_native_System_Delegate::op_Equality___STATIC__BOOLEAN__SystemDelegate__SystemDelegate( stack ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_MulticastDelegate::op_Inequality___STATIC__BOOLEAN__SystemMulticastDelegate__SystemMulticastDelegate( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(Library_corlib_native_System_Delegate::op_Inequality___STATIC__BOOLEAN__SystemDelegate__SystemDelegate( stack ));

    NANOCLR_NOCLEANUP();
}
