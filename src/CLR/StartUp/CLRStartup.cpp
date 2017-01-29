//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CLRStartup.h"


void ClrExit()
{
    NATIVE_PROFILE_CLR_STARTUP();
    CLR_EE_DBG_SET(ExitPending);
}

void ClrReboot()
{
    NATIVE_PROFILE_CLR_STARTUP();
    CLR_EE_REBOOT_SET(ClrOnly);
    CLR_EE_DBG_SET(RebootPending);
}
