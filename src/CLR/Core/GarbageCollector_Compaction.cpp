//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "Core.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

CLR_UINT32 CLR_RT_GarbageCollector::ExecuteCompaction()
{
    NATIVE_PROFILE_CLR_CORE();

    // bump the number of heap compactions
    m_numberOfCompactions++;

#if defined(NANOCLR_PROFILE_NEW_ALLOCATIONS)
    g_CLR_PRF_Profiler.RecordHeapCompactionBegin();
#endif

#if defined(NANOCLR_TRACE_MEMORY_STATS)

    CLR_UINT64 stats_start = HAL_Time_CurrentSysTicks();
    int ellapsedTimeMilliSec = 0;

    if (s_CLR_RT_fTrace_MemoryStats >= c_CLR_RT_Trace_Info)
    {
        CLR_Debug::Printf(
            "\r\n\r\nGC: Relocation - performing heap compaction run #%d @ %s\r\n",
            m_numberOfCompactions,
            HAL_Time_CurrentDateTimeToString());
    }
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////

    CLR_RT_ExecutionEngine::ExecutionConstraint_Suspend();

    Heap_Compact();

    CLR_RT_ExecutionEngine::ExecutionConstraint_Resume();

    ////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(NANOCLR_PROFILE_NEW_ALLOCATIONS)
    g_CLR_PRF_Profiler.RecordHeapCompactionEnd();
#endif

#if defined(NANOCLR_TRACE_MEMORY_STATS)
    if (s_CLR_RT_fTrace_MemoryStats >= c_CLR_RT_Trace_Info)
    {
        CLR_INT64 elapsed = HAL_Time_CurrentSysTicks() - stats_start;
        ellapsedTimeMilliSec =
            (int)((::HAL_Time_SysTicksToTime(elapsed) + TIME_CONVERSION__TICKUNITS - 1) / TIME_CONVERSION__TICKUNITS);

        CLR_Debug::Printf(
            "\r\n\r\nGC: Relocation - heap compaction run #%d completed (took %dmsec)\r\n",
            m_numberOfCompactions,
            ellapsedTimeMilliSec);
    }
#endif

    return 0;
}

////////////////////////////////////////////////////////////////////////////////

void CLR_RT_GarbageCollector::Heap_Compact()
{
    NATIVE_PROFILE_CLR_CORE();

    ValidatePointers();

    //--//

    RelocationRegion relocHelper[c_minimumSpaceForCompact];
    const size_t relocMax = ARRAYSIZE(relocHelper);

    memset(relocHelper, 0, sizeof(relocHelper));

    Heap_Relocate_Prepare(relocHelper, relocMax);

    RelocationRegion *relocBlocks = relocHelper;
    RelocationRegion *relocCurrent = relocBlocks;

    //--//

#if defined(NANOCLR_TRACE_MEMORY_STATS)
    if (s_CLR_RT_fTrace_MemoryStats >= c_CLR_RT_Trace_Verbose)
    {
        CLR_Debug::Printf("\r\nGC: Heap relocate prepare\r\n");
    }
#endif

    TestPointers_PopulateOld();

    CLR_RT_HeapCluster *freeRegion_hc = NULL;

    CLR_RT_HeapBlock_Node *freeRegion = NULL;

    CLR_RT_HeapCluster *currentSource_hc = (CLR_RT_HeapCluster *)g_CLR_RT_ExecutionEngine.m_heap.FirstNode();
    while (currentSource_hc->Next())
    {
        CLR_RT_HeapBlock_Node *currentSource = currentSource_hc->m_payloadStart;
        CLR_RT_HeapBlock_Node *currentSource_end = currentSource_hc->m_payloadEnd;

        if (!freeRegion)
        {
            //
            // Move to the next first free region.
            //
            freeRegion_hc = (CLR_RT_HeapCluster *)g_CLR_RT_ExecutionEngine.m_heap.FirstNode();

            while (true)
            {
                CLR_RT_HeapCluster *freeRegion_hcNext = (CLR_RT_HeapCluster *)freeRegion_hc->Next();

                if (!freeRegion_hcNext)
                {
                    break;
                }

                freeRegion = freeRegion_hc->m_freeList.FirstNode();

                if (freeRegion->Next())
                {
                    break;
                }

                freeRegion = NULL;
                freeRegion_hc = freeRegion_hcNext;
            }

            if (!freeRegion)
            {
                break;
            }
        }

        while (true)
        {
            //
            // We can only move backwards.
            //
            if (currentSource < freeRegion)
            {
                currentSource_hc = freeRegion_hc;
                currentSource = freeRegion;
                currentSource_end = freeRegion_hc->m_payloadEnd;
            }

            while (currentSource < currentSource_end && currentSource->IsFlagSet(CLR_RT_HeapBlock::HB_Unmovable))
            {
                currentSource += currentSource->DataSize();
            }

            if (currentSource == currentSource_end)
            {
                break;
            }

            //////////////////////////////////////////////////////
            //
            // At this point, we have at least ONE movable block.
            //
            //////////////////////////////////////////////////////

#if NANOCLR_VALIDATE_HEAP >= NANOCLR_VALIDATE_HEAP_4_CompactionPlus
            if (IsBlockInFreeList(g_CLR_RT_ExecutionEngine.m_heap, freeRegion, true) == false)
            {
                CLR_Debug::Printf("'freeRegion' is not in a free list!! %08x\r\n", freeRegion);

                NANOCLR_DEBUG_STOP();
            }

            if (IsBlockInFreeList(g_CLR_RT_ExecutionEngine.m_heap, currentSource, false))
            {
                CLR_Debug::Printf("'currentSource' is in a free list!! %08x\r\n", currentSource);

                NANOCLR_DEBUG_STOP();
            }
#endif

            if (m_relocCount >= relocMax)
            {
                ValidateHeap(g_CLR_RT_ExecutionEngine.m_heap);

                Heap_Relocate();

                ValidateHeap(g_CLR_RT_ExecutionEngine.m_heap);

                relocBlocks = m_relocBlocks;
                relocCurrent = relocBlocks;

                TestPointers_PopulateOld();
            }

            {
                CLR_UINT32 move = 0;
                CLR_UINT32 freeRegion_Size = freeRegion->DataSize();
                bool fSlide;

                relocCurrent->m_destination = (CLR_UINT8 *)freeRegion;
                relocCurrent->m_start = (CLR_UINT8 *)currentSource;

#ifdef _WIN64
                relocCurrent->m_offset = (CLR_UINT64)(relocCurrent->m_destination - relocCurrent->m_start);
#else
                relocCurrent->m_offset = (CLR_UINT32)(relocCurrent->m_destination - relocCurrent->m_start);
#endif

                //
                // Are the free block and the last moved block adjacent?
                //
                if (currentSource == freeRegion + freeRegion_Size)
                {
                    while (currentSource < currentSource_end &&
                           currentSource->IsFlagSet(CLR_RT_HeapBlock::HB_Unmovable) == false)
                    {
                        CLR_UINT32 len = currentSource->DataSize();

                        currentSource += len;
                        move += len;
                    }

                    fSlide = true;
                }
                else
                {
                    while (freeRegion_Size && currentSource < currentSource_end &&
                           currentSource->IsFlagSet(CLR_RT_HeapBlock::HB_Unmovable) == false)
                    {
                        CLR_UINT32 len = currentSource->DataSize();

                        if (freeRegion_Size < len)
                        {
                            break;
                        }

                        freeRegion_Size -= len;
                        currentSource += len;
                        move += len;
                    }

                    fSlide = false;
                }

                if (move)
                {
                    //
                    // Skip forward to the next movable block.
                    //
                    while (currentSource < currentSource_end &&
                           currentSource->IsFlagSet(CLR_RT_HeapBlock::HB_Unmovable))
                    {
                        currentSource += currentSource->DataSize();
                    }

                    CLR_UINT32 moveBytes = move * sizeof(*currentSource);

                    relocCurrent->m_end = relocCurrent->m_start + moveBytes;

                    //--//

                    //
                    // Remove the old free block, copy the data, recreate the new free block.
                    // Merge with the following one if they are adjacent now.
                    //
                    CLR_RT_HeapBlock_Node *freeRegionNext = freeRegion->Next();

                    freeRegion->Unlink();

#ifdef DEBUG

                    _ASSERTE(relocCurrent->m_destination >= (CLR_UINT8 *)freeRegion_hc->m_payloadStart);
                    _ASSERTE(relocCurrent->m_destination < (CLR_UINT8 *)freeRegion_hc->m_payloadEnd);
                    _ASSERTE(relocCurrent->m_start >= (CLR_UINT8 *)currentSource_hc->m_payloadStart);
                    _ASSERTE(relocCurrent->m_start < (CLR_UINT8 *)currentSource_hc->m_payloadEnd);
                    _ASSERTE(moveBytes == (move * sizeof(CLR_RT_HeapBlock)));

#endif

                    memmove(relocCurrent->m_destination, relocCurrent->m_start, moveBytes);

                    if (freeRegion_Size)
                    {

                        freeRegion = freeRegion_hc->InsertInOrder(freeRegion + move, freeRegion_Size);
                    }
                    else
                    {
                        freeRegion = freeRegionNext;
                    }

                    if (fSlide == false)
                    {
                        CLR_RT_HeapBlock_Node *dst =
                            currentSource_hc->InsertInOrder((CLR_RT_HeapBlock_Node *)relocCurrent->m_start, move);

                        if (dst < freeRegion && freeRegion < (dst + dst->DataSize()))
                        {
                            freeRegion = dst;
                        }
                    }

                    CLR_RT_GarbageCollector::ValidateCluster(currentSource_hc);
                    CLR_RT_GarbageCollector::ValidateCluster(freeRegion_hc);

                    relocCurrent++;
                    m_relocCount++;
                }
                else
                {
                    freeRegion = freeRegion->Next();
                }

                if (freeRegion->Next() == NULL)
                {

                    freeRegion = NULL;
                    freeRegion_hc = (CLR_RT_HeapCluster *)freeRegion_hc->Next();

                    while (true)
                    {
                        CLR_RT_HeapCluster *freeRegion_hcNext = (CLR_RT_HeapCluster *)freeRegion_hc->Next();
                        if (!freeRegion_hcNext)
                        {
                            break;
                        }

                        freeRegion = freeRegion_hc->m_freeList.FirstNode();

                        if (freeRegion != NULL && freeRegion->Next())
                        {
                            break;
                        }

                        freeRegion = NULL;
                        freeRegion_hc = freeRegion_hcNext;
                    }

                    if (!freeRegion)
                    {
                        break;
                    }
                }
            }
        }

        currentSource_hc = (CLR_RT_HeapCluster *)currentSource_hc->Next();
    }

    if (m_relocCount)
    {
        ValidateHeap(g_CLR_RT_ExecutionEngine.m_heap);

        Heap_Relocate();

        ValidateHeap(g_CLR_RT_ExecutionEngine.m_heap);
    }
}

void CLR_RT_GarbageCollector::Heap_Relocate_Prepare(RelocationRegion *blocks, size_t total)
{
    NATIVE_PROFILE_CLR_CORE();

#if defined(NANOCLR_TRACE_MEMORY_STATS)
    if (s_CLR_RT_fTrace_MemoryStats >= c_CLR_RT_Trace_Verbose)
    {
        CLR_Debug::Printf("\r\nGC: Relocation - prepare\r\n");
    }
#endif

    m_relocBlocks = blocks;
    m_relocTotal = total;
    m_relocCount = 0;
}

void CLR_RT_GarbageCollector::Heap_Relocate_AddBlock(CLR_UINT8 *dst, CLR_UINT8 *src, CLR_UINT32 length)
{
    NATIVE_PROFILE_CLR_CORE();
    RelocationRegion *reloc = m_relocBlocks;
    size_t count = m_relocCount;

    while (count)
    {
        if (reloc->m_start > src)
        {
            //
            // Insert region, so they are sorted by start address.
            //
            memmove(&reloc[1], &reloc[0], count * sizeof(*reloc));
            break;
        }

        reloc++;
        count--;
    }

    reloc->m_start = src;
    reloc->m_end = &src[length];
    reloc->m_destination = dst;
#ifdef _WIN64
    reloc->m_offset = (CLR_UINT64)(dst - src);
#else
    reloc->m_offset = (CLR_UINT32)(dst - src);
#endif

    if (++m_relocCount == m_relocTotal)
    {
        Heap_Relocate();
    }
}

void CLR_RT_GarbageCollector::Heap_Relocate()
{
    NATIVE_PROFILE_CLR_CORE();

#if defined(NANOCLR_TRACE_MEMORY_STATS)
    if (s_CLR_RT_fTrace_MemoryStats >= c_CLR_RT_Trace_Verbose)
    {
        CLR_Debug::Printf("\r\nGC: Relocating Heap\r\n");
    }
#endif

    if (m_relocCount)
    {
        RelocationRegion *relocBlocks = m_relocBlocks;

        CLR_UINT8 *relocMinimum = relocBlocks->m_start;
        CLR_UINT8 *relocMaximum = relocBlocks->m_end;

        for (size_t i = 0; i < m_relocCount; i++, relocBlocks++)
        {
            if (relocMinimum > relocBlocks->m_start)
            {
                relocMinimum = relocBlocks->m_start;
            }

            if (relocMaximum < relocBlocks->m_end)
            {
                relocMaximum = relocBlocks->m_end;
            }
        }

        m_relocMinimum = relocMinimum;
        m_relocMaximum = relocMaximum;

        TestPointers_Remap();

        Heap_Relocate_Pass(NULL);

#if defined(NANOCLR_PROFILE_NEW_ALLOCATIONS)
        g_CLR_PRF_Profiler.TrackObjectRelocation();
#endif

        ValidatePointers();

        TestPointers_PopulateNew();

        m_relocCount = 0;
    }
}

void CLR_RT_GarbageCollector::Heap_Relocate_Pass(RelocateFtn ftn)
{
    NATIVE_PROFILE_CLR_CORE();

#if NANOCLR_VALIDATE_HEAP > NANOCLR_VALIDATE_HEAP_0_None
    m_relocWorker = ftn;
#else
    (void)ftn;
#endif

#if defined(NANOCLR_TRACE_MEMORY_STATS)
    if (s_CLR_RT_fTrace_MemoryStats >= c_CLR_RT_Trace_Verbose)
    {
        CLR_Debug::Printf("\r\nGC: Relocation - pass\r\n");
    }
#endif

    NANOCLR_FOREACH_NODE(CLR_RT_HeapCluster, hc, g_CLR_RT_ExecutionEngine.m_heap)
    {
        CLR_RT_HeapBlock_Node *ptr = hc->m_payloadStart;
        CLR_RT_HeapBlock_Node const *end = hc->m_payloadEnd;

        while (ptr < end)
        {
            // check pointer
            _ASSERTE(ptr >= hc->m_payloadStart && ptr <= hc->m_payloadEnd);

            CLR_RT_HEAPBLOCK_RELOCATE(ptr);

            ptr += ptr->DataSize();
        }
    }
    NANOCLR_FOREACH_NODE_END();

    g_CLR_RT_ExecutionEngine.Relocate();
}

//--//

void CLR_RT_GarbageCollector::Heap_Relocate(CLR_RT_HeapBlock *lst, CLR_UINT32 len)
{
    NATIVE_PROFILE_CLR_CORE();

#if defined(NANOCLR_TRACE_MEMORY_STATS)
    if (s_CLR_RT_fTrace_MemoryStats >= c_CLR_RT_Trace_Verbose)
    {
        CLR_Debug::Printf("\r\nGC: Relocating\r\n");
    }
#endif

    while (len--)
    {
        CLR_RT_HEAPBLOCK_RELOCATE(lst);

        lst++;
    }
}

void CLR_RT_GarbageCollector::Heap_Relocate(void **ref)
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_UINT8 *dst = (CLR_UINT8 *)*ref;

    void *destinationAddress;

#if NANOCLR_VALIDATE_HEAP > NANOCLR_VALIDATE_HEAP_0_None
    if (g_CLR_RT_GarbageCollector.m_relocWorker)
    {

#if defined(NANOCLR_TRACE_MEMORY_STATS)
        if (s_CLR_RT_fTrace_MemoryStats >= c_CLR_RT_Trace_Verbose)
        {
            CLR_Debug::Printf("\r\nGC: Relocating with worker\r\n");
        }
#endif

        g_CLR_RT_GarbageCollector.m_relocWorker(ref);
    }
    else
#endif
    {

#if defined(NANOCLR_TRACE_MEMORY_STATS)
        if (s_CLR_RT_fTrace_MemoryStats >= c_CLR_RT_Trace_Verbose)
        {
            if (dst == nullptr)
            {
                // nothing to do here
                CLR_Debug::Printf("\r\nGC: Skipping relocation as referenced object is null.\r\n");
                return;
            }
            else
            {
                CLR_Debug::Printf("\r\nGC: Relocating Heap\r\n");
            }
        }
#endif

        if (dst >= g_CLR_RT_GarbageCollector.m_relocMinimum && dst < g_CLR_RT_GarbageCollector.m_relocMaximum)
        {
            RelocationRegion const *relocBlocks = g_CLR_RT_GarbageCollector.m_relocBlocks;
            size_t left = 0;
            size_t right = g_CLR_RT_GarbageCollector.m_relocCount;

            while (left < right)
            {
                size_t center = (left + right) / 2;
                RelocationRegion const &relocCurrent = relocBlocks[center];

                if (dst < relocCurrent.m_start)
                {
                    right = center;
                }
                else if (dst >= relocCurrent.m_end)
                {
                    left = center + 1;
                }
                else
                {
                    destinationAddress = (void *)(dst + relocCurrent.m_offset);

#if defined(NANOCLR_PROFILE_NEW_ALLOCATIONS)
                    g_CLR_PRF_Profiler.TrackObjectRelocation(*ref, destinationAddress);
#endif

                    *ref = destinationAddress;

                    return;
                }
            }
        }
    }
}

#if NANOCLR_VALIDATE_HEAP >= NANOCLR_VALIDATE_HEAP_3_Compaction

bool CLR_RT_GarbageCollector::Relocation_JustCheck(void **ref)
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_UINT8 *dst = (CLR_UINT8 *)*ref;

#if defined(NANOCLR_TRACE_MEMORY_STATS)
    if (s_CLR_RT_fTrace_MemoryStats >= c_CLR_RT_Trace_Verbose)
    {
        CLR_Debug::Printf("\r\nGC: Relocation - just checking\r\n");
    }
#endif

    if (dst)
    {
        ValidateBlockNotInFreeList(g_CLR_RT_ExecutionEngine.m_heap, (CLR_RT_HeapBlock_Node *)dst);
    }

    return true;
}

#endif
