//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoCLR_Headers.h>

#include <tx_api.h>

// the byte pool for memory allocation is defined in main()
extern TX_BYTE_POOL byte_pool_0;

void *platform_malloc(size_t size)
{
    uint8_t *pointer = TX_NULL;

    /* Allocate the stack for thread 0.  */
    if(tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, size, TX_NO_WAIT) != TX_SUCCESS)
    {
        while (1)
        {
        }
    }

    return pointer;
}

void platform_free(void *ptr)
{
    if(tx_byte_release(ptr) != TX_SUCCESS)
    {
        while (1)
        {
        }
    }
}

void *platform_realloc(void *ptr, size_t size)
{
    (void)size;

    return ptr;
}
