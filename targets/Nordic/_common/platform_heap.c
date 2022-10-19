//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <zephyr.h>
#include <nanoCLR_Headers.h>

//#define MALLOC_TRACE 1

void *platform_malloc(size_t size)
{
    void *p;

#ifdef MALLOC_TRACE

    printk("k_malloc s:%d ", size);
    p = k_malloc(size);
    printk("ptr: %x\n", (uint32_t)p);

#else

    p = k_malloc(size);

    if (p == 0)
    {
        printk("**** Malloc Fail size %d\n ****\n", size);
    }

#endif

    return p;
}

void platform_free(void *ptr)
{
#ifdef MALLOC_TRACE
    printk("k_free %x\n", (uint32_t)ptr);
#endif
    k_free(ptr);
}

typedef struct A_BLOCK_LINK
{
    struct A_BLOCK_LINK *pxNextFreeBlock; /*<< The next free block in the list. */
    size_t xBlockSize;                    /*<< The size of the free block. */
} BlockLink_t;

//
// Simple heap
//
// #define HDR_VAL 0xBEEF

// #define HEAP_FLG_LASTBLK  0x8000
// #define HEAP_FLG_FREE     0x0001
// #define HEAP_FLG_ENDCAP   (HEAP_FLG_FREE | HEAP_FLG_LASTBLK)
// typedef struct {
//     uint16_t header;
//     uint16_t flags;
//     uint32_t size;
// } BLK_HDR, *PBLK_HDR;

// static uint32_t heap_base = 0;
// static uint32_t heap_size = 0;
// static uint32_t heap_last = 0;
// int plat_mem_init (int heapsize)
// {
//     heap_base = k_malloc (heapsize);
//     if (heap_base == 0)
//     {
//         return -1;
//     }
//     heap_size = heapsize;
// }
