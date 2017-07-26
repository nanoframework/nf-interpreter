//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


#ifndef _NANOFRAMEWORK_RUNTIME_EVENTS_NATIVE_H_
#define _NANOFRAMEWORK_RUNTIME_EVENTS_NATIVE_H_

#include <nanoCLR_Interop.h>
#include "Core.h"

struct Library_nanoframework_runtime_events_native_nanoFramework_Runtime_Events_NativeEventDispatcher
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



extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_Events;

#endif  //_NANOFRAMEWORK_RUNTIME_EVENTS_NATIVE_H_
