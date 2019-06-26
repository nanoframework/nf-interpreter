//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_BlockStorage.h>
#include <Target_BlockStorage_iMXRTFlashDriver.h>

// map here the Block Storage Interface to the iMXRT FlexSPI driver
IBlockStorageDevice iMXRT_BlockStorageInterface =
{                          
    &iMXRTFlexSPIDriver_InitializeDevice,
    &iMXRTFlexSPIDriver_UninitializeDevice,
    &iMXRTFlexSPIDriver_GetDeviceInfo,
    &iMXRTFlexSPIDriver_Read,
    &iMXRTFlexSPIDriver_Write,
    NULL,
    &iMXRTFlexSPIDriver_IsBlockErased,
    &iMXRTFlexSPIDriver_EraseBlock,
    NULL,
    NULL
};
