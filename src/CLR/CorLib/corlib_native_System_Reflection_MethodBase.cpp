//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"




HRESULT Library_corlib_native_System_Reflection_MethodBase::get_Name___STRING( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_MethodDef_Instance md;
    CLR_RT_HeapBlock*         hbMeth  = stack.Arg0().Dereference();

    NANOCLR_CHECK_HRESULT(GetMethodDescriptor( stack, *hbMeth, md ));

    NANOCLR_SET_AND_LEAVE(CLR_RT_HeapBlock_String::CreateInstance( stack.PushValue(), md.m_target->name, md.m_assm ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Reflection_MethodBase::get_DeclaringType___SystemType( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_MethodDef_Instance md;
    CLR_RT_TypeDef_Instance   cls;
    CLR_RT_HeapBlock* hbMeth  = stack.Arg0().Dereference();

    NANOCLR_CHECK_HRESULT(GetMethodDescriptor( stack, *hbMeth, md ));

    if(cls.InitializeFromMethod( md ) == false) NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);

    {
        CLR_RT_HeapBlock& top = stack.PushValue();
        CLR_RT_HeapBlock* hbObj;

        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.m_TypeStatic));
        
        hbObj = top.Dereference();
        hbObj->SetReflection( cls );
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Reflection_MethodBase::get_IsPublic___BOOLEAN( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(CheckFlags( stack, CLR_RECORD_METHODDEF::MD_Scope_Mask, CLR_RECORD_METHODDEF::MD_Scope_Public ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Reflection_MethodBase::get_IsStatic___BOOLEAN( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(CheckFlags( stack, CLR_RECORD_METHODDEF::MD_Static, CLR_RECORD_METHODDEF::MD_Static ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Reflection_MethodBase::get_IsFinal___BOOLEAN( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(CheckFlags( stack, CLR_RECORD_METHODDEF::MD_Final, CLR_RECORD_METHODDEF::MD_Final ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Reflection_MethodBase::get_IsVirtual___BOOLEAN( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(CheckFlags( stack, CLR_RECORD_METHODDEF::MD_Virtual, CLR_RECORD_METHODDEF::MD_Virtual ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Reflection_MethodBase::get_IsAbstract___BOOLEAN( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(CheckFlags( stack, CLR_RECORD_METHODDEF::MD_Abstract, CLR_RECORD_METHODDEF::MD_Abstract ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Reflection_MethodBase::Invoke___OBJECT__OBJECT__SZARRAY_OBJECT( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock&           obj     = stack.Arg1();
    CLR_RT_MethodDef_Instance   md;
    const CLR_RECORD_METHODDEF* mdR;
    CLR_RT_HeapBlock_Array*     pArray  = stack.Arg2().DereferenceArray();
    CLR_RT_HeapBlock*           args    = NULL;
    int                         numArgs = 0;
    CLR_RT_HeapBlock*           hbMeth  = stack.Arg0().Dereference();

    NANOCLR_CHECK_HRESULT(GetMethodDescriptor( stack, *hbMeth, md ));

    mdR = md.m_target;

    if(stack.m_customState == 0)
    {
        stack.m_customState = 1;

        if(pArray)
        {
            args    = (CLR_RT_HeapBlock*)pArray->GetFirstElement();
            numArgs =                    pArray->m_numOfElements;
        }

        NANOCLR_CHECK_HRESULT(stack.MakeCall( md, &obj, args, numArgs ));
    }
    else
    {                
        if(mdR->retVal != DATATYPE_VOID)
        {                    
            if(mdR->retVal < DATATYPE_I4)
            {
                stack.TopValue().ChangeDataType( mdR->retVal );
            }

            NANOCLR_CHECK_HRESULT(stack.TopValue().PerformBoxingIfNeeded());
        }
        else
        {               
            stack.SetResult_Object( NULL );
        }
    }

    NANOCLR_NOCLEANUP();
}

//--//

HRESULT Library_corlib_native_System_Reflection_MethodBase::GetMethodDescriptor( CLR_RT_StackFrame& stack, CLR_RT_HeapBlock& arg, CLR_RT_MethodDef_Instance& inst )
{
    (void)stack;

    NATIVE_PROFILE_CLR_CORE();
    return CLR_RT_ReflectionDef_Index::Convert( arg, inst ) ? S_OK : CLR_E_NULL_REFERENCE;
}

HRESULT Library_corlib_native_System_Reflection_MethodBase::CheckFlags( CLR_RT_StackFrame& stack, CLR_UINT32 mask, CLR_UINT32 flag )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_MethodDef_Instance md;
    bool                      fRes;
    CLR_RT_HeapBlock*         hbMeth = stack.Arg0().Dereference();

    NANOCLR_CHECK_HRESULT(GetMethodDescriptor( stack, *hbMeth, md ));

    if((md.m_target->flags & mask) == flag)
    {
        fRes = true;
    }
    else
    {
        fRes = false;
    }

    stack.SetResult_Boolean( fRes );

    NANOCLR_NOCLEANUP();
}
