//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_TRANSIENTFLASH_DRIVER_H_
#define _TARGET_TRANSIENTFLASH_DRIVER_H_ 1

#include <nanoPAL_BlockStorage.h>

#ifdef __cplusplus
extern "C"
{
#endif

    bool TransientFlashDriver_InitializeDevice(void *);
    bool TransientFlashDriver_UninitializeDevice(void *);
    DeviceBlockInfo *TransientFlashDriver_GetDeviceInfo(void *);
    bool TransientFlashDriver_Read(void *, ByteAddress startAddress, unsigned int numBytes, unsigned char *buffer);
    bool TransientFlashDriver_Write(
        void *,
        ByteAddress startAddress,
        unsigned int numBytes,
        unsigned char *buffer,
        bool readModifyWrite);
    bool TransientFlashDriver_IsBlockErased(void *, ByteAddress blockAddress, unsigned int length);
    bool TransientFlashDriver_EraseBlock(void *, ByteAddress address);

#ifdef __cplusplus
}
#endif

#endif //_TARGET_TRANSIENTFLASH_DRIVER_H_
