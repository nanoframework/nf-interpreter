﻿//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "Core.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(NANOCLR_GC_VERBOSE)

void CLR_RT_GarbageCollector::GC_Stats(
    int &resNumberObjects,
    int &resSizeObjects,
    int &resNumberEvents,
    int &resSizeEvents)
{
    NATIVE_PROFILE_CLR_CORE();
    resNumberObjects = 0;
    resSizeObjects = 0;

    resNumberEvents = 0;
    resSizeEvents = 0;

    NANOCLR_FOREACH_NODE(CLR_RT_HeapCluster, hc, g_CLR_RT_ExecutionEngine.m_heap)
    {
        CLR_RT_HeapBlock_Node *ptr = hc->m_payloadStart;
        CLR_RT_HeapBlock_Node *end = hc->m_payloadEnd;

        while (ptr < end)
        {
            CLR_UINT16 size = ptr->DataSize();

            hc->ValidateBlock(ptr);

            if (ptr->DataType() != DATATYPE_FREEBLOCK)
            {
                if (ptr->IsEvent())
                {
                    resNumberEvents += 1;
                    resSizeEvents += size * sizeof(struct CLR_RT_HeapBlock);
                }
                else
                {
                    resNumberObjects += 1;
                    resSizeObjects += size * sizeof(struct CLR_RT_HeapBlock);
                }
            }

            ptr += size;
        }
    }
    NANOCLR_FOREACH_NODE_END();
}

static void DumpTimeout(CLR_RT_Thread *th, CLR_INT64 &t)
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_Debug::Printf(": %d", th ? th->m_pid : -1);

    if (t < TIMEOUT_INFINITE)
    {
        t -= HAL_Time_CurrentTime();

        CLR_Debug::Printf(" %d", (int)t);
    }
    else
    {
        CLR_Debug::Printf(" INFINITE");
    }
}

void CLR_RT_GarbageCollector::DumpThreads()
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_FOREACH_NODE(CLR_RT_Thread, th, g_CLR_RT_ExecutionEngine.m_threadsReady)
    {
        th->DumpStack();
    }
    NANOCLR_FOREACH_NODE_END();

    NANOCLR_FOREACH_NODE(CLR_RT_Thread, th, g_CLR_RT_ExecutionEngine.m_threadsWaiting)
    {
        th->DumpStack();
    }
    NANOCLR_FOREACH_NODE_END();

    CLR_Debug::Printf("\r\n");
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

#if NANOCLR_VALIDATE_HEAP >= NANOCLR_VALIDATE_HEAP_3_Compaction

void CLR_RT_GarbageCollector::ValidateCluster(CLR_RT_HeapCluster *hc)
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RT_HeapBlock_Node *ptr = hc->m_payloadStart;
    CLR_RT_HeapBlock_Node *end = hc->m_payloadEnd;

    while (ptr < end)
    {
        hc->ValidateBlock(ptr);

        ptr += ptr->DataSize();
    }
}

void CLR_RT_GarbageCollector::ValidateHeap(CLR_RT_DblLinkedList &lst)
{
    NATIVE_PROFILE_CLR_CORE();

#if defined(NANOCLR_TRACE_MEMORY_STATS)
    if (s_CLR_RT_fTrace_MemoryStats >= c_CLR_RT_Trace_Verbose)
    {
        CLR_Debug::Printf("\r\nGC: Validating Heap\r\n");
    }
#endif

    NANOCLR_FOREACH_NODE(CLR_RT_HeapCluster, hc, lst)
    {
        ValidateCluster(hc);
    }

    NANOCLR_FOREACH_NODE_END();
}

void CLR_RT_GarbageCollector::ValidateBlockNotInFreeList(CLR_RT_DblLinkedList &lst, CLR_RT_HeapBlock_Node *dst)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_FOREACH_NODE(CLR_RT_HeapCluster, hc, lst)
    {
        NANOCLR_FOREACH_NODE(CLR_RT_HeapBlock_Node, ptr, hc->m_freeList)
        {
            CLR_RT_HeapBlock_Node *ptrEnd = ptr + ptr->DataSize();

            if (ptr <= dst && dst < ptrEnd)
            {
#ifdef _WIN64
                CLR_Debug::Printf(
                    "Pointer into free list!! 0x%016" PRIxPTR " 0x%016" PRIxPTR " 0x%016" PRIxPTR "\r\n",
                    (uintptr_t)dst,
                    (uintptr_t)ptr,
                    (uintptr_t)ptrEnd);
#else
                CLR_Debug::Printf("Pointer into free list!! %08x %08x %08x\r\n", dst, ptr, ptrEnd);
#endif

                NANOCLR_DEBUG_STOP();
            }
        }
        NANOCLR_FOREACH_NODE_END();
    }
    NANOCLR_FOREACH_NODE_END();
}

bool CLR_RT_GarbageCollector::IsBlockInFreeList(CLR_RT_DblLinkedList &lst, CLR_RT_HeapBlock_Node *dst, bool fExact)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_FOREACH_NODE(CLR_RT_HeapCluster, hc, lst)
    {
        NANOCLR_FOREACH_NODE(CLR_RT_HeapBlock_Node, ptr, hc->m_freeList)
        {
            if (fExact)
            {
                if (ptr == dst)
                    return true;
            }
            else
            {
                CLR_RT_HeapBlock_Node *ptrEnd = ptr + ptr->DataSize();

                if (ptr <= dst && dst < ptrEnd)
                    return true;
            }
        }
        NANOCLR_FOREACH_NODE_END();
    }
    NANOCLR_FOREACH_NODE_END();

    return false;
}

bool CLR_RT_GarbageCollector::IsBlockInHeap(CLR_RT_DblLinkedList &lst, CLR_RT_HeapBlock_Node *dst)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_FOREACH_NODE(CLR_RT_HeapCluster, hc, lst)
    {
        if (hc->m_payloadStart <= dst && dst < hc->m_payloadEnd)
            return true;
    }
    NANOCLR_FOREACH_NODE_END();

    return false;
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

#if NANOCLR_VALIDATE_HEAP >= NANOCLR_VALIDATE_HEAP_4_CompactionPlus

CLR_RT_GarbageCollector::Rel_List CLR_RT_GarbageCollector::s_lstRecords;
CLR_RT_GarbageCollector::Rel_Map CLR_RT_GarbageCollector::s_mapOldToRecord;
CLR_RT_GarbageCollector::Rel_Map CLR_RT_GarbageCollector::s_mapNewToRecord;

//--//

bool CLR_RT_GarbageCollector::TestPointers_PopulateOld_Worker(void **ref)
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_UINT32 *dst = (CLR_UINT32 *)*ref;

    if (dst)
    {
        RelocationRecord *ptr = (RelocationRecord *)platform_malloc(sizeof(RelocationRecord));

        if (!ptr)
        {
            return false;
        }

        s_lstRecords.push_back(ptr);

        ptr->oldRef = ref;
        ptr->oldPtr = dst;

        ptr->newRef = NULL;
        ptr->newPtr = NULL;

        ptr->data = *dst;

        if (s_mapOldToRecord.find(ref) != s_mapOldToRecord.end())
        {
#ifdef _WIN64
            CLR_Debug::Printf("Duplicate base OLD: 0x%016" PRIxPTR "\r\n", (uintptr_t)ref);
#else
            CLR_Debug::Printf("Duplicate base OLD: %08x\r\n", ref);
#endif
            // need to free the memory allocated for the record
            platform_free(ptr);

            NANOCLR_DEBUG_STOP();
        }

        s_mapOldToRecord[ref] = ptr;

        if (IsBlockInFreeList(g_CLR_RT_ExecutionEngine.m_heap, (CLR_RT_HeapBlock_Node *)dst, false))
        {
#ifdef _WIN64
            CLR_Debug::Printf("Some data points into a free list: 0x%016" PRIxPTR "\r\n", (uintptr_t)dst);
#else
            CLR_Debug::Printf("Some data points into a free list: %08x\r\n", dst);
#endif

            // need to free the memory allocated for the record
            platform_free(ptr);

            NANOCLR_DEBUG_STOP();
        }
    }

    return true;
}

void CLR_RT_GarbageCollector::TestPointers_PopulateOld()
{
    NATIVE_PROFILE_CLR_CORE();
    Rel_List_Iter itLst;

    for (itLst = s_lstRecords.begin(); itLst != s_lstRecords.end(); itLst++)
    {
        RelocationRecord *ptr = *itLst;

        // need to free the memory allocated for the record
        platform_free(ptr);
    }

    s_lstRecords.clear();
    s_mapOldToRecord.clear();
    s_mapNewToRecord.clear();

    //--//
#if defined(NANOCLR_TRACE_MEMORY_STATS)
    if (s_CLR_RT_fTrace_MemoryStats >= c_CLR_RT_Trace_Verbose)
    {
        CLR_Debug::Printf("\r\nGC: Testing pointers, populating 'old' objects\r\n");
    }
#endif

    Heap_Relocate_Pass(TestPointers_PopulateOld_Worker);
}

void CLR_RT_GarbageCollector::Relocation_UpdatePointer(void **ref)
{
    (void)ref;

    NATIVE_PROFILE_CLR_CORE();
}

//--//

void CLR_RT_GarbageCollector::TestPointers_Remap()
{
    NATIVE_PROFILE_CLR_CORE();
    Rel_Map_Iter it;

    for (it = s_mapOldToRecord.begin(); it != s_mapOldToRecord.end(); it++)
    {
        RelocationRecord *ptr = it->second;
        void **ref = it->first;
        CLR_RT_GarbageCollector::Relocation_UpdatePointer((void **)&ref);
        CLR_UINT32 *dst = ptr->oldPtr;
        CLR_RT_GarbageCollector::Relocation_UpdatePointer((void **)&dst);

        if (s_mapNewToRecord.find(ref) != s_mapNewToRecord.end())
        {
#ifdef _WIN64
            CLR_Debug::Printf("Duplicate base NEW: 0x%016" PRIxPTR "\r\n", (uintptr_t)ref);
#else
            CLR_Debug::Printf("Duplicate base NEW: %08x\r\n", ref);
#endif
            NANOCLR_DEBUG_STOP();
        }

        s_mapNewToRecord[ref] = ptr;

        ptr->newRef = ref;
        ptr->newPtr = dst;
    }
}

//--//

bool CLR_RT_GarbageCollector::TestPointers_PopulateNew_Worker(void **ref)
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_UINT32 *dst = (CLR_UINT32 *)*ref;

    if (dst)
    {
        Rel_Map_Iter it = s_mapNewToRecord.find(ref);

        if (it != s_mapNewToRecord.end())
        {
            RelocationRecord *ptr = it->second;

            if (ptr->newPtr != dst)
            {
#ifdef _WIN64
                CLR_Debug::Printf(
                    "Bad pointer: 0x%016" PRIxPTR " 0x%016" PRIxPTR "\r\n",
                    (uintptr_t)ptr->newPtr,
                    (uintptr_t)dst);
#else
                CLR_Debug::Printf("Bad pointer: %08x %08x\r\n", ptr->newPtr, dst);
#endif
                NANOCLR_DEBUG_STOP();
            }
            else if (ptr->data != *dst)
            {
#ifdef _WIN64
                CLR_Debug::Printf(
                    "Bad data: 0x%016" PRIxPTR " 0x%016" PRIxPTR "\r\n",
                    (uintptr_t)ptr->data,
                    (uintptr_t)*dst);
#else
                CLR_Debug::Printf("Bad data: %08x %08x\r\n", ptr->data, *dst);
#endif

                NANOCLR_DEBUG_STOP();
            }

            if (IsBlockInFreeList(g_CLR_RT_ExecutionEngine.m_heap, (CLR_RT_HeapBlock_Node *)dst, false))
            {
#ifdef _WIN64
                CLR_Debug::Printf("Some data points into a free list: 0x%016" PRIxPTR "\r\n", (uintptr_t)dst);
#else
                CLR_Debug::Printf("Some data points into a free list: %08x\r\n", dst);
#endif

                NANOCLR_DEBUG_STOP();
            }

            return true;
        }
        else
        {
#ifdef _WIN64
            CLR_Debug::Printf("Bad base: 0x%016" PRIxPTR "\r\n", (uintptr_t)ref);
#else
            CLR_Debug::Printf("Bad base: 0x%08x\r\n", ref);
#endif

            NANOCLR_DEBUG_STOP();
        }
    }

    return false;
}

void CLR_RT_GarbageCollector::TestPointers_PopulateNew()
{
    NATIVE_PROFILE_CLR_CORE();
    Heap_Relocate_Pass(TestPointers_PopulateNew_Worker);
}

#endif
