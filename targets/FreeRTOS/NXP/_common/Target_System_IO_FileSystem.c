//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoHAL_v2.h>

// need to declare these here as extern because they are C++ functions
extern bool FS_MountVolume(const char *rootName, uint32_t deviceFlags, char *fileSystemDriver);
extern void FS_UnmountVolume(const char *rootName);

// need to declare this here as extern
extern void PostManagedEvent(uint8_t category, uint8_t subCategory, uint16_t data1, uint32_t data2);
