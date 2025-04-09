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

// Minimum memory needed for IDF (native code)
#if defined(CONFIG_IDF_TARGET_ESP32H2)
#define MINIMUM_FREE_MEMORY_FOR_IDF (20 * 1024)
#elif defined(CONFIG_IDF_TARGET_ESP32C6)
#define MINIMUM_FREE_MEMORY_FOR_IDF (90 * 1024)
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
#define MINIMUM_FREE_MEMORY_FOR_IDF (130 * 1024)
#else
#define MINIMUM_FREE_MEMORY_FOR_IDF (100 * 1024)
#endif

// If bluetooth configured then increase minimum by value
#define BLE_EXTRA_IDF_MEMORY_RESERVE (30 * 1024)
// If Thread configured then increase minimum by value
#define THREAD_EXTRA_IDF_MEMORY_RESERVE (20 * 1024)

// Space to leave free in internal RAM for allocation by IDF malloc
#define INTERNAL_RAM_LEAVE_FREE_FOR_ALLOCATION (ESP32_RESERVED_RAM_FOR_IDF_ALLOCATION * 1024)

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// uncomment, in case there is a need to check the heap info
// #define PRINT_HEAP_INFO  1

// Saved memory allocation for when heap is reset so we can return same value.
unsigned char *managedHeap = NULL;
size_t managedHeapSize = 0;

#ifdef PRINT_HEAP_INFO
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
#endif

//
//	Return the location and size of the managed heap
//  If called a 2nd time then always return same value
//
void HeapLocation(unsigned char *&baseAddress, unsigned int &sizeInBytes)
{
    NATIVE_PROFILE_PAL_HEAP();

    // Memory allocated yet ?
    if (managedHeap == NULL)
    {
#ifdef PRINT_HEAP_INFO
        print_heap_info(MALLOC_CAP_8BIT | MALLOC_CAP_32BIT | MALLOC_CAP_SPIRAM);
#endif

        // default memory caps
        uint32_t memoryCaps = MALLOC_CAP_8BIT | MALLOC_CAP_32BIT;
        size_t largestFreeBlock;

        // check if we have any SPIRAM
        size_t spiramMaxSize = heap_caps_get_largest_free_block(memoryCaps | MALLOC_CAP_SPIRAM);

        // Sanity check - Make sure there is at least managedHeapSize available in SPIRAM
        if (spiramMaxSize)
        {
            // add SPIRAM to request memory caps
            memoryCaps |= MALLOC_CAP_SPIRAM;

            // store max allocation size
            largestFreeBlock = spiramMaxSize;

            // get heap size to allocate
            managedHeapSize = spiramMaxSize - ESP32_SPIRAM_FOR_IDF_ALLOCATION;

            ESP_LOGI(TAG, "Allocating managed heap from SPIRAM");
        }
        else
        {
            // No spiram found so allocate from internal RAM
            // Calculate minimum memory reserved for IDF
            size_t minIdfMemory = MINIMUM_FREE_MEMORY_FOR_IDF;
#if HAL_USE_BLE
            minIdfMemory += BLE_EXTRA_IDF_MEMORY_RESERVE;
#endif
#if HAL_USE_THREAD
            minIdfMemory += THREAD_EXTRA_IDF_MEMORY_RESERVE;
#endif

            // Calculate extra IDF memory to reserve, at least minimum memory
            // if ESP32_RESERVED_RAM_FOR_IDF_ALLOCATION specified then use it if bigger then minIdfMemory
            size_t IdfToReserve =
                (INTERNAL_RAM_LEAVE_FREE_FOR_ALLOCATION > 0) ? INTERNAL_RAM_LEAVE_FREE_FOR_ALLOCATION : minIdfMemory;

            // Maximum free space in all heap blocks
            size_t freeSpaceHeap = heap_caps_get_free_size(memoryCaps);

            // find out largest available space for managed heap?
            largestFreeBlock = heap_caps_get_largest_free_block(memoryCaps);

            // get managed heap size to allocate and if required adjust down to fit in largest block
            // Reserve minimum IDF memory by subtracting off free space
            // Everything left can be used for managed code
            ASSERT(freeSpaceHeap > IdfToReserve);
            managedHeapSize = (freeSpaceHeap - IdfToReserve);

            // Adjust managed heap size down to fit in largest native heap block
            if (managedHeapSize > largestFreeBlock)
            {
                managedHeapSize = largestFreeBlock;
            }

            ESP_LOGI(TAG, "Allocating managed heap from internal RAM, total free heap %d", freeSpaceHeap);
        }

        // allocate managed heap
        managedHeap = (unsigned char *)heap_caps_malloc(managedHeapSize, memoryCaps);
        if (managedHeap)
        {
            // managed heap successfully allocated
            ESP_LOGI(TAG, "Managed heap allocated: %d (from max:%d)", managedHeapSize, largestFreeBlock);
        }
        else
        {
            // failed to allocate
            ESP_LOGI(TAG, "Failed to allocate managed heap");
        }
    }

    baseAddress = managedHeap;
    sizeInBytes = managedHeapSize;

    // Update System configuration with Heap location and size
    HalSystemConfig.RAM1.Size = managedHeapSize;
    HalSystemConfig.RAM1.Base = (unsigned int)managedHeap;
}
