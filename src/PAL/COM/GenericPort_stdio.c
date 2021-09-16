//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#if defined(NF_TRACE_TO_STDIO)
#include <nanoPAL_COM.h>
uint32_t StdioPort_Write(int portNum, const char *data, size_t size);

uint32_t GenericPort_Write(int comPortNum, const char *data, size_t size)
{
    (void)comPortNum;
    return StdioPort_Write(comPortNum, data, size);
}
#endif
