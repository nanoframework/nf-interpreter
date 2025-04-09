//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_io_ser_native_target.h"
#include <target_platform.h>

#if defined(GECKO_USE_USART0) && (GECKO_USE_USART0 == TRUE)
NF_PAL_UART Uart0_PAL;
#endif
#if defined(GECKO_USE_USART1) && (GECKO_USE_USART1 == TRUE)
NF_PAL_UART Uart1_PAL;
#endif
#if defined(GECKO_USE_USART2) && (GECKO_USE_USART2 == TRUE)
NF_PAL_UART Uart2_PAL;
#endif
#if defined(GECKO_USE_USART3) && (GECKO_USE_USART3 == TRUE)
NF_PAL_UART Uart3_PAL;
#endif
#if (HAL_WP_USE_SERIAL == FALSE) && defined(GECKO_USE_USART4) && (GECKO_USE_USART4 == TRUE)
NF_PAL_UART Uart4_PAL;
#endif
#if defined(GECKO_USE_USART5) && (GECKO_USE_USART5 == TRUE)
NF_PAL_UART Uart5_PAL;
#endif

static NF_PAL_UART *GetUartPAL(uint8_t index)
{
    // Choose the driver for this SerialDevice
    switch (index)
    {
        //////////////////////////////////
        // Gecko USART index is 0 based //
        //////////////////////////////////

#if defined(GECKO_USE_USART0) && (GECKO_USE_USART0 == TRUE)
        case 1:
            return &Uart0_PAL;
#endif

#if defined(GECKO_USE_USART1) && (GECKO_USE_USART1 == TRUE)
        case 2:
            return &Uart1_PAL;
#endif

#if defined(GECKO_USE_USART2) && (GECKO_USE_USART2 == TRUE)
        case 3:
            return &Uart2_PAL;
#endif

#if defined(GECKO_USE_USART3) && (GECKO_USE_USART3 == TRUE)
        case 4:
            return &Uart3_PAL;
#endif

#if (HAL_WP_USE_SERIAL == FALSE) && defined(GECKO_USE_USART4) && (GECKO_USE_USART4 == TRUE)
        case 5:
            return &Uart4_PAL;
#endif

#if defined(GECKO_USE_USART5) && (GECKO_USE_USART5 == TRUE)
        case 6:
            return &Uart5_PAL;
#endif

        default:
            return NULL;
    }
}

// This handler is invoked when a byte has been transmitted by the USART.
static void UsartTxHandler(uint8_t index)
{
    NATIVE_INTERRUPT_START

    NF_PAL_UART *palUart = GetUartPAL(index);

    if (palUart->TxOngoingCount > 0)
    {
        // load the next byte
        palUart->Usart->TXDATA = *palUart->TxBuffer;

        // update pointer
        palUart->TxBuffer++;

        // update counter
        palUart->TxOngoingCount--;
    }
    else
    {
        // no more bytes to send
        Events_Set(SYSTEM_EVENT_FLAG_COM_OUT);
    }

    NATIVE_INTERRUPT_END
}

// This callback is invoked when a character is received
static void UsartRxHandler(uint8_t index)
{
    NATIVE_INTERRUPT_START

    NF_PAL_UART *palUart = GetUartPAL(index);

    uint8_t newChar = (uint8_t)palUart->Usart->RXDATA;

    // store this into the UART Rx buffer
    // don't care about the success of the operation, if it's full we are droping the char anyway
    palUart->RxRingBuffer.Push(newChar);

    // is there a read operation going on?
    if (palUart->RxBytesToRead > 0)
    {
        // yes
        // check if the requested bytes are available in the buffer...
        //... or if the watch char was received
        if ((palUart->RxRingBuffer.Length() >= palUart->RxBytesToRead) || (newChar == palUart->WatchChar))
        {
            // reset Rx bytes to read count
            palUart->RxBytesToRead = 0;

            // fire event for Rx buffer complete
            Events_Set(SYSTEM_EVENT_FLAG_COM_IN);
        }
    }
    else if (palUart->NewLineChar > 0 && newChar == palUart->NewLineChar)
    {
        // fire event for new line char found
        Events_Set(SYSTEM_EVENT_FLAG_COM_IN);
    }
    else
    {
        // no read operation ongoing, so fire an event, if the available bytes are above the threshold
        if (palUart->RxRingBuffer.Length() >= palUart->ReceivedBytesThreshold)
        {
            // post a managed event with the port index and event code (check if this is the watch char or just another
            // char)
            // TODO: check if callbacks are registered so this is called only if there is anyone listening otherwise
            PostManagedEvent(
                EVENT_SERIAL,
                0,
                index,
                (newChar == palUart->WatchChar) ? SerialData_WatchChar : SerialData_Chars);
        }
    }

    NATIVE_INTERRUPT_END
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::get_BytesToRead___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_PAL_UART *palUart;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Choose the driver for this SerialDevice
    palUart = GetUartPAL((int)pThis[FIELD___portIndex].NumericByRef().s4);
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
    offset = stack.Arg2().NumericByRef().s4;
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

    // get a the pointer to the array by using the offset
    data = dataBuffer->GetElement(offset);

    // Choose the driver for this SerialDevice
    palUart = GetUartPAL((int)pThis[FIELD___portIndex].NumericByRef().s4);
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

    uint8_t *buffer = NULL;
    uint32_t bufferLength;

    CLR_RT_HeapBlock &top = stack.PushValue();

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    // Choose the driver for this SerialDevice
    palUart = GetUartPAL((int)pThis[FIELD___portIndex].NumericByRef().s4);
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

    uint8_t *line = NULL;
    const char *newLine = NULL;
    uint32_t newLineLength;

    int64_t *timeoutTicks;
    bool eventResult = true;
    bool newLineFound = false;

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

    // Choose the driver for this SerialDevice
    palUart = GetUartPAL((int)pThis[FIELD___portIndex].NumericByRef().s4);
    if (palUart == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    if (stack.m_customState == 1)
    {
        // disable interrupt to protect access to the Rx buffer
        GLOBAL_LOCK();
        newLineFound = GetLineFromRxBuffer(pThis, &(palUart->RxRingBuffer), line);
        GLOBAL_UNLOCK();

        // check if there is a full line available to read
        if (newLineFound)
        {
            // got one!
            eventResult = false;
        }
        else
        {
            // get new line from field
            newLine = pThis[FIELD___newLine].RecoverString();

            // sanity check for NULL string
            if (newLine == NULL)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            }

            newLineLength = hal_strlen_s(newLine);

            // set new line char as the last one in the string
            // only if this one is found it will have a chance of the others being there
            // need to subtract one because we are 0 indexed
            palUart->NewLineChar = newLine[newLineLength - 1];

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
            // disable interrupt to protect access to the Rx buffer
            GLOBAL_LOCK();
            GetLineFromRxBuffer(pThis, &(palUart->RxRingBuffer), line);
            GLOBAL_UNLOCK();

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

    CLR_RT_HeapBlock_Array *dataBuffer;
    CLR_RT_HeapBlock hbTimeout;
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

    // Choose the driver for this SerialDevice
    palUart = GetUartPAL((int)pThis[FIELD___portIndex].NumericByRef().s4);
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

        // get a the pointer to the array by using the offset
        data = dataBuffer->GetElement(offset);

        // push onto the eval stack how many bytes are being pushed to the UART
        stack.PushValueI4(count);

        // store pointer
        palUart->TxBuffer = data;

        // set TX ongoing count
        palUart->TxOngoingCount = count;

        // decrease count for the char we're about to send
        palUart->TxOngoingCount--;

        // start sending data
        // this will trigger the TX interrupt
        palUart->Usart->TXDATA = *palUart->TxBuffer;

        // update buffer pointer
        palUart->TxBuffer++;

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
            count = stack.m_evalStack[1].NumericByRef().s4;

            // done here
            break;
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
        }
    }

    // pop "count" heap block from stack
    stack.PopValue();

    // pop "hbTimeout" heap block from stack
    stack.PopValue();

    stack.SetResult_U4(count);

    // null pointers and vars
    pThis = NULL;

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeDispose___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Choose the driver for this SerialDevice
    switch ((int)pThis[FIELD___portIndex].NumericByRef().s4)
    {
        //////////////////////////////////
        // Gecko USART index is 0 based //
        //////////////////////////////////

#if defined(GECKO_USE_USART0) && (GECKO_USE_USART0 == TRUE)
        case 1:
            UnInit_UART0();
            break;
#endif

#if defined(GECKO_USE_USART1) && (GECKO_USE_USART1 == TRUE)
        case 2:
            UnInit_UART1();
            break;
#endif

#if defined(GECKO_USE_USART2) && (GECKO_USE_USART2 == TRUE)
        case 3:
            UnInit_UART2();
            break;
#endif

#if defined(GECKO_USE_USART3) && (GECKO_USE_USART3 == TRUE)
        case 4:
            UnInit_UART3();
            break;
#endif

#if (HAL_WP_USE_SERIAL == FALSE) && defined(GECKO_USE_USART4) && (GECKO_USE_USART4 == TRUE)
        case 5:
            UnInit_UART4();
            break;
#endif

#if defined(GECKO_USE_USART5) && (GECKO_USE_USART5 == TRUE)
        case 6:
            UnInit_UART5();
            break;
#endif

        default:
            // this COM port is not valid
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            break;
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_PAL_UART *palUart;
    int32_t bufferSize;
    uint8_t watchChar;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Choose the driver for this SerialDevice
    switch ((int)pThis[FIELD___portIndex].NumericByRef().s4)
    {
        //////////////////////////////////
        // Gecko USART index is 0 based //
        //////////////////////////////////

#if defined(GECKO_USE_USART0) && (GECKO_USE_USART0 == TRUE)
        case 1:
            palUart = &Uart0_PAL;
            break;
#endif
#if defined(GECKO_USE_USART1) && (GECKO_USE_USART1 == TRUE)
        case 2:
            InitConfig_USART1();
            palUart = &Uart1_PAL;
            break;
#endif
#if defined(GECKO_USE_USART2) && (GECKO_USE_USART2 == TRUE)
        case 3:
            palUart = &Uart2_PAL;
            break;
#endif
#if defined(GECKO_USE_USART3) && (GECKO_USE_USART3 == TRUE)
        case 4:
            palUart = &Uart3_PAL;
            break;
#endif
#if (HAL_WP_USE_SERIAL == FALSE) && defined(GECKO_USE_USART4) && (GECKO_USE_USART4 == TRUE)
        case 5:
            palUart = &Uart4_PAL;
            break;
#endif
#if defined(GECKO_USE_USART5) && (GECKO_USE_USART5 == TRUE)
        case 6:
            palUart = &Uart5_PAL;
            break;
#endif
        default:
            // this COM port is not valid
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            break;
    }

    // alloc buffer memory
    bufferSize = pThis[FIELD___bufferSize].NumericByRef().s4;
    palUart->RxBuffer = (uint8_t *)platform_malloc(bufferSize);

    // sanity check
    if (palUart->RxBuffer == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    // init buffer
    palUart->RxRingBuffer.Initialize(palUart->RxBuffer, bufferSize);

    // get watch character
    watchChar = pThis[FIELD___watchChar].NumericByRef().u1;

    // set watch char, if set
    if (watchChar != 0)
    {
        palUart->WatchChar = watchChar;
    }

    // call the configure
    return NativeConfig___VOID(stack);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeConfig___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_PAL_UART *palUart = NULL;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Choose the driver for this SerialDevice
    palUart = GetUartPAL((int)pThis[FIELD___portIndex].NumericByRef().s4);
    if (palUart == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // setup configuration
    palUart->UsartInit = USART_INITASYNC_DEFAULT;

    // data bits
    // TODO

    // parity
    // TODO

    // Check RS485 mode is not selected as currently not supported
    if ((SerialMode)pThis[FIELD___mode].NumericByRef().s4 != SerialMode_Normal)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_NOTIMPL);
    }

    // stop bits

    switch ((StopBits)pThis[FIELD___stopBits].NumericByRef().s4)
    {
        case StopBits_One:
            // already set with the above
            break;
        case StopBits_OnePointFive:
            palUart->UsartInit.stopbits = usartStopbits1p5;
            break;
        case StopBits_Two:
            palUart->UsartInit.stopbits = usartStopbits2;
            break;
    }

    // configure TX, RX signal levels
    if (palUart->SignalLevelsInverted)
    {
        // this driver doesn't support inverted signal levels
        NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
    }

    // baud rate
    palUart->UsartInit.baudrate = (int)pThis[FIELD___baudRate].NumericByRef().s4;

    // stop UART, better do this before changing configuration
    USART_Reset(palUart->Usart);

    // get pointer to PAL UART
    switch ((int)pThis[FIELD___portIndex].NumericByRef().s4)
    {
        //////////////////////////////////
        // Gecko USART index is 0 based //
        //////////////////////////////////

#if defined(GECKO_USE_USART0) && (GECKO_USE_USART0 == TRUE)
        case 1:
            InitConfig_USART0();
            break;
#endif
#if defined(GECKO_USE_USART1) && (GECKO_USE_USART1 == TRUE)
        case 2:
            InitConfig_USART1();
            break;
#endif
#if defined(GECKO_USE_USART2) && (GECKO_USE_USART2 == TRUE)
        case 3:
            InitConfig_USART2();

            break;
#endif
#if defined(GECKO_USE_USART3) && (GECKO_USE_USART3 == TRUE)
        case 4:
            InitConfig_USART3();

            break;
#endif
#if (HAL_WP_USE_SERIAL == FALSE) && defined(GECKO_USE_USART4) && (GECKO_USE_USART4 == TRUE)
        case 5:
            InitConfig_USART4();
            break;
#endif
#if defined(GECKO_USE_USART5) && (GECKO_USE_USART5 == TRUE)
        case 6:
            InitConfig_USART5();
            break;
#endif
    }

    // null pointers and vars
    pThis = NULL;

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeSetWatchChar___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_PAL_UART *palUart;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Choose the driver for this SerialDevice
    palUart = GetUartPAL((int)pThis[FIELD___portIndex].NumericByRef().s4);
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

    NF_PAL_UART *palUart = NULL;

    CLR_RT_HeapBlock hbTimeout;
    int64_t *timeoutTicks;
    bool eventResult = true;

    bool isNewAllocation = false;
    char *buffer = NULL;
    uint32_t bufferLength = 0;
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

    // Choose the driver for this SerialDevice
    palUart = GetUartPAL((int)pThis[FIELD___portIndex].NumericByRef().s4);
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

        // push onto the eval stack if the buffer was allocated
        stack.PushValueI4(isNewAllocation ? 1 : 0);

        // store pointer
        palUart->TxBuffer = (uint8_t *)buffer;

        // set TX ongoing count
        palUart->TxOngoingCount = bufferLength;

        // decrease count for the char we're about to send
        palUart->TxOngoingCount--;

        // start sending data
        // this will trigger the TX interrupt
        palUart->Usart->TXDATA = *palUart->TxBuffer;

        // update buffer pointer
        palUart->TxBuffer++;

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

            // pop "isNewAllocation" from the eval stack
            isNewAllocation = stack.m_evalStack[2].NumericByRef().s4 == 1 ? true : false;

            // get from the eval stack how many bytes were buffered to Tx
            length = stack.m_evalStack[1].NumericByRef().s4;

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

    // pop "isNewAllocation" heap block from stack
    stack.PopValue();

    // pop "hbTimeout" heap block from stack
    stack.PopValue();

    stack.SetResult_U4(length);

    // free memory, if it was allocated
    if (isNewAllocation)
    {
        platform_free(palUart->TxBuffer);
    }

    // null pointers and vars
    pThis = NULL;

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeReceivedBytesThreshold___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_PAL_UART *palUart;
    int32_t threshold;
    uint8_t portIndex;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // check if threshold is valid
    threshold = (int32_t)stack.Arg1().NumericByRef().s4;

    if (threshold <= 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    portIndex = (int)pThis[FIELD___portIndex].NumericByRef().s4;

    // Choose the driver for this SerialDevice
    palUart = GetUartPAL(portIndex);
    if (palUart == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // update field
    pThis[FIELD___receivedBytesThreshold].NumericByRef().s4 = threshold;

    // update threshold value
    palUart->ReceivedBytesThreshold = threshold;

    // fake call to event handler in case port is open and the new threshold was set
    // to a value lower than the bytes that are already available
    if (pThis[FIELD___opened].NumericByRef().u1 && (uint32_t)threshold <= palUart->RxRingBuffer.Length())
    {
        //////////////////////////////////
        // Gecko USART index is 0 based //
        //////////////////////////////////
        PostManagedEvent(EVENT_SERIAL, 0, portIndex + 1, SerialData_Chars);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::GetDeviceSelector___STATIC__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    // declare the device selector string whose max size is "COM1,COM2,COM3,COM4,COM5,COM6,COM7,COM8," + terminator
    // and init with the terminator
    static char deviceSelectorString[] =

#if defined(GECKO_USE_USART0) && (GECKO_USE_USART0 == TRUE)
        "COM1,"
#endif
#if defined(GECKO_USE_USART1) && (GECKO_USE_USART1 == TRUE)
        "COM2,"
#endif
#if defined(GECKO_USE_USART2) && (GECKO_USE_USART2 == TRUE)
        "COM3,"
#endif
#if defined(GECKO_USE_USART3) && (GECKO_USE_USART3 == TRUE)
        "COM4,"
#endif

#if (HAL_WP_USE_SERIAL == FALSE) && defined(GECKO_USE_USART4) && (GECKO_USE_USART4 == TRUE)
        "COM5,"
#endif

#if defined(GECKO_USE_USART5) && (GECKO_USE_USART5 == TRUE)
        "COM6,"
#endif
        ;

    // replace the last comma with a terminator
    if (deviceSelectorString[hal_strlen_s(deviceSelectorString) - 1] == ',')
    {
        deviceSelectorString[hal_strlen_s(deviceSelectorString) - 1] = '\0';
    }

    // because the caller is expecting a result to be returned
    // we need set a return result in the stack argument using the appropriate SetResult according to the variable
    // type (a string here)
    stack.SetResult_String(deviceSelectorString);

    NANOCLR_NOCLEANUP_NOLABEL();
}

#if defined(GECKO_USE_USART0) && (GECKO_USE_USART0 == TRUE)
void USART0_RX_IRQHandler(void)
{
    // Gecko USART index is 0 based
    // need to follow the same index as in the PAL struct
    UsartRxHandler(1);
}
void USART0_TX_IRQHandler(void)
{
    // Gecko USART index is 0 based
    // need to follow the same index as in the PAL struct
    UsartTxHandler(1);
}
#endif

#if defined(GECKO_USE_USART1) && (GECKO_USE_USART1 == TRUE)
void USART1_RX_IRQHandler(void)
{
    // Gecko USART index is 0 based
    // need to follow the same index as in the PAL struct
    UsartRxHandler(2);
}
void USART1_TX_IRQHandler(void)
{
    // Gecko USART index is 0 based
    // need to follow the same index as in the PAL struct
    UsartTxHandler(2);
}
#endif

#if defined(GECKO_USE_USART2) && (GECKO_USE_USART2 == TRUE)
void USART2_RX_IRQHandler(void)
{
    // Gecko USART index is 0 based
    // need to follow the same index as in the PAL struct
    UsartRxHandler(3);
}
void USART2_TX_IRQHandler(void)
{
    // Gecko USART index is 0 based
    // need to follow the same index as in the PAL struct
    UsartTxHandler(3);
}
#endif

#if defined(GECKO_USE_USART3) && (GECKO_USE_USART3 == TRUE)
void USART3_RX_IRQHandler(void)
{
    // Gecko USART index is 0 based
    // need to follow the same index as in the PAL struct
    UsartRxHandler(4);
}
void USART3_TX_IRQHandler(void)
{
    // Gecko USART index is 0 based
    // need to follow the same index as in the PAL struct
    UsartTxHandler(4);
}
#endif

#if (HAL_WP_USE_SERIAL == FALSE) && defined(GECKO_USE_USART4) && (GECKO_USE_USART4 == TRUE)
void USART4_RX_IRQHandler(void)
{
    // Gecko USART index is 0 based
    // need to follow the same index as in the PAL struct
    UsartRxHandler(5);
}
void USART4_TX_IRQHandler(void)
{
    // Gecko USART index is 0 based
    // need to follow the same index as in the PAL struct
    UsartTxHandler(5);
}
#endif

#if defined(GECKO_USE_USART5) && (GECKO_USE_USART5 == TRUE)
void USART5_RX_IRQHandler(void)
{
    // Gecko USART index is 0 based
    // need to follow the same index as in the PAL struct
    UsartRxHandler(6);
}
void USART5_TX_IRQHandler(void)
{
    // Gecko USART index is 0 based
    // need to follow the same index as in the PAL struct
    UsartTxHandler(6);
}
#endif
