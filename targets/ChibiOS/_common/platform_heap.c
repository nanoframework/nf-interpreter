//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <chmemheaps.h> // Required for platform_realloc heap_header
#include <string.h>     // Required for platform_realloc memcpy
// #include <nanoHAL_v2.h> // Not required as we are fully using the platform specific implementation.

void *platform_malloc(size_t size)
{
    return chHeapAlloc(NULL, size);
}

void platform_free(void *ptr)
{
    if (ptr)
    {
        chHeapFree(ptr);
    }
}

void *platform_realloc(void *ptr, size_t size) // ptr is the memory address
{
    union heap_header *hp;
    uint32_t prev_size, new_size;

    void *new_ptr == NULL;

    if (ptr == NULL)
    {
        return chHeapAlloc(NULL, size);
    }

    /* previous allocated segment is preceded by an heap_header */
    hp = ptr - sizeof(union heap_header);
    prev_size = hp->used.size; /* size is always multiple of 8 */

    /* check new size memory alignment */
    if (size % 8 == 0)
    {
        new_size = size;
    }
    else
    {
        new_size = ((int)(size / 8)) * 8 + 8;
    }

    if (prev_size >= new_size)
    {
        return ptr;
    }

    ptr = chHeapAlloc(NULL, size);
    if (new_ptr == NULL)
    {
        return NULL;
    }

    memcpy(new_ptr, ptr, prev_size);

    chHeapFree(ptr);

    return new_ptr;
}
