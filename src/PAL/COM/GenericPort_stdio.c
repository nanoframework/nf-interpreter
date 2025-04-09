//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_COM.h>

uint32_t GenericPort_Write(int portNum, const char *data, size_t size)
{
    (void)portNum;

    return StdioPort_Write(data, size);
}

// defined as weak to allow platform/target provide it's own implementation
__nfweak void StdioPort_Init()
{
}

// defined as weak to allow platform/target provide it's own implementation
__nfweak uint32_t StdioPort_Write(const char *data, size_t size)
{
    (void)data;
    (void)size;

    return 0;
}
