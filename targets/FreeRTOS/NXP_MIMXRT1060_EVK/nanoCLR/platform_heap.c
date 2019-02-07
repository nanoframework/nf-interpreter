//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "FreeRTOS.h"
#include "stdint.h"
#include "stddef.h"
#include <string.h>

void *platform_malloc(size_t size)
{
    return pvPortMalloc(size);
}

void platform_free(void *ptr)
{
    vPortFree(ptr);
}

typedef struct A_BLOCK_LINK
{
	struct A_BLOCK_LINK *pxNextFreeBlock;	/*<< The next free block in the list. */
	size_t xBlockSize;						/*<< The size of the free block. */
} BlockLink_t;

#define heapBITS_PER_BYTE ( ( size_t ) 8 )
static const size_t xHeapStructSize	= ( sizeof( BlockLink_t ) + ( ( size_t ) ( portBYTE_ALIGNMENT - 1 ) ) ) & ~( ( size_t ) portBYTE_ALIGNMENT_MASK );
static size_t	xBlockAllocatedBit = ( ( size_t ) 1 ) << ( ( sizeof( size_t ) * heapBITS_PER_BYTE ) - 1 );

static size_t mem_size(void *pv)
{
    uint8_t *puc = (uint8_t *)pv;
    BlockLink_t *pxLink;

    if (pv != NULL)
    {
        /* The memory being freed will have an BlockLink_t structure immediately
		before it. */
        puc -= xHeapStructSize;

        /* This casting is to keep the compiler from issuing warnings. */
        pxLink = (void *)puc;

        return pxLink->xBlockSize & ~xBlockAllocatedBit;
    }

    return 0;
}

void *platform_realloc(void *ptr, size_t size)
{
    void *newPtr;
    size_t curSize;

    if (ptr == 0)
    {
        return pvPortMalloc(size);
    }

    curSize = mem_size(ptr);

    if (size <= curSize)
    {
        return ptr;
    }

    newPtr = pvPortMalloc(size);

    memcpy(ptr, newPtr, size);

    vPortFree(ptr);
    return newPtr;
}
