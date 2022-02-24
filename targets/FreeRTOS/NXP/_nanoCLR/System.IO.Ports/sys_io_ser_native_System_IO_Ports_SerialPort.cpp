//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_io_ser_native_target.h"

/////////////////////////////////////////////////////////
// UART PAL strucs delcared in win_dev_serial_native.h //
/////////////////////////////////////////////////////////

static LPUART_Type *const lpuart_bases[] = LPUART_BASE_PTRS;

// Static UART config/data structs.
static NF_PAL_UART Uart_PAL1, Uart_PAL2, Uart_PAL3, Uart_PAL4, Uart_PAL5, Uart_PAL6, Uart_PAL7, Uart_PAL8;

// Array of pointers to above config UART structs.
NF_PAL_UART *const
    Uart_PAL[]{NULL, &Uart_PAL1, &Uart_PAL2, &Uart_PAL3, &Uart_PAL4, &Uart_PAL5, &Uart_PAL6, &Uart_PAL7, &Uart_PAL8};

// Task firing up event after receiving wanted amount of bytes
static void vREvent(void *pvParameters)
{
    // This is main UART struct, holding config, handlers, and the rest of variables.d
    (void)pvParameters;
    uint32_t ulNotifiedValue;

    while (1)
    {
        xTaskNotifyWait(
            pdFALSE,          // Don't clear bits on entry.
            0xffffffff,       // Clear all bits on exit.
            &ulNotifiedValue, // Stores the notified value.
            portMAX_DELAY);

        // Check is there event from task reading bytes.
        if ((ulNotifiedValue & 0x02) != 0)
        {
            Events_Set(SYSTEM_EVENT_FLAG_COM_IN);
        }
    }
}

// Task for transmitting data through blocking uart
static void vWEvent(void *pvParameters)
{
    uint32_t ulNotifiedValue;
    uint8_t *uartNum_p = (uint8_t *)pvParameters;
    uint8_t uartNum = *uartNum_p;
    NF_PAL_UART *palUart = Uart_PAL[uartNum];

    while (1)
    {
        xTaskNotifyWait(
            pdFALSE,          // Don't clear bits on entry.
            0xffffffff,       // Clear all bits on exit.
            &ulNotifiedValue, // Stores the notified value.
            portMAX_DELAY);

        if ((ulNotifiedValue & 0x01) != 0)
        {
            LPUART_WriteBlocking(lpuart_bases[uartNum], palUart->xfer.data, palUart->xfer.dataSize);
            Events_Set(SYSTEM_EVENT_FLAG_COM_OUT);
        }
    }
}

static void UART_Handle(LPUART_Type *base, uint8_t uartNum)
{
    NATIVE_INTERRUPT_START
    uint32_t status;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    NF_PAL_UART *palUart = Uart_PAL[uartNum];

    status = LPUART_GetStatusFlags(base);

    if (kLPUART_RxOverrunFlag & status)
    {
        // Clear overrun flag, otherwise the RX does not work.
        base->STAT |= 1U << LPUART_STAT_OR_SHIFT;
    }

    if (kLPUART_RxDataRegFullFlag & status)
    {
        char byte = LPUART_ReadByte(base);
        // push char to ring buffer
        // don't care about the success of the operation, if it's full we are dropping the char anyway
        palUart->RxRingBuffer.Push((uint8_t)byte);

        // is there a read operation going on?
        if (palUart->RxBytesToRead > 0)
        {
            // check if the requested bytes are available in the buffer
            if (palUart->RxRingBuffer.Length() >= palUart->RxBytesToRead)
            {
                // reset Rx bytes to read count
                palUart->RxBytesToRead = 0;

                // Notify task that we want to receive data.
                xTaskNotifyFromISR(palUart->xRTaskToNotify, 0x02, eSetBits, &xHigherPriorityTaskWoken);
            }
        }
        else if (palUart->NewLineChar > 0 && byte == palUart->NewLineChar)
        {
            // fire event for new line char found
            Events_Set(SYSTEM_EVENT_FLAG_COM_IN);
        }
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

    NATIVE_INTERRUPT_END
}

// Override of the default MIMXRT1060 UART interrupt routines to simple UART_Handle function, which
// reads 1 byte of input data to RTOS stream buffer, if there's a buffer overflow it drops data and clears interrupts.

extern "C"
{
    // LPUART8 is currently used for debugging, disable it or will collide with debugger
    // void LPUART1_IRQHandler(void) { UART_Handle(LPUART1, 1); }
    void LPUART2_IRQHandler(void)
    {
        UART_Handle(LPUART2, 2);
    }
    void LPUART3_IRQHandler(void)
    {
        UART_Handle(LPUART3, 3);
    }
    void LPUART4_IRQHandler(void)
    {
        UART_Handle(LPUART4, 4);
    }
    void LPUART5_IRQHandler(void)
    {
        UART_Handle(LPUART5, 5);
    }
    void LPUART6_IRQHandler(void)
    {
        UART_Handle(LPUART6, 6);
    }
    void LPUART7_IRQHandler(void)
    {
        UART_Handle(LPUART7, 7);
    }
    void LPUART8_IRQHandler(void)
    {
        UART_Handle(LPUART8, 8);
    }
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::get_BytesToRead___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_PAL_UART *palUart;

    uint8_t uartNum = 0;
    size_t read_count = 0;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    uartNum = pThis[FIELD___portIndex].NumericByRef().s4;

    // Quit if parameters or device is invalid or out of range
    if (uartNum >= (sizeof(Uart_PAL) / sizeof(Uart_PAL[0])))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    palUart = Uart_PAL[uartNum];
    read_count = palUart->RxBytesToRead;

    stack.SetResult_U4(read_count);

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
    CLR_RT_HeapBlock_Array *dataBuffer = NULL;
    NF_PAL_UART *palUart = NULL;

    uint8_t *data = NULL;

    uint8_t uartNum = 0;
    size_t bytesRead = 0;
    size_t bytesToRead = 0;
    size_t offset = 0;
    uint32_t length;
    size_t count = 0;

    int64_t *timeoutTicks;
    bool eventResult = true;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    uartNum = pThis[FIELD___portIndex].NumericByRef().s4;

    // Quit if parameters or device is invalid or out of range
    if (uartNum >= (sizeof(Uart_PAL) / sizeof(Uart_PAL[0])))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    palUart = Uart_PAL[uartNum];

    // setup timeout
    hbTimeout.SetInteger((CLR_INT64)pThis[FIELD___readTimeout].NumericByRef().s4 * TIME_CONVERSION__TO_MILLISECONDS);
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeoutTicks));

    // dereference the data buffer from the argument
    dataBuffer = stack.Arg1().DereferenceArray();
    // The offset to start filling the buffer
    offset = stack.Arg2().NumericByRef().s4;

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

    // Check what's available in Rx ring buffer
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
            { // need to read from the UART
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
    uint8_t uartNum = 0;
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
    uartNum = pThis[FIELD___portIndex].NumericByRef().s4;

    // Quit if parameters or device is invalid or out of range
    if (uartNum >= (sizeof(Uart_PAL) / sizeof(Uart_PAL[0])))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    palUart = Uart_PAL[uartNum];

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
    uint8_t uartNum = 0;

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
    uartNum = pThis[FIELD___portIndex].NumericByRef().s4;

    // Quit if parameters or device is invalid or out of range
    if (uartNum >= (sizeof(Uart_PAL) / sizeof(Uart_PAL[0])))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    palUart = Uart_PAL[uartNum];

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

    size_t length = 0;

    CLR_RT_HeapBlock_Array *dataBuffer;
    CLR_RT_HeapBlock hbTimeout;
    int64_t *timeoutTicks;
    bool eventResult = true;

    uint8_t *data = NULL;
    uint8_t uartNum = 0;
    size_t count = 0;
    size_t offset = 0;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    uartNum = pThis[FIELD___portIndex].NumericByRef().s4;

    // Quit if parameters or device is invalid or out of range
    if (uartNum >= (sizeof(Uart_PAL) / sizeof(Uart_PAL[0])))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    palUart = Uart_PAL[uartNum];

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

        // store pointer
        palUart->TxBuffer = data;

        // set TX ongoing count
        palUart->TxOngoingCount = count;

        // Set transfer structure to nano ring buffer
        palUart->xfer.data = (uint8_t *)palUart->TxBuffer;
        palUart->xfer.dataSize = count;

        // Notify task that we want to transmit data.
        xTaskNotify(palUart->xWTaskToNotify, 0x01, eSetBits);

        // bump custom state
        stack.m_customState = 2;
    }

    while (eventResult)
    {
        // non-blocking wait allowing other threads to run while we wait for the TX operation to complete
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeoutTicks, Event_SerialPortOut, eventResult));

        if (eventResult)
        {
            // event occurred
            // get from the eval stack how many bytes were buffered to TX
            count = stack.m_evalStack[1].NumericByRef().s4;

            // reset TX ongoing count
            palUart->TxOngoingCount = 0;

            // done here
            break;
        }
        else
        {
            palUart->TxOngoingCount = 0;
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

    NF_PAL_UART *palUart;

    uint8_t uartNum = 0;
    LPUART_Type *base = NULL;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    uartNum = pThis[FIELD___portIndex].NumericByRef().s4;
    base = lpuart_bases[uartNum];

    // Quit if parameters or device is invalid or out of range
    if (uartNum >= (sizeof(Uart_PAL) / sizeof(Uart_PAL[0])) || base == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    palUart = Uart_PAL[uartNum];

    // Free ring buffer memory
    platform_free(palUart->RxBuffer);

    // null all pointers
    palUart->RxBuffer = NULL;
    palUart->TxBuffer = NULL;

    // Deinitialize device and delete FreeRTOS idle tasks
    LPUART_Deinit(base);
    vTaskDelete(palUart->xRTaskToNotify);
    vTaskDelete(palUart->xWTaskToNotify);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    status_t status = 0;
    uint8_t uartNum = 0;
    lpuart_config_t *config = NULL;
    LPUART_Type *base = NULL;
    NF_PAL_UART *palUart = NULL;
    BaseType_t xReturned;
    int32_t bufferSize;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    uartNum = pThis[FIELD___portIndex].NumericByRef().s4;
    config = &Uart_PAL[uartNum]->uartCfg;
    base = lpuart_bases[uartNum];

    // Quit if parameters or device is invalid or out of range
    if (uartNum >= (sizeof(Uart_PAL) / sizeof(Uart_PAL[0])) || config == NULL || base == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    palUart = Uart_PAL[uartNum];

    // Allocate memory for RX circular buffer
    bufferSize = pThis[FIELD___bufferSize].NumericByRef().s4;

    palUart->RxBuffer = (uint8_t *)platform_malloc(bufferSize * sizeof(uint8_t));

    if (palUart->RxBuffer == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    // Initialize RX buffer
    palUart->RxRingBuffer.Initialize(palUart->RxBuffer, bufferSize);
    palUart->RxBytesToRead = 0;

    // now all the rest
    palUart->TxOngoingCount = 0;
    palUart->WatchChar = 0;
    palUart->NewLineChar = 0;

    // Get default config structure for initializing given UART peripheral and enable TX, RX
    LPUART_GetDefaultConfig(config);
    config->enableRx = true;
    config->enableTx = true;

    // FreeRTOS Task needs parameter data survive after this function finish, so write this parameter to Uart_PAL
    // structure.
    palUart->uartNum = uartNum;

    // Enable RX interrupts
    EnableIRQ((IRQn_Type)(19 + uartNum));
    NVIC_SetPriority((IRQn_Type)(19 + uartNum), UART_INTERRUPT_PRIO);

    // Initialize UART peripheral with default config
    status = LPUART_Init(base, config, GetSrcFreq());
    if (status != kStatus_Success)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_IO);
    }

    // Create idle task waiting for read/write.
    xReturned = xTaskCreate(
        vREvent,
        "UART Read Event",
        configMINIMAL_STACK_SIZE,
        (void *)palUart,
        configMAX_PRIORITIES - 1,
        &palUart->xRTaskToNotify);
    if (xReturned == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    xReturned = xTaskCreate(
        vWEvent,
        "UART Read Event",
        configMINIMAL_STACK_SIZE,
        (void *)palUart,
        configMAX_PRIORITIES,
        &palUart->xWTaskToNotify);

    if (xReturned == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeConfig___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    status_t status = 0;
    uint8_t uartNum = 0;
    lpuart_config_t *config = NULL;
    LPUART_Type *base = NULL;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    uartNum = pThis[FIELD___portIndex].NumericByRef().s4;
    config = &Uart_PAL[uartNum]->uartCfg;
    base = lpuart_bases[uartNum];

    if (uartNum >= (sizeof(Uart_PAL) / sizeof(Uart_PAL[0])) || config == NULL || base == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_IO);
    }

    // Check RS485 mode is not selected as currently not supported
    if ((SerialMode)pThis[FIELD___mode].NumericByRef().s4 != SerialMode_Normal)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_NOTIMPL);
    }

    config->baudRate_Bps = (uint32_t)pThis[FIELD___baudRate].NumericByRef().s4;

    switch (pThis[FIELD___dataBits].NumericByRef().s4)
    {
        default:
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            break;
        case 7:
            config->dataBitsCount = kLPUART_SevenDataBits;
            break;
        case 8:
            config->dataBitsCount = kLPUART_EightDataBits;
            break;
    }

    switch (pThis[FIELD___parity].NumericByRef().s4)
    {
        default:
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            break;
        case Parity_None:
            config->parityMode = kLPUART_ParityDisabled;
            break;
        case Parity_Even:
            config->parityMode = kLPUART_ParityEven;
            break;
        case Parity_Odd:
            config->parityMode = kLPUART_ParityOdd;
            break;
    }

    switch ((StopBits)pThis[FIELD___stopBits].NumericByRef().s4)
    {
        default:
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            break;
        case StopBits_One:
            config->stopBitCount = kLPUART_OneStopBit;
            break;
        case StopBits_Two:
            config->stopBitCount = kLPUART_TwoStopBit;
            break;
    }

    // write config to UART peripheral
    status = LPUART_Init(base, config, GetSrcFreq());
    if (status != kStatus_Success)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_IO);
    }

    // Enable interrupts after setting configuration
    // Disable transmitter and receiver
    base->CTRL &= ~(1U << 19);
    base->CTRL &= ~(1U << 18);
    // Enable receiver interrupt
    base->CTRL |= 1U << LPUART_CTRL_RIE_SHIFT;
    // Enable receiver ovverun interrupt
    base->CTRL |= 1U << LPUART_CTRL_ORIE_SHIFT;
    // Renable transmitter and receiver
    base->CTRL |= 1U << 19;
    base->CTRL |= 1U << 18;

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

    // Choose the driver for this SerialDevice
    uartNum = (uint8_t)pThis[FIELD___portIndex].NumericByRef().s4;

    // Quit if parameters or device is invalid or out of range
    if (uartNum >= (sizeof(Uart_PAL) / sizeof(Uart_PAL[0])))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    palUart = Uart_PAL[uartNum];

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

    // Choose the driver for this SerialDevice
    uartNum = (uint8_t)pThis[FIELD___portIndex].NumericByRef().s4;

    // Quit if parameters or device is invalid or out of range
    if (uartNum >= (sizeof(Uart_PAL) / sizeof(Uart_PAL[0])))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    palUart = Uart_PAL[uartNum];

    if (stack.Arg1().RecoverString() == NULL)
    {
        // text string it's empty so there is noting to do here
        stack.SetResult_U4(0);
        NANOCLR_SET_AND_LEAVE(S_OK);
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

        // Set transfer structure to nano ring buffer
        palUart->xfer.data = (uint8_t *)palUart->TxBuffer;
        palUart->xfer.dataSize = bufferLength;

        // Notify task that we want to transmit data.
        xTaskNotify(palUart->xWTaskToNotify, 0x01, eSetBits);

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

            // reset Tx ongoing count
            palUart->TxOngoingCount = 0;

            // done here
            break;
        }
        else
        {
            palUart->TxOngoingCount = 0;

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

    char deviceSelectorString[41] = {0};
    int len = 0;
    for (uint8_t i = 1; i < (sizeof(Uart_PAL) / sizeof(Uart_PAL[0])); i++)
    {
        if (Uart_PAL[i] != NULL)
        {
            char com[6] = "COM1,";
            com[3] = i + '0';
            len += 5;
            strcat(deviceSelectorString, com);
        }
    }
    // remove trailing comma
    deviceSelectorString[len - 1] = 0;

    // because the caller is expecting a result to be returned
    // we need set a return result in the stack argument using the appropriate SetResult according to the variable type
    // (a string here)
    stack.SetResult_String(deviceSelectorString);

    NANOCLR_NOCLEANUP_NOLABEL();
}
