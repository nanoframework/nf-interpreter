//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NANOHAL_STORAGE_OPERATION_H
#define NANOHAL_STORAGE_OPERATION_H

#include <nanoCLR_Headers.h>

#ifdef __cplusplus
extern "C"
{
#endif

    uint32_t HAL_StorageOperation(
        uint8_t operation,
        uint32_t nameLength,
        uint32_t dataLength,
        uint32_t offset,
        uint8_t *data);

#ifdef __cplusplus
}
#endif

#endif // NANOHAL_STORAGE_OPERATION_H
