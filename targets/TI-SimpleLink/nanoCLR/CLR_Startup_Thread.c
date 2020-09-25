//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoCLR_Application.h>
#include <nanoHAL_v2.h>
#include <string.h>
#include <xdc/std.h>

void CLRStartupThread(UArg arg0, UArg arg1)
{
    (void)arg1;

    CLR_SETTINGS* clrSettings = (CLR_SETTINGS*)arg0;

    nanoHAL_Initialize_C();

    ClrStartup(*clrSettings);

    // nothing to deinitialize or cleanup, so it's safe to return
}
