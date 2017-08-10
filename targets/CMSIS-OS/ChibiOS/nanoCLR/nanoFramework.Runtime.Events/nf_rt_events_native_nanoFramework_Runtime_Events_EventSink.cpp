//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nf_rt_events_native.h"

static CLR_RT_HeapBlock_NativeEventDispatcher *g_Context = NULL;

void PostManagedEvent(unsigned char category, unsigned char subCategory, unsigned short data1, unsigned int data2)
{
    if(g_Context != NULL)
    {
        chSysLock();   

        unsigned int d = ((unsigned int)data1 << 16) | (category << 8) | subCategory;

        SaveNativeEventToHALQueue( g_Context, d, data2 );

        chSysUnlock();
    }
}

static HRESULT InitializeEventSink( CLR_RT_HeapBlock_NativeEventDispatcher *pContext, unsigned __int64 userData )
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
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
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
    &g_CLR_AssemblyNative_nanoFramework_Runtime_Events_EventSink
};
