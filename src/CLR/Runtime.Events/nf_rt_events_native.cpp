//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


#include "nf_rt_events_native.h"


static const CLR_RT_MethodHandler method_lookup[] =
{
    Library_nf_rt_events_native_nanoFramework_Runtime_Events_NativeEventDispatcher::EnableInterrupt___VOID,
    Library_nf_rt_events_native_nanoFramework_Runtime_Events_NativeEventDispatcher::DisableInterrupt___VOID,
    Library_nf_rt_events_native_nanoFramework_Runtime_Events_NativeEventDispatcher::Dispose___VOID__BOOLEAN,
    NULL,
    NULL,
    Library_nf_rt_events_native_nanoFramework_Runtime_Events_NativeEventDispatcher::_ctor___VOID__STRING__U8,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_nf_rt_events_native_nanoFramework_Runtime_Events_EventSink::EventConfig___VOID,
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
    NULL,
    NULL,
    NULL,
    NULL,
    Library_nf_rt_events_native_nanoFramework_Runtime_Events_WeakDelegate::Combine___STATIC__mscorlibSystemDelegate__mscorlibSystemDelegate__mscorlibSystemDelegate,
    Library_nf_rt_events_native_nanoFramework_Runtime_Events_WeakDelegate::Remove___STATIC__mscorlibSystemDelegate__mscorlibSystemDelegate__mscorlibSystemDelegate,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_Events =
{
    "nanoFramework.Runtime.Events", 
    0xBFF88292,
    method_lookup,
    { 1, 0, 5, 0 }
};
