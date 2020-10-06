//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_ti_easylink.h"
#include <xdc/runtime/Error.h>
#include <ti/sysbios/knl/Task.h>

typedef Library_nf_ti_easylink_nanoFramework_TI_EasyLink_TransmitPacket TransmitPacket;
typedef Library_nf_ti_easylink_nanoFramework_TI_EasyLink_ReceivedPacket ReceivedPacket;

static EasyLink_RxPacket latestRxPacket;
static EasyLink_Status latestOperationStatus;
static EasyLink_Params easyLink_params;

static Task_Handle abortEasyLinkTask;

// Task to abort EasyLink
static void AbortEasyLink(UArg arg0, UArg arg1)
{
    (void)arg0;
    (void)arg1;

    latestOperationStatus = EasyLink_abort();

    // fire event for EasyLink init completed
    Events_Set(SYSTEM_EVENT_FLAG_RADIO);
}

// handler for TX operation done
static void TxDone(EasyLink_Status status)
{
    // store status
    latestOperationStatus = status;

    // fire event for Tx completed
    Events_Set(SYSTEM_EVENT_FLAG_RADIO);
}

// handler for RX operation done
static void RxDone(EasyLink_RxPacket *rxPacket, EasyLink_Status status)
{
    // store status
    latestOperationStatus = status;

    // packet received successfully
    if (status == EasyLink_Status_Success)
    {
        // copy packet
        memcpy(&latestRxPacket, rxPacket, sizeof(EasyLink_RxPacket));
    }

    // fire event for Rx completed
    Events_Set(SYSTEM_EVENT_FLAG_RADIO);
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::get_AbsoluteTime___U4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    EasyLink_Status status;
    uint32_t absoluteTime;

    status = EasyLink_getAbsTime(&absoluteTime);
    if (status != EasyLink_Status_Success)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    // set result to the value of the absolute time
    stack.SetResult_U4(absoluteTime);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::get_Rssi___I1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    EasyLink_Status status;
    int8_t rssi;

    status = EasyLink_getRssi(&rssi);
    if (status != EasyLink_Status_Success)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    // set result to the value of the RSSI
    stack.SetResult_I1(rssi);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::get_Frequency___U4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint32_t status;

    status = EasyLink_getFrequency();
    if (status == EasyLink_Status_Config_Error)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    // set result to the value of the frequency (in units of kHz)
    stack.SetResult_U4(status);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::get_RfPower___I1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    EasyLink_Status status;
    int8_t power;

    status = EasyLink_getRfPower(&power);
    if (status != EasyLink_Status_Success)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    // set result to the value of the RF power
    stack.SetResult_I1(power);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::get_IeeeAddress___SZARRAY_U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::
    GetConfiguration___U4__nanoFrameworkTIEasyLinkControlOption(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    EasyLink_Status status;
    EasyLink_CtrlOption option;
    uint32_t optionValue;

    // get control option from argument
    option = (EasyLink_CtrlOption)stack.Arg0().NumericByRef().u4;

    status = EasyLink_getCtrl(option, &optionValue);
    if (status != EasyLink_Status_Success)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    // set result to the control option value
    stack.SetResult_U4(optionValue);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::DisposeNative___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    Task_Params taskParams;

    CLR_RT_HeapBlock hbTimeout;
    CLR_INT64 *timeoutTicks;
    CLR_INT32 timeout_ms;
    bool eventResult = true;

    // stop any ongoing async operation that could be occurring
    // don't bother checking the return result

    // set timeout
    // !! need to cast to CLR_INT64 otherwise it wont setup a proper timeout infinite
    hbTimeout.SetInteger((CLR_INT64)-1);
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeoutTicks));

    if (stack.m_customState == 1)
    {
        // setup Task thread
        Task_Params_init(&taskParams);
        taskParams.stackSize = 512;
        taskParams.priority = 4;

        // create task
        abortEasyLinkTask = Task_create((Task_FuncPtr)AbortEasyLink, &taskParams, Error_IGNORE);
        if (abortEasyLinkTask == NULL)
        {
            // store result
            latestOperationStatus = EasyLink_Status_Aborted;

            // prevent event from being processed
            eventResult = false;
        }
        else
        {
            // bump custom state
            stack.m_customState = 2;
        }
    }

    while (eventResult)
    {
        // non-blocking wait allowing other threads to run while we wait for the receive operation to complete
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeoutTicks, Event_Radio, eventResult));

        Task_delete(&abortEasyLinkTask);

        if (eventResult)
        {
            // event occurred!!
        }
        else
        {
            // timeout occurred
            // nothing else that can be done here
        }

        // done here
        break;
    }

    // pop timeout heap block from stack
    stack.PopValue();

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::UpdateRxAddressFilterNative___VOID(
    CLR_RT_StackFrame &stack)
{
    return UpdateRxAddressFilter(stack);
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::InitNative___U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint8_t i = 0;
    EasyLink_PhyType phyType = (EasyLink_PhyType)0;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // initialize the EasyLink parameters to their default values
    EasyLink_Params_init(&easyLink_params);

    // get managed Phy Type
    phyType = (EasyLink_PhyType)(pThis[FIELD___phyType].NumericByRef().u1);

    // check if this PHY config is supported
    while (i < EasyLink_numSupportedPhys)
    {
        if (EasyLink_supportedPhys[i].EasyLink_phyType == phyType)
        {
            break;
        }
        i++;
    }

    if (i == EasyLink_numSupportedPhys)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
    }

    easyLink_params.ui32ModType = phyType;

    // try to start EasyLink
    latestOperationStatus = EasyLink_init(&easyLink_params);

    if (latestOperationStatus != EasyLink_Status_Success)
    {
        // something went wrong
        // call abort to force any TX/RX operation that could be lingering
        EasyLink_abort();

        // try again
        latestOperationStatus = EasyLink_init(&easyLink_params);

        // need to setup Rx address, if any is set
        if (latestOperationStatus == EasyLink_Status_Success)
        {
            UpdateRxAddressFilter(stack);
        }
    }

    // return operation status
    stack.SetResult_U1(latestOperationStatus);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::
    ReceiveNative___U1__BYREF_nanoFrameworkTIEasyLinkReceivedPacket__SystemTimeSpan(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Index receivedPacketTypeDef;
    CLR_RT_HeapBlock_Array *buffer;
    CLR_RT_HeapBlock rxTimeout;
    CLR_RT_HeapBlock *packet;
    CLR_RT_HeapBlock hbObj;

    CLR_INT64 *timeoutDuration;
    CLR_UINT64 timeoutMiliseconds;
    CLR_INT64 *timeoutTicks;
    bool eventResult = true;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // check disposed
    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)
    {
        // has to throw exception on all other situations
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    // get value for timeout parameter
    // timeout parameter its a TimeSpan, which is a primitive type stored as an heap block, therefore needs to
    // be accessed indirectly
    timeoutDuration = Library_corlib_native_System_TimeSpan::GetValuePtr(stack.Arg2());
    FAULT_ON_NULL(timeoutDuration);

    // check for .NET infinite timeout (-1)
    if (*(CLR_UINT64 *)timeoutDuration == -1)
    {
        // "convert" to EasyLink infinite timeout
        timeoutMiliseconds = 0;
    }
    else
    {
        timeoutMiliseconds = *(CLR_UINT64 *)timeoutDuration / TIME_CONVERSION__TO_MILLISECONDS;
    }

    // set thread timeout
    // wait forever because EasyLink timeout will make this work
    rxTimeout.SetInteger((CLR_INT64)-1);
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(rxTimeout, timeoutTicks));

    // set timeout for EasyLink, only required on the 1st pass
    if (stack.m_customState == 1)
    {
        // EasyLink RX timeout
        EasyLink_setCtrl(EasyLink_Ctrl_AsyncRx_TimeOut, EasyLink_ms_To_RadioTime(timeoutMiliseconds));
    }

    // setup the operation
    if (stack.m_customState == 1)
    {
        // enter receive mode
        latestOperationStatus = EasyLink_receiveAsync(RxDone, 0);
        if (latestOperationStatus != EasyLink_Status_Success)
        {
            // fail to start

            // prevent event from being processed
            eventResult = false;
        }
        else
        {
            // bump custom state
            stack.m_customState = 2;
        }
    }

    while (eventResult)
    {
        // non-blocking wait allowing other threads to run while we wait for the receive operation to complete
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeoutTicks, Event_Radio, eventResult));

        // set to NULL and protect from GC
        hbObj.SetObjectReference(NULL);
        CLR_RT_ProtectFromGC gc1(hbObj);

        if (eventResult)
        {
            // event occurred!!

            if (stack.m_customState == 2)
            {
                // receive operation ongoing

                // check operation status
                if (latestOperationStatus == EasyLink_Status_Success)
                {
                    // RX operation successful, OK to populate class

                    // need to create ReceivedPacket object to return on "out" argument
                    // find <ReceivedPacket> type definition, don't bother checking the result as it exists for sure
                    g_CLR_RT_TypeSystem.FindTypeDef(
                        "ReceivedPacket",
                        "nanoFramework.TI.EasyLink",
                        receivedPacketTypeDef);

                    // create instance of <ReceivedPacket>
                    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(hbObj, receivedPacketTypeDef));

                    // need to dereference to access class fields
                    packet = hbObj.Dereference();

                    // fill fields
                    packet[ReceivedPacket::FIELD___rxTimeout].NumericByRef().u4 = latestRxPacket.rxTimeout;
                    packet[ReceivedPacket::FIELD___rssi].NumericByRef().s1 = latestRxPacket.rssi;
                    packet[ReceivedPacket::FIELD___absoluteTime].NumericByRef().u4 = latestRxPacket.absTime;

                    // address it's an array
                    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(
                        packet[ReceivedPacket::FIELD___address],
                        (CLR_UINT32)ARRAYSIZE(latestRxPacket.dstAddr),
                        g_CLR_RT_WellKnownTypes.m_UInt8));
                    buffer = packet[ReceivedPacket::FIELD___address].DereferenceArray();
                    // copy address
                    memcpy(buffer->GetFirstElement(), latestRxPacket.dstAddr, ARRAYSIZE(latestRxPacket.dstAddr));

                    // payload it's an array
                    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(
                        packet[ReceivedPacket::FIELD___payload],
                        (CLR_UINT32)ARRAYSIZE(latestRxPacket.payload),
                        g_CLR_RT_WellKnownTypes.m_UInt8));
                    buffer = packet[ReceivedPacket::FIELD___payload].DereferenceArray();
                    // copy payload content
                    memcpy(buffer->GetFirstElement(), latestRxPacket.payload, ARRAYSIZE(latestRxPacket.payload));
                }
            }

            // done here
            break;
        }
        else
        {
            // timeout occurred

            // done here
            break;
        }
    }

    if (latestOperationStatus == EasyLink_Status_Success)
    {
        // sanity check
        _ASSERTE(stack.Arg1().DataType() == DATATYPE_BYREF);

        // packet it's passed as "out" meaning BYREF
        // need to store the ReceivedPacket object in its reference
        // hbObj it's either NULL or it's a properly formated ReceivedPacket object
        NANOCLR_CHECK_HRESULT(hbObj.StoreToReference(stack.Arg1(), 0));
    }

    // pop timeout heap block from stack
    stack.PopValue();

    // return operation status
    stack.SetResult_U1(latestOperationStatus);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::
    SetConfigurationNative___U1__nanoFrameworkTIEasyLinkControlOption__U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    EasyLink_Status status;
    EasyLink_CtrlOption option;
    uint32_t optionValue;

    // get control option
    option = (EasyLink_CtrlOption)stack.Arg0().NumericByRef().u4;
    optionValue = stack.Arg1().NumericByRef().u4;

    status = EasyLink_setCtrl(option, optionValue);

    // set result to the operation return status
    stack.SetResult_U1(status);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::SetFrequencyNative___U1__U4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint16_t frequency;
    EasyLink_Status status;

    // get value for the frequency
    frequency = (uint16_t)stack.Arg0().NumericByRef().u4;

    status = EasyLink_setFrequency(frequency);

    // set result to the operation return status
    stack.SetResult_U1(status);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::SetRfPowerNative___U1__I1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    EasyLink_Status status;
    int8_t power;

    // get value for the tx power
    power = (int8_t)stack.Arg0().NumericByRef().u1;

    status = EasyLink_setRfPower(power);

    // set result to the operation return status
    stack.SetResult_U1(status);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::
    TransmitNative___U1__nanoFrameworkTIEasyLinkTransmitPacket__SystemTimeSpan__SystemTimeSpan(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array *payloadBuffer;
    CLR_RT_HeapBlock_Array *address;
    CLR_RT_HeapBlock txTimeout;
    CLR_RT_HeapBlock *packet;

    CLR_INT64 *timeoutDuration;
    CLR_INT64 *dueTimeDuration;
    CLR_UINT64 timeoutMiliseconds;
    CLR_UINT64 dueTimeMiliseconds;
    CLR_INT64 *timeoutTicks;
    uint32_t absTime;
    EasyLink_TxPacket txPacket = {{0}, 0, 0, {0}};
    bool eventResult = true;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // check disposed
    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)
    {
        // has to throw exception on all other situations
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    // get value for timeout parameter
    // timeout parameter its a TimeSpan, which is a primitive type stored as an heap block, therefore needs to
    // be accessed indirectly
    timeoutDuration = Library_corlib_native_System_TimeSpan::GetValuePtr(stack.Arg2());
    FAULT_ON_NULL(timeoutDuration);

    // check for .NET infinite timeout (-1)
    if (*(CLR_UINT64 *)timeoutDuration == -1)
    {
        // "convert" to EasyLink infinite timeout
        timeoutMiliseconds = 0;
    }
    else
    {
        timeoutMiliseconds = *(CLR_UINT64 *)timeoutDuration / TIME_CONVERSION__TO_MILLISECONDS;
    }

    // set timeout, if any
    if (timeoutMiliseconds > 0)
    {
        // add 100ms to allow EasyLink timeout to occur before CLR thread timeout
        txTimeout.SetInteger(timeoutMiliseconds + 100);
    }
    else
    {
        // no timeout so wait forever
        txTimeout.SetInteger((CLR_INT64)-1);
    }
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(txTimeout, timeoutTicks));

    // setup the operation and init buffers
    if (stack.m_customState == 1)
    {
        // get pointer to managed TransmitPacket
        packet = stack.Arg1().Dereference();
        FAULT_ON_NULL(packet);

        // dereference the payload buffer
        payloadBuffer = packet[TransmitPacket::FIELD___payload].DereferenceArray();

        // get payload length
        txPacket.len = payloadBuffer->m_numOfElements;

        // copy buffer to packet
        memcpy(txPacket.payload, payloadBuffer->GetFirstElement(), txPacket.len);

        // dereference the address buffer
        address = packet[TransmitPacket::FIELD___address].DereferenceArray();

        // copy buffer to packet
        memcpy(txPacket.dstAddr, address->GetFirstElement(), address->m_numOfElements);

        // get dueTime managed parameter

        // dueTime parameter its a TimeSpan, which is a primitive type stored as an heap block, therefore needs to
        // be accessed indirectly
        dueTimeDuration = Library_corlib_native_System_TimeSpan::GetValuePtr(stack.Arg3());
        FAULT_ON_NULL(dueTimeDuration);

        dueTimeMiliseconds = *(CLR_UINT64 *)dueTimeDuration / TIME_CONVERSION__TO_MILLISECONDS;

        if (dueTimeMiliseconds > 0)
        {
            EasyLink_getAbsTime(&absTime);
            txPacket.absTime = absTime + EasyLink_ms_To_RadioTime(dueTimeMiliseconds);
        }
        else
        {
            // Set Tx absolute time to current time + 100ms
            txPacket.absTime = absTime + EasyLink_ms_To_RadioTime(100);
        }

        // start transmission
        latestOperationStatus = EasyLink_transmitAsync(&txPacket, TxDone);

        if (latestOperationStatus != EasyLink_Status_Success)
        {
            // fail to start

            // prevent event from being processed
            eventResult = false;
        }
        else
        {
            // bump custom state
            stack.m_customState = 2;
        }
    }

    while (eventResult)
    {
        // non-blocking wait allowing other threads to run while we wait for the transmit operation to complete
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeoutTicks, Event_Radio, eventResult));

        if (eventResult)
        {
            // event occurred
        }
        else
        {
            // timeout occurred

            latestOperationStatus = EasyLink_Status_Tx_Error;

            // instead of throwing a timeout exception return OK
            // the failure it's already on the return result
        }

        if( (latestOperationStatus != EasyLink_Status_Success) &&
            (latestOperationStatus != EasyLink_Status_Aborted))
        {
            // abort EasyLink operation
            EasyLink_abort();
        }

        // done here
        break;
    }

    // pop timeout heap block from stack
    stack.PopValue();

    stack.SetResult_U1(latestOperationStatus);

    NANOCLR_NOCLEANUP();
}

// To make it more efficient this processing is separated, making it reusable
HRESULT Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::UpdateRxAddressFilter(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint8_t *addressTable = NULL;
    uint8_t *addressTableCursor = NULL;
    uint8_t addressSize;
    // need to initialize these because they are passed by ref
    int addressCount = 0, capacity = 0;
    CLR_RT_HeapBlock_Array *addressList = NULL;

    CLR_RT_HeapBlock_Array *address;
    CLR_RT_HeapBlock *addressField;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    addressField = &pThis[FIELD___addressFilter];

    NANOCLR_CHECK_HRESULT(
        CLR_RT_ArrayListHelper::ExtractArrayFromArrayList(*addressField, addressList, addressCount, capacity));

    if (addressCount > 0)
    {
        //... and size of the addressese (assuming that all of them, if any others, have the same length)
        address = ((CLR_RT_HeapBlock_Array *)addressList->GetFirstElement())->DereferenceArray();

        addressSize = address->m_numOfElements;

        // allocate memory for the address table to be built
        addressTable = (uint8_t *)platform_malloc(addressCount * addressSize);

        // sanity check
        if (addressTable == NULL)
        {
            // mem allocation failed
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }

        // need to copy the pointer to reuse it later
        addressTableCursor = addressTable;

        // now loop through the collection to fill the table
        // have to use the items count and NOT the number of elements because that's the array list capacity
        for (int i = 0; i < addressCount; i++)
        {
            address = ((CLR_RT_HeapBlock_Array *)addressList->GetElement(i))->DereferenceArray();
            if (address != NULL)
            {
                memcpy(addressTableCursor, address->GetFirstElement(), addressSize);

                addressTableCursor += addressSize;
            }
        }

        // don't check the result, failures occur when this is set without the module initialized
        EasyLink_enableRxAddrFilter(addressTable, addressCount, addressSize);
    }

    if (addressTable != NULL)
    {
        platform_free(addressTable);
    }

    NANOCLR_NOCLEANUP();
}
