//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"

HRESULT Library_corlib_native_System_Enum::HasFlag___BOOLEAN__SystemEnum(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_TypeDescriptor enumTypeDesc;
    CLR_RT_TypeDescriptor flagTypeDesc;
    CLR_RT_HeapBlock *pEnum;
    CLR_RT_HeapBlock *enumValue;
    CLR_RT_HeapBlock *pFlag;
    CLR_RT_HeapBlock *flagValue;

    // get Enum value from 'This'
    pEnum = &(stack.ThisRef());
    enumValue = pEnum;

    // need to unbox Enum to get value
    NANOCLR_CHECK_HRESULT(enumTypeDesc.InitializeFromObject(*enumValue));
    NANOCLR_CHECK_HRESULT(enumValue->PerformUnboxing(enumTypeDesc.m_handlerCls));

    // get Enum value from 'flag' arg
    pFlag = &(stack.Arg1());
    flagValue = pFlag;

    // need to unbox Enum to get value
    NANOCLR_CHECK_HRESULT(flagTypeDesc.InitializeFromObject(*enumValue));
    NANOCLR_CHECK_HRESULT(flagValue->PerformUnboxing(flagTypeDesc.m_handlerCls));

    // check if both types are equivalent
    if (enumTypeDesc.m_handlerCls.Type() != flagTypeDesc.m_handlerCls.Type())
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    stack.SetResult_Boolean(enumValue->NumericByRef().s4 & flagValue->NumericByRef().s4);

    NANOCLR_NOCLEANUP();
}
