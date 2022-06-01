//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <esp32_idf.h>

#include <nanoCLR_Application.h>
#include <nanoHAL_v2.h>

void CLRStartupThread(void const *argument)
{
    CLR_SETTINGS *clrSettings = (CLR_SETTINGS *)argument;

    nanoHAL_Initialize_C();

    ClrStartup(*clrSettings);

    // nothing to deinitialize or cleanup, so it's safe to return
}
