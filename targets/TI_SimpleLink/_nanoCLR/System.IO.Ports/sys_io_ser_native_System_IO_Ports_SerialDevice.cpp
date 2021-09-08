//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include "sys_io_ser_native_target.h"

////////////////////////////////////////////////////////////
// UART PAL strucs delcared in sys_io_ser_native_target.h //
////////////////////////////////////////////////////////////
#if defined(NF_SERIAL_COMM_TI_USE_UART1) && (NF_SERIAL_COMM_TI_USE_UART1 == TRUE)
NF_PAL_UART Uart1_PAL;
#endif

// in UWP the COM ports are named COM1, COM2, COM3. But TI SimpleLink uses internally UART0, UART1, UART2. This maps the
// port index 1, 2 or 3 to the UART number 0, 1, etc
#define PORT_INDEX_TO_UART_NUM(portIndex) ((portIndex)-1)

// in UWP the COM ports are named COM1, COM2, COM3. But TI SimpleLink uses internally UART0, UART1, etc. This maps the
// UART number 0, 1 (..) to the port index 1, 2, etc
#define UART_NUM_TO_PORT_INDEX(uartNum) ((uartNum) + 1)

// UART RX operation timeout
#define UART_TX_TIMEOUT_MILLISECONDS 5000

// Stack size in bytes
#define SERIAL_TASKSTACKSIZE 1024
// better declare the task stack statically to check allocation
uint8_t SerialRxHandlerStack[SERIAL_TASKSTACKSIZE];
Task_Struct SerialRxTaskStruct;

NF_PAL_UART *GetPalUartFromUartNum(int uart_num)
{
    NF_PAL_UART *palUart = NULL;

    switch (uart_num)
    {
#if defined(NF_SERIAL_COMM_TI_USE_UART1) && (NF_SERIAL_COMM_TI_USE_UART1 == TRUE)
        case UART1:
            // set UART PAL
            palUart = &Uart1_PAL;
            break;
#endif

        default:
            break;
    }

    return palUart;
}

void UnitializePalUart(NF_PAL_UART *palUart)
{
    if (palUart && palUart->UartDriver)
    {
        // destroy task UART task
        if (palUart->WorkingTask != NULL)
        {
            Task_destruct(&SerialRxTaskStruct);
        }

        UART2_close(palUart->UartDriver);

        // free buffers memory
        platform_free(palUart->TxBuffer);
        platform_free(palUart->RxBuffer);

        // null all pointers
        palUart->TxBuffer = NULL;
        palUart->RxBuffer = NULL;
        palUart->UartDriver = NULL;
        palUart->WorkingTask = NULL;
        palUart->WatchChar = 0;
    }
}

// estimate the time required to perform the TX transaction
bool IsLongRunningOperation(uint32_t bufferSize, uint32_t baudRate, uint32_t &estimatedDurationMiliseconds)
{
    // simplifying calculation assuming worst case values for stop bits
    estimatedDurationMiliseconds = ((8 + 2) * bufferSize * 1000) / baudRate;

    if (estimatedDurationMiliseconds > CLR_RT_Thread::c_TimeQuantum_Milliseconds)
    {
        // total operation time will exceed thread quantum, so this is a long running operation
        return true;
    }
    else
    {
        return false;
    }
}

// This callback is invoked when a transmission buffer has been completely read by the driver.
static void TxCallback(UART2_Handle handle, void *buf, size_t count, void *userArg, int_fast16_t status)
{
    //(void)uartp;

    NATIVE_INTERRUPT_START

    NF_PAL_UART *palUart = (NF_PAL_UART *)userArg;

    if (status == UART2_STATUS_SUCCESS)
    {
        // reset Tx ongoing count, if we are done here
        if (count > 0)
        {
            if (palUart->TxOngoingCount > 0)
            {
                // pop elements from ring buffer, just pop
                palUart->TxRingBuffer.Pop(count);

                palUart->TxOngoingCount -= count;

                // it TX complete?
                if (palUart->TxOngoingCount == 0)
                {
                    Events_Set(SYSTEM_EVENT_FLAG_COM_OUT);
                }
            }
        }
    }

    NATIVE_INTERRUPT_END
}

void SerialRxTask(UArg a0, UArg a1)
{
    (void)a1;
    size_t bufferIndex = 0;
    uint8_t input;
    size_t bytesRead;
    bool watchCharFound = false;
    int_fast16_t status = 1;

    NF_PAL_UART *palUart = GetPalUartFromUartNum((int)a0);

    while (1)
    {

        status = UART2_read(palUart->UartDriver, &input, 1, &bytesRead);

        if (status == UART2_STATUS_SUCCESS)
        {
            // store this into the UART Rx buffer

            // push char to ring buffer
            // don't care about the success of the operation, if it's full we are droping the char anyway
            palUart->RxRingBuffer.Push((uint8_t)input);

            // is there a read operation going on?
            if (palUart->RxBytesToRead > 0)
            {
                // yes
                // check if the requested bytes are available in the buffer...
                //... or if the watch char was received
                if (palUart->RxRingBuffer.Length() >= palUart->RxBytesToRead || watchCharFound)
                {
                    // reset Rx bytes to read count
                    palUart->RxBytesToRead = 0;

                    // fire event for Rx buffer complete
                    Events_Set(SYSTEM_EVENT_FLAG_COM_IN);
                }
            }
            else
            {
                // no read operation ongoing, so fire an event
                // post a managed event with the port index and event code (check if this is the watch char or just
                // another another)
                PostManagedEvent(
                    EVENT_SERIAL,
                    0,
                    UART_NUM_TO_PORT_INDEX(palUart->UartNum),
                    (input == palUart->WatchChar) ? SerialData_WatchChar : SerialData_Chars);
            }
        }
    }
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::get_BytesToRead___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_PAL_UART *palUart;
    uint8_t uartNum;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    uartNum = PORT_INDEX_TO_UART_NUM(pThis[FIELD___portIndex].NumericByRef().s4);

    // Choose the driver for this SerialDevice
    palUart = GetPalUartFromUartNum(uartNum);
    if (palUart == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // get length of Rx ring buffer
    stack.SetResult_U4(palUart->RxRingBuffer.Length());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeDispose___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint8_t uartNum;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Get Uart number for serial device
    uartNum = PORT_INDEX_TO_UART_NUM(pThis[FIELD___portIndex].NumericByRef().s4);

    UnitializePalUart(GetPalUartFromUartNum(uartNum));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        NF_PAL_UART *palUart;
        uint8_t uartNum;
        uint16_t txBufferSize;
        uint16_t rxBufferSize;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // Get UART number from PortIndex
        uartNum = PORT_INDEX_TO_UART_NUM(pThis[FIELD___portIndex].NumericByRef().s4);
        if (uartNum > CONFIG_TI_DRIVERS_UART2_COUNT || uartNum < 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // call the configure and abort if not OK
        NANOCLR_CHECK_HRESULT(NativeConfig___VOID(stack));

        // Choose the driver for this SerialDevice
        palUart = GetPalUartFromUartNum(uartNum);
        if (palUart == NULL)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

#if defined(NF_SERIAL_COMM_TI_USE_UART1) && (NF_SERIAL_COMM_TI_USE_UART1 == TRUE)
        // assign buffers, if not already done
        if (palUart->TxBuffer == NULL && palUart->RxBuffer == NULL)
        {
            palUart->TxBuffer = (uint8_t *)platform_malloc(UART1_TX_SIZE);
            palUart->RxBuffer = (uint8_t *)platform_malloc(UART1_TX_SIZE);

            // check allocation
            if (palUart->TxBuffer == NULL || palUart->RxBuffer == NULL)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
            }

            // init buffers
            palUart->TxRingBuffer.Initialize(palUart->TxBuffer, UART1_TX_SIZE);
            palUart->RxRingBuffer.Initialize(palUart->RxBuffer, UART1_RX_SIZE);
        }
#endif

        // all the rest
        palUart->WatchChar = 0;
        palUart->RxBytesToRead = 0;
        palUart->TxOngoingCount = 0;
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeConfig___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_PAL_UART *palUart = NULL;
    uint8_t uartNum;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    uartNum = PORT_INDEX_TO_UART_NUM(pThis[FIELD___portIndex].NumericByRef().s4);

    // Choose the driver for this SerialDevice
    palUart = GetPalUartFromUartNum(uartNum);
    if (palUart == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // store UART index
    palUart->UartNum = uartNum;

    // setup configuration

    // Check RS485 mode is not selected as currently not supported
    if ((SerialMode)pThis[FIELD___mode].NumericByRef().s4 != SerialMode_Normal)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_NOTIMPL);
    }

    // Create a UART with data processing off
    UART2_Params_init(&palUart->UartParams);

    // set R/W mode with callbacks
    palUart->UartParams.writeMode = UART2_Mode_CALLBACK;
    palUart->UartParams.readMode = UART2_Mode_BLOCKING;

    // store pointer to PAL UART
    palUart->UartParams.userArg = palUart;

    // palUart->UartParams.readCallback = RxCallback;
    palUart->UartParams.writeCallback = TxCallback;

    palUart->UartParams.readReturnMode = UART2_ReadReturnMode_FULL;
    palUart->UartParams.baudRate = (int)pThis[FIELD___baudRate].NumericByRef().s4;

    // UART2_DataLen goes from 5->0 to 8->3 bits.
    // Because the parameter carries the bits count, subtracting 5 gives the correct setting.
    palUart->UartParams.dataLength = (UART2_DataLen)(pThis[FIELD___dataBits].NumericByRef().s4 - 5);

    // parity (SerialParity enum matches UART2_Parity)
    palUart->UartParams.parityType = (UART2_Parity)pThis[FIELD___parity].NumericByRef().s4;

    // stop bits
    switch ((StopBits)pThis[FIELD___stopBits].NumericByRef().s4)
    {
        case StopBits_One:
            palUart->UartParams.stopBits = UART2_StopBits_1;
            break;

        case StopBits_Two:
            palUart->UartParams.stopBits = UART2_StopBits_2;
            break;

        default:
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // abort any ongoing operations, just in case
    UART2_readCancel(palUart->UartDriver);
    UART2_writeCancel(palUart->UartDriver);

    if (palUart->UartDriver != NULL)
    {
        // stop UART before changing configuration, just in case
        UART2_close(palUart->UartDriver);

        // destroy task UART task, if it's running
        if (palUart->WorkingTask != NULL)
        {
            Task_destruct(&SerialRxTaskStruct);

            // null pointer
            palUart->WorkingTask = NULL;
        }
    }

    palUart->UartDriver = UART2_open(uartNum, &palUart->UartParams);

    // check if UART was opened
    if (palUart->UartDriver == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    // OK to enable RX on UART now
    UART2_rxEnable(palUart->UartDriver);

    // create RX task
    // this can be replaced with an RX handler when TI decides to add a decent RX event to their API
    Task_Params taskParams;
    Task_Params_init(&taskParams);
    taskParams.stackSize = SERIAL_TASKSTACKSIZE;
    taskParams.priority = 4;
    taskParams.stack = &SerialRxHandlerStack;
    taskParams.arg0 = (UArg)palUart->UartNum;

    // construct task
    Task_construct(&SerialRxTaskStruct, (Task_FuncPtr)SerialRxTask, &taskParams, NULL);

    // store pointer to task
    palUart->WorkingTask = Task_handle(&SerialRxTaskStruct);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeWrite___VOID__SZARRAY_U1__I4__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        NF_PAL_UART *palUart = NULL;

        uint8_t uartNum;
        uint8_t *data;
        unsigned int length = 0;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        if (pThis[FIELD___disposed].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        // dereference the data buffer from the argument
        CLR_RT_HeapBlock_Array *dataBuffer = stack.Arg1().DereferenceArray();

        // get a the pointer to the array by using the first element of the array
        data = dataBuffer->GetFirstElement();

        // get the size of the buffer
        length = dataBuffer->m_numOfElements;

        uartNum = PORT_INDEX_TO_UART_NUM(pThis[FIELD___portIndex].NumericByRef().s4);

        // get pointer to PAL UART
        palUart = GetPalUartFromUartNum(uartNum);
        if (palUart == NULL)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // check if there is enough room in the buffer
        if (palUart->TxRingBuffer.Capacity() - palUart->TxRingBuffer.Length() < length)
        {
            // not enough room in the buffer
            NANOCLR_SET_AND_LEAVE(CLR_E_BUFFER_TOO_SMALL);
        }

        // push data to buffer
        size_t bytesWritten = palUart->TxRingBuffer.Push(data, length);

        // check if all requested bytes were written
        if (bytesWritten != length)
        {
            // not sure if this is the best exception to throw here...
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }

        // // need to update the _unstoredBufferLength field in the SerialDeviceOutputStream
        // // get pointer to outputStream field
        // CLR_RT_HeapBlock *outputStream =
        //     pThis[Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::FIELD___outputStream]
        //         .Dereference();
        // // get pointer to _unstoredBufferLength field and udpate field value
        // outputStream[Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDeviceOutputStream::
        //                  FIELD___unstoredBufferLength]
        //     .NumericByRef()
        //     .s4 = palUart->TxRingBuffer.Length();
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeStore___U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock hbTimeout;

    NF_PAL_UART *palUart = NULL;

    uint32_t estimatedDurationMiliseconds;
    size_t length = 0;
    uint8_t uartNum;

    int64_t *timeoutTicks;
    bool eventResult = true;
    bool txOk = false;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    uartNum = PORT_INDEX_TO_UART_NUM(pThis[FIELD___portIndex].NumericByRef().s4);

    // Choose the driver for this SerialDevice
    palUart = GetPalUartFromUartNum(uartNum);
    if (palUart == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    if (stack.m_customState == 0)
    {
        // check if this is a long running operation
        palUart->IsLongRunning = IsLongRunningOperation(
            palUart->TxRingBuffer.Length(),
            (uint32_t)pThis[FIELD___baudRate].NumericByRef().s4,
            (uint32_t &)estimatedDurationMiliseconds);
    }

    // check if there is anything the buffer
    if (palUart->TxRingBuffer.Length() > 0)
    {
        // check if there is a TX operation ongoing
        if (palUart->TxOngoingCount == 0)
        {
            // OK to Tx
            txOk = true;
        }
        else
        {
            // need to wait for the ongoing operation to complete before starting a new one
        }
    }

    if (txOk)
    {
        // optimize buffer for sequential reading
        palUart->TxRingBuffer.OptimizeSequence();

        // get data length available in the buffer
        length = palUart->TxRingBuffer.Length();

        if (palUart->IsLongRunning)
        {

            // setup timeout
            hbTimeout.SetInteger(
                (CLR_INT64)pThis[FIELD___writeTimeout].NumericByRef().s4 * TIME_CONVERSION__TO_MILLISECONDS);
            NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeoutTicks));

            // this is a long running operation and hasn't started yet
            // perform operation by launching a thread to
            if (stack.m_customState == 1)
            {
                // push to the stack how many bytes bytes where buffered for Tx
                stack.PushValueI4(length);

                // set TX count
                palUart->TxOngoingCount = length;

                // Write data to start sending
                // by design: don't bother checking the return value
                UART2_write(palUart->UartDriver, (const void *)palUart->TxRingBuffer.Reader(), length, NULL);

                // bump custom state so the read value above is pushed only once
                stack.m_customState = 2;
            }
        }
        else
        {
            // this is NOT a long running operation
            // perform TX operation right away

            // Write data to ring buffer to start sending
            // by design: don't bother checking the return value
            UART2_write(palUart->UartDriver, (const void *)palUart->TxRingBuffer.Reader(), length, NULL);

            // pop data that was written to FIFO
            // pop elements from ring buffer, just pop
            palUart->TxRingBuffer.Pop(length);
        }
    }

    while (eventResult)
    {
        if (!palUart->IsLongRunning)
        {
            // this is not a long running operation so nothing to do here
            break;
        }

        // non-blocking wait allowing other threads to run while we wait for the Tx operation to complete
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeoutTicks, Event_SerialPortOut, eventResult));

        if (eventResult)
        {
            // event occurred
            // get from the eval stack how many bytes were buffered to Tx
            length = stack.m_evalStack[1].NumericByRef().s4;

            // reset Tx ongoing count
            palUart->TxOngoingCount = 0;

            // done here
            break;
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
        }
    }

    if (palUart->IsLongRunning)
    {
        // pop length heap block from stack
        stack.PopValue();

        // pop timeout heap block from stack
        stack.PopValue();
    }

    stack.SetResult_U4(length);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeRead___U4__SZARRAY_U1__I4__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array *dataBuffer;
    NF_PAL_UART *palUart = NULL;
    CLR_RT_HeapBlock hbTimeout;

    uint8_t uartNum;
    uint8_t *data;
    size_t dataLength = 0;

    size_t count = 0;
    size_t bytesRead = 0;
    size_t bytesToRead = 0;
    size_t readOffset = 0;

    int64_t *timeoutTicks;
    bool eventResult = true;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    // dereference the data buffer from the argument
    dataBuffer = stack.Arg1().DereferenceArray();

    // The offset to start filling the buffer
    readOffset = stack.Arg2().NumericByRef().s4;

    // get a the pointer to the array by using the first element of the array
    data = dataBuffer->GetElement(readOffset);

    // get how many bytes are requested to read
    count = stack.Arg3().NumericByRef().s4;

    uartNum = PORT_INDEX_TO_UART_NUM(pThis[FIELD___portIndex].NumericByRef().s4);

    // Choose the driver for this SerialDevice
    palUart = GetPalUartFromUartNum(uartNum);
    if (palUart == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // setup timeout from the _readtimeout heap block
    hbTimeout.SetInteger((CLR_INT64)pThis[FIELD___readTimeout].NumericByRef().s4 * TIME_CONVERSION__TO_MILLISECONDS);
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeoutTicks));

    // figure out what's available in the Rx ring buffer
    if (palUart->RxRingBuffer.Length() >= count)
    {
        // read from Rx ring buffer
        bytesToRead = count;

        // we have enough bytes, skip wait for event
        eventResult = false;

        // clear event by getting it
        Events_Get(SYSTEM_EVENT_FLAG_COM_IN);
    }
    else
    {
        if (stack.m_customState == 1)
        {
            // not enough bytes available, have to read from UART
            palUart->RxBytesToRead = count;

            // clear event by getting it
            Events_Get(SYSTEM_EVENT_FLAG_COM_IN);

            // don't read anything from the buffer yet
            bytesToRead = 0;
        }
    }

    while (eventResult)
    {
        if (stack.m_customState == 1)
        {
            if (bytesToRead > 0)
            {
                // enough bytes available
                eventResult = false;
            }
            else
            {
                // need to read from the UART
                // update custom state
                stack.m_customState = 2;
            }
        }
        else
        {
            // wait for event
            NANOCLR_CHECK_HRESULT(
                g_CLR_RT_ExecutionEngine
                    .WaitEvents(stack.m_owningThread, *timeoutTicks, Event_SerialPortIn, eventResult));

            if (!eventResult)
            {
                // event timeout
                NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
            }
        }
    }

    if (bytesToRead > 0)
    {
        // pop the requested bytes from the ring buffer
        bytesRead = palUart->RxRingBuffer.Pop(data, bytesToRead);
    }

    // pop timeout heap block from stack
    stack.PopValue();

    // return how many bytes were read
    stack.SetResult_U4(bytesRead);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeSetWatchChar___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_PAL_UART *palUart;
    uint8_t uartNum;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    uartNum = PORT_INDEX_TO_UART_NUM(pThis[FIELD___portIndex].NumericByRef().s4);

    palUart = GetPalUartFromUartNum(uartNum);
    if (palUart == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // set watch char
    palUart->WatchChar = (uint8_t)pThis[FIELD___watchChar].NumericByRef().u1;

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::GetDeviceSelector___STATIC__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    // declare the device selector string whose max size is "COM2," + terminator
    // and init with the terminator
    char deviceSelectorString[4 + 1] = {0};

#if defined(NF_SERIAL_COMM_TI_USE_UART1) && (NF_SERIAL_COMM_TI_USE_UART1 == TRUE)
    strcat(deviceSelectorString, "COM2,");
#endif

    // replace the last comma with a terminator
    deviceSelectorString[hal_strlen_s(deviceSelectorString) - 1] = '\0';

    // because the caller is expecting a result to be returned
    // we need set a return result in the stack argument using the appropriate SetResult according to the variable
    // type (a string here)
    stack.SetResult_String(deviceSelectorString);

    NANOCLR_NOCLEANUP_NOLABEL();
}
