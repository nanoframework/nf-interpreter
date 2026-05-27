//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "FreeRTOS.h"
#include <nanoCLR_Headers.h>

void *platform_malloc(size_t size)
{
    return pvPortMalloc(size);
}

void platform_free(void *ptr)
{
    vPortFree(ptr);
}
