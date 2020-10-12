//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"


HRESULT Library_corlib_native_System_Delegate::Equals___BOOLEAN__OBJECT( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    stack.SetResult_Boolean( CLR_RT_HeapBlock::Compare_Unsigned_Values( stack.Arg0(), stack.Arg1() ) == 0 );

    NANOCLR_NOCLEANUP_NOLABEL();
}

#if (NANOCLR_REFLECTION == TRUE)

HRESULT Library_corlib_native_System_Delegate::GetInvocationList___SZARRAY_SystemDelegate( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Delegate *dlg = stack.Arg0().DereferenceDelegate();
    CLR_RT_HeapBlock_Delegate_List *lst = (CLR_RT_HeapBlock_Delegate_List *)dlg;

    int delegatesCount = 0;
    CLR_RT_HeapBlock *returnArray = NULL;

    // put the return array on the stack
    CLR_RT_HeapBlock &top = stack.PushValueAndClear();

    if (dlg) {
        if (dlg->DataType() == DATATYPE_DELEGATELIST_HEAD) {
            delegatesCount = lst->m_length;
        } else {
            delegatesCount = 1;
        }
    }

    // create the result array
    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(top, delegatesCount, g_CLR_RT_WellKnownTypes.m_Delegate));

    if (delegatesCount > 0) {

        // get the pointer to the first element
        returnArray = (CLR_RT_HeapBlock *)top.DereferenceArray()->GetFirstElement();
       
        if (delegatesCount > 1) {
            CLR_RT_HeapBlock *ptr = lst->GetDelegates();
            // fill the array with the delegates
            for (int i = 0; i < delegatesCount; i++) {

                // create an instance of delegate
                NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*returnArray, g_CLR_RT_WellKnownTypes.m_Delegate));

                //fetch delegate from list
                dlg = ptr[i].DereferenceDelegate();
                //set delegate reference to return element
                returnArray->SetObjectReference(dlg);

                returnArray++;
            }
        } else {
            // create an instance of delegate
            NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*returnArray, g_CLR_RT_WellKnownTypes.m_Delegate));
            //set delegate reference to return element
            returnArray->SetObjectReference(dlg);
        }
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Delegate::get_Method___SystemReflectionMethodInfo( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Delegate* dlg = stack.Arg0().DereferenceDelegate();

    dlg = GetLastDelegate( dlg ); if(!dlg) NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);

    {
        CLR_RT_HeapBlock& top = stack.PushValue();
        CLR_RT_HeapBlock* hbObj;
        
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.m_MethodInfo));
        hbObj = top.Dereference();
        
        hbObj->SetReflection( dlg->DelegateFtn() ); 
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Delegate::get_Target___OBJECT( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Delegate* dlg = stack.Arg0().DereferenceDelegate();

    dlg = GetLastDelegate( dlg ); if(!dlg) NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);

    {
        stack.PushValueAndAssign( dlg->m_object );
    }

    NANOCLR_NOCLEANUP();
}

#endif // NANOCLR_REFLECTION

HRESULT Library_corlib_native_System_Delegate::Combine___STATIC__SystemDelegate__SystemDelegate__SystemDelegate( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(CLR_RT_HeapBlock_Delegate_List::Combine( stack.PushValue(), stack.Arg0(), stack.Arg1(), false ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Delegate::Remove___STATIC__SystemDelegate__SystemDelegate__SystemDelegate( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(CLR_RT_HeapBlock_Delegate_List::Remove( stack.PushValue(), stack.Arg0(), stack.Arg1() ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Delegate::op_Equality___STATIC__BOOLEAN__SystemDelegate__SystemDelegate( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(Library_corlib_native_System_Delegate::Equals___BOOLEAN__OBJECT( stack ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Delegate::op_Inequality___STATIC__BOOLEAN__SystemDelegate__SystemDelegate( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    NANOCLR_CHECK_HRESULT(Library_corlib_native_System_Delegate::op_Equality___STATIC__BOOLEAN__SystemDelegate__SystemDelegate( stack ));

    stack.NegateResult();

    NANOCLR_NOCLEANUP();
}

//--//

CLR_RT_HeapBlock_Delegate* Library_corlib_native_System_Delegate::GetLastDelegate( CLR_RT_HeapBlock_Delegate* dlg )
{
    NATIVE_PROFILE_CLR_CORE();
    if(dlg)
    {
        if(dlg->DataType() == DATATYPE_DELEGATELIST_HEAD)
        {
            CLR_RT_HeapBlock_Delegate_List* lst = (CLR_RT_HeapBlock_Delegate_List*)dlg;

            if(lst->m_length == 0)
            {
                dlg = NULL;
            }
            else
            {
                CLR_RT_HeapBlock* ptr = lst->GetDelegates();

                dlg = ptr[ lst->m_length-1 ].DereferenceDelegate();
            }
        }
    }

    return dlg;
}
