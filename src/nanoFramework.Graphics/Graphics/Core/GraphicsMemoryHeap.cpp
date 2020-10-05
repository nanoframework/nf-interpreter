//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Graphics Memory Heap is for use by the graphics only
// Each platform needs to allocate a dedicated space

#include "GraphicsMemoryHeap.h"
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Runtime__HeapBlock.h>

#define NOT_APPLICABLE 0

struct GraphicsMemoryHeap g_GraphicsMemoryHeap;

enum loopingState
{
    freeBlockSearch,
    found,
    failed
};
enum blockStatus
{
    heapStart = 0xFACEFACE,
    blockFree = 0xDEADBEEF,
    blockBusy = 0xDEADBE5E,
    heapEnd = 0xDEADDEAD
};

struct BlockHeader
{
    struct BlockHeader *next;
    struct BlockHeader *prev;
    CLR_UINT32 dataLength;
    blockStatus status;
};

struct BlockHeader *ptrfirstBlockHeader;
struct BlockHeader *ptrlastBlockHeader;

CLR_UINT8 *graphicsHeapBeginAddress;
CLR_UINT8 *graphicsHeapEndAddress;
const int blockHeaderSize = sizeof(struct BlockHeader);

bool GraphicsMemoryHeap::Initialize()
{
    GraphicsMemory gm;

    if (gm.GraphicsHeapLocation(graphicsHeapBeginAddress, graphicsHeapEndAddress))
    {
        CLR_UINT32 SizeInBytes = graphicsHeapEndAddress - graphicsHeapBeginAddress;

        memset(graphicsHeapBeginAddress, 0, SizeInBytes);

        ptrfirstBlockHeader = (BlockHeader *)graphicsHeapBeginAddress;
        ptrlastBlockHeader = (struct BlockHeader *)((CLR_UINT32)graphicsHeapEndAddress - blockHeaderSize);

        ptrfirstBlockHeader->dataLength = SizeInBytes - 2 * blockHeaderSize; // Allow for first and last block headers
        ptrfirstBlockHeader->next = ptrlastBlockHeader;
        ptrfirstBlockHeader->prev = NOT_APPLICABLE; // bottom of heap
        ptrfirstBlockHeader->status = blockFree;

        ptrlastBlockHeader->dataLength = 0;
        ptrlastBlockHeader->next = NOT_APPLICABLE; // end of heap
        ptrlastBlockHeader->prev = ptrfirstBlockHeader;
        ptrlastBlockHeader->status = heapEnd;
        return true;
    }
    return false;
}
void *GraphicsMemoryHeap::Allocate(CLR_UINT32 requestedSize)
{
    struct BlockHeader *currentBlk = ptrfirstBlockHeader; // Begin at the bottom of the heap
    volatile loopingState looping = freeBlockSearch;

    // Split a block with enough free space into two regions by inserting a new block
    while (looping == freeBlockSearch)
    {
        if (currentBlk->status == blockFree) // Find a free block
        {
            if (currentBlk->dataLength >=
                requestedSize + blockHeaderSize) // Room for the requested data and the next block (to split the region)
            {
                // Break the current block into busy block of requested size, and free block of remaining size
                currentBlk->dataLength = requestedSize;
                currentBlk->status = blockBusy;

                struct BlockHeader *insertedBlockHeader =
                    (struct BlockHeader *)((CLR_UINT32)currentBlk + blockHeaderSize + requestedSize);
                insertedBlockHeader->status = blockFree;

                // Update the linked list pointers
                (currentBlk->next)->prev = insertedBlockHeader;
                insertedBlockHeader->next = currentBlk->next;
                currentBlk->next = insertedBlockHeader;
                insertedBlockHeader->prev = currentBlk;

                insertedBlockHeader->dataLength =
                    (CLR_UINT32)insertedBlockHeader->next - (CLR_UINT32)insertedBlockHeader - blockHeaderSize;

                looping = found;
                break;
            }
        }
        currentBlk = currentBlk->next;
        if (currentBlk->status == heapEnd)
        {
            looping = failed;
            break;
        }
    }
    if (looping == found)
    {
        CLR_UINT32 ptrDataArea =
            ((CLR_UINT32)currentBlk + blockHeaderSize); // return a pointer to start of allocated memory
        return (void *)ptrDataArea;
    }
    else
    {
        return 0; // TODO:  need to return out of graphics memory
    }
}

bool GraphicsMemoryHeap::Release(void *pHeapBlockData)
{

    struct BlockHeader *currentBlk = (struct BlockHeader *)((CLR_UINT32)pHeapBlockData - blockHeaderSize);

    if (currentBlk >= ptrfirstBlockHeader && currentBlk < ptrlastBlockHeader)
    {
        struct BlockHeader *nextBlock = currentBlk->next;
        struct BlockHeader *prevBlock = currentBlk->prev;

        currentBlk->status = blockFree;
        // If the next block is free, then subsume next block into current including data length and block header
        if (nextBlock->status == blockFree) // end block status is always heap_end
        {
            currentBlk->dataLength += nextBlock->dataLength + blockHeaderSize;
            currentBlk->next = nextBlock->next;
            (nextBlock->next)->prev = currentBlk;
        }
        // If the previous block is free, then subsume current block into previous including data length and block
        // header
        if (prevBlock->status == blockFree && prevBlock != ptrfirstBlockHeader) // Guard against looking outside heap
        {
            prevBlock->dataLength += currentBlk->dataLength + blockHeaderSize;
            prevBlock->next = currentBlk->next;
            (nextBlock->next)->prev = prevBlock;
        }
    }
    else
    {
        ASSERT(CLR_E_FAIL);
    }
    return true;
}
