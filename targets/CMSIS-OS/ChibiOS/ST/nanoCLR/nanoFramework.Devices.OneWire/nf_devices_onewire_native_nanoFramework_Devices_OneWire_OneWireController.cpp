//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_devices_onewire_native_target.h"

// working thread to execute long running 1-Wire operations
static thread_t *OneWireWorkingThread;

// struct for working threads
OneWireFindStruct FindStruct;

// ChibiOS OneWire working thread
static THD_FUNCTION(OneWireFindFirstWorkingThread, arg)
{
    msg_t deviceFound;

    OneWireFindStruct *findStruct = (OneWireFindStruct *)arg;

    deviceFound = oneWireFindFirst(findStruct->DoReset, findStruct->AlarmOnly);

    // fire event for 1-Wire operarion completed
    Events_Set(SYSTEM_EVENT_FLAG_ONEWIRE_MASTER);

    chThdExit(deviceFound);
}

static THD_FUNCTION(OneWireFindNextWorkingThread, arg)
{
    msg_t deviceFound;

    OneWireFindStruct *findStruct = (OneWireFindStruct *)arg;

    deviceFound = oneWireFindNext(findStruct->DoReset, findStruct->AlarmOnly);

    // fire event for 1-Wire operarion completed
    Events_Set(SYSTEM_EVENT_FLAG_ONEWIRE_MASTER);

    chThdExit(deviceFound);
}

HRESULT Library_nf_devices_onewire_native_nanoFramework_Devices_OneWire_OneWireController::TouchReset___BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    volatile bool reset = oneWireTouchReset();
    stack.SetResult_Boolean(reset);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_devices_onewire_native_nanoFramework_Devices_OneWire_OneWireController::TouchBit___BOOLEAN__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    bool value = stack.Arg1().NumericByRefConst().u1 != 0;

    stack.SetResult_Boolean(oneWireTouchBit(value));

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_devices_onewire_native_nanoFramework_Devices_OneWire_OneWireController::TouchByte___U1__U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint8_t value = (uint8_t)stack.Arg1().NumericByRefConst().u1;
    uint8_t result = oneWireTouchByte(value);
    stack.SetResult_U1(result);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_devices_onewire_native_nanoFramework_Devices_OneWire_OneWireController::WriteByte___U1__U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint8_t cmd = (uint8_t)stack.Arg1().NumericByRefConst().u1;
    uint8_t result = oneWireWriteByte(cmd);
    stack.SetResult_U1(result);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_devices_onewire_native_nanoFramework_Devices_OneWire_OneWireController::ReadByte___U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint8_t result = oneWireReadByte();
    stack.SetResult_U1(result);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_devices_onewire_native_nanoFramework_Devices_OneWire_OneWireController::
    FindFirstDevice___BOOLEAN__BOOLEAN__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint8_t *serialNumberPointer;
    msg_t oneWireOperationResult = FALSE;

    CLR_RT_HeapBlock hbTimeout;
    CLR_INT64 *timeout;
    bool eventResult = true;

    // set an infinite timeout to wait forever for the operation to complete
    // this value has to be in ticks to be properly loaded by SetupTimeoutFromTicks() below
    hbTimeout.SetInteger((CLR_INT64)-1);
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));

    // this is going to be used to check for the right event in case of simultaneous 1-Wire operations
    if (stack.m_customState == 1)
    {
        FindStruct.DoReset = stack.Arg1().NumericByRefConst().u1 != 0;
        FindStruct.AlarmOnly = stack.Arg2().NumericByRefConst().u1 != 0;

        // because the 1-Wire bus is shared, acquire the module
        oneWireAcquireModule();

        // spawn working thread to perform the 1-Wire operations
        OneWireWorkingThread = chThdCreateFromHeap(
            NULL,
            THD_WORKING_AREA_SIZE(256),
            "OWWT",
            NORMALPRIO,
            OneWireFindFirstWorkingThread,
            &FindStruct);

        // bump custom state
        stack.m_customState = 2;
    }

    while (eventResult)
    {
        if (OneWireWorkingThread->state == CH_STATE_FINAL)
        {
            // OneWire working thread is now complete
            break;
        }

        // non-blocking wait allowing other threads to run while we wait for the 1-Wire operations to complete
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_OneWireMaster, eventResult));
    }

    if (eventResult)
    {
        // event occurred

        // ChibiOS requirement: need to call chThdWait for working thread in order to have it's memory released to the
        // heap, otherwise it won't be returned
        oneWireOperationResult = chThdWait(OneWireWorkingThread);

        oneWireReleaseModule();

        // get the result from the working thread execution
        if (oneWireOperationResult)
        {
            // if a device was found update serialNumber field in managed class

            // get a pointer to the managed object instance and check that it's not NULL
            CLR_RT_HeapBlock *pThis = stack.This();
            FAULT_ON_NULL(pThis);

            // get a pointer to the serial number field in the OneWireController instance
            CLR_RT_HeapBlock_Array *serialNumberField = pThis[FIELD___serialNumber].DereferenceArray();

            _ASSERTE(serialNumberField->m_numOfElements == 8);

            // get a pointer to the first element of the byte array
            serialNumberPointer = (uint8_t *)serialNumberField->GetFirstElement();

            oneWireSerialNum(serialNumberPointer, TRUE);
        }
    }

    // pop timeout heap block from stack
    stack.PopValue();

    // set result
    stack.SetResult_Boolean(oneWireOperationResult);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_devices_onewire_native_nanoFramework_Devices_OneWire_OneWireController::
    FindNextDevice___BOOLEAN__BOOLEAN__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint8_t *serialNumberPointer;
    msg_t oneWireOperationResult = FALSE;

    CLR_RT_HeapBlock hbTimeout;
    CLR_INT64 *timeout;
    bool eventResult = true;

    // set an infinite timeout to wait forever for the operation to complete
    // this value has to be in ticks to be properly loaded by SetupTimeoutFromTicks() below
    hbTimeout.SetInteger((CLR_INT64)-1);
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));

    // this is going to be used to check for the right event in case of simultaneous 1-Wire operations
    if (stack.m_customState == 1)
    {
        FindStruct.DoReset = stack.Arg1().NumericByRefConst().u1 != 0;
        FindStruct.AlarmOnly = stack.Arg2().NumericByRefConst().u1 != 0;

        // because the 1-Wire bus is shared, acquire the module
        oneWireAcquireModule();

        // spawn working thread to perform the 1-Wire operations
        OneWireWorkingThread = chThdCreateFromHeap(
            NULL,
            THD_WORKING_AREA_SIZE(256),
            "OWWT",
            NORMALPRIO,
            OneWireFindNextWorkingThread,
            &FindStruct);

        // bump custom state
        stack.m_customState = 2;
    }

    while (eventResult)
    {
        if (OneWireWorkingThread->state == CH_STATE_FINAL)
        {
            // OneWire working thread is now complete
            break;
        }

        // non-blocking wait allowing other threads to run while we wait for the 1-Wire operations to complete
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_OneWireMaster, eventResult));
    }

    if (eventResult)
    {
        // event occurred

        // ChibiOS requirement: need to call chThdWait for working thread in order to have it's memory released to the
        // heap, otherwise it won't be returned
        oneWireOperationResult = chThdWait(OneWireWorkingThread);

        oneWireReleaseModule();

        // get the result from the working thread execution
        if (oneWireOperationResult)
        {
            // update serialNumber field

            // get a pointer to the managed object instance and check that it's not NULL
            CLR_RT_HeapBlock *pThis = stack.This();
            FAULT_ON_NULL(pThis);

            // get a pointer to the serial number field in the OneWireController instance
            CLR_RT_HeapBlock_Array *serialNumberField = pThis[FIELD___serialNumber].DereferenceArray();

            _ASSERTE(serialNumberField->m_numOfElements == 8);

            // get a pointer to the first element of the byte array
            serialNumberPointer = (uint8_t *)serialNumberField->GetFirstElement();

            oneWireSerialNum(serialNumberPointer, TRUE);
        }
    }

    // pop timeout heap block from stack
    stack.PopValue();

    // set result
    stack.SetResult_Boolean(oneWireOperationResult);

    NANOCLR_NOCLEANUP();
}
