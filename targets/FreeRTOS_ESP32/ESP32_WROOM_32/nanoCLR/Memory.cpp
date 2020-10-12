//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>

//
//  Allocate Memory for Managed heap 
//  
//  if spiRam is available then use all available ram - SPI_ALLOW_FREE
//
//  if no spiRam or spiRam malloc fails then malloc from normal ram
//
//  The CLR calls this multiple times. Once memory is allocated return the same memory for all 
//  subsequent calls.


static const char* TAG = "Memory";

// Space to leave free in spiRam
#define SPI_ALLOW_FREE (256 * 1024)

// You can't go much bigger than this when allocating in normal memory to 
// get memory in one continous lump.
#define NORMAL_MEMORY_SIZE (115 * 1024)


// Saved memory allocation for when heap is reset so we can return same value.
unsigned char* pManagedHeap = NULL;
size_t         managedHeapSize = 0;

//
//	Return the location and size of the managed heap
//  If called a 2nd time then always return same value
//
void HeapLocation(unsigned char*& baseAddress, unsigned int& sizeInBytes)
{
	NATIVE_PROFILE_PAL_HEAP();

	// Memory allocated yet ?
	if (pManagedHeap == NULL)
	{
		// Memory size to allocate in normal memory if spiRam not available
		managedHeapSize = NORMAL_MEMORY_SIZE;

		// See if we have any SPIRAM
		size_t spiramMaxSize = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT | MALLOC_CAP_32BIT | MALLOC_CAP_SPIRAM);

		// Sanity check - Make sure there is at least managedHeapSize available in SPIRAM
		if (spiramMaxSize > (SPI_ALLOW_FREE + managedHeapSize))
		{
			// Allocate heap from SPIRAM
			managedHeapSize = spiramMaxSize - SPI_ALLOW_FREE;
			pManagedHeap = (unsigned char *)heap_caps_malloc(managedHeapSize, MALLOC_CAP_8BIT | MALLOC_CAP_32BIT | MALLOC_CAP_SPIRAM);
			if (pManagedHeap)
				ESP_LOGI(TAG, "Managed heap allocated, spiRam size:%d max:%d", managedHeapSize, spiramMaxSize);
		}

		while(pManagedHeap == NULL)
		{
			// Allocate heap from Internal RAM
			pManagedHeap = (unsigned char *)heap_caps_malloc(managedHeapSize, MALLOC_CAP_8BIT | MALLOC_CAP_32BIT);
			if (pManagedHeap)
			{
				ESP_LOGI(TAG, "Managed heap allocated, internal size:%d", managedHeapSize);
			}
			else
			{
				// Subtract 1K and try again
				managedHeapSize -= 1024;
				if (managedHeapSize <= 0)
				{
					ESP_LOGE(TAG, "Unable to allocate any Managed heap");
					return;
				}
			}
		}
	}

	baseAddress = pManagedHeap;
	sizeInBytes = managedHeapSize;


	// Update System config with Heap location and size
	HalSystemConfig.RAM1.Size = managedHeapSize;
	HalSystemConfig.RAM1.Base = (unsigned int)pManagedHeap;
}
