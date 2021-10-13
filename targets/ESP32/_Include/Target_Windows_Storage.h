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

    bool Storage_InitSDCardSPI(char *vfsName, int maxFiles, int pin_Miso, int pin_Mosi, int pin_Clk, int pin_Cs);
    bool Storage_InitSDCardMMC(char *vfsName, int maxFiles, bool bit1Mode);
    bool Storage_UnMountSDCard();

#ifdef __cplusplus
}
#endif

#endif // TARGET_WINDOWS_STORAGE_H
