//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"

HRESULT Library_corlib_native_System_Reflection_ConstructorInfo::GetCustomAttributes___SZARRAY_OBJECT__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Reflection_ConstructorInfo::Invoke___OBJECT__SZARRAY_OBJECT(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *thisRef = stack.ThisRef().Dereference();
    CLR_RT_MethodDef_Instance md{};
    CLR_RT_HeapBlock_Array *pArray = stack.Arg1().DereferenceArray();
    CLR_RT_HeapBlock *args = nullptr;
    int ArgumentsCount = 0;

    if (md.InitializeFromIndex(thisRef->ReflectionDataConst().data.method) == false)
        NANOCLR_SET_AND_LEAVE(CLR_E_NULL_REFERENCE);

    if (stack.m_customState == 0)
    {
        stack.m_customState = 1;

        if (pArray)
        {
            args = (CLR_RT_HeapBlock *)pArray->GetFirstElement();
            ArgumentsCount = pArray->m_numOfElements;
        }

        NANOCLR_CHECK_HRESULT(stack.MakeCall(md, nullptr, args, ArgumentsCount));
    }

    NANOCLR_NOCLEANUP();
}
