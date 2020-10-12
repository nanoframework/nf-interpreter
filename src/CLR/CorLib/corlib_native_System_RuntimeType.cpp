//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"


HRESULT Library_corlib_native_System_RuntimeType::get_Assembly___SystemReflectionAssembly( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Instance td;
    CLR_RT_HeapBlock*       hbType = stack.Arg0().Dereference();
    
    NANOCLR_CHECK_HRESULT(GetTypeDescriptor( *hbType, td, NULL ));

    {
        CLR_RT_Assembly_Index idx; idx.Set( td.Assembly() );
        CLR_RT_HeapBlock&     top = stack.PushValue();
        CLR_RT_HeapBlock*     hbObj;
        
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.m_Assembly));
        hbObj = top.Dereference();
        hbObj->SetReflection( idx );
    }
    
    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_RuntimeType::get_Name___STRING( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();
    CLR_RT_HeapBlock* hbType = stack.Arg0().Dereference();

    NANOCLR_CHECK_HRESULT(GetName( *hbType, false, stack.PushValueAndClear() ));
  
    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_RuntimeType::get_FullName___STRING( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();
    CLR_RT_HeapBlock* hbType = stack.Arg0().Dereference();

    NANOCLR_CHECK_HRESULT(GetName( *hbType, true, stack.PushValueAndClear() ));
 
    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_RuntimeType::get_BaseType___SystemType( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Instance td;
    CLR_UINT32              levels;
    CLR_RT_HeapBlock&       top = stack.PushValueAndClear();
    CLR_RT_HeapBlock*       hbType = stack.Arg0().Dereference();

    NANOCLR_CHECK_HRESULT(GetTypeDescriptor( *hbType, td, &levels ));
        
    if(levels > 0)
    {
        CLR_RT_HeapBlock*     hbObj;
        
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.m_TypeStatic));
        hbObj = top.Dereference();
        hbObj->SetReflection( g_CLR_RT_WellKnownTypes.m_Array );
    }
    else if(td.SwitchToParent())
    {
        CLR_RT_HeapBlock*     hbObj;
        
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.m_TypeStatic));
        hbObj = top.Dereference();
        hbObj->SetReflection( td );
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_RuntimeType::GetMethods___SZARRAY_SystemReflectionMethodInfo__SystemReflectionBindingFlags( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    return Library_corlib_native_System_Type::GetMethods( stack, NULL, stack.Arg1().NumericByRef().s4, NULL, 0, true );
}

HRESULT Library_corlib_native_System_RuntimeType::GetField___SystemReflectionFieldInfo__STRING__SystemReflectionBindingFlags( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    return Library_corlib_native_System_Type::GetFields( stack, stack.Arg1().RecoverString(), stack.Arg2().NumericByRef().s4, false );
}

HRESULT Library_corlib_native_System_RuntimeType::GetFields___SZARRAY_SystemReflectionFieldInfo__SystemReflectionBindingFlags( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    return Library_corlib_native_System_Type::GetFields( stack, NULL, stack.Arg1().NumericByRef().s4, true );
}

HRESULT Library_corlib_native_System_RuntimeType::GetInterfaces___SZARRAY_SystemType( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Instance td;    
    CLR_RT_HeapBlock& top = stack.PushValueAndClear();
    CLR_RT_HeapBlock* ptr = NULL;
    CLR_RT_HeapBlock* hbType = stack.Arg0().Dereference();
    int count = 0;

    // 2-pass algorithm. 1. count the interfaces; 2. store the interfaces in an array
    for (int pass = 1; pass <= 2; pass++)
    {
        NANOCLR_CHECK_HRESULT(GetTypeDescriptor( *hbType, td ));

        do
        {
            // Scan the list of interfaces.
            CLR_RT_SignatureParser          parser; parser.Initialize_Interfaces( td.m_assm, td.m_target );
            CLR_RT_SignatureParser::Element res;
            
            // 1. pass count
            if (pass == 1)
            {
                count += parser.Available();
            }
            else
            {
                // 2. pass fill the data into the array
                while(parser.Available() > 0)
                {
                    CLR_RT_HeapBlock*     hbObj;

                    NANOCLR_CHECK_HRESULT(parser.Advance( res ));        

                    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*ptr, g_CLR_RT_WellKnownTypes.m_TypeStatic));
                    hbObj = ptr->Dereference();
                    hbObj->SetReflection( res.m_cls );

                    ptr++;
                    count--;
                }

                // array full? => Done
                if (count == 0)
                {
                    break;
                }
            }
        }
        while(td.SwitchToParent());
        
        if (pass == 1)
        {
            // create the result array
            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( top, count, g_CLR_RT_WellKnownTypes.m_TypeStatic ));
            // don't need the second pass if nothing found
            if (count == 0) break;
            // get the pointer to the first element
            ptr = (CLR_RT_HeapBlock*)top.DereferenceArray()->GetFirstElement();
        }
    }
    
    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_RuntimeType::GetElementType___SystemType( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();
                
    CLR_RT_TypeDescriptor   desc;
    CLR_RT_TypeDescriptor   descSub;    
    CLR_RT_HeapBlock& top = stack.PushValueAndClear();
    CLR_RT_HeapBlock* hbType = stack.Arg0().Dereference();

    NANOCLR_CHECK_HRESULT(desc.InitializeFromReflection( hbType->ReflectionDataConst() ));

    if(desc.GetElementType( descSub ))
    {
        CLR_RT_HeapBlock*     hbObj;

        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.m_TypeStatic));
        hbObj = top.Dereference();
        hbObj->SetReflection( descSub.m_reflex );
    }
        
    NANOCLR_NOCLEANUP();
}

//--//

HRESULT Library_corlib_native_System_RuntimeType::GetTypeDescriptor( CLR_RT_HeapBlock& arg, CLR_RT_TypeDef_Instance& inst, CLR_UINT32* levels )
{
    NATIVE_PROFILE_CLR_CORE();
    return CLR_RT_ReflectionDef_Index::Convert( arg, inst, levels ) ? S_OK : CLR_E_NULL_REFERENCE;
}

HRESULT Library_corlib_native_System_RuntimeType::GetTypeDescriptor( CLR_RT_HeapBlock& arg, CLR_RT_TypeDef_Instance& inst )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_UINT32 levels;

    NANOCLR_CHECK_HRESULT(GetTypeDescriptor( arg, inst, &levels ));

    if(levels > 0)
    {
        inst.InitializeFromIndex( g_CLR_RT_WellKnownTypes.m_Array );
    }

    NANOCLR_NOCLEANUP();    
}

HRESULT Library_corlib_native_System_RuntimeType::GetName( CLR_RT_HeapBlock& arg, bool fFullName, CLR_RT_HeapBlock& res )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Instance td;
    CLR_UINT32              levels;
    char                    rgBuffer[ 256 ];
    char*                   szBuffer;
    size_t                  iBuffer;

    NANOCLR_CHECK_HRESULT(GetTypeDescriptor( arg, td, &levels ));
    
    szBuffer = rgBuffer;
    iBuffer  = MAXSTRLEN(rgBuffer);

    NANOCLR_CHECK_HRESULT(g_CLR_RT_TypeSystem.BuildTypeName( td, szBuffer, iBuffer, fFullName ? CLR_RT_TypeSystem::TYPENAME_FLAGS_FULL : 0, levels ));

    NANOCLR_SET_AND_LEAVE(CLR_RT_HeapBlock_String::CreateInstance( res, rgBuffer ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_RuntimeType::GetCustomAttributesNative___SZARRAY_OBJECT__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock*       returnArray = NULL;
    CLR_RT_HeapBlock*       callerType = NULL;
    CLR_RT_TypeDef_Instance typeDefinition;
    int count = 0;

    // put the return array on the stack
    CLR_RT_HeapBlock& top = stack.PushValueAndClear();

    // get the caller type
    callerType = stack.Arg0().Dereference();

    NANOCLR_CHECK_HRESULT(GetTypeDescriptor( *callerType, typeDefinition ));

    // setup attribute enumerator
    CLR_RT_AttributeEnumerator attributeEnumerator;
    attributeEnumerator.Initialize( typeDefinition );

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

            // use this to skip the 2nd pass if no attribute was found
            if (count == 0)
            {
                break;
            }

            // get the pointer to the first element
            returnArray = (CLR_RT_HeapBlock*)top.DereferenceArray()->GetFirstElement();

            // reset attribute enumerator
            attributeEnumerator.Initialize( typeDefinition );

            break;
        }
    } 
    while(true);

    // 2nd pass: fill the array with the attributes types, if any
    NANOCLR_CHECK_HRESULT( GetCustomAttributes( attributeEnumerator, returnArray, count ) );

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_RuntimeType::GetCustomAttributes( CLR_RT_AttributeEnumerator attributeEnumerator, CLR_RT_HeapBlock* &returnArray, int count )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    while(count > 0)
    {
        // move to the next attribute in the collection, if there is another
        if(attributeEnumerator.Advance())
        {
            CLR_RT_TypeDef_Instance instanceTypeDef;
            
            // get the type def for the current attribute
            attributeEnumerator.GetCurrent(&instanceTypeDef);

            // setup attribute parser
            CLR_RT_AttributeParser parser;
            NANOCLR_CHECK_HRESULT(parser.Initialize( attributeEnumerator ));

            while(true)
            {
                CLR_RT_AttributeParser::Value* val;

                // parse next attribute, if there is another
                NANOCLR_CHECK_HRESULT(parser.Next( val ));

                if(val == NULL)
                {
                    break;
                }
                else
                {
                    // check if this attribute has a default constructor or one with argument
                    if(val->m_mode == CLR_RT_AttributeParser::Value::c_DefaultConstructor)
                    {
                        // default constructor here

                        // get attribute type
                        returnArray->LoadFromReference(val->m_value);

                        // no parameters for the constructor
                        returnArray++;

                        // set next position to NULL
                        returnArray->SetObjectReference( NULL );
                    }
                    else if(val->m_mode == CLR_RT_AttributeParser::Value::c_ConstructorArgument)
                    {
                        // constructor with argument

                        // get the type for the class object 
                        // the assembly has to be the instance type
                        CLR_RT_MethodDef_Index    md    ; md.Set( instanceTypeDef.m_assm->m_idx, parser.m_mdIdx.Method() );
                        CLR_RT_MethodDef_Instance mdInst; mdInst.InitializeFromIndex( md );

                        CLR_RT_TypeDef_Instance cls; 
                        if(cls.InitializeFromMethod( mdInst ) == false) NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);

                        // create a new object for the attribute type and put it on the return array
                        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObject(*returnArray, cls));

                        // now the constructor parameter(s)
                        returnArray++;

                        // load the constructor parameter
                        // TODO: improve this to be able to handle constuctors with multiple parameters
                        returnArray->LoadFromReference(val->m_value);
                    }
                }
            }

            returnArray++;
            count--;
        }
        else
        {
            // no more attributes
            break;
        }
    }

    NANOCLR_NOCLEANUP();
}
