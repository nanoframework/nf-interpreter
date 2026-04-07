//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_v2.h>
#include <tx_api.h>

// the byte pool for memory allocation is defined in main()
extern TX_BYTE_POOL byte_pool_0;

void *platform_malloc(size_t size)
{
    uint8_t *pointer = TX_NULL;

#ifdef BUILD_RTM
    tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, size, TX_NO_WAIT);
#else
    uint32_t allocStatus = tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, size, TX_NO_WAIT);
    ASSERT(allocStatus == TX_SUCCESS);
#endif

    return pointer;
}

void platform_free(void *ptr)
{

#ifdef BUILD_RTM
    tx_byte_release(ptr);
#else
    uint32_t freeStatus = tx_byte_release(ptr);
    ASSERT(freeStatus == TX_SUCCESS);
#endif
}
