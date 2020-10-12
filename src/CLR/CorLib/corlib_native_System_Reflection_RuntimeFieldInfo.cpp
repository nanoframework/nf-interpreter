//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"


HRESULT Library_corlib_native_System_Reflection_RuntimeFieldInfo::get_Name___STRING( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();
    CLR_RT_HeapBlock* hbField = stack.Arg0().Dereference();

    CLR_RT_FieldDef_Instance fd; if(GetFieldDescriptor( stack, *hbField, fd ) == false) NANOCLR_SET_AND_LEAVE(CLR_E_NULL_REFERENCE);

    NANOCLR_SET_AND_LEAVE(CLR_RT_HeapBlock_String::CreateInstance( stack.PushValue(), fd.m_target->name, fd.m_assm ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Reflection_RuntimeFieldInfo::get_DeclaringType___SystemType( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_FieldDef_Instance fd;
    CLR_RT_TypeDef_Instance  cls;
    CLR_RT_HeapBlock* hbField = stack.Arg0().Dereference();

    if(GetFieldDescriptor( stack, *hbField, fd ) == false) NANOCLR_SET_AND_LEAVE(CLR_E_NULL_REFERENCE);

    if(cls.InitializeFromField( fd ) == false) NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);

    {
        CLR_RT_HeapBlock& top = stack.PushValue();
        CLR_RT_HeapBlock* hbObj;
        
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.m_TypeStatic));

        hbObj = top.Dereference();
        hbObj->SetReflection( cls );
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Reflection_RuntimeFieldInfo::get_FieldType___SystemType( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_TypeDescriptor desc;
    CLR_RT_FieldDef_Instance fd;
    CLR_RT_HeapBlock* hbField = stack.Arg0().Dereference();
    
    if(GetFieldDescriptor( stack, *hbField, fd ) == false) NANOCLR_SET_AND_LEAVE(CLR_E_NULL_REFERENCE);

    NANOCLR_CHECK_HRESULT(desc.InitializeFromFieldDefinition( fd ));

    {
        CLR_RT_HeapBlock& top = stack.PushValue();
        CLR_RT_HeapBlock* hbObj;
        
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.m_TypeStatic));
        hbObj = top.Dereference();
        hbObj->SetReflection( desc.m_handlerCls );
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Reflection_RuntimeFieldInfo::GetValue___OBJECT__OBJECT( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_FieldDef_Instance   instFD;
    CLR_RT_TypeDef_Instance    instTD;
    const CLR_RECORD_FIELDDEF* fd;
    CLR_RT_HeapBlock*          obj;
    CLR_RT_HeapBlock           dst;
                    
    NANOCLR_CHECK_HRESULT(Library_corlib_native_System_Reflection_FieldInfo::Initialize( stack, instFD, instTD, obj ));

    fd = instFD.m_target;
    if(fd->flags & CLR_RECORD_FIELDDEF::FD_NoReflection) // don't allow reflection for fields with NoReflection attribute
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
    }

    {
        dst.Assign( *obj );

#if defined(NANOCLR_APPDOMAINS)
        CLR_RT_HeapBlock*          argObj = &stack.Arg1();

        //Marshal if necessary.
        if(argObj->IsTransparentProxy())
        {
            _ASSERTE(argObj->DataType() == DATATYPE_OBJECT);
            
            argObj = argObj->Dereference();

            _ASSERTE(argObj != NULL && argObj->DataType() == DATATYPE_TRANSPARENT_PROXY);
    
            NANOCLR_CHECK_HRESULT(argObj->TransparentProxyValidate());
            
            NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.GetCurrentAppDomain()->MarshalObject( *obj, dst, argObj->TransparentProxyAppDomain() ));
        }
#endif

        CLR_RT_HeapBlock& res = stack.PushValueAndAssign( dst );

        NANOCLR_CHECK_HRESULT(res.PerformBoxingIfNeeded());            
    }

    NANOCLR_NOCLEANUP();
}

//--//

bool Library_corlib_native_System_Reflection_RuntimeFieldInfo::GetFieldDescriptor( CLR_RT_StackFrame& stack, CLR_RT_HeapBlock& arg, CLR_RT_FieldDef_Instance& inst )
{
    (void)stack;

    NATIVE_PROFILE_CLR_CORE();
    return CLR_RT_ReflectionDef_Index::Convert( arg, inst );
}
