//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <target_platform.h>
#include <esp32_idf.h>

//
//  Allocate Memory for Managed heap
//
//  if spiRam is available then use all available ram - SPI_ALLOW_FREE
//
//  if no spiRam or spiRam malloc fails then malloc from normal ram
//
//  The CLR calls this multiple times. Once memory is allocated return the same memory for all
//  subsequent calls.

static const char *TAG = "Memory";

// Space to leave free in SPIRAM for allocation by IDF malloc
#define SPIRAM_MEMORY_LEAVE_FOR_ALLOCATION (256 * 1024)

// You can't go much bigger than this when allocating in internal memory to
// get memory in one continuous lump.
#if (HAL_USE_BLE == TRUE)
#define INTERNAL_MEMORY_SIZE (54 * 1024) // Reduce Managed heap if using BLE without spiram
#else
#define INTERNAL_MEMORY_SIZE (84 * 1024)
#endif

// Saved memory allocation for when heap is reset so we can return same value.
unsigned char *pManagedHeap = NULL;
size_t managedHeapSize = 0;

extern "C"
{
    typedef portMUX_TYPE multi_heap_lock_t;
#include <heap_private.h>

    extern struct registered_heap_ll registered_heaps;
}

void print_heap_info(uint32_t caps)
{
    multi_heap_info_t info;

    debug_printf("Heap summary for capabilities 0x%08X:\n", caps);

    heap_t *heap;

    SLIST_FOREACH(heap, &registered_heaps, next)
    {
        if (heap_caps_match(heap, caps))
        {
            multi_heap_get_info(heap->heap, &info);

            debug_printf(
                "  At 0x%08x len %d free %d allocated %d min_free %d\n",
                heap->start,
                heap->end - heap->start,
                info.total_free_bytes,
                info.total_allocated_bytes,
                info.minimum_free_bytes);
            debug_printf(
                "    largest_free_block %d alloc_blocks %d free_blocks %d total_blocks %d\n",
                info.largest_free_block,
                info.allocated_blocks,
                info.free_blocks,
                info.total_blocks);
        }
    }

    debug_printf("  Totals:\n");
    heap_caps_get_info(&info, caps);

    debug_printf(
        "    free %d allocated %d min_free %d largest_free_block %d\n",
        info.total_free_bytes,
        info.total_allocated_bytes,
        info.minimum_free_bytes,
        info.largest_free_block);
}

//
//	Return the location and size of the managed heap
//  If called a 2nd time then always return same value
//
void HeapLocation(unsigned char *&baseAddress, unsigned int &sizeInBytes)
{
    NATIVE_PROFILE_PAL_HEAP();

    // Memory allocated yet ?
    if (pManagedHeap == NULL)
    {
        // Memory size to allocate in normal memory if PSRAM not available
        managedHeapSize = INTERNAL_MEMORY_SIZE;

        //////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////
        // uncomment, in case there is a need to check the heap info
        // print_heap_info(MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
        //////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////

        // See if we have any SPIRAM
        size_t spiramMaxSize = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT | MALLOC_CAP_32BIT | MALLOC_CAP_SPIRAM);

        // Sanity check - Make sure there is at least managedHeapSize available in SPIRAM
        if (spiramMaxSize)
        {
            // Allocate heap from SPIRAM
            managedHeapSize = spiramMaxSize - SPIRAM_MEMORY_LEAVE_FOR_ALLOCATION;

            pManagedHeap = (unsigned char *)heap_caps_malloc(
                managedHeapSize,
                MALLOC_CAP_8BIT | MALLOC_CAP_32BIT | MALLOC_CAP_SPIRAM);

            if (pManagedHeap)
            {
                // managed heap successfully allocated from SPIRAM
                ESP_LOGI(TAG, "Managed heap allocated from SPIRAM: %d (from max:%d)", managedHeapSize, spiramMaxSize);
            }
            else
            {
                // failed to allocation managed heap from SPIRAM
                ESP_LOGI(TAG, "Failed to allocate managed heap from SPIRAM");
            }
        }

        while (pManagedHeap == NULL)
        {
            // Allocate heap from Internal RAM
            pManagedHeap = (unsigned char *)heap_caps_malloc(managedHeapSize, MALLOC_CAP_8BIT | MALLOC_CAP_32BIT);

            if (pManagedHeap)
            {
                ESP_LOGI(TAG, "Managed heap allocated from internal RAM: %d", managedHeapSize);
            }
            else
            {
                // Subtract 1K and try again
                managedHeapSize -= 1024;

                if (managedHeapSize <= 0)
                {
                    ESP_LOGE(TAG, "Unable to allocate memory for managed heap");

                    ASSERT(false);

                    return;
                }
            }
        }
    }

    baseAddress = pManagedHeap;
    sizeInBytes = managedHeapSize;

    // Update System configuration with Heap location and size
    HalSystemConfig.RAM1.Size = managedHeapSize;
    HalSystemConfig.RAM1.Base = (unsigned int)pManagedHeap;
}
