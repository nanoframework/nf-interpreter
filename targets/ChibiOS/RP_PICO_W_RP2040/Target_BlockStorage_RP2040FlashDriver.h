//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGET_RP2040FLASH_DRIVER_H
#define TARGET_RP2040FLASH_DRIVER_H

#include <ch.h>
#include <hal.h>
#include <nanoPAL_BlockStorage.h>

#ifdef __cplusplus
extern "C"
{
#endif

    bool RP2040FlashDriver_InitializeDevice(void *);
    bool RP2040FlashDriver_UninitializeDevice(void *);
    DeviceBlockInfo *RP2040FlashDriver_GetDeviceInfo(void *);
    bool RP2040FlashDriver_Read(void *, ByteAddress startAddress, unsigned int numBytes, unsigned char *buffer);
    bool RP2040FlashDriver_Write(
        void *,
        ByteAddress startAddress,
        unsigned int numBytes,
        unsigned char *buffer,
        bool readModifyWrite);
    bool RP2040FlashDriver_IsBlockErased(void *, ByteAddress blockAddress, unsigned int length);
    bool RP2040FlashDriver_EraseBlock(void *, ByteAddress address);

#ifdef __cplusplus
}
#endif

#endif // TARGET_RP2040FLASH_DRIVER_H
