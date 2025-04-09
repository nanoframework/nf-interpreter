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

    if (hr != S_OK)
    {
        CLR_RT_HeapBlock &res = stack.m_owningThread->m_currentException;

        CLR_RT_TypeDef_Index serializationExceptionTypeDef;

        // find <SerializationException> type definition, don't bother checking the result as it exists for sure
        g_CLR_RT_TypeSystem.FindTypeDef(
            "SerializationException",
            "System.Runtime.Serialization",
            serializationExceptionTypeDef);

        if ((Library_corlib_native_System_Exception::CreateInstance(res, serializationExceptionTypeDef, hr, &stack)) ==
            S_OK)
        {
            if (hash != 0)
            {
                // if there is a hash, set the Type field ONLY IF the type is found
                CLR_RT_ReflectionDef_Index idx{};
                CLR_RT_TypeDef_Instance inst{};

                idx.InitializeFromHash(hash);

                if (inst.InitializeFromReflection(idx, NULL))
                {
                    res.Dereference()
                        [Library_nf_system_runtime_serialization_System_Runtime_Serialization_SerializationException::
                             FIELD__Type]
                            .SetReflection(idx);
                }
            }
        }

        hr = CLR_E_PROCESS_EXCEPTION;
    }

    NANOCLR_CLEANUP_END();
}
