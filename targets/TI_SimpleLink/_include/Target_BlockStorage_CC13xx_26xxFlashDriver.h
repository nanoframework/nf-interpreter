//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGET_CC13xx_26xxFLASH_DRIVER_H
#define TARGET_CC13xx_26xxFLASH_DRIVER_H

#include <nanoPAL_BlockStorage.h>

#ifdef __cplusplus
extern "C"
{
#endif

    bool CC13xx_26xxFlashDriver_InitializeDevice(void *);
    bool CC13xx_26xxFlashDriver_UninitializeDevice(void *);
    DeviceBlockInfo *CC13xx_26xxFlashDriver_GetDeviceInfo(void *);
    bool CC13xx_26xxFlashDriver_Read(void *, ByteAddress startAddress, unsigned int numBytes, unsigned char *buffer);
    bool CC13xx_26xxFlashDriver_Write(
        void *,
        ByteAddress startAddress,
        unsigned int numBytes,
        unsigned char *buffer,
        bool readModifyWrite);
    bool CC13xx_26xxFlashDriver_IsBlockErased(void *, ByteAddress blockAddress, unsigned int length);
    bool CC13xx_26xxFlashDriver_EraseBlock(void *, ByteAddress address);
    void CC13xx_26xxFlashDriver_SetPowerState(void *, unsigned int state);

#ifdef __cplusplus
}
#endif

#endif // TARGET_CC13xx_26xxFLASH_DRIVER_H
