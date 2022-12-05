//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "stdafx.h"
#include "nanoCLR_native.h"

void HAL_Windows_Debug_Print(char *szText)
{
    if (gDebugPrintCallback)
        gDebugPrintCallback(szText);
}
