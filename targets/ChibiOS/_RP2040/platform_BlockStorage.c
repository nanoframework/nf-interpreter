//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_BlockStorage.h>
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
