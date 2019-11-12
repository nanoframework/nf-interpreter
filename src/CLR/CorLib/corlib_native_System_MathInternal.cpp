//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"

HRESULT Library_corlib_native_System_MathInternal::Abs___STATIC__I4__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_INT32 d = stack.Arg0().NumericByRefConst().s4;
    CLR_INT32 res = abs( d );

    stack.SetResult_I4( res );

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_corlib_native_System_MathInternal::Max___STATIC__I4__I4__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_INT32 x = stack.Arg0().NumericByRefConst().s4;
    CLR_INT32 y = stack.Arg1().NumericByRefConst().s4;
    CLR_INT32 res = x >= y ? x : y;

    stack.SetResult_I4( res );

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_corlib_native_System_MathInternal::Min___STATIC__I4__I4__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_INT32 x = stack.Arg0().NumericByRefConst().s4;
    CLR_INT32 y = stack.Arg1().NumericByRefConst().s4;
    CLR_INT32 res = x <= y ? x : y;

    stack.SetResult_I4( res );

    NANOCLR_NOCLEANUP_NOLABEL();
}

