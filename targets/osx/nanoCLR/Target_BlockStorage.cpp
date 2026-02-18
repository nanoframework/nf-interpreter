//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <cstdint>

void BlockStorage_AddDevices()
{
    // TODO: wire block storage implementation for host runtime mode.
}

bool BlockStorageStream_Initialize(void *stream, uint32_t blockUsage)
{
    (void)stream;
    (void)blockUsage;
    return false;
}
