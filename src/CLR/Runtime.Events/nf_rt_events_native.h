//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


#ifndef _NF_RT_EVENTS_NATIVE_H_
#define _NF_RT_EVENTS_NATIVE_H_


#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>


struct Library_nf_rt_events_native_nanoFramework_Runtime_Events_NativeEventDispatcher
{
    static const int FIELD__threadSpawn = 1;
    static const int FIELD__callbacks = 2;
    static const int FIELD__disposed = 3;
    static const int FIELD___NativeEventDispatcher = 4;

    NANOCLR_NATIVE_DECLARE(EnableInterrupt___VOID);
    NANOCLR_NATIVE_DECLARE(DisableInterrupt___VOID);
    NANOCLR_NATIVE_DECLARE(Dispose___VOID__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__STRING__U8);

    //--//
    static CLR_RT_ObjectToEvent_Source*             GetEventDispReference( CLR_RT_StackFrame& stack                                  );
    static HRESULT                                  GetEventDispatcher   ( CLR_RT_StackFrame& stack, CLR_RT_HeapBlock_NativeEventDispatcher*& event  );
    static CLR_RT_HeapBlock_NativeEventDispatcher*  GetEventDispatcher   ( CLR_RT_StackFrame& stack                                  );
};

struct Library_nf_rt_events_native_nanoFramework_Runtime_Events_EventSink
{
    static const int FIELD_STATIC__s_eventSink = 0;
    static const int FIELD_STATIC__s_eventInfoTable = 1;

    NANOCLR_NATIVE_DECLARE(EventConfig___VOID);

    //--//

};

struct Library_nf_rt_events_native_nanoFramework_Runtime_Events_EventSink__EventInfo
{
    static const int FIELD__EventListener = 1;
    static const int FIELD__EventFilter = 2;
    static const int FIELD__EventProcessor = 3;
    static const int FIELD__Category = 4;


    //--//

};

struct Library_nf_rt_events_native_nanoFramework_Runtime_Events_BaseEvent
{
    static const int FIELD__Source = 1;
    static const int FIELD__Message = 2;


    //--//

};

struct Library_nf_rt_events_native_nanoFramework_Runtime_Events_EventArgs
{
    static const int FIELD_STATIC__Empty = 2;


    //--//

};

struct Library_nf_rt_events_native_nanoFramework_Runtime_Events_GenericEvent
{
    static const int FIELD__Category = 3;
    static const int FIELD__Data = 4;
    static const int FIELD__Time = 5;


    //--//

};

struct Library_nf_rt_events_native_nanoFramework_Runtime_Events_WeakDelegate
{
    NANOCLR_NATIVE_DECLARE(Combine___STATIC__mscorlibSystemDelegate__mscorlibSystemDelegate__mscorlibSystemDelegate);
    NANOCLR_NATIVE_DECLARE(Remove___STATIC__mscorlibSystemDelegate__mscorlibSystemDelegate__mscorlibSystemDelegate);

    //--//

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_Events_EventSink_DriverProcs;

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_Events;

#endif  //_NF_RT_EVENTS_NATIVE_H_
