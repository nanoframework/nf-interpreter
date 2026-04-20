//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// MCUboot OS malloc abstraction for nanoFramework bare-metal builds.
//
// MCUboot's loader.c includes <os/os_malloc.h> to get OS-specific memory
// management wrappers.  For the bare-metal STM32 bootloader we map directly
// to the newlib-nano standard C heap (malloc / calloc / realloc / free).
// The heap is provided by the linker script (._user_heap_stack section).
//

#ifndef __OS_MALLOC_H__
#define __OS_MALLOC_H__

#include <stdlib.h>

#endif // __OS_MALLOC_H__
