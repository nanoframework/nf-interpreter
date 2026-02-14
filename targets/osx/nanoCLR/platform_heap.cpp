//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <cstdlib>

void *platform_malloc(size_t size)
{
    return std::malloc(size);
}

void platform_free(void *ptr)
{
    std::free(ptr);
}
