//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_system_runtime_serialization.h"

HRESULT Library_nf_system_runtime_serialization_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter::
    Serialize___STATIC__SZARRAY_U1__OBJECT(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(CLR_RT_BinaryFormatter::Serialize(stack.PushValue(), stack.Arg0()));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_system_runtime_serialization_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter::
    Deserialize___STATIC__OBJECT__SZARRAY_U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_UINT32 hash = 0;

    NANOCLR_SET_AND_LEAVE(CLR_RT_BinaryFormatter::Deserialize(stack.PushValue(), stack.Arg0(), &hash, 0));

    NANOCLR_CLEANUP();

    if (hr == CLR_E_WRONG_TYPE && hash != 0)
    {
        CLR_RT_HeapBlock &res = stack.m_owningThread->m_currentException;

        CLR_RT_TypeDef_Index serializationExceptionTypeDef;

        // find <SerializationException> type definition, don't bother checking the result as it exists for sure
        g_CLR_RT_TypeSystem.FindTypeDef(
            "SerializationException",
            "System.Runtime.Serialization",
            serializationExceptionTypeDef);

        if ((Library_corlib_native_System_Exception::CreateInstance(
                res,
                serializationExceptionTypeDef,
                CLR_E_UNKNOWN_TYPE,
                &stack)) == S_OK)
        {
            CLR_RT_ReflectionDef_Index idx;

            idx.InitializeFromHash(hash);

            res.Dereference()
                [Library_nf_system_runtime_serialization_System_Runtime_Serialization_SerializationException::
                     FIELD__Type]
                    .SetReflection(idx);
        }

        hr = CLR_E_PROCESS_EXCEPTION;
    }

    NANOCLR_CLEANUP_END();
}
