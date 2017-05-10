//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"


HRESULT Library_corlib_native_System_TimeZone::GetTimeZoneOffset___STATIC__I8( CLR_RT_StackFrame& stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();    

    // TODO 
    // check what's the best way of dealing with timezone offsets...
    //CLR_INT64 offset = TIME_ZONE_OFFSET;
    CLR_INT64 offset = 0;

    stack.SetResult_I8( offset );

    NANOCLR_NOCLEANUP_NOLABEL();
}
