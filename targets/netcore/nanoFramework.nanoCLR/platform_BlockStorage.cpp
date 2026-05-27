//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "stdafx.h"

#include <nanoPAL_BlockStorage.h>
#include <Target_BlockStorage_TransientFlashDriver.h>

// map here the Block Storage Interface to the Transient driver

IBlockStorageDevice TransientFlash_BlockStorageInterface = {
    &TransientFlashDriver_InitializeDevice,
    &TransientFlashDriver_UninitializeDevice,
    &TransientFlashDriver_GetDeviceInfo,
    &TransientFlashDriver_Read,
    &TransientFlashDriver_Write,
    nullptr,
    &TransientFlashDriver_IsBlockErased,
    &TransientFlashDriver_EraseBlock,
    nullptr,
    nullptr};
