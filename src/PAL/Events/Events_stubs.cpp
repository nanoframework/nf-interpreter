//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_Events.h>

__nfweak bool Events_Initialize()
{
    NATIVE_PROFILE_PAL_EVENTS();
    return true;
}

__nfweak bool Events_Uninitialize()
{
    NATIVE_PROFILE_PAL_EVENTS();
    return true;
}

__nfweak void Events_SetBoolTimer( bool* timerCompleteFlag, unsigned int millisecondsFromNow )
{
    NATIVE_PROFILE_PAL_EVENTS();
}
