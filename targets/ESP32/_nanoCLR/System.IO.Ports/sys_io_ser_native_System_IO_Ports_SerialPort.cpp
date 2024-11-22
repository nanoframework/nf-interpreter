//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoHAL_Time.h>
#include "sys_io_ser_native_target.h"
#include <Esp32_DeviceMapping.h>
#include <esp32_idf.h>

// in UWP the COM ports are named COM1, COM2, COM3. But ESP32 uses internally UART0, UART1, UART2. This maps the port
// index 1, 2 or 3 to the uart number 0, 1 or 2
#define PORT_INDEX_TO_UART_NUM(portIndex) ((portIndex) - 1)
// in UWP the COM ports are named COM1, COM2, COM3. But ESP32 uses internally UART0, UART1, UART2. This maps the uart
// number 0, 1 or 2 to the port index 1, 2 or 3
#define UART_NUM_TO_PORT_INDEX(uart_num) ((uart_num) + 1)

static const char *TAG = "SerialDevice";

NF_PAL_UART Uart0_PAL;
NF_PAL_UART Uart1_PAL;
#if SOC_UART_HP_NUM > 2
NF_PAL_UART Uart2_PAL;
#endif
#if SOC_UART_HP_NUM > 3
NF_PAL_UART Uart3_PAL;
#endif

NF_PAL_UART *GetPalUartFromUartNum_sys(int uart_num)
{
    switch (uart_num)
    {
        case UART_NUM_0:
            // set UART PAL
            return &Uart0_PAL;

        case UART_NUM_1:
            // set UART PAL
            return &Uart1_PAL;

#if SOC_UART_HP_NUM > 2
        case UART_NUM_2:
            // set UART PAL
            return &Uart2_PAL;
#endif

#if SOC_UART_HP_NUM > 3
        case UART_NUM_2:
            // set UART PAL
            return &Uart3_PAL;
#endif

        default:
            break;
    }

    return NULL;
}

void UninitializePalUart_sys(NF_PAL_UART *palUart)
{
    if (palUart && palUart->UartEventTask)
    {
        // send the exit signal to the UART event handling queue
        uart_event_t event;
        event.type = UART_EVENT_MAX;
        xQueueSend(palUart->UartEventQueue, &event, 0);
        palUart->UartEventTask = NULL;

        // free buffer memory
        platform_free(palUart->RxBuffer);

        // null all pointers
        palUart->RxBuffer = NULL;
        palUart->TxBuffer = NULL;

        // delete driver
        uart_driver_delete((uart_port_t)palUart->UartNum);
    }
}

void Esp32_Serial_UninitializeAll_sys()
{
    for (int uart_num = 0; uart_num < UART_NUM_MAX; uart_num++)
    {
        // free buffers memory
        UninitializePalUart_sys(GetPalUartFromUartNum_sys(uart_num));
    }
}

void uart_event_task_sys(void *pvParameters)
{
    // get PAL UART from task parameters
    NF_PAL_UART *palUart = (NF_PAL_UART *)pvParameters;

    uart_event_t event;
    bool run = true;
    bool readData;
    int32_t watchCharPos;
    int32_t readCount;
    size_t bufferedSize;
    uint8_t *buffer;

    while (run)
    {
        // Waiting for UART event.
        if (xQueueReceive(palUart->UartEventQueue, &event, portMAX_DELAY))
        {
            // reset vars
            readData = false;
            watchCharPos = -1;

            // deal with event type
            switch (event.type)
            {
                case UART_DATA:
                    // set flag
                    readData = true;
                    break;

                case UART_PATTERN_DET:
                    // Pattern detection used for the WatchChar
                    watchCharPos = uart_pattern_get_pos(palUart->UartNum);

                    if (watchCharPos == 0)
                    {
                        // patter position has 0 index meaning that we are reading it now
                        // pop it from the pattern queue
                        uart_pattern_pop_pos(palUart->UartNum);
                    }

                    // set flag
                    readData = true;
                    break;

                case UART_FIFO_OVF:
                case UART_BUFFER_FULL:
                    uart_flush_input(palUart->UartNum);
                    xQueueReset(palUart->UartEventQueue);
                    break;

                case UART_EVENT_MAX:
                    // signal to end the task (UART_EVENT_MAX used)
                    run = false;
                    break;

                default:
                    break;
            }

            if (readData)
            {
                // get how many chars are buffered on RX FIFO
                uart_get_buffered_data_len(palUart->UartNum, &bufferedSize);

                if (bufferedSize > 0)
                {
                    // alloc buffer to read RX FIFO
                    buffer = (uint8_t *)platform_malloc(bufferedSize);

                    // sanity check
                    if (buffer)
                    {
                        // try to read RX FIFO
                        readCount = uart_read_bytes(palUart->UartNum, buffer, bufferedSize, portMAX_DELAY);

                        // push to UART RX buffer
                        palUart->RxRingBuffer.Push(buffer, readCount);

                        // is there a read operation going on?
                        if (palUart->RxBytesToRead > 0)
                        {
                            // yes
                            // check if the requested bytes are available in the buffer...
                            //... or if the watch char was received
                            if ((palUart->RxRingBuffer.Length() >= palUart->RxBytesToRead) || (watchCharPos > -1))
                            {
                                // reset Rx bytes to read count
                                palUart->RxBytesToRead = 0;

                                // fire event for Rx buffer complete
                                Events_Set(SYSTEM_EVENT_FLAG_COM_IN);
                            }
                        }
                        else if (palUart->NewLineChar > 0)
                        {
                            // try to find the new line char we're waiting for
                            do
                            {
                                if (buffer[readCount - 1] == palUart->NewLineChar)
                                {
                                    // fire event for new line char found
                                    Events_Set(SYSTEM_EVENT_FLAG_COM_IN);

                                    // done here
                                    break;
                                }
                            } while (--readCount >= 0);
                        }
                        else
                        {
                            // no read operation ongoing, so fire an event, if the available bytes are above the
                            // threshold
                            if (palUart->RxRingBuffer.Length() >= palUart->ReceivedBytesThreshold)
                            {
                                // post a managed event with the port index and event code (check if there is a watch
                                // char in the buffer or just any char)
                                // TODO: check if callbacks are registered so this is called only if there is anyone
                                // listening otherwise don't bother
                                PostManagedEvent(
                                    EVENT_SERIAL,
                                    0,
                                    UART_NUM_TO_PORT_INDEX(palUart->UartNum),
                                    (watchCharPos > -1) ? SerialData_WatchChar : SerialData_Chars);
                            }
                        }

                        // free buffer
                        platform_free(buffer);
                    }
                }
            }
        }
    }

    vTaskDelete(NULL);
}

void UartTxWorkerTask_sys(void *pvParameters)
{
    // get PAL UART from task parameters
    NF_PAL_UART *palUart = (NF_PAL_UART *)pvParameters;

    // Write data directly to UART FIFO
    // by design: don't bother checking the return value
    uart_write_bytes(palUart->UartNum, (const char *)palUart->TxBuffer, palUart->TxOngoingCount);

    // set event flag for COM OUT
    Events_Set(SYSTEM_EVENT_FLAG_COM_OUT);

    // delete task
    vTaskDelete(NULL);
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::get_BytesToRead___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_PAL_UART *palUart = NULL;
    uart_port_t uart_num;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    // Get Uart number for serial device
    uart_num = (uart_port_t)PORT_INDEX_TO_UART_NUM(pThis[FIELD___portIndex].NumericByRef().s4);

    // get pointer to PAL UART
    palUart = GetPalUartFromUartNum_sys(uart_num);
    if (palUart == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // return how many bytes can be read from the Rx buffer
    stack.SetResult_U4(palUart->RxRingBuffer.Length());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::get_InvertSignalLevels___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_PAL_UART *palUart = NULL;
    uart_port_t uart_num;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    // Get UART number for serial device
    uart_num = (uart_port_t)PORT_INDEX_TO_UART_NUM(pThis[FIELD___portIndex].NumericByRef().s4);

    // get pointer to PAL UART
    palUart = GetPalUartFromUartNum_sys(uart_num);
    if (palUart == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    stack.SetResult_Boolean(palUart->SignalLevelsInverted);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::set_InvertSignalLevels___VOID__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_PAL_UART *palUart = NULL;
    uart_port_t uart_num;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    // Get UART number for serial device
    uart_num = (uart_port_t)PORT_INDEX_TO_UART_NUM(pThis[FIELD___portIndex].NumericByRef().s4);

    // get pointer to PAL UART
    palUart = GetPalUartFromUartNum_sys(uart_num);
    if (palUart == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // check if UART it's already opened
    if (palUart->UartEventTask)
    {
        // it is opened, so we can't change the signal levels
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    palUart->SignalLevelsInverted = (bool)stack.Arg1().NumericByRef().u1;

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
    size_t count = 0;
    size_t bytesRead = 0;
    size_t bytesToRead = 0;
    size_t offset = 0;
    uart_port_t uart_num;

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

    // Get Uart number for serial device
    uart_num = (uart_port_t)PORT_INDEX_TO_UART_NUM(pThis[FIELD___portIndex].NumericByRef().s4);

    // get pointer to PAL UART
    palUart = GetPalUartFromUartNum_sys(uart_num);
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
    uart_port_t uart_num;

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

    // Get Uart number for serial device
    uart_num = (uart_port_t)PORT_INDEX_TO_UART_NUM(pThis[FIELD___portIndex].NumericByRef().s4);

    // get pointer to PAL UART
    palUart = GetPalUartFromUartNum_sys(uart_num);
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
    uart_port_t uart_num;

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

    // Get Uart number for serial device
    uart_num = (uart_port_t)PORT_INDEX_TO_UART_NUM(pThis[FIELD___portIndex].NumericByRef().s4);

    // get pointer to PAL UART
    palUart = GetPalUartFromUartNum_sys(uart_num);
    if (palUart == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    if (stack.m_customState == 1)
    {
        // check if there is a full line available to read
        GLOBAL_LOCK();
        newLineFound = GetLineFromRxBuffer(pThis, &(palUart->RxRingBuffer), line);
        GLOBAL_UNLOCK();

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

    int32_t length = 0;
    uart_port_t uart_num;

    uint8_t *data;
    int32_t count = 0;
    int32_t offset = 0;

    int64_t *timeoutTicks;
    bool eventResult = true;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    // Get Uart number for serial device
    uart_num = (uart_port_t)PORT_INDEX_TO_UART_NUM(pThis[FIELD___portIndex].NumericByRef().s4);

    // get pointer to PAL UART
    palUart = GetPalUartFromUartNum_sys(uart_num);
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

        // Try to send buffer to fifo first.
        // if not all data written then use long running operation to complete.
        palUart->IsLongRunning = false;
        int txCount = uart_tx_chars(uart_num, (const char *)data, count);
        if (txCount < count)
        {
            palUart->IsLongRunning = true;
            if (txCount >= 0)
            {
                // Any written then update ptr / count
                count -= txCount;
                data += txCount;
            }
        }

        if (palUart->IsLongRunning)
        {
            hbTimeout.SetInteger(
                (CLR_INT64)pThis[FIELD___writeTimeout].NumericByRef().s4 * TIME_CONVERSION__TO_MILLISECONDS);
            // setup timeout
            NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeoutTicks));

            // this is a long running operation and

            // push to the stack how many bytes bytes where buffered for Tx
            stack.PushValueI4(count);

            // store pointer
            palUart->TxBuffer = data;

            // set TX count
            palUart->TxOngoingCount = count;

            // Create a task to handle UART event from ISR
            char task_name[16];
            snprintf(task_name, ARRAYSIZE(task_name), "uart%d_tx", uart_num);

            if (xTaskCreate(UartTxWorkerTask_sys, task_name, 2048, palUart, 12, NULL) != pdPASS)
            {
                ESP_LOGE(TAG, "Failed to start UART TX task");
                NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
            }

            // bump custom state so the read value above is pushed only once
            stack.m_customState = 2;
        }
    }

    /////////////////////////////
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

    // null pointers and vars
    pThis = NULL;

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeDispose___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uart_port_t uart_num;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Get Uart number for serial device
    uart_num = (uart_port_t)PORT_INDEX_TO_UART_NUM(pThis[FIELD___portIndex].NumericByRef().s4);

    UninitializePalUart_sys(GetPalUartFromUartNum_sys(uart_num));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    char task_name[16];
    uart_port_t uart_num;
    esp_err_t esp_err;
    int32_t bufferSize;
    uint8_t watchChar;

    NF_PAL_UART *palUart;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Get Uart number for serial device
    uart_num = (uart_port_t)PORT_INDEX_TO_UART_NUM(pThis[FIELD___portIndex].NumericByRef().s4);
    if (uart_num > UART_NUM_MAX || uart_num < 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // unless the build is configure to use USB CDC, COM1 is being used for VS debug, so it's not available
#if !defined(CONFIG_TINYUSB_CDC_ENABLED) && !defined(CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG_ENABLED)
    if (uart_num == 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }
#endif

    palUart = GetPalUartFromUartNum_sys(uart_num);
    if (palUart == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // If driver already installed then clean up first
    // Open/Close/Open without dispose
    if (uart_is_driver_installed(uart_num))
    {
        UninitializePalUart_sys(palUart);
    }

    // call the configure and abort if not OK
    NANOCLR_CHECK_HRESULT(NativeConfig___VOID(stack));

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

    // set/reset all the rest
    palUart->UartNum = uart_num;
    palUart->TxOngoingCount = 0;
    palUart->RxBytesToRead = 0;
    palUart->NewLineChar = 0;

    // Install driver
    esp_err = uart_driver_install(
        uart_num,
        // rx_buffer_size
        bufferSize,
        // tx_buffer_size, not buffered
        0,
        // queue_size
        20,
        &(palUart->UartEventQueue),
        ESP_INTR_FLAG_IRAM);
    if (esp_err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to install uart driver");
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    NANOCLR_CHECK_HRESULT(NativeConfig___VOID(stack));

    // Get the watch char
    watchChar = (uint8_t)pThis[FIELD___watchChar].NumericByRef().u1;

    // set watch char, if set
    if (watchChar != 0)
    {
        // Enable pattern detection for the serial device
        uart_enable_pattern_det_baud_intr(uart_num, watchChar, 1, 9, 0, 00);
        // Reset the pattern queue length to record at most 10 pattern positions.
        uart_pattern_queue_reset(uart_num, 10);
    }

    // Create a task to handle UART event from ISR
    snprintf(task_name, ARRAYSIZE(task_name), "uart%d_events", uart_num);
    if (xTaskCreate(uart_event_task_sys, task_name, 2048, palUart, 12, &(palUart->UartEventTask)) != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to start UART events task");
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    // Ensure driver gets uninitialized during soft reboot
    HAL_AddSoftRebootHandler(Esp32_Serial_UninitializeAll_sys);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeConfig___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        uart_config_t uart_config = {};

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // Init clock source (added in IDF 5.x)
        uart_config.source_clk = UART_SCLK_DEFAULT;

        // Get Uart number for serial device
        uart_port_t uart_num = (uart_port_t)PORT_INDEX_TO_UART_NUM(pThis[FIELD___portIndex].NumericByRef().s4);
        if (uart_num > UART_NUM_MAX || uart_num < 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // setup configuration
        // baud rate
        uart_config.baud_rate = (uint32_t)pThis[FIELD___baudRate].NumericByRef().s4;

        // data bits
        switch ((uint16_t)pThis[FIELD___dataBits].NumericByRef().s4)
        {
            case 5:
                uart_config.data_bits = UART_DATA_5_BITS;
                break;

            case 6:
                uart_config.data_bits = UART_DATA_6_BITS;
                break;

            case 7:
                uart_config.data_bits = UART_DATA_7_BITS;
                break;

            case 8:
                uart_config.data_bits = UART_DATA_8_BITS;
                break;

            default:
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // parity
        switch ((Parity)pThis[FIELD___parity].NumericByRef().s4)
        {
            default:
            case Parity_None:
                uart_config.parity = UART_PARITY_DISABLE;
                break;

            case Parity_Even:
                uart_config.parity = UART_PARITY_EVEN;
                break;

            case Parity_Odd:
                uart_config.parity = UART_PARITY_ODD;
                break;
        }

        // stop bits
        switch ((StopBits)pThis[FIELD___stopBits].NumericByRef().s4)
        {
            default:
            case StopBits_One:
                uart_config.stop_bits = UART_STOP_BITS_1;
                break;

            case StopBits_OnePointFive:
                uart_config.stop_bits = UART_STOP_BITS_1_5;
                break;

            case StopBits_Two:
                uart_config.stop_bits = UART_STOP_BITS_2;
                break;
        }

        uart_config.rx_flow_ctrl_thresh = 120;

        bool EnableXonXoff = false;
        switch ((Handshake)pThis[FIELD___handshake].NumericByRef().s4)
        {
            default:
            case Handshake_None:
                uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
                break;

            case Handshake_RequestToSend:
                uart_config.flow_ctrl = UART_HW_FLOWCTRL_RTS;
                uart_config.rx_flow_ctrl_thresh = 122;
                break;

            case Handshake_RequestToSendXOnXOff:
                uart_config.flow_ctrl = UART_HW_FLOWCTRL_RTS;
                uart_config.rx_flow_ctrl_thresh = 122;
                EnableXonXoff = true;
                break;

            case Handshake_XOnXOff:
                EnableXonXoff = true;
                uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
                break;
        }

        bool rs485Mode = ((SerialMode)pThis[FIELD___mode].NumericByRef().s4 == SerialMode_RS485);
        if (rs485Mode)
        {
            // Disable any flow control & Set RS485 half duplex mode
            uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
            uart_set_mode(uart_num, UART_MODE_RS485_HALF_DUPLEX);
        }
        else
        {
            // Reset to normal mode
            uart_set_mode(uart_num, UART_MODE_UART);
        }

        // Already Initialised ?
        if (GetPalUartFromUartNum_sys(uart_num)->UartEventTask)
        {
            int errors = 0;

            errors += uart_set_word_length(uart_num, uart_config.data_bits);
            errors += uart_set_baudrate(uart_num, uart_config.baud_rate);
            errors += uart_set_parity(uart_num, uart_config.parity);
            errors += uart_set_stop_bits(uart_num, uart_config.stop_bits);
            errors += uart_set_hw_flow_ctrl(uart_num, uart_config.flow_ctrl, uart_config.rx_flow_ctrl_thresh);
            if (errors)
            {
                ESP_LOGE(TAG, "Failed to set UART parameters configuration");
                NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
            }
        }
        else
        {
            // First time make sure UART is reset so use uart_param_config.
            // If you call this once driver installed it resets UART and stop events ISR being called.
            if (uart_param_config(uart_num, &uart_config) != ESP_OK)
            {
                ESP_LOGE(TAG, "Failed to set UART parameters configuration");
                NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
            }

            ////////////////////////////////////////////////////////////////////////////
            // signal level inversion can only be configured when UART is not running //
            ////////////////////////////////////////////////////////////////////////////

            // default is to not invert
            uart_signal_inv_t inversionMmask = UART_SIGNAL_INV_DISABLE;

            // get pointer to PAL UART
            NF_PAL_UART *palUart = GetPalUartFromUartNum_sys(uart_num);
            if (palUart == NULL)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            }

            // check if signal levels are to be inverted
            if (palUart->SignalLevelsInverted)
            {
                inversionMmask = (uart_signal_inv_t)(UART_SIGNAL_RXD_INV | UART_SIGNAL_TXD_INV);
            }

            // config signal inversion (or not)
            if (uart_set_line_inverse(uart_num, inversionMmask) != ESP_OK)
            {
                ESP_LOGE(TAG, "Failed to set UART parameters configuration");
                NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
            }
        }

        if (EnableXonXoff)
        {
            uart_set_sw_flow_ctrl(uart_num, true, 20, 40);
        }

        // Map to currently assigned pins
        int txPin = Esp32_GetMappedDevicePins(DEV_TYPE_SERIAL, uart_num, Esp32SerialPin_Tx);
        int rxPin = Esp32_GetMappedDevicePins(DEV_TYPE_SERIAL, uart_num, Esp32SerialPin_Rx);
        int rtsPin = Esp32_GetMappedDevicePins(DEV_TYPE_SERIAL, uart_num, Esp32SerialPin_Rts);
        int ctsPin = Esp32_GetMappedDevicePins(DEV_TYPE_SERIAL, uart_num, Esp32SerialPin_Cts);

        // check if TX, RX pins have been previously set
        if (txPin == UART_PIN_NO_CHANGE || rxPin == UART_PIN_NO_CHANGE)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_PIN_UNAVAILABLE);
        }

        // Don't use RTS/CTS pins if no hardware handshake enabled unless in RS485 mode
        if (rs485Mode)
        {
            ctsPin = UART_PIN_NO_CHANGE; // no need for a CTS pin enabled, just RTS
        }
        else
        {
            if (uart_config.flow_ctrl == UART_HW_FLOWCTRL_DISABLE)
            {
                rtsPin = UART_PIN_NO_CHANGE;
                ctsPin = UART_PIN_NO_CHANGE;
            }
        }

        if (uart_set_pin(uart_num, txPin, rxPin, rtsPin, ctsPin) != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to set UART pins");
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }

        // null pointers and vars
        pThis = NULL;
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeSetWatchChar___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uart_port_t uart_num;
    uint8_t watchChar;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    // Get Uart number for serial device
    uart_num = (uart_port_t)PORT_INDEX_TO_UART_NUM(pThis[FIELD___portIndex].NumericByRef().s4);

    // Get the watch char
    watchChar = (uint8_t)pThis[FIELD___watchChar].NumericByRef().u1;

    // Enable pattern detection for the serial device
    uart_enable_pattern_det_baud_intr(uart_num, watchChar, 1, 9, 0, 00);
    // Reset the pattern queue length to record at most 10 pattern positions.
    uart_pattern_queue_reset(uart_num, 10);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeWriteString___VOID__STRING__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_PAL_UART *palUart = NULL;
    uart_port_t uart_num;

    CLR_RT_HeapBlock hbTimeout;
    int64_t *timeoutTicks;
    bool eventResult = true;

    bool isNewAllocation = false;
    char *buffer = NULL;
    const char *bufferPointer = NULL;
    uint32_t bufferLength;
    int32_t length = 0;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    // Get UART number for serial device
    uart_num = (uart_port_t)PORT_INDEX_TO_UART_NUM(pThis[FIELD___portIndex].NumericByRef().s4);

    // get pointer to PAL UART
    palUart = GetPalUartFromUartNum_sys(uart_num);
    if (palUart == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    if (stack.Arg1().RecoverString() == NULL)
    {
        // text string it's empty so there is noting to do here
        stack.SetResult_U4(0);
        NANOCLR_SET_AND_LEAVE(S_OK);
    }

    // perform parameter validation and setup TX operation
    if (stack.m_customState == 0)
    {
        // get buffer to output
        NANOCLR_CHECK_HRESULT(SetupWriteLine(stack, &buffer, &bufferLength, &isNewAllocation));

        // Try to send buffer to fifo first.
        // if not all data written then use long running operation to complete.
        palUart->IsLongRunning = false;

        // store pointer because it will be changed after this call
        bufferPointer = buffer;

        int txCount = uart_tx_chars(uart_num, (const char *)buffer, bufferLength);

        if (txCount < (int)bufferLength)
        {
            palUart->IsLongRunning = true;
            if (txCount >= 0)
            {
                // Any written then update ptr / count
                bufferLength -= txCount;
                bufferPointer += txCount;
            }
        }

        if (palUart->IsLongRunning)
        {
            // setup timeout
            hbTimeout.SetInteger(
                (CLR_INT64)pThis[FIELD___writeTimeout].NumericByRef().s4 * TIME_CONVERSION__TO_MILLISECONDS);
            NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeoutTicks));

            // store pointer
            palUart->TxBuffer = (uint8_t *)bufferPointer;

            // set TX count
            palUart->TxOngoingCount = bufferLength;

            // push onto the eval stack how many bytes are being pushed to the UART
            stack.PushValueI4(bufferLength);

            // push onto the eval stack if the buffer was allocated
            stack.PushValueI4(isNewAllocation ? 1 : 0);

            // push buffer pointer to eval stack
            // need this to release the buffer later, because the pointer is changed
            stack.PushValueU4((CLR_UINT32)&bufferPointer);

            // Create a task to handle UART event from ISR
            char task_name[16];
            snprintf(task_name, ARRAYSIZE(task_name), "uart%d_tx", uart_num);

            if (xTaskCreate(UartTxWorkerTask_sys, task_name, 2048, palUart, 12, NULL) != pdPASS)
            {
                ESP_LOGE(TAG, "Failed to start UART TX task");
                NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
            }

            // bump custom state so the read value above is pushed only once
            stack.m_customState = 2;
        }
    }

    while (eventResult)
    {
        if (!palUart->IsLongRunning)
        {
            // this is not a long running operation so nothing to do here
            break;
        }

        // non-blocking wait allowing other threads to run while we wait for the TX operation to complete
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeoutTicks, Event_SerialPortOut, eventResult));

        if (eventResult)
        {
            // event occurred

            // pop "buffer pointer" from the eval stack
            buffer = (char *)stack.m_evalStack[3].NumericByRef().u4;

            // pop "isNewAllocation" from the eval stack
            isNewAllocation = stack.m_evalStack[2].NumericByRef().s4 == 1 ? true : false;

            // get from the eval stack how many bytes were buffered to TX
            length = stack.m_evalStack[1].NumericByRef().s4;

            // reset TX ongoing count
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
        // pop "length" heap block from stack
        stack.PopValue();

        // pop "isNewAllocation" heap block from stack
        stack.PopValue();

        // pop "buffer" heap block from stack
        stack.PopValue();

        // pop "hbTimeout" heap block from stack
        stack.PopValue();
    }

    stack.SetResult_U4(length);

    // free memory, if it was allocated
    if (isNewAllocation && buffer != NULL)
    {
        platform_free(buffer);
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
    uart_port_t uart_num;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // check if threshold is valid
    threshold = (int32_t)stack.Arg1().NumericByRef().s4;

    if (threshold <= 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    // Get UART number for serial device
    uart_num = (uart_port_t)PORT_INDEX_TO_UART_NUM(pThis[FIELD___portIndex].NumericByRef().s4);

    // get pointer to PAL UART
    palUart = GetPalUartFromUartNum_sys(uart_num);
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
        PostManagedEvent(EVENT_SERIAL, 0, UART_NUM_TO_PORT_INDEX(palUart->UartNum), SerialData_Chars);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::GetDeviceSelector___STATIC__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    // declare the device selector string whose max size is "COM1,COM2,COM3" + terminator
    // and init with the terminator
    static char deviceSelectorString[] =

    // unless the build is configure to use USB CDC, COM1 is being used for VS debug, so it's not available
#if defined(CONFIG_TINYUSB_CDC_ENABLED) || defined(CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG_ENABLED)
        "COM1,"
#endif
#if SOC_UART_HP_NUM > 1
        "COM2,"
#endif
#if SOC_UART_HP_NUM > 2
        "COM3,"
#endif
#if SOC_UART_HP_NUM > 3
        "COM4,"
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
