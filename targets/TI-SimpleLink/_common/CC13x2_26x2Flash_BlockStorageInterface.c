//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_BlockStorage.h>
#include <Target_BlockStorage_CC13x2_26x2FlashDriver.h>

// map here the Block Storage Interface to the CC13x2_26x2 driver
IBlockStorageDevice CC13x2_26x2Flash_BlockStorageInterface =
{
    &CC13x2_26x2FlashDriver_InitializeDevice,
    &CC13x2_26x2FlashDriver_UninitializeDevice,
    &CC13x2_26x2FlashDriver_GetDeviceInfo,
    &CC13x2_26x2FlashDriver_Read,
    &CC13x2_26x2FlashDriver_Write,
    NULL,
    &CC13x2_26x2FlashDriver_IsBlockErased,
    &CC13x2_26x2FlashDriver_EraseBlock,
    NULL,
    NULL
};
