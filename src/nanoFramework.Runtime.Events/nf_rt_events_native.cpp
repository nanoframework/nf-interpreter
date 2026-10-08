//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

// need this include here to use this in nanoCLR WIN32 project
#include <stdafx.h>

#include "nf_rt_events_native.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    Library_nf_rt_events_native_nanoFramework_Runtime_Events_EventSink::EventConfig___VOID,
    Library_nf_rt_events_native_nanoFramework_Runtime_Events_NativeEventDispatcher::DisableInterrupt___VOID,
    Library_nf_rt_events_native_nanoFramework_Runtime_Events_NativeEventDispatcher::Dispose___VOID__BOOLEAN,
    Library_nf_rt_events_native_nanoFramework_Runtime_Events_NativeEventDispatcher::EnableInterrupt___VOID,
    Library_nf_rt_events_native_nanoFramework_Runtime_Events_NativeEventDispatcher::_ctor___VOID__STRING__U8,
    Library_nf_rt_events_native_nanoFramework_Runtime_Events_WeakDelegate::Combine___STATIC__SystemDelegate__SystemDelegate__SystemDelegate,
    Library_nf_rt_events_native_nanoFramework_Runtime_Events_WeakDelegate::Remove___STATIC__SystemDelegate__SystemDelegate__SystemDelegate,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_Events =
{
    "nanoFramework.Runtime.Events",
    0x774AB407,
    method_lookup,
    ARRAYSIZE(method_lookup)
};

// clang-format on
