//-----------------------------------------------------------------------------
//
//                   ** WARNING! **
//    This file was generated automatically by a tool.
//    Re-running the tool will overwrite this file.
//    You should copy this file to a custom location
//    before adding any customization in the copy to
//    prevent loss of your changes when the tool is
//    re-run.
//
//-----------------------------------------------------------------------------

#include "nanoFramework_espnow_native.h"

HRESULT
Library_nanoFramework_espnow_native_nanoFramework_Espnow_EspNowController__EspNowEventHandler::CopyByteArrayToCLRArray(
    CLR_RT_HeapBlock &target,
    uint8_t *src,
    CLR_UINT32 length)
{
    NANOCLR_HEADER();

    DEBUG_FENTER();

    CLR_UINT8 *firstTargetByte;
    CLR_RT_HeapBlock_Array *targetArray;

    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(target, length, g_CLR_RT_WellKnownTypes.m_UInt8));

    targetArray = target.DereferenceArray();
    if (!targetArray)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    DEBUG_WRITELINE("copying %d bytes", length);

    firstTargetByte = targetArray->GetFirstElement();
    memcpy(firstTargetByte, src, length);

    DEBUG_FEXIT();

    NANOCLR_NOCLEANUP();
}

HRESULT
Library_nanoFramework_espnow_native_nanoFramework_Espnow_EspNowController__EspNowEventHandler::ProcessEvent_DataSent(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    DEBUG_FENTER();

    EspNowDataSentEventData *sentEventData = (EspNowDataSentEventData *)stack.Arg2().NumericByRef().u4;
    CLR_RT_HeapBlock *dataSentEvent;

    // create nanoFramework.Espnow.EspNowController.DataSentEventInternal instance to return
    CLR_RT_HeapBlock &clrRet = stack.PushValueAndClear();

    NANOCLR_CHECK_HRESULT(
        g_CLR_RT_ExecutionEngine.NewObjectFromIndex(clrRet, g_CLR_RT_WellKnownTypes.m_EspNowDataSentEvent));

    dataSentEvent = clrRet.Dereference();
    if (!dataSentEvent)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    // clrRet.PeerMac
    NANOCLR_CHECK_HRESULT(CopyByteArrayToCLRArray(
        dataSentEvent[Library_nanoFramework_espnow_native_nanoFramework_Espnow_DataSentEventInternal::FIELD__PeerMac],
        sentEventData->peer_mac,
        6));

    // clrRet.Status
    dataSentEvent[Library_nanoFramework_espnow_native_nanoFramework_Espnow_DataSentEventInternal::FIELD__Status]
        .SetInteger(sentEventData->status);

    DEBUG_FEXIT();

    NANOCLR_NOCLEANUP();
}

HRESULT
Library_nanoFramework_espnow_native_nanoFramework_Espnow_EspNowController__EspNowEventHandler::ProcessEvent_DataRecv(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    DEBUG_FENTER();

    EspNowDataRecvEventData *recvEventData = (EspNowDataRecvEventData *)stack.Arg2().NumericByRef().u4;
    CLR_RT_HeapBlock *dataRecvEvent;

    // create nanoFramework.Espnow.EspNowController.DataRecvEventInternal instance to return
    CLR_RT_HeapBlock &clrRet = stack.PushValue();
    NANOCLR_CHECK_HRESULT(
        g_CLR_RT_ExecutionEngine.NewObjectFromIndex(clrRet, g_CLR_RT_WellKnownTypes.m_EspNowDataRecvEvent));

    dataRecvEvent = clrRet.Dereference();
    if (!dataRecvEvent)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    // clrRet.PeerMac
    NANOCLR_CHECK_HRESULT(CopyByteArrayToCLRArray(
        dataRecvEvent[Library_nanoFramework_espnow_native_nanoFramework_Espnow_DataRecvEventInternal::FIELD__PeerMac],
        recvEventData->peer_mac,
        6));

    // clrRet.Data
    NANOCLR_CHECK_HRESULT(CopyByteArrayToCLRArray(
        dataRecvEvent[Library_nanoFramework_espnow_native_nanoFramework_Espnow_DataRecvEventInternal::FIELD__Data],
        recvEventData->data,
        recvEventData->dataLen));

    // clrRet.DataLen
    dataRecvEvent[Library_nanoFramework_espnow_native_nanoFramework_Espnow_DataRecvEventInternal::FIELD__DataLen]
        .SetInteger(recvEventData->dataLen);

    DEBUG_FEXIT();

    NANOCLR_NOCLEANUP();
}

HRESULT
Library_nanoFramework_espnow_native_nanoFramework_Espnow_EspNowController__EspNowEventHandler::
    ProcessEvent___nanoFrameworkRuntimeEventsBaseEvent__U4__U4__SystemDateTime(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    DEBUG_FENTER();

    CLR_UINT32 data1 = stack.Arg1().NumericByRef().u4;
    CLR_UINT32 subEvent = data1 >> 16;

    DEBUG_WRITELINE("subevent: %d", subEvent);

    switch (subEvent)
    {
        case EVENT_ESPNOW_DATASENT:
            hr = ProcessEvent_DataSent(stack);
            break;
        case EVENT_ESPNOW_DATARECV:
            hr = ProcessEvent_DataRecv(stack);
            break;
        default:
            NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());
    }

    if (FAILED(hr))
    {
        DEBUG_WRITELINE("failed hr: %x", hr);
    }

    DEBUG_FEXIT();

    NANOCLR_NOCLEANUP();
}
