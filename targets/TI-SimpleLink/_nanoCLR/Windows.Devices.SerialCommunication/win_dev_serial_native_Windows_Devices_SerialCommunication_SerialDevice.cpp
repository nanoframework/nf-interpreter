//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include "win_dev_serial_native_target.h"

/////////////////////////////////////////////////////////
// UART PAL strucs delcared in win_dev_serial_native.h //
/////////////////////////////////////////////////////////
#if defined(NF_SERIAL_COMM_TI_USE_UART1) && (NF_SERIAL_COMM_TI_USE_UART1 == TRUE)
NF_PAL_UART Uart1_PAL;
#endif

// in UWP the COM ports are named COM1, COM2, COM3. But TI SimpleLink uses internally UART0, UART1, UART2. This maps the
// port index 1, 2 or 3 to the UART number 0, 1, etc
#define PORT_INDEX_TO_UART_NUM(portIndex) ((portIndex)-1)

// in UWP the COM ports are named COM1, COM2, COM3. But TI SimpleLink uses internally UART0, UART1, etc. This maps the
// UART number 0, 1 (..) to the port index 1, 2, etc
#define UART_NUM_TO_PORT_INDEX(uartNum) ((uartNum) + 1)

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
        UART2_close(palUart->UartDriver);

        // free buffers meory
        platform_free(palUart->TxBuffer);

        // null all pointers
        palUart->TxBuffer = NULL;
        palUart->UartDriver = NULL;
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

// This callback is invoked when a character is received but the application was not ready to receive it, the character
// is passed as parameter.
static void RxCallback(UART2_Handle handle, void *buf, size_t count, void *userArg, int_fast16_t status)
{
    NATIVE_INTERRUPT_START

    size_t bufferIndex = 0;
    uint8_t *bufferCursor = (uint8_t *)buf;
    bool watchCharFound = false;

    NF_PAL_UART *palUart = (NF_PAL_UART *)userArg;

    if (status == UART2_STATUS_SUCCESS)
    {
        // store this into the UART Rx buffer

        // // push char to ring buffer
        // // don't care about the success of the operation, if it's full we are droping the char anyway
        // palUart->RxRingBuffer.Push((uint8_t *)buf, count);

        if (palUart->WatchChar != 0)
        {
            while (bufferIndex < count)
            {
                if (*bufferCursor == palUart->WatchChar)
                {
                    watchCharFound = true;

                    break;
                }
            }
        }

        // is there a read operation going on?
        if (palUart->RxBytesToRead > 0)
        {
            // yes
            // check if the requested bytes are available in the buffer...
            //... or if the watch char was received
            // if (palUart->RxRingBuffer.Length() >= palUart->RxBytesToRead || watchCharFound)
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

            // post a managed event with the port index and event code (check if this is the watch char or just another
            // another)
            PostManagedEvent(
                EVENT_SERIAL,
                0,
                UART_NUM_TO_PORT_INDEX(palUart->UartNum),
                watchCharFound ? SerialData_WatchChar : SerialData_Chars);
        }
    }

    NATIVE_INTERRUPT_END
}

HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeDispose___VOID(
    CLR_RT_StackFrame &stack)
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

HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeInit___VOID(
    CLR_RT_StackFrame &stack)
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
        txBufferSize = UART1_TX_SIZE;
#endif
        // alloc buffer memory
        palUart->TxBuffer = (uint8_t *)platform_malloc(txBufferSize);
        // sanity check
        if (palUart->TxBuffer == NULL)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }

        // init buffer
        palUart->TxRingBuffer.Initialize(palUart->TxBuffer, txBufferSize);

        // all the rest
        palUart->UartNum = uartNum;
        palUart->WatchChar = 0;
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeConfig___VOID(
    CLR_RT_StackFrame &stack)
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
    palUart->UartParams.readMode = UART2_Mode_CALLBACK;

    // store pointer to PAL UART
    palUart->UartParams.userArg = palUart;

    palUart->UartParams.readCallback = RxCallback;
    palUart->UartParams.writeCallback = TxCallback;

    palUart->UartParams.readReturnMode = UART2_ReadReturnMode_PARTIAL;
    palUart->UartParams.baudRate = (int)pThis[FIELD___baudRate].NumericByRef().s4;

    // UART2_DataLen goes from 5->0 to 8->3 bits.
    // Because the parameter carries the bits count, subtracting 5 gives the correct setting.
    palUart->UartParams.dataLength = (UART2_DataLen)(pThis[FIELD___dataBits].NumericByRef().s4 - 5);

    // parity (SerialParity enum matches UART2_Parity)
    palUart->UartParams.parityType = (UART2_Parity)pThis[FIELD___parity].NumericByRef().s4;

    // stop bits
    switch ((SerialStopBitCount)pThis[FIELD___stopBits].NumericByRef().s4)
    {
        case SerialStopBitCount_One:
            palUart->UartParams.stopBits = UART2_StopBits_1;
            break;

        case SerialStopBitCount_Two:
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
    }

    palUart->UartDriver = UART2_open(uartNum, &palUart->UartParams);

    UART2_rxEnable(palUart->UartDriver);

    // check if UART was opened
    if (palUart->UartDriver == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeWrite___VOID__SZARRAY_U1(
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

        // need to update the _unstoredBufferLength field in the SerialDeviceOutputStream
        // get pointer to outputStream field
        CLR_RT_HeapBlock *outputStream =
            pThis[Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::FIELD___outputStream]
                .Dereference();
        // get pointer to _unstoredBufferLength field and udpate field value
        outputStream[Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDeviceOutputStream::
                         FIELD___unstoredBufferLength]
            .NumericByRef()
            .s4 = palUart->TxRingBuffer.Length();

        // null pointers and vars
        pThis = NULL;
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeStore___U4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

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
            NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTimeSpan(pThis[FIELD___writeTimeout], timeoutTicks));

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

HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::
    NativeRead___U4__SZARRAY_U1__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array *dataBuffer;
    NF_PAL_UART *palUart = NULL;

    uint8_t uartNum;
    uint8_t *data;
    size_t dataLength = 0;

    size_t count = 0;
    size_t bytesRead = 0;
    size_t bytesToRead = 0;

    InputStreamOptions options = InputStreamOptions_None;

    int64_t *timeoutTicks;
    bool eventResult = true;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    if (!(pThis))
        FAULT_ON_NULL(pThis);

    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    // dereference the data buffer from the argument
    dataBuffer = stack.Arg1().DereferenceArray();

    // get a the pointer to the array by using the first element of the array
    data = dataBuffer->GetFirstElement();

    // get the length of the data buffer
    dataLength = dataBuffer->m_numOfElements;

    // get how many bytes are requested to read
    count = stack.Arg2().NumericByRef().s4;

    // get the InputStreamOptions option
    options = (InputStreamOptions)stack.Arg3().NumericByRef().s4;

    uartNum = PORT_INDEX_TO_UART_NUM(pThis[FIELD___portIndex].NumericByRef().s4);

    // Choose the driver for this SerialDevice
    palUart = GetPalUartFromUartNum(uartNum);
    if (palUart == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // setup timeout from the _readtimeout heap block
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTimeSpan(pThis[FIELD___readTimeout], timeoutTicks));

    // figure out what's available in the Rx ring buffer
    if (UART2_getRxCount(palUart->UartDriver) >= count)
    {
        // read from Rx ring buffer
        bytesToRead = count;

        // is the read ahead option enabled?
        if (options == InputStreamOptions_ReadAhead)
        {
            // yes
            // check how many bytes we can store in the buffer argument
            if (dataLength < ((UART2_HWAttrs *)UART2_config[uartNum].hwAttrs)->rxBufSize)
            {
                // read as many bytes has the buffer can hold
                bytesToRead = dataLength;
            }
            else
            {
                // read everything that's available in the ring buffer
                bytesToRead = UART2_getRxCount(palUart->UartDriver);
            }
        }

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

                // compute how many bytes to read
                // considering the InputStreamOptions read ahead option
                if (options == InputStreamOptions_ReadAhead)
                {
                    // yes
                    // check how many bytes we can store in the buffer argument
                    if (dataLength < ((UART2_HWAttrs *)UART2_config[uartNum].hwAttrs)->rxBufSize)
                    {
                        // read as many bytes has the buffer can hold
                        bytesToRead = dataLength;
                    }
                    else
                    {
                        // read everything that's available in the ring buffer
                        bytesToRead = UART2_getRxCount(palUart->UartDriver);
                    }
                }
                else
                {
                    // take InputStreamOptions_Partial as default and read requested quantity or what's available
                    bytesToRead = count;

                    if (count > UART2_getRxCount(palUart->UartDriver))
                    {
                        // need to adjust because there aren't enough bytes available
                        bytesToRead = UART2_getRxCount(palUart->UartDriver);
                    }
                }
            }
        }
    }

    if (bytesToRead > 0)
    {
        // pop the requested bytes from the ring buffer
        UART2_read(palUart->UartDriver, data, bytesToRead, &bytesRead);
    }

    // pop timeout heap block from stack
    stack.PopValue();

    // return how many bytes were read
    stack.SetResult_U4(bytesRead);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeSetWatchChar___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
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
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::get_BytesToRead___U4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        NF_PAL_UART *palUart;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // Choose the driver for this SerialDevice
        palUart = GetPalUartFromUartNum((int)pThis[FIELD___portIndex].NumericByRef().s4);
        if (palUart == NULL)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // get length of Rx ring buffer
        stack.SetResult_U4(UART2_getRxCount(palUart->UartDriver));
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::
    GetDeviceSelector___STATIC__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
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
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
