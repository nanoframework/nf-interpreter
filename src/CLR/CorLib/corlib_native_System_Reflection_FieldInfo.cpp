//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"


HRESULT Library_corlib_native_System_Reflection_FieldInfo::SetValue___VOID__OBJECT__OBJECT( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_FieldDef_Instance   instFD;
    CLR_RT_TypeDef_Instance    instTD;
    CLR_RT_TypeDescriptor      instTDescObj;
    CLR_RT_TypeDef_Instance    instTDField;
    const CLR_RECORD_FIELDDEF* fd;
    CLR_RT_HeapBlock*          obj;
    bool                       fValueType;
    CLR_RT_HeapBlock&          srcVal = stack.Arg2();        
    CLR_RT_HeapBlock           val; val.Assign( srcVal );    
    CLR_RT_ProtectFromGC       gc( val );


    NANOCLR_CHECK_HRESULT(Library_corlib_native_System_Reflection_FieldInfo::Initialize( stack, instFD, instTD, obj ));

    fd = instFD.m_target;
    
    if(fd->flags & CLR_RECORD_FIELDDEF::FD_NoReflection) // don't allow reflection for fields with NoReflection attribute
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
    }

    NANOCLR_CHECK_HRESULT(instTDescObj.InitializeFromFieldDefinition(instFD));

    // make sure the right side object is of the same type as the left side        
    if(NULL != val.Dereference() && !CLR_RT_ExecutionEngine::IsInstanceOf(val, instTDescObj.m_handlerCls)) NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);

    fValueType = obj->IsAValueType();
    if(fValueType || (c_CLR_RT_DataTypeLookup[ obj->DataType() ].m_flags & CLR_RT_DataTypeLookup::c_OptimizedValueType))
    {
        if(val.Dereference() == NULL || !val.Dereference()->IsBoxed()) NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    
        if(fValueType)
        {
            _ASSERTE(NULL != obj->Dereference());
            if(NULL == obj->Dereference()) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            instTDField.InitializeFromIndex( obj->Dereference()->ObjectCls() );
        }
        else
        {
            instTDField.InitializeFromIndex( *c_CLR_RT_DataTypeLookup[ obj->DataType() ].m_cls );
        }
                    
        NANOCLR_CHECK_HRESULT(val.PerformUnboxing( instTDField ));
    }
    else
    {
#if defined(NANOCLR_APPDOMAINS)
        CLR_RT_HeapBlock&          srcObj = stack.Arg1();        

        if(srcObj.IsTransparentProxy())
        {
            _ASSERTE(srcObj.DataType() == DATATYPE_OBJECT);
            _ASSERTE(srcObj.Dereference() != NULL && srcObj.Dereference()->DataType() == DATATYPE_TRANSPARENT_PROXY);

            NANOCLR_CHECK_HRESULT(srcObj.Dereference()->TransparentProxyValidate());
            NANOCLR_CHECK_HRESULT(srcObj.Dereference()->TransparentProxyAppDomain()->MarshalObject( val, val ));
        }
#endif
    }

    switch(obj->DataType())
    {
        case DATATYPE_DATETIME: // Special case.
        case DATATYPE_TIMESPAN: // Special case.
            obj->NumericByRef().s8 = val.NumericByRefConst().s8;
            break;

        default:
            obj->Assign( val );
            break;
    }

    NANOCLR_NOCLEANUP();
}

//--//

HRESULT Library_corlib_native_System_Reflection_FieldInfo::Initialize( CLR_RT_StackFrame& stack, CLR_RT_FieldDef_Instance& instFD, CLR_RT_TypeDef_Instance& instTD, CLR_RT_HeapBlock*& obj )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();
    CLR_RT_HeapBlock* hbField = stack.Arg0().Dereference();

    if(CLR_RT_ReflectionDef_Index::Convert( *hbField, instFD ) == false ||
       instTD.InitializeFromField         (           instFD ) == false  )
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    if(instFD.m_target->flags & CLR_RECORD_FIELDDEF::FD_Static)
    {
        obj = CLR_RT_ExecutionEngine::AccessStaticField( instFD );
                
        if(obj == NULL) NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }
    else
    {
        NANOCLR_CHECK_HRESULT(stack.Arg1().EnsureObjectReference( obj ));

        if(CLR_RT_ExecutionEngine::IsInstanceOf( *obj, instTD ) == false)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
        }
                
        obj = &obj[ instFD.CrossReference().m_offset ];
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Reflection_FieldInfo::GetCustomAttributesNative___SZARRAY_OBJECT__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock*           callerField;
    CLR_RT_HeapBlock*           returnArray = NULL;
    CLR_RT_FieldDef_Instance    fieldDefinition;
    int count = 0;

    // put the return array on the stack
    CLR_RT_HeapBlock& top = stack.PushValueAndClear();

    // get the caller field
    callerField = stack.Arg0().Dereference();

    NANOCLR_CHECK_HRESULT(Library_corlib_native_System_Reflection_RuntimeFieldInfo::GetFieldDescriptor( stack, *callerField, fieldDefinition ));

    // setup attribute enumerator
    CLR_RT_AttributeEnumerator attributeEnumerator;
    attributeEnumerator.Initialize( fieldDefinition );

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
            attributeEnumerator.Initialize( fieldDefinition );

            break;
        }
    } 
    while(true);

    // 2nd pass: fill the array with the attributes types, if any
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
                        CLR_RT_MethodDef_Index    md    ; md.Set( parser.m_assm->m_idx, parser.m_mdIdx.Method() );
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
