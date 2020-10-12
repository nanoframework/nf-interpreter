//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_CC32XXFLASH_DRIVER_H_
#define _TARGET_CC32XXFLASH_DRIVER_H_ 1

#include <nanoPAL_BlockStorage.h>

#ifdef __cplusplus
extern "C" {
#endif

bool CC32xxFlashDriver_InitializeDevice(void*);
bool CC32xxFlashDriver_UninitializeDevice(void*);
DeviceBlockInfo* CC32xxFlashDriver_GetDeviceInfo(void*);
bool CC32xxFlashDriver_Read(void*, ByteAddress startAddress, unsigned int numBytes, unsigned char* buffer);
bool CC32xxFlashDriver_Write(void*, ByteAddress startAddress, unsigned int numBytes, unsigned char* buffer, bool readModifyWrite);
bool CC32xxFlashDriver_IsBlockErased(void*, ByteAddress blockAddress, unsigned int length);
bool CC32xxFlashDriver_EraseBlock(void*, ByteAddress address);
void CC32xxFlashDriver_SetPowerState(void*, unsigned int state);

#ifdef __cplusplus
}
#endif

#endif  //_TARGET_CC32XXFLASH_DRIVER_H_
