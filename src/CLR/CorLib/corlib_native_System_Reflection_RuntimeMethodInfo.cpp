//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"


HRESULT Library_corlib_native_System_Reflection_RuntimeMethodInfo::get_ReturnType___SystemType( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_MethodDef_Instance md;
    CLR_RT_SignatureParser    parser;
    CLR_RT_TypeDescriptor     desc;
    CLR_RT_HeapBlock*         hbMeth = stack.Arg0().Dereference();

    NANOCLR_CHECK_HRESULT(Library_corlib_native_System_Reflection_MethodBase::GetMethodDescriptor( stack, *hbMeth, md ));

    parser.Initialize_MethodSignature( md.m_assm, md.m_target );

    NANOCLR_CHECK_HRESULT(desc.InitializeFromSignatureParser( parser ));

    {
        CLR_RT_HeapBlock& top = stack.PushValue();
        CLR_RT_HeapBlock* hbObj;
        
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.m_TypeStatic));
        hbObj = top.Dereference();
        hbObj->SetReflection( desc.m_reflex );
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Reflection_RuntimeMethodInfo::GetCustomAttributesNative___SZARRAY_OBJECT__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock*           returnArray = NULL;
    CLR_RT_MethodDef_Instance   methodDefinition;
    CLR_RT_HeapBlock*           callerMethod;
    int count = 0;

    // put the return array on the stack
    CLR_RT_HeapBlock& top = stack.PushValueAndClear();

    // get the caller method
    callerMethod = stack.Arg0().Dereference();

    NANOCLR_CHECK_HRESULT(Library_corlib_native_System_Reflection_MethodBase::GetMethodDescriptor( stack, *callerMethod, methodDefinition ));

    // setup attribute enumerator
    CLR_RT_AttributeEnumerator attributeEnumerator;
    attributeEnumerator.Initialize( methodDefinition );

    // the return array has two positions for each attribute:
    // 1st: the attribute type
    // 2nd: the constructor parameters or NULL, if the attribute has no constructor

    // 1st pass: count attributes
    do
    {
        // move to the next attribute in the collection, if any
        if(attributeEnumerator.Advance())
        {
            count++;
        }
        else
        {
            // done sweeping attributes

            // create the result array
            // (2 positions for each attribute)
            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( top, ( count * 2 ), g_CLR_RT_WellKnownTypes.m_Object ));

            // use this to skip to the 2nd pass if no attribute was found
            if (count == 0)
            {
                break;
            }

            // get the pointer to the first element
            returnArray = (CLR_RT_HeapBlock*)top.DereferenceArray()->GetFirstElement();

            // reset attribute enumerator
            attributeEnumerator.Initialize( methodDefinition );

            break;
        }
    } 
    while(true);

    // 2nd pass: fill the array with the attributes types, if any
    NANOCLR_CHECK_HRESULT( Library_corlib_native_System_RuntimeType::GetCustomAttributes( attributeEnumerator, returnArray, count ) );

    NANOCLR_NOCLEANUP();
}
