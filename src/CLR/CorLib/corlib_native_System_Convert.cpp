//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"
#include "stdlib.h"

HRESULT Library_corlib_native_System_Convert::NativeToInt32___STATIC__I4__STRING__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    bool fromBase = stack.Arg1().NumericByRef().s4 != 0;
    CLR_RT_HeapBlock& pArgs = stack.Arg0();
    const char * szName = pArgs.RecoverString();
    
	CLR_INT32 ret = (CLR_INT32) strtol(szName, nullptr, fromBase);

    stack.SetResult(ret, DATATYPE_I4 );

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Convert::NativeToInt64___STATIC__I8__STRING__BOOLEAN__I8__I8( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    CLR_INT64 minValue  = stack.Arg2().NumericByRef().s8;
    CLR_INT64 maxValue = stack.Arg3().NumericByRef().s8;
    bool isSigned = stack.Arg1().NumericByRef().u1 != 0;
    CLR_RT_HeapBlock& pArgs = stack.Arg0();
    const char * szName = pArgs.RecoverString();
    
    int base = (szName[0] == '0' && (szName[1] == 'X' || szName[1] == 'x' ))  ? 16 : 10;
	CLR_INT64 ret = isSigned ? (CLR_INT64) strtol(szName, nullptr, base) : (CLR_INT64) strtoul(szName, nullptr, base);
	if (ret < minValue || ret > maxValue) return 0;

    stack.SetResult_I8(ret);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Convert::NativeToDouble___STATIC__R8__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock& pArgs = stack.Arg0();
    const char * szName = pArgs.RecoverString();
    double ret = strtod(szName, nullptr);

	stack.SetResult_R8(ret);

    NANOCLR_NOCLEANUP();
}