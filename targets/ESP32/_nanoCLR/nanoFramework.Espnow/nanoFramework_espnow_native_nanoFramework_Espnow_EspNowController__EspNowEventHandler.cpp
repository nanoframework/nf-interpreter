// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include "nanoFramework_espnow_native.h"

static const int EVENT_ESPNOW_DATARECV = 2;

HRESULT Library_nanoFramework_EspNow_native_nanoFramework_EspNow_EspNowControllerEventListener::ProcessEvent_DataRecv(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock &clrRet = stack.PushValue();
    NANOCLR_CHECK_HRESULT(
        g_CLR_RT_ExecutionEngine.NewObjectFromIndex(clrRet, g_CLR_RT_WellKnownTypes.m_EspNowDataRecvEvent));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_EspNow_native_nanoFramework_EspNow_EspNowControllerEventListener::
    ProcessEvent___nanoFrameworkRuntimeEventsBaseEvent__U4__U4__SystemDateTime(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_UINT32 data1 = stack.Arg1().NumericByRef().u4;
    CLR_UINT32 subEvent = data1 >> 16;

    switch (subEvent)
    {
        case EVENT_ESPNOW_DATARECV:
            hr = ProcessEvent_DataRecv(stack);
            break;
        default:
            NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());
    }

    NANOCLR_NOCLEANUP();
}
