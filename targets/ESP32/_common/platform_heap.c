//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <esp32_idf.h>
#include <nanoHAL_v2.h>

// setting here the default malloc capabilities that will be used when performing malloc operations
#define DEFAULT_MALLOC_CAPS MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL

void *platform_malloc(size_t size)
{
    return heap_caps_malloc(size, DEFAULT_MALLOC_CAPS);
}

void platform_free(void *ptr)
{
    heap_caps_free(ptr);
}

void *platform_realloc(void *ptr, size_t size)
{
    return heap_caps_realloc(ptr, size, DEFAULT_MALLOC_CAPS);
}
