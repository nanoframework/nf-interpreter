//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NANOHAL_STORAGE_OPERATION_H
#define NANOHAL_STORAGE_OPERATION_H

#include <nanoCLR_Headers.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH nanoFramework.Tools.Debugger.WireProtocol.StorageOperationErrorCode (in managed code) !!! //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum StorageOperationErrorCode
{
    // no error
    NoError = 0x0001,
    // write error
    WriteError = 0x0010,
    // delete error
    DeleteError = 0x0020,
    // platform error
    PlatformError = 0x0030,

} StorageOperationErrorCode;

#ifdef __cplusplus
extern "C"
{
#endif

    uint32_t HAL_StorageOperation(uint8_t operation, uint32_t dataLength, uint32_t offset, uint8_t *data);

#ifdef __cplusplus
}
#endif

#endif // NANOHAL_STORAGE_OPERATION_H
