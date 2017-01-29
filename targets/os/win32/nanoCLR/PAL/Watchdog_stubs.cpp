//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "stdafx.h"

//--//

BOOL Watchdog_GetSetEnabled( BOOL enabled, BOOL fSet )
{
    return FALSE;
}

UINT32 Watchdog_GetSetTimeout( INT32 timeout_ms , BOOL fSet )
{
    return 0;
}

//Watchdog_Behavior Watchdog_GetSetBehavior( Watchdog_Behavior behavior, BOOL fSet )
//{
//    return Watchdog_Behavior__None;
//}

BOOL Watchdog_LastOccurence( INT64& time, INT64& timeout, UINT32& assembly, UINT32& method, BOOL fSet )
{
    return FALSE;
}
