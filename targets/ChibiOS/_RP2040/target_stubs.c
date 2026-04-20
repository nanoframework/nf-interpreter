//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <stdbool.h>

// Stub implementations for functions not available when GPIO API is disabled.

__attribute__((weak)) bool CPU_GPIO_Initialize()
{
    return true;
}

__attribute__((weak)) bool CPU_GPIO_Uninitialize()
{
    return true;
}
