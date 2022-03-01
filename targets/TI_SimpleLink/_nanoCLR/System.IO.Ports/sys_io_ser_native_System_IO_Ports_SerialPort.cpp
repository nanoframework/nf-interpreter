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
        // reset TX ongoing count, if we are done here
        if (count > 0)
        {
            if (palUart->TxOngoingCount > 0)
            {
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
            else if (palUart->NewLineChar > 0 && input == palUart->NewLineChar)
            {
                // fire event for new line char found
                Events_Set(SYSTEM_EVENT_FLAG_COM_IN);
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

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::get_InvertSignalLevels___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    (void)stack;

    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::set_InvertSignalLevels___VOID__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    (void)stack;

    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::Read___I4__SZARRAY_U1__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock hbTimeout;
    CLR_RT_HeapBlock_Array *dataBuffer;
    NF_PAL_UART *palUart = NULL;
    uint8_t uartNum;

    uint8_t *data;

    uint32_t length;
    uint32_t count = 0;
    uint32_t bytesRead = 0;
    uint32_t offset = 0;
    uint32_t bytesToRead = 0;

    int64_t *timeoutTicks;
    bool eventResult = true;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    // setup timeout
    hbTimeout.SetInteger((CLR_INT64)pThis[FIELD___readTimeout].NumericByRef().s4 * TIME_CONVERSION__TO_MILLISECONDS);
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeoutTicks));

    // dereference the data buffer from the argument
    dataBuffer = stack.Arg1().DereferenceArray();
    // The offset to start filling the buffer
    offset = stack.Arg2().NumericByRef().s4;
    // get how many bytes are requested to read
    count = stack.Arg3().NumericByRef().s4;

    // perform parameter validation (only on initial call)
    if (stack.m_customState == 1)
    {
        // get the size of the buffer
        length = dataBuffer->m_numOfElements;

        // check parameters
        FAULT_ON_NULL_ARG(dataBuffer);

        if ((offset > length) || (count > length))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }

        if (offset + count > length)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }

    // get a the pointer to the array by using the first element of the array
    data = dataBuffer->GetElement(offset);

    uartNum = PORT_INDEX_TO_UART_NUM(pThis[FIELD___portIndex].NumericByRef().s4);

    // Choose the driver for this SerialDevice
    palUart = GetPalUartFromUartNum(uartNum);
    if (palUart == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

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

    // pop "hbTimeout" heap block from stack
    stack.PopValue();

    // return how many bytes were read
    stack.SetResult_U4(bytesRead);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::ReadExisting___STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_PAL_UART *palUart = NULL;
    uint8_t uartNum;

    uint8_t *buffer;
    uint32_t bufferLength;

    CLR_RT_HeapBlock &top = stack.PushValue();

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

    bufferLength = palUart->RxRingBuffer.Length();

    if (bufferLength)
    {
        // there are bytes available in the Rx buffer
        // setup read buffer
        buffer = (uint8_t *)platform_malloc(bufferLength);

        // sanity check
        if (buffer == NULL)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }

        // fill data buffer from Rx buffer
        palUart->RxRingBuffer.Pop(buffer, bufferLength);

        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(top, (const char *)buffer, bufferLength));
    }
    else
    {
        // create an empty <string>
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(top, (const char *)NULL));
    }

    NANOCLR_CLEANUP();

    if (buffer != NULL)
    {
        platform_free(buffer);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::ReadLine___STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock hbTimeout;
    NF_PAL_UART *palUart = NULL;
    uint8_t uartNum;

    uint8_t *line = NULL;
    const char *newLine;
    uint32_t newLineLength;

    int64_t *timeoutTicks;
    bool eventResult = true;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    // setup timeout
    hbTimeout.SetInteger((CLR_INT64)pThis[FIELD___readTimeout].NumericByRef().s4 * TIME_CONVERSION__TO_MILLISECONDS);
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeoutTicks));

    uartNum = PORT_INDEX_TO_UART_NUM(pThis[FIELD___portIndex].NumericByRef().s4);

    // Choose the driver for this SerialDevice
    palUart = GetPalUartFromUartNum(uartNum);
    if (palUart == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    if (stack.m_customState == 1)
    {
        // check if there is a full line available to read
        if (GetLineFromRxBuffer(pThis, &(palUart->RxRingBuffer), line))
        {
            // got one!
            eventResult = false;
        }
        else
        {
            // get new line from field
            newLine = pThis[FIELD___newLine].RecoverString();
            newLineLength = hal_strlen_s(newLine);
            // need to subtract one because we are 0 indexed
            newLineLength--;

            // set new line char as the last one in the string
            // only if this one is found it will have a chance of the others being there
            palUart->NewLineChar = newLine[newLineLength];

            stack.m_customState = 2;
        }
    }

    while (eventResult)
    {
        // wait for event
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeoutTicks, Event_SerialPortIn, eventResult));

        // clear the new line watch char
        palUart->NewLineChar = 0;

        if (eventResult)
        {
            GetLineFromRxBuffer(pThis, &(palUart->RxRingBuffer), line);

            // done here
            break;
        }
        else
        {
            // event timeout
            NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
        }
    }

    // pop "hbTimeout" heap block from stack
    stack.PopValue();

    // return how many bytes were read
    stack.SetResult_String((const char *)line);

    // free memory, if needed
    if (line != NULL)
    {
        platform_free(line);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::Write___VOID__SZARRAY_U1__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_PAL_UART *palUart = NULL;
    uint8_t uartNum;

    CLR_RT_HeapBlock_Array *dataBuffer;
    CLR_RT_HeapBlock hbTimeout;

    uint32_t estimatedDurationMiliseconds;

    int64_t *timeoutTicks;
    bool eventResult = true;

    uint8_t *data;
    int32_t length = 0;
    int32_t count = 0;
    int32_t offset = 0;

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

    // perform parameter validation and setup TX operation
    if (stack.m_customState == 0)
    {
        // dereference the data buffer from the argument
        dataBuffer = stack.Arg1().DereferenceArray();
        offset = stack.Arg2().NumericByRef().s4;
        count = stack.Arg3().NumericByRef().s4;

        // get the size of the buffer
        length = dataBuffer->m_numOfElements;

        // check parameters
        FAULT_ON_NULL_ARG(dataBuffer);

        if ((offset > length) || (count > length))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }

        if (offset + count > length)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // get a the pointer to the array by using the first element of the array
        data = dataBuffer->GetElement(offset);

        // check if this is a long running operation
        palUart->IsLongRunning = IsLongRunningOperation(
            count,
            (uint32_t)pThis[FIELD___baudRate].NumericByRef().s4,
            (uint32_t &)estimatedDurationMiliseconds);

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
                // push to the stack how many bytes bytes where buffered for TX
                stack.PushValueI4(count);

                // set TX count
                palUart->TxOngoingCount = count;

                // Write data to start sending
                // by design: don't bother checking the return value
                UART2_write(palUart->UartDriver, (const void *)data, count, NULL);

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
            UART2_write(palUart->UartDriver, (const void *)data, count, NULL);
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
            count = stack.m_evalStack[1].NumericByRef().s4;

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
        // pop "count" heap block from stack
        stack.PopValue();

        // pop "hbTimeout" heap block from stack
        stack.PopValue();
    }

    stack.SetResult_U4(count);

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
        int32_t bufferSize;

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
        if (palUart->RxBuffer == NULL)
        {
            // alloc buffer memory
            bufferSize = pThis[FIELD___bufferSize].NumericByRef().s4;

            palUart->RxBuffer = (uint8_t *)platform_malloc(bufferSize);

            // check allocation
            if (palUart->RxBuffer == NULL)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
            }

            // init buffer
            palUart->RxRingBuffer.Initialize(palUart->RxBuffer, bufferSize);
        }

#endif

        // all the rest
        palUart->WatchChar = 0;
        palUart->NewLineChar = 0;
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

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeWriteString___VOID__STRING__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_PAL_UART *palUart;
    uint8_t uartNum;

    CLR_RT_HeapBlock hbTimeout;
    int64_t *timeoutTicks;
    bool eventResult = true;

    bool isNewAllocation = false;
    char *buffer = NULL;
    uint32_t bufferLength;
    int32_t length = 0;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    if (stack.Arg1().RecoverString() == NULL)
    {
        // text string it's empty so there is noting to do here
        stack.SetResult_U4(0);
        NANOCLR_SET_AND_LEAVE(S_OK);
    }

    uartNum = PORT_INDEX_TO_UART_NUM(pThis[FIELD___portIndex].NumericByRef().s4);

    palUart = GetPalUartFromUartNum(uartNum);
    if (palUart == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // setup timeout
    hbTimeout.SetInteger((CLR_INT64)pThis[FIELD___writeTimeout].NumericByRef().s4 * TIME_CONVERSION__TO_MILLISECONDS);
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeoutTicks));

    // perform parameter validation and setup TX operation
    if (stack.m_customState == 1)
    {
        // get buffer to output
        NANOCLR_CHECK_HRESULT(SetupWriteLine(stack, &buffer, &bufferLength, &isNewAllocation));

        // push onto the eval stack how many bytes are being pushed to the UART
        stack.PushValueI4(bufferLength);

        // store pointer
        palUart->TxBuffer = (uint8_t *)buffer;

        // set TX ongoing count
        palUart->TxOngoingCount = bufferLength;

        UART2_write(palUart->UartDriver, (const void *)buffer, bufferLength, NULL);

        // bump custom state
        stack.m_customState = 2;
    }

    while (eventResult)
    {
        // non-blocking wait allowing other threads to run while we wait for the Tx operation to complete
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeoutTicks, Event_SerialPortOut, eventResult));

        if (eventResult)
        {
            // event occurred
            // get from the eval stack how many bytes were buffered to Tx
            length = stack.m_evalStack[1].NumericByRef().s4;

            palUart->TxOngoingCount = 0;

            // done here
            break;
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
        }
    }

    // pop "length" heap block from stack
    stack.PopValue();

    // pop "hbTimeout" heap block from stack
    stack.PopValue();

    stack.SetResult_U4(length);

    // free memory, if it was allocated
    if (isNewAllocation && buffer)
    {
        platform_free(buffer);
    }

    // null pointers and vars
    pThis = NULL;

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
