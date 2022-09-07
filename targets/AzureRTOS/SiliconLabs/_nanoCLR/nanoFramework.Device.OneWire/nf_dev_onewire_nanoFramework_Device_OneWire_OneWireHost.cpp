//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_dev_onewire_target.h"


// struct for working threads
static OneWireFindStruct FindStruct;
static bool OneWireOperationResult;
static TaskHandle_t WaitingTask;
static uint8_t LastDiscrepancy;
static uint8_t LastFamilyDiscrepancy;
static uint8_t LastDevice;
static uint8_t SerialNum[8];

// Driver state.
static oneWireState DriverState = ONEWIRE_UNINIT;

// working thread to execute long running 1-Wire operations
static thread_t *OneWireWorkingThread;


void IRAM_ATTR oneWireStop()
{
    // stop UART
    uart_driver_delete(NF_ONEWIRE_ESP32_UART_NUM);

    // driver is stopped
    DriverState = ONEWIRE_STOP;
}

HRESULT IRAM_ATTR oneWireInit()
{
    DriverState = ONEWIRE_STOP;

    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
        .use_ref_tick = false,
    };

    // get GPIO pins configured for UART assigned to 1-Wire
    // need to subtract one to get the correct index of UART in mapped device pins
    int txPin = Esp32_GetMappedDevicePins(DEV_TYPE_SERIAL, NF_ONEWIRE_ESP32_UART_NUM, Esp32SerialPin_Tx);
    int rxPin = Esp32_GetMappedDevicePins(DEV_TYPE_SERIAL, NF_ONEWIRE_ESP32_UART_NUM, Esp32SerialPin_Rx);

    // check if TX, RX pins have been previously set
    if (txPin == UART_PIN_NO_CHANGE || rxPin == UART_PIN_NO_CHANGE)
    {
        return CLR_E_PIN_UNAVAILABLE;
    }

    // configure GPIO and UART
    if (gpio_set_direction(gpio_num_t(txPin), GPIO_MODE_OUTPUT_OD) != ESP_OK)
    {
        return CLR_E_INVALID_OPERATION;
    }

    if (uart_param_config(NF_ONEWIRE_ESP32_UART_NUM, &uart_config) != ESP_OK)
    {
        return CLR_E_INVALID_OPERATION;
    }

    if (uart_set_pin(NF_ONEWIRE_ESP32_UART_NUM, txPin, rxPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE) != ESP_OK)
    {
        return CLR_E_INVALID_OPERATION;
    }

    if (uart_driver_install(NF_ONEWIRE_ESP32_UART_NUM, 256, 256, 0, NULL, ESP_INTR_FLAG_IRAM) != ESP_OK)
    {
        return CLR_E_INVALID_OPERATION;
    }

    // driver need to be deleted on soft reboot
    HAL_AddSoftRebootHandler(oneWireStop);

    DriverState = ONEWIRE_READY;

    return S_OK;
}


uint8_t oneWireTouchReset(void)
{
    char reset = 0xF0;
    uint8_t presence;

    // flush DMA buffer to ensure cache coherency
    uart_flush(NF_ONEWIRE_ESP32_UART_NUM);
    // set UART baud rate to 9600bps (required to send the RESET condition to the 1-Wire bus)
    uart_set_baudrate(NF_ONEWIRE_ESP32_UART_NUM, 9600);

    uart_write_bytes(NF_ONEWIRE_ESP32_UART_NUM, (const char *)&reset, 1);
    uart_read_bytes(NF_ONEWIRE_ESP32_UART_NUM, &presence, 1, 20 / portTICK_RATE_MS);

    // set UART baud rate to 115200bps (normal comm is performed at this baud rate)
    uart_set_baudrate(NF_ONEWIRE_ESP32_UART_NUM, 115200);

    // check for presence pulse
    return (presence != reset);
}

////////////////////////////////////////
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

HRESULT Library_nf_dev_onewire_nanoFramework_Device_OneWire_OneWireHost::TouchReset___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    volatile bool reset = oneWireTouchReset();
    stack.SetResult_Boolean(reset);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_dev_onewire_nanoFramework_Device_OneWire_OneWireHost::TouchBit___BOOLEAN__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    bool value = stack.Arg1().NumericByRefConst().u1 != 0;

    stack.SetResult_Boolean(oneWireTouchBit(value));

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_dev_onewire_nanoFramework_Device_OneWire_OneWireHost::TouchByte___U1__U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint8_t value = (uint8_t)stack.Arg1().NumericByRefConst().u1;
    uint8_t result = oneWireTouchByte(value);
    stack.SetResult_U1(result);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_dev_onewire_nanoFramework_Device_OneWire_OneWireHost::WriteByte___U1__U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint8_t cmd = (uint8_t)stack.Arg1().NumericByRefConst().u1;
    uint8_t result = oneWireWriteByte(cmd);
    stack.SetResult_U1(result);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_dev_onewire_nanoFramework_Device_OneWire_OneWireHost::ReadByte___U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint8_t result = oneWireReadByte();
    stack.SetResult_U1(result);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_dev_onewire_nanoFramework_Device_OneWire_OneWireHost::FindFirstDevice___BOOLEAN__BOOLEAN__BOOLEAN(
    CLR_RT_StackFrame &stack)
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
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_OneWireHost, eventResult));
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

HRESULT Library_nf_dev_onewire_nanoFramework_Device_OneWire_OneWireHost::FindNextDevice___BOOLEAN__BOOLEAN__BOOLEAN(
    CLR_RT_StackFrame &stack)
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
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_OneWireHost, eventResult));
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

HRESULT Library_nf_dev_onewire_nanoFramework_Device_OneWire_OneWireHost::NativeDispose___VOID(CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();

    oneWireStop();

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_dev_onewire_nanoFramework_Device_OneWire_OneWireHost::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();

    // startup 1-Wire driver
    oneWireStart();

    NANOCLR_NOCLEANUP_NOLABEL();
}
