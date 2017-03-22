//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "stdafx.h"
//--//

//HRESULT Watchdog_Enable( unsigned int TimeoutMilliseconds, WATCHDOG_INTERRUPT_CALLBACK callback, void* context )
//{
//    return S_OK; // UNDONE: FIXME: return EmulatorNative::GetIWatchdogDriver()->Enable( TimeoutMilliseconds );
//}

void Watchdog_Disable()
{
    // UNDONE: FIXME: EmulatorNative::GetIWatchdogDriver()->Disable();
}

void Watchdog_ResetCpu()
{
    // UNDONE: FIXME: EmulatorNative::GetIWatchdogDriver()->ResetCpu();
}

void Watchdog_ResetCounter()
{
    // UNDONE: FIXME: EmulatorNative::GetIWatchdogDriver()->ResetCounter();
}

