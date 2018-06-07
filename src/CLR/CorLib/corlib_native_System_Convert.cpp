//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "corlib_native.h"

HRESULT Library_corlib_native_System_Convert::NativeToInt64___STATIC__I8__STRING__BOOLEAN__I8__I8__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_INT64 _result = 0;
        CLR_INT64 _zero = 0;

		const char * _string = stack.Arg0().RecoverString();
        bool _signed = (bool)stack.Arg1().NumericByRef().u1;
        long long _minValue = stack.Arg2().NumericByRef().s8;
        long long _maxValue = stack.Arg3().NumericByRef().s8;
        signed int radix = stack.Arg4().NumericByRef().s4;

        _result = _signed ? strtoll(_string, nullptr, radix) : (long long) strtoull(_string, nullptr, radix);

        stack.SetResult_I8 ((_result > _maxValue || _result < _minValue) ? _zero : _result);
	}
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_corlib_native_System_Convert::NativeToDouble___STATIC__R8__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
		const char * _string = stack.Arg0().RecoverString();

        stack.SetResult_R8 (strtod(_string, nullptr));
	}
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_corlib_native_System_Convert::ToBase64String___STATIC__STRING__SZARRAY_U1__I4__I4__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Convert::FromBase64CharArray___STATIC__SZARRAY_U1__SZARRAY_CHAR__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}
