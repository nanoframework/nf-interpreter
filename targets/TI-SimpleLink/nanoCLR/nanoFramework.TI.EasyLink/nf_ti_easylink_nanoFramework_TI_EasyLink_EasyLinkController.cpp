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

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::DisposeNative___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::UpdateRxAddressFilterNative___VOID( CLR_RT_StackFrame& stack )
{
    return UpdateRxAddressFilter(stack);
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::InitNative___I4( CLR_RT_StackFrame& stack )
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

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::ReceiveNative___I4__BYREF_nanoFrameworkTIEasyLinkReceivedPacket__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Index    receivedPacketTypeDef;
    CLR_RT_HeapBlock_Array* buffer;
    CLR_RT_HeapBlock        rxTimeout;
    CLR_RT_HeapBlock*       packet;
    CLR_RT_HeapBlock        hbObj;

    CLR_INT64*          timeoutTicks;
    CLR_INT32           timeout_ms;
    EasyLink_Status     status;
    bool                eventResult = true;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    // get value for timeout parameter
    timeout_ms = stack.Arg2().NumericByRef().s4;

    // set timeout
    rxTimeout.SetInteger( timeout_ms );
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks( rxTimeout, timeoutTicks ));

    // setup the operation
    if(stack.m_customState == 1)
    {
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
            // event occurred!!
            // need to create ReceivedPacket object to return on "out" argument

            // find <ReceivedPacket> type definition, don't bother checking the result as it exists for sure
            g_CLR_RT_TypeSystem.FindTypeDef( "ReceivedPacket", "nanoFramework.TI.EasyLink", receivedPacketTypeDef );

            // set to NULL and protect from GC
            hbObj.SetObjectReference( NULL );
            CLR_RT_ProtectFromGC gc1( hbObj );

            // create instance of <ReceivedPacket>
            NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(hbObj, receivedPacketTypeDef));
            
            // need to dereference to access class fields
            packet = hbObj.Dereference();

            // fill fields
            packet[ ReceivedPacket::FIELD___rxTimeout ].NumericByRef().u4 = latestRxPacket.rxTimeout;
            packet[ ReceivedPacket::FIELD___rssi ].NumericByRef().s1 = latestRxPacket.rssi;
            packet[ ReceivedPacket::FIELD___absoluteTime ].NumericByRef().u4 = latestRxPacket.absTime;

            // address it's an array
            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( packet[ ReceivedPacket::FIELD___address ], (CLR_UINT32)ARRAYSIZE(latestRxPacket.dstAddr), g_CLR_RT_WellKnownTypes.m_UInt8 ));
            buffer = packet[ ReceivedPacket::FIELD___address ].DereferenceArray();
            // copy address
            memcpy(buffer->GetFirstElement(), latestRxPacket.dstAddr, ARRAYSIZE(latestRxPacket.dstAddr));

            // payload it's an array
            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( packet[ ReceivedPacket::FIELD___payload ], (CLR_UINT32)ARRAYSIZE(latestRxPacket.payload), g_CLR_RT_WellKnownTypes.m_UInt8 ));
            buffer = packet[ ReceivedPacket::FIELD___payload ].DereferenceArray();
            // copy payload content
            memcpy(buffer->GetFirstElement(), latestRxPacket.payload, ARRAYSIZE(latestRxPacket.payload));

            // sanity check
            _ASSERTE(stack.Arg1().DataType() == DATATYPE_BYREF);

            // packet it's passed as "out" meaning BYREF
            // need to store the ReceivedPacket object in its reference
            NANOCLR_CHECK_HRESULT(hbObj.StoreToReference( stack.Arg1(), 0 ));

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

    // return operation status
    stack.SetResult_I4(latestOperationStatus);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::TransmitNative___I4__nanoFrameworkTIEasyLinkTransmitPacket__I4__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array* payloadBuffer;
    CLR_RT_HeapBlock_Array* address;
    CLR_RT_HeapBlock        txTimeout;
    CLR_RT_HeapBlock*       packet;

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

    NANOCLR_NOCLEANUP();
}

// To make it more efficient this processing is separated, making it reusable  
HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::UpdateRxAddressFilter( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    uint8_t* addressTable = NULL;
    uint8_t* addressTableCursor = NULL;
    uint8_t addressSize;
    // need to initialize these because they are passed by ref
    int addressCount = 0, capacity = 0;
    CLR_RT_HeapBlock_Array* addressList = NULL;

    CLR_RT_HeapBlock_Array* address;
    CLR_RT_HeapBlock* addressField;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    addressField = &pThis[ FIELD___addressFilter ];

    NANOCLR_CHECK_HRESULT(CLR_RT_ArrayListHelper::ExtractArrayFromArrayList( *addressField, addressList, addressCount, capacity ));

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
        // have to use the items count and NOT the number of elements because that's the array list capacity
        for(int i = 0; i < addressCount; i++)
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
