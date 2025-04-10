//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "Core.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

void CLR_RT_HeapCluster::HeapCluster_Initialize(CLR_UINT32 size, CLR_UINT32 blockSize)
{
    NATIVE_PROFILE_CLR_CORE();
    GenericNode_Initialize();

    size = (size - sizeof(*this)) / sizeof(CLR_RT_HeapBlock);

    m_freeList.DblLinkedList_Initialize();
    m_payloadStart = static_cast<CLR_RT_HeapBlock_Node *>(&this[1]);
    m_payloadEnd = &m_payloadStart[size];

    // Scan memory looking for possible objects to salvage
    CLR_RT_HeapBlock_Node *ptr = m_payloadStart;
    CLR_RT_HeapBlock_Node *end = m_payloadEnd;

    while (ptr < end)
    {
        if (ptr->DataType() == DATATYPE_WEAKCLASS)
        {
            CLR_RT_HeapBlock_WeakReference *weak = (CLR_RT_HeapBlock_WeakReference *)(ptr);

            if (weak->DataSize() == CONVERTFROMSIZETOHEAPBLOCKS(sizeof(*weak)) && weak->m_targetSerialized != NULL &&
                (weak->m_identity.m_flags & CLR_RT_HeapBlock_WeakReference::WR_SurviveBoot))
            {
                weak->SetNext(NULL);
                weak->SetPrev(NULL);

                weak->m_identity.m_flags &= ~CLR_RT_HeapBlock_WeakReference::WR_Persisted;

                weak->m_targetDirect = NULL;

                weak->MarkAlive();

                ptr += ptr->DataSize();
                continue;
            }
        }
        else if (ptr->DataType() == DATATYPE_SZARRAY)
        {
            CLR_RT_HeapBlock_Array *array = (CLR_RT_HeapBlock_Array *)ptr;

            if (array->m_typeOfElement == DATATYPE_U1 && array->m_fReference == 0)
            {
                CLR_UINT32 tot = sizeof(*array) + array->m_sizeOfElement * array->m_numOfElements;

                if (array->DataSize() == CONVERTFROMSIZETOHEAPBLOCKS(tot) && (ptr + ptr->DataSize()) <= end)
                {
                    array->MarkAlive();

                    ptr += ptr->DataSize();
                    continue;
                }
            }
        }

        if ((uintptr_t)(ptr + blockSize) > (uintptr_t)end)
        {
            blockSize = (CLR_UINT32)(end - ptr);
        }

        ptr->SetDataId(CLR_RT_HEAPBLOCK_RAW_ID(DATATYPE_FREEBLOCK, 0, blockSize));
        ptr += blockSize;
    }

    RecoverFromGC();

    m_freeList.ValidateList();
}

CLR_RT_HeapBlock *CLR_RT_HeapCluster::ExtractBlocks(CLR_UINT32 dataType, CLR_UINT32 flags, CLR_UINT32 length)
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RT_HeapBlock_Node *res = NULL;
    CLR_UINT32 available = 0;

    m_freeList.ValidateList();

    if (flags & CLR_RT_HeapBlock::HB_Event)
    {
        NANOCLR_FOREACH_NODE_BACKWARD(CLR_RT_HeapBlock_Node, ptr, m_freeList)
        {
            available = ptr->DataSize();

            if (available >= length)
            {
                res = ptr;
                break;
            }
        }
        NANOCLR_FOREACH_NODE_BACKWARD_END();
    }
    else
    {
        NANOCLR_FOREACH_NODE(CLR_RT_HeapBlock_Node, ptr, m_freeList)
        {
            available = ptr->DataSize();

            if (available >= length)
            {
                res = ptr;

                // sanity checks for out of bounds
                if ((void *)res < (void *)s_CLR_RT_Heap.m_location ||
                    (void *)res >= (void *)(s_CLR_RT_Heap.m_location + s_CLR_RT_Heap.m_size))
                {
                    return NULL;
                }

                break;
            }
        }
        NANOCLR_FOREACH_NODE_END();
    }

    if (res)
    {
        CLR_RT_HeapBlock_Node *next = res->Next();
        CLR_RT_HeapBlock_Node *prev = res->Prev();

        available -= length;

        // sanity checks for out of bounds
        if (((void *)next < (void *)s_CLR_RT_Heap.m_location) ||
            (void *)next >= (void *)(s_CLR_RT_Heap.m_location + s_CLR_RT_Heap.m_size) ||
            (void *)prev < (void *)s_CLR_RT_Heap.m_location ||
            (void *)prev >= (void *)(s_CLR_RT_Heap.m_location + s_CLR_RT_Heap.m_size))
        {
            return NULL;
        }

        if (available != 0)
        {
            if (flags & CLR_RT_HeapBlock::HB_Event)
            {
                res->SetDataId(CLR_RT_HEAPBLOCK_RAW_ID(DATATYPE_FREEBLOCK, CLR_RT_HeapBlock::HB_Pinned, available));

                res += available;

                // sanity checks for out of bounds
                if ((void *)res < (void *)s_CLR_RT_Heap.m_location ||
                    (void *)res >= (void *)(s_CLR_RT_Heap.m_location + s_CLR_RT_Heap.m_size))
                {
                    return NULL;
                }
            }
            else
            {
                CLR_RT_HeapBlock_Node *ptr = &res[length];

                // sanity checks for out of bounds
                if ((void *)ptr < (void *)s_CLR_RT_Heap.m_location ||
                    (void *)ptr >= (void *)(s_CLR_RT_Heap.m_location + s_CLR_RT_Heap.m_size))
                {
                    return NULL;
                }

                //
                // Relink to the new free block.
                //
                ptr->SetDataId(CLR_RT_HEAPBLOCK_RAW_ID(DATATYPE_FREEBLOCK, CLR_RT_HeapBlock::HB_Pinned, available));

                ptr->SetNext(next);
                ptr->SetPrev(prev);

                prev->SetNext(ptr);
                next->SetPrev(ptr);
            }
        }
        else
        {
            //
            // Unlink the whole block.
            //
            prev->SetNext(next);
            next->SetPrev(prev);
        }

        res->SetDataId(CLR_RT_HEAPBLOCK_RAW_ID(dataType, flags, length));

        if (flags & CLR_RT_HeapBlock::HB_InitializeToZero)
        {
            res->InitializeToZero();
        }
        else
        {
            res->Debug_ClearBlock(SENTINEL_CLEAR_BLOCK);
        }
    }

    m_freeList.ValidateList();

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void CLR_RT_HeapCluster::RecoverFromGC()
{
    NATIVE_PROFILE_CLR_CORE();

    CLR_RT_HeapBlock_Node *ptr = m_payloadStart;
    CLR_RT_HeapBlock_Node const *end = m_payloadEnd;

    //
    // Open the free list.
    //
    CLR_RT_HeapBlock_Node *last = m_freeList.Head();
    last->SetPrev(NULL);

    while (ptr < end)
    {

        ValidateBlock(ptr);

        if (ptr->IsAlive() == false)
        {
            CLR_RT_HeapBlock_Node *next = ptr;
            CLR_UINT32 lenTot = 0;

            do
            {
                ValidateBlock(next);

                NANOCLR_CHECK_EARLY_COLLECTION(next);

                int len = next->DataSize();

                // length of the block can not be 0, so something very wrong happened
                _ASSERTE(len > 0);

                next += len;
                lenTot += len;

            } while (next < end && next->IsAlive() == false);

#if defined(NANOCLR_PROFILE_NEW_ALLOCATIONS)

            // don't report free blocks, as they are not being deleted, rather grouped
            if (ptr->DataType() != DATATYPE_FREEBLOCK)
            {
                g_CLR_PRF_Profiler.TrackObjectDeletion(ptr);
            }

#endif
            ptr->SetDataId(CLR_RT_HEAPBLOCK_RAW_ID(DATATYPE_FREEBLOCK, CLR_RT_HeapBlock::HB_Pinned, lenTot));

            //
            // Link to the free list.
            //
            last->SetNext(ptr);
            ptr->SetPrev(last);
            last = ptr;

            ptr->Debug_ClearBlock(SENTINEL_RECOVERED);

            ptr = next;
        }
        else
        {
            if (ptr->IsEvent() == false)
            {
                ptr->MarkDead();
            }

            int len = ptr->DataSize();

            // length of the block can not be 0, so something very wrong happened
            _ASSERTE(len > 0);

            ptr += len;
        }
    }

    //
    // Close the free list.
    //
    last->SetNext(m_freeList.Tail());
    m_freeList.Tail()->SetPrev(last);
    m_freeList.Tail()->SetNext(NULL);
}

CLR_RT_HeapBlock_Node *CLR_RT_HeapCluster::InsertInOrder(CLR_RT_HeapBlock_Node *node, CLR_UINT32 size)
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RT_HeapBlock_Node *ptr = nullptr;

    NANOCLR_FOREACH_NODE__NODECL(CLR_RT_HeapBlock_Node, ptr, m_freeList)
    {
        if (ptr > node)
        {
            break;
        }
    }
    NANOCLR_FOREACH_NODE_END();

    node->ClearData();
    m_freeList.InsertBeforeNode(ptr, node);

    //
    // Try to coalesce with the following free block.
    //
    ptr = node->Next();
    if (ptr->Next() && (node + size) == ptr)
    {
        size += ptr->DataSize();
        ptr->Unlink();
    }

    //
    // Try to coalesce with the preceding free block.
    //
    ptr = node->Prev();
    if (ptr->Prev() && (ptr + ptr->DataSize()) == node)
    {
        size += ptr->DataSize();
        node->Unlink();
        node = ptr;
    }

    node->SetDataId(CLR_RT_HEAPBLOCK_RAW_ID(DATATYPE_FREEBLOCK, CLR_RT_HeapBlock::HB_Pinned, size));
    node->Debug_ClearBlock(SENTINEL_CLUSTER_INSERT);

    return node;
}

//--//

#if NANOCLR_VALIDATE_HEAP >= NANOCLR_VALIDATE_HEAP_1_HeapBlocksAndUnlink

void CLR_RT_HeapCluster::ValidateBlock(CLR_RT_HeapBlock *ptr)
{
    NATIVE_PROFILE_CLR_CORE();

    while (true)
    {
        if (ptr < m_payloadStart || ptr >= m_payloadEnd)
        {
#ifdef _WIN64
            CLR_Debug::Printf(
                "Block beyond cluster limits: 0x%016" PRIxPTR " [0x%016" PRIxPTR " : 0x%016" PRIxPTR "-0x%016" PRIxPTR
                "]\r\n",
                (uintptr_t)ptr,
                (uintptr_t)this,
                (uintptr_t)m_payloadStart,
                (uintptr_t)m_payloadEnd);
#else

            CLR_Debug::Printf(
                "Block beyond cluster limits: 0x%08" PRIxPTR " [0x%08" PRIxPTR " : 0x%08" PRIxPTR "-0x%08" PRIxPTR
                "]\r\n",
                (uintptr_t)ptr,
                (uintptr_t)this,
                (uintptr_t)m_payloadStart,
                (uintptr_t)m_payloadEnd);
#endif

            break;
        }

        if (ptr->DataType() >= DATATYPE_FIRST_INVALID)
        {
#ifdef _WIN64
            CLR_Debug::Printf(
                "Bad Block Type: 0x%016" PRIxPTR " %02x [0x%016" PRIxPTR " : 0x%016" PRIxPTR "-0x%016" PRIxPTR "]\r\n",
                (uintptr_t)ptr,
                ptr->DataType(),
                (uintptr_t)this,
                (uintptr_t)m_payloadStart,
                (uintptr_t)m_payloadEnd);
#else
            CLR_Debug::Printf(
                "Bad Block Type: 0x%08" PRIxPTR " %02x [0x%08" PRIxPTR " : 0x%08" PRIxPTR "-0x%08" PRIxPTR "]\r\n",
                (uintptr_t)ptr,
                ptr->DataType(),
                (uintptr_t)this,
                (uintptr_t)m_payloadStart,
                (uintptr_t)m_payloadEnd);
#endif

            break;
        }

        if (ptr->DataSize() == 0)
        {
#ifdef _WIN64
            CLR_Debug::Printf(
                "Bad Block null-size: 0x%016" PRIxPTR " [0x%016" PRIxPTR " : 0x%016" PRIxPTR "-0x%016" PRIxPTR "]\r\n",
                (uintptr_t)ptr,
                (uintptr_t)this,
                (uintptr_t)m_payloadStart,
                (uintptr_t)m_payloadEnd);
#else
            CLR_Debug::Printf(
                "Bad Block null-size: 0x%08" PRIxPTR " [0x%08" PRIxPTR " : 0x%08" PRIxPTR "-0x%08" PRIxPTR "]\r\n",
                (uintptr_t)ptr,
                (uintptr_t)this,
                (uintptr_t)m_payloadStart,
                (uintptr_t)m_payloadEnd);
#endif

            break;
        }

        if (ptr + ptr->DataSize() > m_payloadEnd)
        {
#ifdef _WIN64
            CLR_Debug::Printf(
                "Bad Block size: 0x%016" PRIxPTR " [0x%016" PRIxPTR " : 0x%016" PRIxPTR "-0x%016" PRIxPTR "]\r\n",
                (uintptr_t)ptr,
                (uintptr_t)this,
                (uintptr_t)m_payloadStart,
                (uintptr_t)m_payloadEnd);
#else
            CLR_Debug::Printf(
                "Bad Block size: %d 0x%08" PRIxPTR " [0x%08" PRIxPTR " : 0x%08" PRIxPTR "-0x%08" PRIxPTR "]\r\n",
                ptr->DataSize(),
                (uintptr_t)ptr,
                (uintptr_t)this,
                (uintptr_t)m_payloadStart,
                (uintptr_t)m_payloadEnd);
#endif

            break;
        }

        return;
    }

    NANOCLR_DEBUG_STOP();
}

#endif
