//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "Core.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT CLR_RT_HeapBlock_Delegate::CreateInstance(
    CLR_RT_HeapBlock &reference,
    const CLR_RT_MethodDef_Index &ftn,
    CLR_RT_StackFrame *call)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    reference.SetObjectReference(nullptr);

    CLR_UINT32 length = 0;

#if defined(NANOCLR_DELEGATE_PRESERVE_STACK)
    if (call)
    {
        NANOCLR_FOREACH_NODE_BACKWARD__DIRECT(CLR_RT_StackFrame, ptr, call)
        {
            length++;
        }
        NANOCLR_FOREACH_NODE_BACKWARD_END();
    }

    //
    // Limit depth to three callers.
    //
    if (length > 3)
        length = 3;
#else
    (void)call;
#endif

    CLR_UINT32 totLength = (CLR_UINT32)(sizeof(CLR_RT_HeapBlock_Delegate) + length * sizeof(CLR_RT_MethodDef_Index));

    CLR_RT_HeapBlock_Delegate *dlg = (CLR_RT_HeapBlock_Delegate *)g_CLR_RT_ExecutionEngine.ExtractHeapBytesForObjects(
        DATATYPE_DELEGATE_HEAD,
        0,
        totLength);
    CHECK_ALLOCATION(dlg);

    reference.SetObjectReference(dlg);

    dlg->ClearData();
    dlg->m_cls.Clear();
    dlg->m_ftn = ftn;
#if defined(NANOCLR_DELEGATE_PRESERVE_STACK)
    dlg->m_numOfStackFrames = length;
#endif

    dlg->m_object.SetObjectReference(nullptr);
    dlg->m_genericTypeSpec.Clear();
    dlg->m_genericMethodSpec.Clear();
    dlg->m_delegateTypeSpec.Clear();

#if defined(NANOCLR_APPDOMAINS)
    dlg->m_appDomain = g_CLR_RT_ExecutionEngine.GetCurrentAppDomain();
#endif

#if defined(NANOCLR_DELEGATE_PRESERVE_STACK)
    if (call)
    {
        CLR_RT_MethodDef_Index *callStack = dlg->GetStackFrames();

        NANOCLR_FOREACH_NODE_BACKWARD__DIRECT(CLR_RT_StackFrame, ptr, call)
        {
            if (length-- == 0)
                break;

            *callStack++ = ptr->m_call;
        }
        NANOCLR_FOREACH_NODE_BACKWARD_END();
    }
#endif

    NANOCLR_NOCLEANUP();
}

const CLR_RT_HeapBlock_Delegate *CLR_RT_HeapBlock_Delegate::TypeCarrier(const CLR_RT_HeapBlock *obj)
{
    if (obj->DataType() == DATATYPE_DELEGATELIST_HEAD)
    {
        CLR_RT_HeapBlock_Delegate_List *list = (CLR_RT_HeapBlock_Delegate_List *)obj;
        CLR_RT_HeapBlock *items = list->GetDelegates();

        for (CLR_UINT32 i = 0; i < list->m_length; i++)
        {
            if (items[i].DereferenceDelegate() != nullptr)
            {
                return items[i].DereferenceDelegate();
            }
        }

        return nullptr;
    }

    return (const CLR_RT_HeapBlock_Delegate *)obj;
}

bool CLR_RT_HeapBlock_Delegate::SameType(const CLR_RT_HeapBlock_Delegate &other) const
{
    if (m_cls.data == 0 || other.m_cls.data == 0)
    {
        return true;
    }

    return m_cls.data == other.m_cls.data &&
           (m_delegateTypeSpec.data == 0 || other.m_delegateTypeSpec.data == 0 ||
            CLR_RT_TypeSystem::IsSameGenericInstance(m_delegateTypeSpec, other.m_delegateTypeSpec));
}

int CLR_RT_HeapBlock_Delegate::CompareGenericContext(const CLR_RT_HeapBlock_Delegate &other) const
{
    if (!SameType(other))
    {
        return m_cls.data != other.m_cls.data ? (m_cls.data > other.m_cls.data ? 1 : -1)
                                              : (m_delegateTypeSpec.data > other.m_delegateTypeSpec.data ? 1 : -1);
    }

    if (m_genericMethodSpec.data != other.m_genericMethodSpec.data)
    {
        return m_genericMethodSpec.data > other.m_genericMethodSpec.data ? 1 : -1;
    }

    if (m_genericTypeSpec.data != other.m_genericTypeSpec.data &&
        !(m_genericTypeSpec.data != 0 && other.m_genericTypeSpec.data != 0 &&
          CLR_RT_TypeSystem::IsSameGenericInstance(m_genericTypeSpec, other.m_genericTypeSpec)))
    {
        return m_genericTypeSpec.data > other.m_genericTypeSpec.data ? 1 : -1;
    }

    return 0;
}

void CLR_RT_HeapBlock_Delegate::Relocate()
{
    NATIVE_PROFILE_CLR_CORE();
    m_object.Relocate__HeapBlock();
}
