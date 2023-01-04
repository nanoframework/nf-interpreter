//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2006-2015, ARM Limited, All Rights Reserved
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <ssl.h> 
#include "mbedtls.h"

bool ssl_initialize_internal()
{
    memset(&g_SSL_Driver, 0, sizeof(g_SSL_Driver));

    return true;
}
