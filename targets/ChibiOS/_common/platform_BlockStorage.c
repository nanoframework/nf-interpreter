//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_BlockStorage.h>

#if defined(RP2040) || defined(RP2350)

#include <Target_BlockStorage_RP2040FlashDriver.h>

// Map here the Block Storage Interface to the RP2040 flash driver
IBlockStorageDevice RP2040Flash_BlockStorageInterface = {
    &RP2040FlashDriver_InitializeDevice,
    &RP2040FlashDriver_UninitializeDevice,
    &RP2040FlashDriver_GetDeviceInfo,
    &RP2040FlashDriver_Read,
    &RP2040FlashDriver_Write,
    NULL,
    &RP2040FlashDriver_IsBlockErased,
    &RP2040FlashDriver_EraseBlock,
    NULL,
    NULL};

#else

#include <Target_BlockStorage_STM32FlashDriver.h>

// map here the Block Storage Interface to the STM32 driver
IBlockStorageDevice STM32Flash_BlockStorageInterface =
{                          
    &STM32FlashDriver_InitializeDevice,
    &STM32FlashDriver_UninitializeDevice,
    &STM32FlashDriver_GetDeviceInfo,
    &STM32FlashDriver_Read,
    &STM32FlashDriver_Write,
    NULL,
    &STM32FlashDriver_IsBlockErased,    
    &STM32FlashDriver_EraseBlock,
    NULL,
    NULL
};

#endif
