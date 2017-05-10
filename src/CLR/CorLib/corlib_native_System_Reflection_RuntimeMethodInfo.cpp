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
