//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"

void CLR_RT_HeapBlock_WeakReference::InsertInPriorityOrder()
{
    NATIVE_PROFILE_CLR_HEAP_PERSISTENCE();

    // Remove from the list before looking for a spot, to avoid comparing against ourselves.
    this->Unlink();

    g_CLR_RT_ExecutionEngine.m_weakReferences.LinkAtBack(this);
}

void CLR_RT_HeapBlock_WeakReference::Relocate()
{
    NATIVE_PROFILE_CLR_HEAP_PERSISTENCE();
    CLR_RT_HeapBlock_Node::Relocate();

    CLR_RT_GarbageCollector::Heap_Relocate((void **)&m_targetDirect);
    CLR_RT_GarbageCollector::Heap_Relocate((void **)&m_targetSerialized);
}

CLR_UINT32 CLR_RT_HeapBlock_WeakReference_Identity::ComputeCRC(const CLR_UINT8 *ptr, CLR_UINT32 len) const
{
    NATIVE_PROFILE_CLR_HEAP_PERSISTENCE();
    CLR_UINT32 hash;

    hash = SUPPORT_ComputeCRC(ptr, len, g_buildCRC);
    hash = SUPPORT_ComputeCRC(&m_selectorHash, sizeof(m_selectorHash), hash);
    hash = SUPPORT_ComputeCRC(&m_id, sizeof(m_id), hash);

    return hash;
}

void CLR_RT_HeapBlock_WeakReference::RecoverObjects(CLR_RT_DblLinkedList &lstHeap)
{
    NATIVE_PROFILE_CLR_HEAP_PERSISTENCE();
    NANOCLR_FOREACH_NODE(CLR_RT_HeapCluster, hc, lstHeap)
    {
        CLR_RT_HeapBlock_Node *ptr = hc->m_payloadStart;
        CLR_RT_HeapBlock_Node *end = hc->m_payloadEnd;

        while (ptr < end)
        {
            if (ptr->DataType() == DATATYPE_WEAKCLASS)
            {
                CLR_RT_HeapBlock_WeakReference *weak = (CLR_RT_HeapBlock_WeakReference *)ptr;
                CLR_RT_HeapBlock *target = weak->m_targetSerialized;

                NANOCLR_FOREACH_NODE(CLR_RT_HeapCluster, hc2, lstHeap)
                {
                    CLR_RT_HeapBlock_Node *ptr2 = hc2->m_payloadStart;
                    CLR_RT_HeapBlock_Node *end2 = hc2->m_payloadEnd;

                    if (ptr2 <= target && target < end2)
                    {
                        while (ptr2 < end2)
                        {
                            if (ptr2 == target)
                            {
                                CLR_RT_HeapBlock_Array *array = (CLR_RT_HeapBlock_Array *)ptr2;

                                if (array->m_numOfElements == weak->m_identity.m_length)
                                {
                                    if (weak->m_identity.m_crc == weak->m_identity.ComputeCRC(
                                                                      array->GetFirstElement(),
                                                                      weak->m_identity.m_length))
                                    {
                                        weak->m_identity.m_flags |= CLR_RT_HeapBlock_WeakReference::WR_Restored;

                                        weak->InsertInPriorityOrder();

                                        weak = NULL;
                                    }
                                }

                                break;
                            }

                            ptr2 += ptr2->DataSize();
                        }

                        if (ptr2 < end2)
                        {
                            break;
                        }
                    }
                }
                NANOCLR_FOREACH_NODE_END();

                if (weak)
                {
                    weak->ChangeDataType(DATATYPE_FREEBLOCK);
                }
            }

            ptr += ptr->DataSize();
        }
    }
    NANOCLR_FOREACH_NODE_END();
}

HRESULT CLR_RT_HeapBlock_WeakReference::GetTarget(CLR_RT_HeapBlock &targetReference)
{
    NATIVE_PROFILE_CLR_HEAP_PERSISTENCE();
    NANOCLR_HEADER();

    if (CLR_RT_BinaryFormatter::SerializationEnabled())
    {
        targetReference.SetObjectReference(NULL);

        if (m_targetDirect == NULL && m_targetSerialized != NULL)
        {
            if (m_identity.m_flags & CLR_RT_HeapBlock_WeakReference::WR_ArrayOfBytes)
            {
                m_targetDirect = m_targetSerialized;
            }
            else
            {
                CLR_RT_HeapBlock input;
                input.SetObjectReference(m_targetSerialized);

                CLR_RT_HeapBlock output;
                output.SetObjectReference(NULL);

                {
                    CLR_RT_ProtectFromGC gcInput(input);
                    CLR_RT_ProtectFromGC gcOutput(output);

                    if (FAILED(CLR_RT_BinaryFormatter::Deserialize(output, input, NULL, NULL, 0)))
                    {
                        output.SetObjectReference(NULL);
                    }
                }

                m_targetDirect = output.Dereference();
            }
        }
    }

    targetReference.SetObjectReference(m_targetDirect);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT CLR_RT_HeapBlock_WeakReference::SetTarget(CLR_RT_HeapBlock &targetReference)
{
    NATIVE_PROFILE_CLR_HEAP_PERSISTENCE();
    NANOCLR_HEADER();

    // Only classes and value types can be associated with a weak reference!
    if (targetReference.DataType() != DATATYPE_OBJECT)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

    m_targetDirect = targetReference.Dereference();

    InsertInPriorityOrder();

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_WeakReference::get_IsAlive___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_WeakReference *weak;

    weak = (CLR_RT_HeapBlock_WeakReference *)stack.This();
    FAULT_ON_NULL(weak);

    stack.SetResult_Boolean(weak->m_targetDirect != NULL);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_WeakReference::get_Target___OBJECT(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_WeakReference *weak;

    weak = (CLR_RT_HeapBlock_WeakReference *)stack.This();
    FAULT_ON_NULL(weak);

    NANOCLR_SET_AND_LEAVE(weak->GetTarget(stack.PushValue()));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_WeakReference::set_Target___VOID__OBJECT(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_WeakReference *weak;

    weak = (CLR_RT_HeapBlock_WeakReference *)stack.This();
    FAULT_ON_NULL(weak);

    NANOCLR_SET_AND_LEAVE(weak->SetTarget(stack.Arg1()));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_WeakReference::_ctor___VOID__OBJECT(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_WeakReference *weak = (CLR_RT_HeapBlock_WeakReference *)stack.This();
    FAULT_ON_NULL(weak);

    NANOCLR_SET_AND_LEAVE(weak->SetTarget(stack.Arg1()));

    NANOCLR_NOCLEANUP();
}
