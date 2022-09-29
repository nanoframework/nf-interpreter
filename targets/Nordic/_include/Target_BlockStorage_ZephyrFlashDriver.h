//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGET_ZEPHYR_FLASH_DRIVER_H
#define TARGET_ZEPHYR_FLASH_DRIVER_H

#include <zephyr.h>

#include <nanoPAL_BlockStorage.h>

#ifdef __cplusplus
extern "C"
{
#endif

    bool ZephyrFlashDriver_InitializeDevice(void *);
    bool ZephyrFlashDriver_UninitializeDevice(void *);
    DeviceBlockInfo *ZephyrFlashDriver_GetDeviceInfo(void *);
    bool ZephyrFlashDriver_Read(void *, ByteAddress startAddress, unsigned int numBytes, unsigned char *buffer);
    bool ZephyrFlashDriver_Write(
        void *,
        ByteAddress startAddress,
        unsigned int numBytes,
        unsigned char *buffer,
        bool readModifyWrite);
    bool ZephyrFlashDriver_IsBlockErased(void *, ByteAddress blockAddress, unsigned int length);
    bool ZephyrFlashDriver_EraseBlock(void *, ByteAddress address);

#ifdef __cplusplus
}
#endif

#endif // TARGET_ZEPHYR_FLASH_DRIVER_H
