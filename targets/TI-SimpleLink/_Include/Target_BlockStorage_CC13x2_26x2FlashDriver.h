//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_CC13x2_26x2FLASH_DRIVER_H_
#define _TARGET_CC13x2_26x2FLASH_DRIVER_H_ 1

#include <nanoPAL_BlockStorage.h>

#ifdef __cplusplus
extern "C" {
#endif

bool CC13x2_26x2FlashDriver_InitializeDevice(void*);
bool CC13x2_26x2FlashDriver_UninitializeDevice(void*);
DeviceBlockInfo* CC13x2_26x2FlashDriver_GetDeviceInfo(void*);
bool CC13x2_26x2FlashDriver_Read(void*, ByteAddress startAddress, unsigned int numBytes, unsigned char* buffer);
bool CC13x2_26x2FlashDriver_Write(void*, ByteAddress startAddress, unsigned int numBytes, unsigned char* buffer, bool readModifyWrite);
bool CC13x2_26x2FlashDriver_IsBlockErased(void*, ByteAddress blockAddress, unsigned int length);
bool CC13x2_26x2FlashDriver_EraseBlock(void*, ByteAddress address);
void CC13x2_26x2FlashDriver_SetPowerState(void*, unsigned int state);

#ifdef __cplusplus
}
#endif

#endif  //_TARGET_CC13x2_26x2FLASH_DRIVER_H_
