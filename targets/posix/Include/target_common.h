// Licensed to the .NET Foundation under one or more agreements.
// See LICENSE file in the project root for full license information.

#ifndef TARGET_COMMON_H
#define TARGET_COMMON_H

#define TARGETNAMESTRING "Virtual nanoDevice"
#define PLATFORMNAMESTRING NANOCLR_PLATFORM_NAME
// TARGETINFOSTRING is defined in target_os.h (included via nanoHAL_Boot.h).
// Do NOT redefine it here — nanoHAL_v2.h uses a relative include for nanoHAL_Boot.h
// which finds src/HAL/Include/nanoHAL_Boot.h before our posix stub, causing a
// macro-redefined warning on every translation unit.
#define OEMSYSTEMINFOSTRING "nanoCLR running @ " NANOCLR_PLATFORM_NAME

#endif // TARGET_COMMON_H
