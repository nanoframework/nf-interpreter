//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGET_WINDOWS_STORAGE_H
#define TARGET_WINDOWS_STORAGE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <esp_vfs.h>

    bool Storage_MountMMC(bool bit1Mode, int driveIndex);
    bool Storage_MountSpi(int spiBus, uint32_t CSPin, int driveIndex);
    bool Storage_UnMountSDCard();

#ifdef __cplusplus
}
#endif

#endif // TARGET_WINDOWS_STORAGE_H
