//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "stdafx.h"

//--//

bool Watchdog_GetSetEnabled( bool enabled, bool fSet )
{
    return FALSE;
}

unsigned int Watchdog_GetSetTimeout( signed int timeout_ms , bool fSet )
{
    return 0;
}

//Watchdog_Behavior Watchdog_GetSetBehavior( Watchdog_Behavior behavior, bool fSet )
//{
//    return Watchdog_Behavior__None;
//}

bool Watchdog_LastOccurence( signed __int64& time, signed __int64& timeout, unsigned int& assembly, unsigned int& method, bool fSet )
{
    return FALSE;
}
