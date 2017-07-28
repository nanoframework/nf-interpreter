//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


#ifndef _NANOFRAMEWORK_RUNTIME_EVENTS_H_
#define _NANOFRAMEWORK_RUNTIME_EVENTS_H_

#include <nanoCLR_Interop.h>
#include "Core.h"

struct Library_nanoframework_runtime_events_nanoFramework_Runtime_Events_NativeEventDispatcher
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
    static HRESULT                                  GetEventDispatcher   ( CLR_RT_StackFrame& stack, CLR_RT_HeapBlock_NativeEventDispatcher*& port  );
    static CLR_RT_HeapBlock_NativeEventDispatcher*  GetEventDispatcher   ( CLR_RT_StackFrame& stack                                  );
};

struct Library_nanoframework_runtime_events_nanoFramework_Runtime_Events_EventSink
{
    static const int FIELD_STATIC___eventSink = 0;
    static const int FIELD_STATIC___eventInfoTable = 1;

    NANOCLR_NATIVE_DECLARE(EventConfig___VOID);

    //--//

};

struct Library_nanoframework_runtime_events_nanoFramework_Runtime_Events_EventSink__EventInfo
{
    static const int FIELD__EventListener = 1;
    static const int FIELD__EventFilter = 2;
    static const int FIELD__EventProcessor = 3;
    static const int FIELD__Category = 4;


    //--//

};

struct Library_nanoframework_runtime_events_nanoFramework_Runtime_Events_BaseEvent
{
    static const int FIELD__Source = 1;
    static const int FIELD__Message = 2;


    //--//

};

struct Library_nanoframework_runtime_events_nanoFramework_Runtime_Events_GenericEvent
{
    static const int FIELD__Category = 3;
    static const int FIELD__Data = 4;
    static const int FIELD__Tag = 5;
    static const int FIELD__Time = 6;


    //--//
};



extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_Events;

#endif  //_NANOFRAMEWORK_RUNTIME_EVENTS_H_
