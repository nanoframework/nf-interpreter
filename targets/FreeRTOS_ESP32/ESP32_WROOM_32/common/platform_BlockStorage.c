//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_BlockStorage.h>
#include <Target_BlockStorage_ESP32FlashDriver.h>

// map here the Block Storage Interface to the ESP32 driver
IBlockStorageDevice ESP32Flash_BlockStorageInterface =
{                          
    &Esp32FlashDriver_InitializeDevice,
    &Esp32FlashDriver_UninitializeDevice,
    &Esp32FlashDriver_GetDeviceInfo,
    &Esp32FlashDriver_Read,
    &Esp32FlashDriver_Write,
    NULL,
    &Esp32FlashDriver_IsBlockErased,
    &Esp32FlashDriver_EraseBlock,
    NULL,
    &Esp32FlashDriver_GetMemoryMappedAddress,
};
