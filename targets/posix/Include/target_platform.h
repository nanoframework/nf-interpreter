// Licensed to the .NET Foundation under one or more agreements.
// See LICENSE file in the project root for full license information.

#ifndef TARGET_PLATFORM_H
#define TARGET_PLATFORM_H

// Mark this build as the POSIX host target, distinct from the Win32 VIRTUAL_DEVICE
// and from bare-metal PLATFORM_ARM / PLATFORM_ESP32.  All other POSIX-gate checks
// in the CLR headers key off this symbol.
#define PLATFORM_POSIX_HOST 1

#endif // TARGET_PLATFORM_H
