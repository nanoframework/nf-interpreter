//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>

void* platform_malloc(size_t size) {
    return chHeapAlloc(NULL, size);
}

void  platform_free(void*  ptr) {
    chHeapFree(ptr);
}

void* platform_realloc(void*  ptr, size_t size) {
    (void)size;

    return ptr;
}
