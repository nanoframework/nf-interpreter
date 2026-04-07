//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGET_SL_MSCFLASH_DRIVER_H
#define TARGET_SL_MSCFLASH_DRIVER_H

#include <nanoPAL_BlockStorage.h>
#include <em_msc.h>

#ifdef __cplusplus
extern "C"
{
#endif

    bool SL_MscFlashDriver_InitializeDevice(void *);
    bool SL_MscFlashDriver_UninitializeDevice(void *);
    DeviceBlockInfo *SL_MscFlashDriver_GetDeviceInfo(void *);
    bool SL_MscFlashDriver_Read(void *, ByteAddress startAddress, unsigned int numBytes, unsigned char *buffer);
    bool SL_MscFlashDriver_Write(
        void *,
        ByteAddress startAddress,
        unsigned int numBytes,
        unsigned char *buffer,
        bool readModifyWrite);
    bool SL_MscFlashDriver_IsBlockErased(void *, ByteAddress blockAddress, unsigned int length);
    bool SL_MscFlashDriver_EraseBlock(void *, ByteAddress address);

#ifdef __cplusplus
}
#endif

#endif // TARGET_SL_MSCFLASH_DRIVER_H
