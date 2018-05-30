//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "stdafx.h"
#include "nf_rt_events_native.h"

static CLR_RT_HeapBlock_NativeEventDispatcher *g_Context = NULL;

void PostManagedEvent(uint8_t category, uint8_t subCategory, uint16_t data1, uint32_t data2)
{
    if(g_Context != NULL)
    {
        uint16_t d = ((uint16_t)data1 << 16) | (category << 8) | subCategory;

        SaveNativeEventToHALQueue( g_Context, d, data2 );
    }
}

static HRESULT InitializeEventSink( CLR_RT_HeapBlock_NativeEventDispatcher *pContext, uint64_t userData )
{
   g_Context  = pContext;

   return S_OK;
}

static HRESULT EnableDisableEventSink( CLR_RT_HeapBlock_NativeEventDispatcher *pContext, bool fEnable )
{
   return S_OK;
}

static HRESULT CleanupEventSink( CLR_RT_HeapBlock_NativeEventDispatcher *pContext )
{
    g_Context = NULL;

    CleanupNativeEventsFromHALQueue( pContext );

    return S_OK;
}

HRESULT Library_nf_rt_events_native_nanoFramework_Runtime_Events_EventSink::EventConfig___VOID( CLR_RT_StackFrame& stack )
{
    return S_OK;
}

static const CLR_RT_DriverInterruptMethods g_CLR_AssemblyNative_nanoFramework_Runtime_Events_EventSink = 
{ 
    InitializeEventSink,
    EnableDisableEventSink,
    CleanupEventSink
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_Events_EventSink_DriverProcs =
{
    "EventSink", 
    DRIVER_INTERRUPT_METHODS_CHECKSUM,
    &g_CLR_AssemblyNative_nanoFramework_Runtime_Events_EventSink,
    { 1, 0, 0, 0 }
};
