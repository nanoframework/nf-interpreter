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

    uint32_t allocStatus = tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, size, TX_NO_WAIT);
    ASSERT(allocStatus == TX_SUCCESS);

    return pointer;
}

void platform_free(void *ptr)
{
    uint32_t freeStatus = tx_byte_release(ptr);
    ASSERT(freeStatus == TX_SUCCESS);
}

void *platform_realloc(void *ptr, size_t size)
{
    (void)size;

    return ptr;
}
