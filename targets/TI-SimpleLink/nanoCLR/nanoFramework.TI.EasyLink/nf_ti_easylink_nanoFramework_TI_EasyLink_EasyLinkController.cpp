//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_ti_easylink_target.h"


typedef Library_nf_ti_easylink_nanoFramework_TI_EasyLink_TransmitPacket TransmitPacket;
typedef Library_nf_ti_easylink_nanoFramework_TI_EasyLink_ReceivedPacket ReceivedPacket;

static EasyLink_RxPacket latestRxPacket;
static EasyLink_Status latestOperationStatus;

void TxDone(EasyLink_Status status)
{
    // store status
    latestOperationStatus = status;

    // fire event for Tx completed
    Events_Set(SYSTEM_EVENT_FLAG_RADIO);
}

static void RxDone(EasyLink_RxPacket* rxPacket, EasyLink_Status status)
{
    // store status
    latestOperationStatus = status;

    // packet received successfully
    if (status == EasyLink_Status_Success)
    {
        // copy packet
        memcpy(&latestRxPacket, rxPacket, sizeof(EasyLink_RxPacket));
    }
    else
    {
        // TODO
    }

    // fire event for Rx completed
    Events_Set(SYSTEM_EVENT_FLAG_RADIO);
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

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::GetConfiguration___U4__nanoFrameworkTIEasyLinkControlOption( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::SetConfigurationNative___I4__nanoFrameworkTIEasyLinkControlOption__U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

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

    NANOCLR_CHECK_HRESULT(EnableRxAddressFilter(stack));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::InitNative___STATIC__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    uint8_t i = 0;
    EasyLink_Status  status;
    EasyLink_PhyType phyType = (EasyLink_PhyType)0;

    CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

    // Initialize the EasyLink parameters to their default values 
    EasyLink_Params easyLink_params;
    EasyLink_Params_init(&easyLink_params);

    // get managed Phy Type
    phyType = (EasyLink_PhyType)(pThis[ FIELD___phyType ].NumericByRef().u1);

    // check if this PHY config is supported
    while(i < EasyLink_numSupportedPhys)
    {
        if(EasyLink_supportedPhys[i].EasyLink_phyType == phyType)
        {
            break;
        }
        i++;
    }

    if(i == EasyLink_numSupportedPhys)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
    }

    easyLink_params.ui32ModType = phyType;

    // Initialize EasyLink
    status = EasyLink_init(&easyLink_params);
    if(status != EasyLink_Status_Success)
    {
        // fail to start
        stack.SetResult_I4(status);

        NANOCLR_SET_AND_LEAVE(S_OK);
    }

    latestOperationStatus = EasyLink_Status_Success;

    stack.SetResult_I4(status);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::ReceiveNative___STATIC__I4__BYREF_nanoFrameworkTIEasyLinkReceivedPacket__mscorlibSystemTimeSpan( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array* payloadBuffer;
    CLR_RT_HeapBlock_Array* address;

    CLR_RT_HeapBlock    rxTimeout;
    CLR_RT_HeapBlock*   packet;
    CLR_INT64*          timeoutTicks;
    CLR_INT32           timeout_ms;
    EasyLink_Status     status;
    bool                eventResult = true;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    // get value for timeout parameter
    timeout_ms = stack.Arg1().NumericByRef().s4;

    // set timeout
    rxTimeout.SetInteger( timeout_ms );

    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks( rxTimeout, timeoutTicks ));

    // setup the operation and init buffers
    if(stack.m_customState == 1)
    {
        // // get pointer to managed TransmitPacket
        // packet = stack.Arg0().Dereference();  FAULT_ON_NULL(packet);

        // // dereference the payload buffer
        // payloadBuffer = packet[ TransmitPacket::FIELD___payload ].DereferenceArray();

        // // get payload length
        // txPacket.len = payloadBuffer->m_numOfElements;

        // // copy buffer to packet
        // memcpy(txPacket.payload, payloadBuffer->GetFirstElement(), txPacket.len);

        // // dereference the address buffer
        // address = packet[ TransmitPacket::FIELD___address ].DereferenceArray();

        // // get payload length
        // txPacket.len = payloadBuffer->m_numOfElements;

        // // copy buffer to packet
        // memcpy(txPacket.dstAddr, address->GetFirstElement(), address->m_numOfElements);

        // enter receive mode
        status = EasyLink_receiveAsync(RxDone, 0);
        if(status != EasyLink_Status_Success)
        {
            // fail to start
            stack.SetResult_I4(status);

            NANOCLR_SET_AND_LEAVE(S_OK);
        }

        // bump custom state
        stack.m_customState = 2;
    }

    while(eventResult)
    {
        // non-blocking wait allowing other threads to run while we wait for the receive operation to complete
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.WaitEvents( stack.m_owningThread, *timeoutTicks, CLR_RT_ExecutionEngine::c_Event_Radio, eventResult ));
    
        if(eventResult)
        {
            // event occurred

            // get argument BYREF
            CLR_RT_HeapBlock& packetBlock = stack.Arg0();
            
            // need to dereference to access class fields
            CLR_RT_HeapBlock* packet = packetBlock.Dereference();

            // fill fields
            packet[ ReceivedPacket::FIELD___rxTimeout ].NumericByRef().u4 = latestRxPacket.rxTimeout;
            packet[ ReceivedPacket::FIELD___rssi ].NumericByRef().s1 = latestRxPacket.rssi;
            packet[ ReceivedPacket::FIELD___absoluteTime ].NumericByRef().u4 = latestRxPacket.absTime;

            // address it's an array
            address = packet[ ReceivedPacket::FIELD___address ].DereferenceArray();
            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( *address, (CLR_UINT32)ARRAYSIZE(latestRxPacket.dstAddr), g_CLR_RT_WellKnownTypes.m_UInt8 ));
            // copy address
            memcpy(address->GetFirstElement(), latestRxPacket.dstAddr, ARRAYSIZE(latestRxPacket.dstAddr));

            // payload it's an array
            payloadBuffer = packet[ ReceivedPacket::FIELD___payload ].DereferenceArray();
            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( *payloadBuffer, (CLR_UINT32)ARRAYSIZE(latestRxPacket.payload), g_CLR_RT_WellKnownTypes.m_UInt8 ));
            // copy payload content
            memcpy(payloadBuffer->GetFirstElement(), latestRxPacket.payload, ARRAYSIZE(latestRxPacket.payload));

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

    // TODO: should return the real status
    stack.SetResult_I4(latestOperationStatus);

    // null pointers and vars
    pThis = NULL;

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::TransmitNative___STATIC__I4__nanoFrameworkTIEasyLinkTransmitPacket__mscorlibSystemTimeSpan__I4( CLR_RT_StackFrame& stack )
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
    EasyLink_Status     status;
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
        status = EasyLink_transmitAsync(&txPacket, TxDone);
        if(status != EasyLink_Status_Success)
        {
            // fail to start
            stack.SetResult_I4(status);

            NANOCLR_SET_AND_LEAVE(S_OK);
        }

        // bump custom state
        stack.m_customState = 2;
    }

    while(eventResult)
    {
        // non-blocking wait allowing other threads to run while we wait for the transmit operation to complete
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.WaitEvents( stack.m_owningThread, *timeoutTicks, CLR_RT_ExecutionEngine::c_Event_Radio, eventResult ));
    
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

    stack.SetResult_I4(latestOperationStatus);

    // null pointers and vars
    pThis = NULL;

    NANOCLR_NOCLEANUP();
}

// To make it more efficient this processing is separated, making it reusable  
HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::EnableRxAddressFilter( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    uint8_t* addressTable = NULL;
    uint8_t* addressTableCursor = NULL;
    uint8_t addressCount, addressSize;

    CLR_RT_HeapBlock_Array* addressList;
    CLR_RT_HeapBlock_Array* address;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    addressList = pThis[ FIELD___addressFilter ].DereferenceArray();

    // check collection length...
    addressCount = addressList->m_numOfElements;

    if(addressCount > 0)
    {
        //... and size of the addressese (assuming that all of them, if any others, have the same length)
        address = ((CLR_RT_HeapBlock_Array*)addressList->GetFirstElement())->DereferenceArray();

        addressSize = address->m_numOfElements;

        // allocate memory for the address table to be built
        addressTable = (uint8_t*)platform_malloc( addressCount * addressSize );
        
        // sanity check
        if( addressTable == NULL )
        {
            // mem allocation failed
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }

        // need to copy the pointer to reuse it later
        addressTableCursor = addressTable;

        // now loop through the collection to fill the table
        for(int i = 0; i < addressList->m_numOfElements; i++)
        {
            address = ((CLR_RT_HeapBlock_Array*)addressList->GetElement(i))->DereferenceArray();
            if(address != NULL)
            {
                memcpy(addressTableCursor, address->GetFirstElement(), addressSize);

                addressTableCursor += addressSize;
            }
        }

        // don't check the result, failures occur when this is set without the module initialized
        EasyLink_enableRxAddrFilter(addressTable, addressCount, addressSize);
    }

    if(addressTable != NULL)
    {
        platform_free(addressTable);
    }

    NANOCLR_NOCLEANUP();
}
