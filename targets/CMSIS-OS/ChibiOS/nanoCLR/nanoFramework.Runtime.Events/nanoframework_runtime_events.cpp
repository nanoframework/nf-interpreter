//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


#include "nanoframework_runtime_events.h"


static const CLR_RT_MethodHandler method_lookup[] =
{
    Library_nanoframework_runtime_events_nanoFramework_Runtime_Events_NativeEventDispatcher::EnableInterrupt___VOID,
    Library_nanoframework_runtime_events_nanoFramework_Runtime_Events_NativeEventDispatcher::DisableInterrupt___VOID,
    Library_nanoframework_runtime_events_nanoFramework_Runtime_Events_NativeEventDispatcher::Dispose___VOID__BOOLEAN,
    NULL,
    NULL,
    Library_nanoframework_runtime_events_nanoFramework_Runtime_Events_NativeEventDispatcher::_ctor___VOID__STRING__U8,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_nanoframework_runtime_events_nanoFramework_Runtime_Events_EventSink::EventConfig___VOID,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_Events =
{
    "nanoFramework.Runtime.Events", 
    0x9AFF4455,
    method_lookup
};
