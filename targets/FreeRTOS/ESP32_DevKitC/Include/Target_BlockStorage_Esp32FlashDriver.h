//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_ESP32FLASH_DRIVER_H_
#define _TARGET_ESP32FLASH_DRIVER_H_ 1

#include <esp32_os.h>
#include <esp_partition.h>

#include <nanoPAL_BlockStorage.h>


//!< .NanoFramework deploy partition
#define     ESP_PARTITION_SUBTYPE_DATA_NANOCLR  (esp_partition_subtype_t)0x84                                
//--//

bool Esp32FlashDriver_InitializeDevice(void*);
bool Esp32FlashDriver_UninitializeDevice(void*);
DeviceBlockInfo* Esp32FlashDriver_GetDeviceInfo(void*);
bool Esp32FlashDriver_Read(void*, ByteAddress startAddress, unsigned int numBytes, unsigned char* buffer);
bool Esp32FlashDriver_Write(void*, ByteAddress startAddress, unsigned int numBytes, unsigned char* buffer, bool readModifyWrite);
bool Esp32FlashDriver_IsBlockErased(void*, ByteAddress blockAddress, unsigned int length);
bool Esp32FlashDriver_EraseBlock(void*, ByteAddress address);
void Esp32FlashDriver_SetPowerState(void*, unsigned int state);

extern const DRAM_ATTR esp_partition_t * g_pFlashDriver_partition;
extern const void * g_esp32_flash_start_ptr;

#endif  //_TARGET_ESP32FLASH_DRIVER_H_
