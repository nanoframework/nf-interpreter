//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
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
