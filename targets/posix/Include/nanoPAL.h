// Licensed to the .NET Foundation under one or more agreements.
// See LICENSE file in the project root for full license information.

// POSIX host shim for nanoPAL.h.
// Pulls in the minimal PAL types needed by the CLR headers.

#ifndef NANOPAL_H
#define NANOPAL_H

#include <nanoHAL.h>
#include <nanoPAL_BlockStorage.h>
#include <nanoPAL_AsyncProcCalls_decl.h>
#include <nanoPAL_COM.h>
#include <nanoPAL_FileSystem.h>
#include <nanoPAL_Time.h>

// hal_strlen_s - mapped to strlen on all non-VIRTUAL_DEVICE builds
#include <string.h>
#define hal_strlen_s(str) strlen(str)

// hal_strcpy_s / hal_strncpy_s — Annex K not available on POSIX; map to safe equivalents.
// Returns 0 on success, 1 on constraint violation (NULL pointer, zero size, or truncation),
// matching ChibiOS/ThreadX implementations and Annex K semantics.
inline int hal_strcpy_s(char *dst, size_t size, const char *src)
{
    if (!dst || !src || size == 0)
        return 1;
    size_t srcLen = strlen(src);
    if (srcLen >= size)
    {
        // truncation would occur — copy what fits, NUL-terminate, return error
        memcpy(dst, src, size - 1);
        dst[size - 1] = '\0';
        return 1;
    }
    memcpy(dst, src, srcLen + 1);
    return 0;
}
inline int hal_strncpy_s(char *dst, size_t size, const char *src, size_t count)
{
    if (!dst || !src || size == 0)
        return 1;
    size_t n = count < size - 1 ? count : size - 1;
    memcpy(dst, src, n);
    dst[n] = '\0';
    // return 1 if the source was longer than the buffer allows (truncation occurred)
    return (count >= size) ? 1 : 0;
}

// __isnanf - POSIX equivalent (macOS/Linux use isnan() for float too)
#include <cmath>
#ifndef __isnanf
#define __isnanf(x) std::isnan(static_cast<float>(x))
#endif

// PAL Events declarations
#include <nanoPAL_Events.h>

void HeapLocation(unsigned char *&BaseAddress, unsigned int &SizeInBytes);

#endif // NANOPAL_H
