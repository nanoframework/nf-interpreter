//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_ti_easylink_target.h"


typedef Library_nf_ti_easylink_nanoFramework_TI_EasyLink_TransmitPacket TransmitPacket;

void TxDone(EasyLink_Status status)
{
    // fire event for Tx completed
    Events_Set(SYSTEM_EVENT_FLAG_RADIO_TX);
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::get_AbsoluteTime___U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::get_Rssi___I1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::get_Frequency___U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::set_Frequency___VOID__U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::get_RfPower___I1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::set_RfPower___VOID__I1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::get_IeeeAddress___SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::SetConfiguration___nanoFrameworkTIEasyLinkStatus__nanoFrameworkTIEasyLinkControlOption__U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::GetConfiguration___U4__nanoFrameworkTIEasyLinkControlOption( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::InitNative___STATIC__VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    uint8_t i = 0;
    EasyLink_PhyType phyType = (EasyLink_PhyType)0;

    CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

    // Initialize the EasyLink parameters to their default values 
    EasyLink_Params easyLink_params;
    EasyLink_Params_init(&easyLink_params);

    // get managed Phy Type
    phyType = (EasyLink_PhyType)pThis[ FIELD___phyType ].NumericByRef().u1;

    // check if this PHY config is supported
    while(i < EasyLink_numSupportedPhys)
    {
        if(EasyLink_supportedPhys[i].EasyLink_phyType == phyType)
        {
            break;
        }
        i++;
    }

    if(i > EasyLink_numSupportedPhys)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
    }

    easyLink_params.ui32ModType = phyType;

    // Initialize EasyLink
    if(EasyLink_init(&easyLink_params) != EasyLink_Status_Success)
    {
        // TODO
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::DisposeNative___STATIC__VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::EnableRxAddressFilterNative___STATIC__VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::TransmitNative___STATIC__nanoFrameworkTIEasyLinkStatus__nanoFrameworkTIEasyLinkTransmitPacket__mscorlibSystemTimeSpan__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array* payloadBuffer;
    CLR_RT_HeapBlock_Array* address;

    CLR_RT_HeapBlock    txTimeout;
    CLR_RT_HeapBlock*   packet;
    CLR_INT64*          timeoutTicks;
    CLR_INT32           timeout_ms;
    uint32_t            dueTime;
    uint32_t            absTime;
    EasyLink_TxPacket   txPacket = { {0}, 0, 0, {0} };
    bool                eventResult = true;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    // get value for timeout parameter
    timeout_ms = stack.Arg1().NumericByRef().s4;

    // set timeout
    txTimeout.SetInteger( timeout_ms );

    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks( txTimeout, timeoutTicks ));

    // setup the operation and init buffers
    if(stack.m_customState == 1)
    {
        // get pointer to managed TransmitPacket
        packet = stack.Arg0().Dereference();  FAULT_ON_NULL(packet);

        // dereference the payload buffer
        payloadBuffer = packet[ TransmitPacket::FIELD___payload ].DereferenceArray();

        // get payload length
        txPacket.len = payloadBuffer->m_numOfElements;

        // copy buffer to packet
        memcpy(txPacket.payload, payloadBuffer->GetFirstElement(), txPacket.len);

        // dereference the address buffer
        address = packet[ TransmitPacket::FIELD___address ].DereferenceArray();

        // get payload length
        txPacket.len = payloadBuffer->m_numOfElements;

        // copy buffer to packet
        memcpy(txPacket.dstAddr, address->GetFirstElement(), address->m_numOfElements);

        // get dueTime managed argument
        dueTime = stack.Arg2().NumericByRef().s4;

        if(dueTime > 0)
        {
            EasyLink_getAbsTime(&absTime);
            txPacket.absTime = absTime + EasyLink_ms_To_RadioTime(dueTime);
        }

        // start transmission
        EasyLink_transmitAsync(&txPacket, TxDone);

        // bump custom state
        stack.m_customState = 2;
    }

    while(eventResult)
    {
        // non-blocking wait allowing other threads to run while we wait for the Spi transaction to complete
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.WaitEvents( stack.m_owningThread, *timeoutTicks, CLR_RT_ExecutionEngine::c_Event_RadioTx, eventResult ));
    
        if(eventResult)
        {
            // event occurred

            // done here
            break;
        }
        else
        {
            // timeout occurred
            NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
        }
    }

    // pop timeout heap block from stack
    stack.PopValue();

    stack.SetResult_U4(0);

    // null pointers and vars
    pThis = NULL;

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::ReceiveNative___STATIC__nanoFrameworkTIEasyLinkStatus__BYREF_nanoFrameworkTIEasyLinkReceivedPacket__mscorlibSystemTimeSpan( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}
