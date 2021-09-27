//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <stdlib.h>

void *platform_malloc(size_t size)
{

    // need to undef in order to call the real function
#undef malloc

    return malloc(size);

    // define back
#define malloc YOU_SHALL_NOT_USE_malloc
}

void platform_free(void *ptr)
{

// need to undef in order to call the real function
#undef free

    free(ptr);

    // define back
#define malloc YOU_SHALL_NOT_USE_free
}

void *platform_realloc(void *ptr, size_t size)
{

// need to undef in order to call the real function
#undef realloc

    return realloc(ptr, size);

    // define back
#define realloc YOU_SHALL_NOT_USE_realloc
}
