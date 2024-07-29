//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGET_SYS_IO_FILESYSTEM_H
#define TARGET_SYS_IO_FILESYSTEM_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <esp_vfs.h>

#if SOC_SDMMC_HOST_SUPPORTED
    bool Storage_MountMMC(bool bit1Mode, int driveIndex);
#endif
    bool Storage_MountSpi(int spiBus, uint32_t CSPin, int driveIndex);
    bool Storage_UnMountSDCard(int driveIndex);

#ifdef __cplusplus
}
#endif

#endif // TARGET_SYS_IO_FILESYSTEM_H
