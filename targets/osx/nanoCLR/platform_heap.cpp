//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <cstddef>

#if __has_include(<nanoHAL_v2.h>)
#include <nanoHAL_v2.h>
#else
extern "C"
{
    void *platform_malloc(size_t size);
    void platform_free(void *ptr);
}
#endif

#include <cstdlib>

void *platform_malloc(size_t size)
{
    return std::malloc(size);
}

void platform_free(void *ptr)
{
    std::free(ptr);
}
