//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_STM32FLASH_DRIVER_H_
#define _TARGET_STM32FLASH_DRIVER_H_ 1

#include <ch.h>
#include <hal.h>
#include <nanoPAL_BlockStorage.h>
#include <hal_nf_community.h>

#ifdef __cplusplus
extern "C" {
#endif

bool STM32FlashDriver_InitializeDevice(void*);
bool STM32FlashDriver_UninitializeDevice(void*);
DeviceBlockInfo* STM32FlashDriver_GetDeviceInfo(void*);
bool STM32FlashDriver_Read(void*, ByteAddress startAddress, unsigned int numBytes, unsigned char* buffer);
bool STM32FlashDriver_Write(void*, ByteAddress startAddress, unsigned int numBytes, unsigned char* buffer, bool readModifyWrite);
bool STM32FlashDriver_IsBlockErased(void*, ByteAddress blockAddress, unsigned int length);
bool STM32FlashDriver_EraseBlock(void*, ByteAddress address);

#ifdef __cplusplus
}
#endif

#endif  //_TARGET_STM32FLASH_DRIVER_H_
