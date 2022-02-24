//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <hal.h>
#include <nanoHAL.h>
#include "sys_io_ser_native_target.h"

#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART1) && (NF_SERIAL_COMM_STM32_UART_USE_USART1 == TRUE)
NF_PAL_UART Uart1_PAL;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART2) && (NF_SERIAL_COMM_STM32_UART_USE_USART2 == TRUE)
NF_PAL_UART Uart2_PAL;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART3) && (NF_SERIAL_COMM_STM32_UART_USE_USART3 == TRUE)
NF_PAL_UART Uart3_PAL;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART4) && (NF_SERIAL_COMM_STM32_UART_USE_UART4 == TRUE)
NF_PAL_UART Uart4_PAL;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART5) && (NF_SERIAL_COMM_STM32_UART_USE_UART5 == TRUE)
NF_PAL_UART Uart5_PAL;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART6) && (NF_SERIAL_COMM_STM32_UART_USE_USART6 == TRUE)
NF_PAL_UART Uart6_PAL;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART7) && (NF_SERIAL_COMM_STM32_UART_USE_UART7 == TRUE)
NF_PAL_UART Uart7_PAL;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART8) && (NF_SERIAL_COMM_STM32_UART_USE_UART8 == TRUE)
NF_PAL_UART Uart8_PAL;
#endif

static NF_PAL_UART *GetUartPAL(int index)
{
    // Choose the driver for this SerialDevice
    switch (index)
    {
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART1) && (NF_SERIAL_COMM_STM32_UART_USE_USART1 == TRUE)
        case 1:
            return &Uart1_PAL;
#endif

#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART2) && (NF_SERIAL_COMM_STM32_UART_USE_USART2 == TRUE)
        case 2:
            return &Uart2_PAL;
#endif

#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART3) && (NF_SERIAL_COMM_STM32_UART_USE_USART3 == TRUE)
        case 3:
            return &Uart3_PAL;
#endif

#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART4) && (NF_SERIAL_COMM_STM32_UART_USE_UART4 == TRUE)
        case 4:
            return &Uart4_PAL;
#endif

#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART5) && (NF_SERIAL_COMM_STM32_UART_USE_UART5 == TRUE)
        case 5:
            return &Uart5_PAL;
#endif

#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART6) && (NF_SERIAL_COMM_STM32_UART_USE_USART6 == TRUE)
        case 6:
            return &Uart6_PAL;
#endif

#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART7) && (NF_SERIAL_COMM_STM32_UART_USE_UART7 == TRUE)
        case 7:
            return &Uart7_PAL;
#endif

#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART8) && (NF_SERIAL_COMM_STM32_UART_USE_UART8 == TRUE)
        case 8:
            return &Uart8_PAL;
#endif
        default:
            // this COM port is not valid;
            return NULL;
    }
}

// This callback is invoked when a transmission buffer has been completely read by the driver.
static void TxEnd1(UARTDriver *uartp)
{
    (void)uartp;

    NATIVE_INTERRUPT_START

    NF_PAL_UART *palUart;

#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART1) && (NF_SERIAL_COMM_STM32_UART_USE_USART1 == TRUE)
    if (uartp == &UARTD1)
    {
        palUart = &Uart1_PAL;
    }
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART2) && (NF_SERIAL_COMM_STM32_UART_USE_USART2 == TRUE)
    if (uartp == &UARTD2)
    {
        palUart = &Uart2_PAL;
    }
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART3) && (NF_SERIAL_COMM_STM32_UART_USE_USART3 == TRUE)
    if (uartp == &UARTD3)
    {
        palUart = &Uart3_PAL;
    }
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART4) && (NF_SERIAL_COMM_STM32_UART_USE_UART4 == TRUE)
    if (uartp == &UARTD4)
    {
        palUart = &Uart4_PAL;
    }
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART5) && (NF_SERIAL_COMM_STM32_UART_USE_UART5 == TRUE)
    if (uartp == &UARTD5)
    {
        palUart = &Uart5_PAL;
    }
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART6) && (NF_SERIAL_COMM_STM32_UART_USE_USART6 == TRUE)
    if (uartp == &UARTD6)
    {
        palUart = &Uart6_PAL;
    }
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART7) && (NF_SERIAL_COMM_STM32_UART_USE_UART7 == TRUE)
    if (uartp == &UARTD7)
    {
        palUart = &Uart7_PAL;
    }
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART8) && (NF_SERIAL_COMM_STM32_UART_USE_UART8 == TRUE)
    if (uartp == &UARTD8)
    {
        palUart = &Uart8_PAL;
    }
#endif

    // reset Tx ongoing count
    palUart->TxOngoingCount = 0;

    Events_Set(SYSTEM_EVENT_FLAG_COM_OUT);

    NATIVE_INTERRUPT_END
}

// This callback is invoked when a character is received but the application was not ready to receive it, the character
// is passed as parameter.
static void RxChar(UARTDriver *uartp, uint16_t c)
{
    NATIVE_INTERRUPT_START

    NF_PAL_UART *palUart;
    uint8_t portIndex = 0;

#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART1) && (NF_SERIAL_COMM_STM32_UART_USE_USART1 == TRUE)
    if (uartp == &UARTD1)
    {
        palUart = &Uart1_PAL;
        portIndex = 1;
    }
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART2) && (NF_SERIAL_COMM_STM32_UART_USE_USART2 == TRUE)
    if (uartp == &UARTD2)
    {
        palUart = &Uart2_PAL;
        portIndex = 2;
    }
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART3) && (NF_SERIAL_COMM_STM32_UART_USE_USART3 == TRUE)
    if (uartp == &UARTD3)
    {
        palUart = &Uart3_PAL;
        portIndex = 3;
    }
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART4) && (NF_SERIAL_COMM_STM32_UART_USE_UART4 == TRUE)
    if (uartp == &UARTD4)
    {
        palUart = &Uart4_PAL;
        portIndex = 4;
    }
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART5) && (NF_SERIAL_COMM_STM32_UART_USE_UART5 == TRUE)
    if (uartp == &UARTD5)
    {
        palUart = &Uart5_PAL;
        portIndex = 5;
    }
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART6) && (NF_SERIAL_COMM_STM32_UART_USE_USART6 == TRUE)
    if (uartp == &UARTD6)
    {
        palUart = &Uart6_PAL;
        portIndex = 6;
    }
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART7) && (NF_SERIAL_COMM_STM32_UART_USE_UART7 == TRUE)
    if (uartp == &UARTD7)
    {
        palUart = &Uart7_PAL;
        portIndex = 7;
    }
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART8) && (NF_SERIAL_COMM_STM32_UART_USE_UART8 == TRUE)
    if (uartp == &UARTD8)
    {
        palUart = &Uart8_PAL;
        portIndex = 8;
    }
#endif

    // store this into the UART Rx buffer

    // push char to ring buffer
    // don't care about the success of the operation, if it's full we are droping the char anyway
    palUart->RxRingBuffer.Push((uint8_t)c);

    // is there a read operation going on?
    if (palUart->RxBytesToRead > 0)
    {
        // yes
        // check if the requested bytes are available in the buffer...
        //... or if the watch char was received
        if ((palUart->RxRingBuffer.Length() >= palUart->RxBytesToRead) || (c == palUart->WatchChar))
        {
            // reset Rx bytes to read count
            palUart->RxBytesToRead = 0;

            // fire event for Rx buffer complete
            Events_Set(SYSTEM_EVENT_FLAG_COM_IN);
        }
    }
    else if (palUart->NewLineChar > 0 && c == palUart->NewLineChar)
    {
        // fire event for new line char found
        Events_Set(SYSTEM_EVENT_FLAG_COM_IN);
    }
    else
    {
        // no read operation ongoing, so fire an event

        // post a managed event with the port index and event code (check if this is the watch char or just another
        // another)
        // TODO: check if callbacks are registered so this is called only if there is anyone listening otherwise don't
        // bother for that to happen ChibiOS callback has to accept arg which we would passing the GpioPin
        // CLR_RT_HeapBlock (see Gpio handler) check http://www.chibios.com/forum/viewtopic.php?f=36&t=4798
        PostManagedEvent(
            EVENT_SERIAL,
            0,
            portIndex,
            (c == palUart->WatchChar) ? SerialData_WatchChar : SerialData_Chars);
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

#if defined(USART_CR2_TXINV) && defined(USART_CR2_RXINV)

    NF_PAL_UART *palUart = NULL;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    // get the driver for this SerialDevice
    palUart = GetUartPAL((int)pThis[FIELD___portIndex].NumericByRef().s4);
    if (palUart == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    stack.SetResult_Boolean(palUart->SignalLevelsInverted);

#else

    (void)stack;

    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);

#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::set_InvertSignalLevels___VOID__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(USART_CR2_TXINV) && defined(USART_CR2_RXINV)

    NF_PAL_UART *palUart = NULL;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    // get the driver for this SerialDevice
    palUart = GetUartPAL((int)pThis[FIELD___portIndex].NumericByRef().s4);
    if (palUart == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    palUart->SignalLevelsInverted = (bool)stack.Arg1().NumericByRef().u1;

    // call config
    NativeConfig___VOID(stack);
#else

    (void)stack;

    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);

#endif

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

    // get a the pointer to the array by using the first element of the array
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

    // Choose the driver for this SerialDevice
    palUart = GetUartPAL((int)pThis[FIELD___portIndex].NumericByRef().s4);
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

        // get a the pointer to the array by using the first element of the array
        data = dataBuffer->GetElement(offset);

        // push onto the eval stack how many bytes are being pushed to the UART
        stack.PushValueI4(count);

        // flush DMA buffer to ensure cache coherency
        // (only required for Cortex-M7)
        cacheBufferFlush(data, count);

        // store pointer
        palUart->TxBuffer = data;

        // set TX ongoing count
        palUart->TxOngoingCount = count;

        // because the UART can be accessed from several threads need to get exclusive access to it
        uartAcquireBus(palUart->UartDriver);

        // start sending data (DMA will read from the ring buffer)
        uartStartSend(palUart->UartDriver, count, (uint8_t *)data);

        // bump custom state
        stack.m_customState = 2;
    }

    while (eventResult)
    {
        // non-blocking wait allowing other threads to run while we wait for the Tx operation to complete
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeoutTicks, Event_SerialPortOut, eventResult));

        // done here, release the UART
        uartReleaseBus(palUart->UartDriver);

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

#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART1) && (NF_SERIAL_COMM_STM32_UART_USE_USART1 == TRUE)
        case 1:
            UnInit_UART1();
            break;
#endif

#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART2) && (NF_SERIAL_COMM_STM32_UART_USE_USART2 == TRUE)
        case 2:
            UnInit_UART2();
            break;
#endif

#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART3) && (NF_SERIAL_COMM_STM32_UART_USE_USART3 == TRUE)
        case 3:
            UnInit_UART3();
            break;
#endif

#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART4) && (NF_SERIAL_COMM_STM32_UART_USE_UART4 == TRUE)
        case 4:
            UnInit_UART4();
            break;
#endif

#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART5) && (NF_SERIAL_COMM_STM32_UART_USE_UART5 == TRUE)
        case 5:
            UnInit_UART5();
            break;
#endif

#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART6) && (NF_SERIAL_COMM_STM32_UART_USE_USART6 == TRUE)
        case 6:
            UnInit_UART6();
            break;
#endif

#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART7) && (NF_SERIAL_COMM_STM32_UART_USE_UART7 == TRUE)
        case 7:
            UnInit_UART7();
            break;
#endif

#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART8) && (NF_SERIAL_COMM_STM32_UART_USE_UART8 == TRUE)
        case 8:
            UnInit_UART8();
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

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Choose the driver for this SerialDevice
    switch ((int)pThis[FIELD___portIndex].NumericByRef().s4)
    {
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART1) && (NF_SERIAL_COMM_STM32_UART_USE_USART1 == TRUE)
        case 1:
            Init_UART1();
            Uart1_PAL.UartDriver = &UARTD1;
            palUart = &Uart1_PAL;
            break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART2) && (NF_SERIAL_COMM_STM32_UART_USE_USART2 == TRUE)
        case 2:
            Init_UART2();
            Uart2_PAL.UartDriver = &UARTD2;
            palUart = &Uart2_PAL;
            break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART3) && (NF_SERIAL_COMM_STM32_UART_USE_USART3 == TRUE)
        case 3:
            Init_UART3();
            Uart3_PAL.UartDriver = &UARTD3;
            palUart = &Uart3_PAL;
            break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART4) && (NF_SERIAL_COMM_STM32_UART_USE_UART4 == TRUE)
        case 4:
            Init_UART4();
            Uart4_PAL.UartDriver = &UARTD4;
            palUart = &Uart4_PAL;
            break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART5) && (NF_SERIAL_COMM_STM32_UART_USE_UART5 == TRUE)
        case 5:
            Init_UART5();
            Uart5_PAL.UartDriver = &UARTD5;
            palUart = &Uart5_PAL;
            break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART6) && (NF_SERIAL_COMM_STM32_UART_USE_USART6 == TRUE)
        case 6:
            Init_UART6();
            Uart6_PAL.UartDriver = &UARTD6;
            palUart = &Uart6_PAL;
            break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART7) && (NF_SERIAL_COMM_STM32_UART_USE_UART7 == TRUE)
        case 7:
            Init_UART7();
            Uart7_PAL.UartDriver = &UARTD7;
            palUart = &Uart7_PAL;
            break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART8) && (NF_SERIAL_COMM_STM32_UART_USE_UART8 == TRUE)
        case 8:
            Init_UART8();
            Uart8_PAL.UartDriver = &UARTD8;
            palUart = &Uart8_PAL;
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

    // configure UART handlers
    palUart->Uart_cfg.txend1_cb = TxEnd1;
    palUart->Uart_cfg.rxchar_cb = RxChar;
    // palUart->Uart_cfg.rxend_cb = RxEnd;

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

    // data bits @ CR1:M1&M0
    // TODO

    // parity @ CR1:PS
    // TODO

    // Check RS485 mode is not selected as currently not supported
    if ((SerialMode)pThis[FIELD___mode].NumericByRef().s4 != SerialMode_Normal)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_NOTIMPL);
    }

    // stop bits @ CR2:STOP1&STOP0
    // clear bits for stop bits setting
    palUart->Uart_cfg.cr2 &= ~USART_CR2_STOP;

    switch ((StopBits)pThis[FIELD___stopBits].NumericByRef().s4)
    {
        case StopBits_One:
            // already set with the above
            break;
        case StopBits_OnePointFive:
            palUart->Uart_cfg.cr2 |= USART_CR2_STOP_1 + USART_CR2_STOP_0;
            break;
        case StopBits_Two:
            palUart->Uart_cfg.cr2 |= USART_CR2_STOP_1;
            break;
    }

#if defined(USART_CR2_TXINV) && defined(USART_CR2_RXINV)

    // configure TX, RX signal levels
    if (palUart->SignalLevelsInverted)
    {
        palUart->Uart_cfg.cr2 |= USART_CR2_TXINV | USART_CR2_RXINV;
    }
    else
    {
        palUart->Uart_cfg.cr2 &= ~(USART_CR2_TXINV | USART_CR2_RXINV);
    }

#endif

    // baud rate
    palUart->Uart_cfg.speed = (int)pThis[FIELD___baudRate].NumericByRef().s4;

    // stop UART, better do this before changing configuration
    uartStop(palUart->UartDriver);

    // get pointer to PAL UART
    switch ((int)pThis[FIELD___portIndex].NumericByRef().s4)
    {
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART1) && (NF_SERIAL_COMM_STM32_UART_USE_USART1 == TRUE)
        case 1:
            ConfigPins_UART1();
            break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART2) && (NF_SERIAL_COMM_STM32_UART_USE_USART2 == TRUE)
        case 2:
            ConfigPins_UART2();
            break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART3) && (NF_SERIAL_COMM_STM32_UART_USE_USART3 == TRUE)
        case 3:
            ConfigPins_UART3();
            break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART4) && (NF_SERIAL_COMM_STM32_UART_USE_UART4 == TRUE)
        case 4:
            ConfigPins_UART4();
            break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART5) && (NF_SERIAL_COMM_STM32_UART_USE_UART5 == TRUE)
        case 5:
            ConfigPins_UART5();
            break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART6) && (NF_SERIAL_COMM_STM32_UART_USE_USART6 == TRUE)
        case 6:
            ConfigPins_UART6();
            break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART7) && (NF_SERIAL_COMM_STM32_UART_USE_UART7 == TRUE)
        case 7:
            ConfigPins_UART7();
            break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART8) && (NF_SERIAL_COMM_STM32_UART_USE_UART8 == TRUE)
        case 8:
            ConfigPins_UART8();
            break;
#endif
    }

    uartStart(palUart->UartDriver, &palUart->Uart_cfg);
    uartStopSend(palUart->UartDriver);
    uartStopReceive(palUart->UartDriver);

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

        // flush DMA buffer to ensure cache coherency
        // (only required for Cortex-M7)
        cacheBufferFlush(buffer, bufferLength);

        // store pointer
        palUart->TxBuffer = (uint8_t *)buffer;

        // set TX ongoing count
        palUart->TxOngoingCount = bufferLength;

        // because the UART can be accessed from several threads need to get exclusive access to it
        uartAcquireBus(palUart->UartDriver);

        // start sending data (DMA will read from the ring buffer)
        uartStartSend(palUart->UartDriver, bufferLength, (uint8_t *)buffer);

        // bump custom state
        stack.m_customState = 2;
    }

    while (eventResult)
    {
        // non-blocking wait allowing other threads to run while we wait for the Tx operation to complete
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeoutTicks, Event_SerialPortOut, eventResult));

        // done here, release the UART
        uartReleaseBus(palUart->UartDriver);

        if (eventResult)
        {
            // event occurred
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

    // declare the device selector string whose max size is "COM1,COM2,COM3,COM4,COM5,COM6,COM7,COM8," + terminator
    // and init with the terminator
    char deviceSelectorString[40 + 1] = {0};

#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART1) && (NF_SERIAL_COMM_STM32_UART_USE_USART1 == TRUE)
    strcat(deviceSelectorString, "COM1,");
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART2) && (NF_SERIAL_COMM_STM32_UART_USE_USART2 == TRUE)
    strcat(deviceSelectorString, "COM2,");
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART3) && (NF_SERIAL_COMM_STM32_UART_USE_USART3 == TRUE)
    strcat(deviceSelectorString, "COM3,");
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART4) && (NF_SERIAL_COMM_STM32_UART_USE_UART4 == TRUE)
    strcat(deviceSelectorString, "COM4,");
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART5) && (NF_SERIAL_COMM_STM32_UART_USE_UART5 == TRUE)
    strcat(deviceSelectorString, "COM5,");
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART6) && (NF_SERIAL_COMM_STM32_UART_USE_USART6 == TRUE)
    strcat(deviceSelectorString, "COM6,");
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART7) && (NF_SERIAL_COMM_STM32_UART_USE_UART7 == TRUE)
    strcat(deviceSelectorString, "COM7,");
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART8) && (NF_SERIAL_COMM_STM32_UART_USE_UART8 == TRUE)
    strcat(deviceSelectorString, "COM8,");
#endif

    // replace the last comma with a terminator
    deviceSelectorString[hal_strlen_s(deviceSelectorString) - 1] = '\0';

    // because the caller is expecting a result to be returned
    // we need set a return result in the stack argument using the appropriate SetResult according to the variable
    // type (a string here)
    stack.SetResult_String(deviceSelectorString);

    NANOCLR_NOCLEANUP_NOLABEL();
}
