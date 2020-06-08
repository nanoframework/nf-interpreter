//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"
#include "nanoPAL_NativeDouble.h"

#if !defined(NANOCLR_EMULATED_FLOATINGPOINT)

HRESULT Library_corlib_native_System_Double::CompareTo___STATIC__I4__R8__R8( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    double d = stack.Arg0().NumericByRefConst().r8;
    double val = stack.Arg1().NumericByRefConst().r8;
    CLR_INT32 res = System::Double::CompareTo( d, val );

    stack.PushValue().SetInteger( res );

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_corlib_native_System_Double::IsInfinity___STATIC__BOOLEAN__R8( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    double d = stack.Arg0().NumericByRefConst().r8;
    bool res = System::Double::IsInfinity( d );

    stack.SetResult_Boolean( res );    

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_corlib_native_System_Double::IsNaN___STATIC__BOOLEAN__R8( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    double d = stack.Arg0().NumericByRefConst().r8;
    bool res = System::Double::IsNaN( d );

    stack.SetResult_Boolean( res );    

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_corlib_native_System_Double::IsNegativeInfinity___STATIC__BOOLEAN__R8( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    double d = stack.Arg0().NumericByRefConst().r8;
    bool res = System::Double::IsNegativeInfinity( d );

    stack.SetResult_Boolean( res );    

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_corlib_native_System_Double::IsPositiveInfinity___STATIC__BOOLEAN__R8( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    double d = stack.Arg0().NumericByRefConst().r8;
    bool res = System::Double::IsPositiveInfinity( d );

    stack.SetResult_Boolean( res );    

    NANOCLR_NOCLEANUP_NOLABEL();
}

#else

HRESULT Library_corlib_native_System_Double::CompareTo___STATIC__I4__R8__R8( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_INT64 left  = stack.Arg0().NumericByRef().s8;
    CLR_INT64 right = stack.Arg1().NumericByRef().s8;

    stack.SetResult_I4((left < right) ? -1 : (left > right) ? 1 : 0);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_corlib_native_System_Double::IsInfinity___STATIC__BOOLEAN__R8( CLR_RT_StackFrame& stack )
{
    stack.NotImplementedStub();
}

HRESULT Library_corlib_native_System_Double::IsNaN___STATIC__BOOLEAN__R8( CLR_RT_StackFrame& stack )
{
    stack.NotImplementedStub();
}

HRESULT Library_corlib_native_System_Double::IsNegativeInfinity___STATIC__BOOLEAN__R8( CLR_RT_StackFrame& stack )
{
    stack.NotImplementedStub();
}

HRESULT Library_corlib_native_System_Double::IsPositiveInfinity___STATIC__BOOLEAN__R8( CLR_RT_StackFrame& stack )
{
    stack.NotImplementedStub();
}

#endif
