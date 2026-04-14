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

// hal_strcpy_s / hal_strncpy_s — Annex K not available on POSIX; map to safe equivalents
// strncpy_s(dst, size, src, count): copy at most count chars, always NUL-terminate
// We satisfy the contract: return 0 on success.
inline int hal_strcpy_s(char *dst, size_t size, const char *src)
{
    if (!dst || size == 0)
        return 1;
    strncpy(dst, src, size - 1);
    dst[size - 1] = '\0';
    return 0;
}
inline int hal_strncpy_s(char *dst, size_t size, const char *src, size_t count)
{
    if (!dst || size == 0)
        return 1;
    size_t n = count < size - 1 ? count : size - 1;
    strncpy(dst, src, n);
    dst[n] = '\0';
    return 0;
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
