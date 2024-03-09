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

    void HAL_StorageOperation(
        uint8_t operation,
        uint32_t *length,
        uint8_t *storageName,
        uint8_t *data,
        uint32_t *errorCode);

#ifdef __cplusplus
}
#endif

#endif // NANOHAL_STORAGE_OPERATIN_H