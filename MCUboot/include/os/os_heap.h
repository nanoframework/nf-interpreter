//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Minimal OS heap shim for MCUboot.
// MCUboot's boot_status and image verification routines call os_malloc / os_free
// when MCUBOOT_USE_FLASH_AREA_GET_SECTORS is defined.  This shim redirects those
// calls to the standard C library heap, which is available in both ChibiOS (via
// newlib) and ESP-IDF runtime environments.

#ifndef OS_HEAP_H
#define OS_HEAP_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

    static inline void *os_malloc(size_t size)
    {
        return malloc(size);
    }

    static inline void os_free(void *mem)
    {
        free(mem);
    }

#ifdef __cplusplus
}
#endif

#endif // OS_HEAP_H
