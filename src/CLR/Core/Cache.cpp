//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "Core.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(NANOCLR_USE_AVLTREE_FOR_METHODLOOKUP)

int CLR_RT_EventCache::LookupEntry::Callback_Compare(
    void *state,
    CLR_RT_AVLTree::Entry *left,
    CLR_RT_AVLTree::Entry *right)
{
    NATIVE_PROFILE_CLR_CORE();

    LookupEntry *leftDirect = (LookupEntry *)left;
    LookupEntry *rightDirect = (LookupEntry *)right;

    return leftDirect->m_payload.Compare(rightDirect->m_payload);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void CLR_RT_EventCache::VirtualMethodTable::Initialize()
{
    NATIVE_PROFILE_CLR_CORE();

    m_entries = (Payload *)&g_scratchVirtualMethodPayload[0];

    LookupEntry *node;
    size_t i;

    m_tree.Initialize();

    m_list_freeItems.DblLinkedList_Initialize();
    m_list_inUse.DblLinkedList_Initialize();

    m_tree.m_owner.m_ftn_compare = LookupEntry ::Callback_Compare;
    m_tree.m_owner.m_ftn_newNode = VirtualMethodTable::Callback_NewNode;
    m_tree.m_owner.m_ftn_freeNode = VirtualMethodTable::Callback_FreeNode;
    m_tree.m_owner.m_ftn_reassignNode = VirtualMethodTable::Callback_Reassign;
    m_tree.m_owner.m_state = this;

    for (i = 0, node = m_entries; i < PayloadArraySize(); i++, node++)
    {
        node->GenericNode_Initialize();

        m_list_freeItems.LinkAtBack(node);
    }
}

bool CLR_RT_EventCache::VirtualMethodTable::FindVirtualMethod(
    const CLR_RT_TypeDef_Index &cls,
    const CLR_RT_MethodDef_Index &mdVirtual,
    CLR_RT_MethodDef_Index &md)
{
    NATIVE_PROFILE_CLR_CORE();
    LookupEntry *en;
    LookupEntry key;
    CLR_RT_MethodDef_Instance instMD;
    CLR_RT_TypeDef_Instance instCLS;

    instMD.InitializeFromIndex(mdVirtual);
    instCLS.InitializeFromMethod(instMD);

    //
    // Shortcut for terminal virtual methods.
    //
    if (cls.m_data == instCLS.m_data)
    {
        if ((instMD.m_target->flags & CLR_RECORD_METHODDEF::MD_Abstract) == 0)
        {
            md = mdVirtual;

            return true;
        }
    }

    key.m_payload.m_mdVirtual = mdVirtual;
    key.m_payload.m_cls = cls;
    key.m_payload.m_md.Clear();

    en = (LookupEntry *)m_tree.Find(&key);
    if (en)
    {
        md = en->m_payload.m_md;

        //
        // Move the node to the top of the MRU list.
        //
        m_list_inUse.LinkAtFront(en);

        return true;
    }

    {
        if (g_CLR_RT_TypeSystem.FindVirtualMethodDef(cls, mdVirtual, md) == false)
        {
            return false;
        }
    }

    {
        if (m_list_freeItems.IsEmpty())
        {
            en = (LookupEntry *)m_list_inUse.LastNode();
            if (en->Prev() == nullptr)
            {
                //
                // No node to steal, return.
                //
                return true;
            }

            m_tree.Remove(en);

            CLR_PROF_Handler::SuspendTime();
            if (!ConsistencyCheck())
            {
                DumpTree();
            }
            CLR_PROF_Handler::ResumeTime();
        }

        key.m_payload.m_md = md;
        m_tree.Insert(&key);

        CLR_PROF_Handler::SuspendTime();
        if (!ConsistencyCheck())
        {
            DumpTree();
        }
        CLR_PROF_Handler::ResumeTime();
    }

    return true;
}

//--//

CLR_RT_AVLTree::Entry *CLR_RT_EventCache::VirtualMethodTable::Callback_NewNode(
    void *state,
    CLR_RT_AVLTree::Entry *payload)
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RT_EventCache::VirtualMethodTable *pThis = (CLR_RT_EventCache::VirtualMethodTable *)state;
    LookupEntry *payloadDirect = (LookupEntry *)payload;
    LookupEntry *en;

    en = (LookupEntry *)pThis->m_list_freeItems.ExtractFirstNode();
    if (en)
    {
        en->m_payload = payloadDirect->m_payload;

        pThis->m_list_inUse.LinkAtFront(en);
    }

    return en;
}

void CLR_RT_EventCache::VirtualMethodTable::Callback_FreeNode(void *state, CLR_RT_AVLTree::Entry *node)
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RT_EventCache::VirtualMethodTable *pThis = (CLR_RT_EventCache::VirtualMethodTable *)state;
    LookupEntry *nodeDirect = (LookupEntry *)node;

    pThis->m_list_freeItems.LinkAtBack(nodeDirect);
}

void CLR_RT_EventCache::VirtualMethodTable::Callback_Reassign(
    void *state,
    CLR_RT_AVLTree::Entry *from,
    CLR_RT_AVLTree::Entry *to)
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RT_EventCache::VirtualMethodTable *pThis = (CLR_RT_EventCache::VirtualMethodTable *)state;
    LookupEntry *fromDirect = (LookupEntry *)from;
    LookupEntry *toDirect = (LookupEntry *)to;

    pThis->m_list_inUse.InsertAfterNode(fromDirect, toDirect);

    toDirect->m_payload = fromDirect->m_payload;
}

#else

// uncomment the following line to enable diagnostics for VMT structure sizes and alignment
// #define DEBUG_VMT_DIAGNOSTICS

void CLR_RT_EventCache::VirtualMethodTable::Initialize()
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_UINT32 index;

    m_entries = (Link *)&g_scratchVirtualMethodTableLink[0];
    m_entriesMRU = (Link *)&g_scratchVirtualMethodTableLinkMRU[0];
    m_payloads = (Payload *)&g_scratchVirtualMethodPayload[0];

#ifdef DEBUG_VMT_DIAGNOSTICS

    // DIAGNOSTIC: Verify structure sizes and alignment
    CLR_Debug::Printf("\r\n========== VirtualMethodTable::Initialize DIAGNOSTICS ==========\r\n");
    CLR_Debug::Printf("sizeof(Link) = %u, alignof(Link) = %u\r\n", sizeof(Link), alignof(Link));
    CLR_Debug::Printf("sizeof(Payload) = %u, alignof(Payload) = %u\r\n", sizeof(Payload), alignof(Payload));
    CLR_Debug::Printf("sizeof(Payload::Key) = %u\r\n", sizeof(Payload::Key));
    CLR_Debug::Printf("LinkArraySize() = %u (expected: 641)\r\n", LinkArraySize());
    CLR_Debug::Printf("LinkMRUArraySize() = %u (expected: 513)\r\n", LinkMRUArraySize());
    CLR_Debug::Printf("PayloadArraySize() = %u (expected: 512)\r\n", PayloadArraySize());

    // Verify array base addresses don't overlap
    uintptr_t entries_start = (uintptr_t)m_entries;
    uintptr_t entries_end = entries_start + (LinkArraySize() * sizeof(Link));
    uintptr_t entriesMRU_start = (uintptr_t)m_entriesMRU;
    uintptr_t entriesMRU_end = entriesMRU_start + (LinkMRUArraySize() * sizeof(Link));
    uintptr_t payloads_start = (uintptr_t)m_payloads;
    uintptr_t payloads_end = payloads_start + (PayloadArraySize() * sizeof(Payload));

#ifdef _WIN64

    CLR_Debug::Printf(
        "m_entries: 0x%" PRIx64 " - 0x% " PRIx64 " (%u bytes)\r\n",
        entries_start,
        entries_end,
        (unsigned int)(entries_end - entries_start));
    CLR_Debug::Printf(
        "m_entriesMRU: 0x%" PRIx64 " - 0x% " PRIx64 " (%u bytes)\r\n",
        entriesMRU_start,
        entriesMRU_end,
        (unsigned int)(entriesMRU_end - entriesMRU_start));
    CLR_Debug::Printf(
        "m_payloads:   0x% " PRIx64 " - 0x% " PRIx64 " (%u bytes)\r\n",
        payloads_start,
        payloads_end,
        (unsigned int)(payloads_end - payloads_start));

#else

    CLR_Debug::Printf(
        "m_entries: 0x%08X - 0x%08X (%u bytes)\r\n",
        entries_start,
        entries_end,
        (unsigned int)(entries_end - entries_start));
    CLR_Debug::Printf(
        "m_entriesMRU: 0x%08X - 0x%08X (%u bytes)\r\n",
        entriesMRU_start,
        entriesMRU_end,
        (unsigned int)(entriesMRU_end - entriesMRU_start));
    CLR_Debug::Printf(
        "m_payloads:   0x%08X - 0x%08X (%u bytes)\r\n",
        payloads_start,
        payloads_end,
        (unsigned int)(payloads_end - payloads_start));

#endif

    // Check for overlaps
    if (entries_end > entriesMRU_start && entries_start < entriesMRU_end)
    {
        CLR_Debug::Printf("*** WARNING: m_entries and m_entriesMRU OVERLAP! ***\r\n");
    }
    if (entries_end > payloads_start && entries_start < payloads_end)
    {
        CLR_Debug::Printf("*** WARNING: m_entries and m_payloads OVERLAP! ***\r\n");
    }
    if (entriesMRU_end > payloads_start && entriesMRU_start < payloads_end)
    {
        CLR_Debug::Printf("*** WARNING: m_entriesMRU and m_payloads OVERLAP! ***\r\n");
    }

    CLR_Debug::Printf("================================================================\r\n\r\n");

#endif

    //
    // Link all the entries to themselves => no elements in the lists.
    //
    for (index = 0; index < LinkArraySize(); index++)
    {
        Link &lnk = m_entries[index];

        lnk.m_next = index;
        lnk.m_prev = index;
    }

    //
    // Link all the entries to the following one => all the elements are in the MRU list.
    //
    _ASSERTE(LinkMRUArraySize() < 0xFFFF);

    for (index = 0; index < LinkMRUArraySize(); index++)
    {
        Link &lnk = m_entriesMRU[index];

        lnk.m_next = index == LinkMRUArraySize() - 1 ? 0 : index + 1;
        lnk.m_prev = index == 0 ? (CLR_UINT16)LinkMRUArraySize() - 1 : index - 1;
    }
}

bool CLR_RT_EventCache::VirtualMethodTable::FindVirtualMethod(
    const CLR_RT_TypeDef_Index &cls,
    const CLR_RT_MethodDef_Index &mdVirtual,
    CLR_RT_MethodDef_Index &md)
{
    NATIVE_PROFILE_CLR_CORE();
    Payload::Key key;
    CLR_UINT32 index;
    CLR_UINT32 indexHead;
    CLR_UINT32 clsData = cls.data;
    CLR_UINT32 mdVirtualData = mdVirtual.data;

#if defined(VIRTUAL_DEVICE)
    bool fVerify = false;

    {
        CLR_RT_MethodDef_Instance instMD{};
        CLR_RT_TypeDef_Instance instCLS{};

        instMD.InitializeFromIndex(mdVirtual);
        instCLS.InitializeFromMethod(instMD);

        //
        // Shortcut for terminal virtual methods.
        //
        if (clsData == instCLS.data)
        {
            if ((instMD.target->flags & CLR_RECORD_METHODDEF::MD_Abstract) == 0)
            {
                md = mdVirtual;

                fVerify = true;
            }
        }
    }
#endif

    if (cls.Assembly() == mdVirtual.Assembly())
    {
        CLR_RT_Assembly *assm = g_CLR_RT_TypeSystem.m_assemblies[mdVirtual.Assembly() - 1];
        CLR_INDEX owner = assm->crossReferenceMethodDef[mdVirtual.Method()].GetOwner();

        if (cls.Type() == owner)
        {
#if defined(VIRTUAL_DEVICE)
            if (fVerify != true)
            {
                CLR_Debug::Printf(
                    "INTERNAL ERROR: Shortcut for terminal virtual methods failed: CLS:%08x:%08x => %08x\r\n",
                    cls.data,
                    mdVirtual.data,
                    md.data);
                ::DebugBreak();
            }
#endif

            md = mdVirtual;

            return true;
        }
    }

#if defined(VIRTUAL_DEVICE)
    if (fVerify != false)
    {
        CLR_Debug::Printf(
            "INTERNAL ERROR: Shortcut for terminal virtual methods failed: CLS:%08x:%08x\r\n",
            cls.data,
            mdVirtual.data);
        ::DebugBreak();
    }
#endif

    key.m_mdVirtual.data = mdVirtualData;
    key.m_cls.data = clsData;

    indexHead =
        (SUPPORT_ComputeCRC(&key, sizeof(key), 0) % (LinkArraySize() - PayloadArraySize())) + PayloadArraySize();

    for (index = m_entries[indexHead].m_next;; index = m_entries[index].m_next)
    {
#if defined(DEBUG_VMT_DIAGNOSTICS) && defined(_WIN64)
        CLR_Debug::Printf("  Loop: index=%u, indexHead=%u\r\n", index, indexHead);
#endif

        // validate index before using it to prevent crashes from corrupted data
        if (index >= LinkArraySize())
        {
            // !! corrupted index detected !!
            // // repair the hash chain and treat as cache miss
            m_entries[indexHead].m_next = indexHead;
            m_entries[indexHead].m_prev = indexHead;

            index = indexHead;
        }

        if (index != indexHead)
        {
            _ASSERTE(index < PayloadArraySize());

            Payload &res = m_payloads[index];

            if (res.m_key.m_mdVirtual.data != mdVirtualData)
            {
                continue;
            }

            if (res.m_key.m_cls.data != clsData)
            {
                continue;
            }

            md = res.m_md;

            break;
        }
        else
        {
            if (g_CLR_RT_TypeSystem.FindVirtualMethodDef(cls, mdVirtual, md) == false)
            {
                return false;
            }

            index = GetNewEntry();

#if defined(DEBUG_VMT_DIAGNOSTICS) && defined(_WIN64)
            CLR_Debug::Printf("  GetNewEntry returned: %u\r\n", index);
#endif

            // initialize the entry's links before use to prevent corruption
            m_entries[index].m_next = index;
            m_entries[index].m_prev = index;

            Payload &res = m_payloads[index];

            res.m_md = md;
            res.m_key = key;

            break;
        }
    }

    MoveEntryToTop(m_entries, indexHead, index);
    MoveEntryToTop(m_entriesMRU, LinkMRUArraySize() - 1, index);

    return true;
}

void CLR_RT_EventCache::VirtualMethodTable::MoveEntryToTop(Link *entries, CLR_UINT32 slot, CLR_UINT32 index)
{
    NATIVE_PROFILE_CLR_CORE();
    Link &list = entries[slot];

    if (list.m_next != index)
    {
        Link &node = entries[index];
        CLR_UINT32 next;
        CLR_UINT32 prev;

        //
        // Unlink.
        //
        next = node.m_next;
        prev = node.m_prev;

        entries[next].m_prev = prev;
        entries[prev].m_next = next;

        //
        // Insert.
        //
        next = list.m_next;

        node.m_next = next;
        node.m_prev = slot;

        list.m_next = index;
        entries[next].m_prev = index;
    }
}

#endif // #if defined(NANOCLR_USE_AVLTREE_FOR_METHODLOOKUP)

////////////////////////////////////////////////////////////////////////////////////////////////////

void CLR_RT_EventCache::EventCache_Initialize()
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_CLEAR(*this);

    m_events = (BoundedList *)&m_scratch[0];

    BoundedList *lst = m_events;
    size_t num = c_maxFastLists;

    while (num--)
    {
        lst->m_blocks.DblLinkedList_Initialize();

        lst++;
    }

    m_lookup_VirtualMethod.Initialize();

#ifndef NANOCLR_NO_IL_INLINE
    m_inlineBufferStart = (CLR_RT_InlineBuffer *)g_scratchInlineBuffer;

    num = InlineBufferCount() - 1;

    for (int i = 0; i < (int)num; i++)
    {
        m_inlineBufferStart[i].m_pNext = &m_inlineBufferStart[i + 1];
    }
    m_inlineBufferStart[num].m_pNext = nullptr;
#endif
}

CLR_UINT32 CLR_RT_EventCache::EventCache_Cleanup()
{
    NATIVE_PROFILE_CLR_CORE();
    BoundedList *lst = m_events;
    size_t num = c_maxFastLists;
    CLR_UINT32 tot = 0;

    while (num--)
    {
        NANOCLR_FOREACH_NODE(CLR_RT_HeapBlock_Node, ptr, lst->m_blocks)
        {
            ptr->SetDataId(CLR_RT_HEAPBLOCK_RAW_ID(DATATYPE_FREEBLOCK, CLR_RT_HeapBlock::HB_Pinned, ptr->DataSize()));
            ptr->ClearData();

            tot += ptr->DataSize();
        }
        NANOCLR_FOREACH_NODE_END();

        lst->m_blocks.DblLinkedList_Initialize();

        lst++;
    }

    return tot;
}

//--//

void CLR_RT_EventCache::Append_Node(CLR_RT_HeapBlock *node)
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RT_HeapBlock_Node *ptr = (CLR_RT_HeapBlock_Node *)node;
    CLR_UINT32 blocks = ptr->DataSize();
    BoundedList &lst = m_events[blocks < c_maxFastLists ? blocks : 0];

    ptr->ChangeDataType(DATATYPE_CACHEDBLOCK);
    ptr->ChangeDataFlags(CLR_RT_HeapBlock::HB_Alive | CLR_RT_HeapBlock::HB_Event);

    NANOCLR_CHECK_EARLY_COLLECTION(ptr);

    ptr->Debug_ClearBlock(SENTINEL_NODE_APPENDED);

    lst.m_blocks.LinkAtBack(ptr);
}

CLR_RT_HeapBlock *CLR_RT_EventCache::Extract_Node_Slow(CLR_UINT32 dataType, CLR_UINT32 flags, CLR_UINT32 blocks)
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RT_HeapBlock_Node *node;
    CLR_RT_HeapBlock_Node *best = nullptr;
    CLR_UINT32 bestSize = 0;

    NANOCLR_FOREACH_NODE(CLR_RT_HeapBlock_Node, ptr, m_events[0].m_blocks)
    {
        CLR_UINT32 size = ptr->DataSize();

        if (size == blocks)
        {
            best = ptr;
            bestSize = blocks;
            break;
        }

        if (size >= blocks)
        {
            if ((best && (size < bestSize)) ||
                (!best && (size <= (blocks * 20) / 16))) // Accept a maximum overhead of 25%.
            {
                best = ptr;
                bestSize = size;
            }
        }
    }
    NANOCLR_FOREACH_NODE_END();

    node = best;

    if (node)
    {
        //
        // Did we select a block bigger than requested? Requeue the tail.
        //
        if (bestSize > blocks)
        {
            CLR_RT_HeapBlock_Node *next = &node[blocks];

            node->SetDataId(CLR_RT_HEAPBLOCK_RAW_ID(
                DATATYPE_CACHEDBLOCK,
                (CLR_RT_HeapBlock::HB_Alive | CLR_RT_HeapBlock::HB_Event),
                blocks));
            next->SetDataId(CLR_RT_HEAPBLOCK_RAW_ID(
                DATATYPE_CACHEDBLOCK,
                (CLR_RT_HeapBlock::HB_Alive | CLR_RT_HeapBlock::HB_Event),
                (bestSize - blocks)));
            next->ClearData();

            Append_Node(next);
        }

        node->Unlink();

        node->ChangeDataType(dataType);
        node->ChangeDataFlags(CLR_RT_HeapBlock::HB_Alive | CLR_RT_HeapBlock::HB_Event);

        if (flags & CLR_RT_HeapBlock::HB_InitializeToZero)
        {
            node->InitializeToZero();
        }
        else
        {
            node->Debug_ClearBlock(SENTINEL_NODE_EXTRACTED);
        }

#if defined(NANOCLR_PROFILE_NEW_ALLOCATIONS)
        g_CLR_PRF_Profiler.TrackObjectCreation(node);
#endif

        return node;
    }

    return g_CLR_RT_ExecutionEngine.ExtractHeapBlocksForEvents(dataType, flags, blocks);
}

CLR_RT_HeapBlock *CLR_RT_EventCache::Extract_Node_Fast(CLR_UINT32 dataType, CLR_UINT32 flags, CLR_UINT32 blocks)
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RT_HeapBlock_Node *ptr = m_events[blocks].m_blocks.FirstNode();

    if (ptr->Next())
    {
        ptr->Unlink();

        ptr->ChangeDataType(dataType);
        ptr->ChangeDataFlags(CLR_RT_HeapBlock::HB_Alive | CLR_RT_HeapBlock::HB_Event);

        if (flags & CLR_RT_HeapBlock::HB_InitializeToZero)
        {
            ptr->InitializeToZero();
        }
        else
        {
            ptr->Debug_ClearBlock(SENTINEL_NODE_EXTRACTED);
        }

#if defined(NANOCLR_PROFILE_NEW_ALLOCATIONS)
        g_CLR_PRF_Profiler.TrackObjectCreation(ptr);
#endif

        return ptr;
    }

    return g_CLR_RT_ExecutionEngine.ExtractHeapBlocksForEvents(dataType, flags, blocks);
}

CLR_RT_HeapBlock *CLR_RT_EventCache::Extract_Node_Bytes(CLR_UINT32 dataType, CLR_UINT32 flags, CLR_UINT32 bytes)
{
    NATIVE_PROFILE_CLR_CORE();
    return Extract_Node(dataType, flags, CONVERTFROMSIZETOHEAPBLOCKS(bytes));
}

CLR_RT_HeapBlock *CLR_RT_EventCache::Extract_Node(CLR_UINT32 dataType, CLR_UINT32 flags, CLR_UINT32 blocks)
{
    NATIVE_PROFILE_CLR_CORE();

#if defined(NANOCLR_FORCE_GC_BEFORE_EVERY_ALLOCATION)
    return g_CLR_RT_ExecutionEngine.ExtractHeapBlocksForEvents(dataType, flags, blocks);
#else

#if !defined(BUILD_RTM) || defined(VIRTUAL_DEVICE)
    if (g_CLR_RT_ExecutionEngine.m_fPerformGarbageCollection)
    {
        return g_CLR_RT_ExecutionEngine.ExtractHeapBlocksForEvents(dataType, flags, blocks);
    }
    else
#endif
    {
        if (blocks > 0 && blocks < c_maxFastLists)
        {
            return Extract_Node_Fast(dataType, flags, blocks);
        }
        else
        {
            return Extract_Node_Slow(dataType, flags, blocks);
        }
    }
#endif
}

//--//

bool CLR_RT_EventCache::FindVirtualMethod(
    const CLR_RT_TypeDef_Index &cls,
    const CLR_RT_MethodDef_Index &mdVirtual,
    CLR_RT_MethodDef_Index &md)
{
    NATIVE_PROFILE_CLR_CORE();
    return m_lookup_VirtualMethod.FindVirtualMethod(cls, mdVirtual, md);
}

// -- //

#ifndef NANOCLR_NO_IL_INLINE
bool CLR_RT_EventCache::GetInlineFrameBuffer(CLR_RT_InlineBuffer **ppBuffer)
{
    if (m_inlineBufferStart != nullptr)
    {
        *ppBuffer = m_inlineBufferStart;

        m_inlineBufferStart = m_inlineBufferStart->m_pNext;

        return true;
    }

    *ppBuffer = nullptr;

    return false;
}

bool CLR_RT_EventCache::FreeInlineBuffer(CLR_RT_InlineBuffer *pBuffer)
{
    pBuffer->m_pNext = m_inlineBufferStart;
    m_inlineBufferStart = pBuffer;
    return true;
}
#endif

////////////////////////////////////////////////////////////////////////////////
