//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include "win_dev_serial_native_target.h"
#include <Esp32_DeviceMapping.h>

// UART buffer size: 256 bytes
#define UART_BUFER_SIZE 256

// in UWP the COM ports are named COM1, COM2, COM3. But ESP32 uses internally UART0, UART1, UART2. This maps the port
// index 1, 2 or 3 to the uart number 0, 1 or 2
#define PORT_INDEX_TO_UART_NUM(portIndex) ((portIndex)-1)
// in UWP the COM ports are named COM1, COM2, COM3. But ESP32 uses internally UART0, UART1, UART2. This maps the uart
// number 0, 1 or 2 to the port index 1, 2 or 3
#define UART_NUM_TO_PORT_INDEX(uart_num) ((uart_num) + 1)

static const char *TAG = "SerialDevice";

static NF_PAL_UART Uart0_PAL;
static NF_PAL_UART Uart1_PAL;
static NF_PAL_UART Uart2_PAL;

NF_PAL_UART *GetPalUartFromUartNum(int uart_num)
{
    NF_PAL_UART *palUart = NULL;

    switch (uart_num)
    {
        case UART_NUM_0:
            // set UART PAL
            palUart = &Uart0_PAL;
            break;

        case UART_NUM_1:
            // set UART PAL
            palUart = &Uart1_PAL;
            break;

        case UART_NUM_2:
            // set UART PAL
            palUart = &Uart2_PAL;
            break;

        default:
            break;
    }
    return palUart;
}

void UnitializePalUart(NF_PAL_UART *palUart)
{
    if (palUart && palUart->SerialDevice)
    {
        // send the exit signal to the UART event handling queue
        uart_event_t event;
        event.type = UART_EVENT_MAX;
        xQueueSend(palUart->UartEventQueue, &event, (portTickType)0);

        // free buffers meory
        platform_free(palUart->RxBuffer);
        platform_free(palUart->TxBuffer);

        // null all pointers
        palUart->RxBuffer = NULL;
        palUart->TxBuffer = NULL;
        palUart->SerialDevice = NULL;

        // delete driver
        uart_driver_delete((uart_port_t)palUart->UartNum);

        // delete task, if any
        if (palUart->UartEventTask)
        {
            vTaskDelete(palUart->UartEventTask);
        }

        palUart->UartEventTask = NULL;
    }
}

void Esp32_Serial_UnitializeAll()
{
    for (int uart_num = 0; uart_num < UART_NUM_MAX; uart_num++)
    {
        // free buffers memory
        UnitializePalUart(GetPalUartFromUartNum(uart_num));
    }
}

void uart_event_task(void *pvParameters)
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
        if (xQueueReceive(palUart->UartEventQueue, &event, (portTickType)portMAX_DELAY))
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
                        readCount = uart_read_bytes(palUart->UartNum, buffer, bufferedSize, 1);

                        // push to UART RX buffer
                        palUart->RxRingBuffer.Push(buffer, readCount);

                        // free buffer
                        platform_free(buffer);

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
                        else
                        {
                            // no read operation ongoing, so fire an event

                            // post a managed event with the port index and event code (check if there is a watch char
                            // in the buffer or just any char)
                            //  check if callbacks are registered so this is called only if there is anyone listening
                            //  otherwise don't bother
                            if (palUart
                                    ->SerialDevice
                                        [Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::
                                             FIELD___callbacksDataReceivedEvent]
                                    .Dereference() != NULL)
                            {
                                PostManagedEvent(
                                    EVENT_SERIAL,
                                    0,
                                    UART_NUM_TO_PORT_INDEX(palUart->UartNum),
                                    (watchCharPos > -1) ? SerialData_WatchChar : SerialData_Chars);
                            }
                        }
                    }
                }
            }
        }
    }

    vTaskDelete(NULL);
}

void UartTxWorkerTask(void *pvParameters)
{
    // get PAL UART from task parameters
    NF_PAL_UART *palUart = (NF_PAL_UART *)pvParameters;

    // Write data directly to UART FIFO
    // by design: don't bother checking the return value
    uart_write_bytes(palUart->UartNum, (const char *)palUart->TxRingBuffer.Reader(), palUart->TxOngoingCount);

    // pop elements from ring buffer, just pop
    palUart->TxRingBuffer.Pop(palUart->TxOngoingCount);

    // set event flag for COM OUT
    Events_Set(SYSTEM_EVENT_FLAG_COM_OUT);

    // delete task
    vTaskDelete(NULL);
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

HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeDispose___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uart_port_t uart_num;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Get Uart number for serial device
    uart_num = (uart_port_t)PORT_INDEX_TO_UART_NUM(pThis[FIELD___portIndex].NumericByRef().s4);

    UnitializePalUart(GetPalUartFromUartNum(uart_num));

    NANOCLR_NOCLEANUP();
}

//
//  Initialise a new Serial port
//
HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeInit___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    char task_name[16];
    uart_port_t uart_num;
    esp_err_t esp_err;

    NF_PAL_UART *palUart;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Get Uart number for serial device
    uart_num = (uart_port_t)PORT_INDEX_TO_UART_NUM(pThis[FIELD___portIndex].NumericByRef().s4);
    if (uart_num > UART_NUM_2 || uart_num < 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // call the configure and abort if not OK
    NANOCLR_CHECK_HRESULT(NativeConfig___VOID(stack));

    palUart = GetPalUartFromUartNum(uart_num);
    if (palUart == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // alloc buffers memory
    palUart->TxBuffer = (uint8_t *)platform_malloc(UART_BUFER_SIZE);
    palUart->RxBuffer = (uint8_t *)platform_malloc(UART_BUFER_SIZE);

    // sanity check
    if (palUart->TxBuffer == NULL || palUart->RxBuffer == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    // init buffers
    palUart->TxRingBuffer.Initialize(palUart->TxBuffer, UART_BUFER_SIZE);
    palUart->RxRingBuffer.Initialize(palUart->RxBuffer, UART_BUFER_SIZE);

    // set/reset all the rest
    palUart->SerialDevice = stack.This();
    palUart->UartNum = uart_num;
    palUart->TxOngoingCount = 0;
    palUart->RxBytesToRead = 0;

    // Install driver
    esp_err = uart_driver_install(
        uart_num,
        UART_BUFER_SIZE,            // rx_buffer_size,
        0,                          // tx_buffer_size, not buffered
        20,                         // queue_size
        &(palUart->UartEventQueue), // QueueHandle_t *uart_queue ( none for now )
        0                           // intr_alloc_flags
    );
    if (esp_err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to install uart driver");
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    NANOCLR_CHECK_HRESULT(NativeConfig___VOID(stack));

    // Create a task to handle UART event from ISR
    sprintf(task_name, "uart%d_events", uart_num);
    if (xTaskCreatePinnedToCore(uart_event_task, task_name, 2048, palUart, 12, &(palUart->UartEventTask), 1) != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to start UART events task");
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    // Ensure driver gets unitialized during soft reboot
    HAL_AddSoftRebootHandler(Esp32_Serial_UnitializeAll);

    NANOCLR_NOCLEANUP();
}

//
//  Set up serial port Configuration
//
HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeConfig___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        uart_config_t uart_config;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // Get Uart number for serial device
        uart_port_t uart_num = (uart_port_t)PORT_INDEX_TO_UART_NUM(pThis[FIELD___portIndex].NumericByRef().s4);
        if (uart_num > UART_NUM_2 || uart_num < 0)
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
        switch ((SerialParity)pThis[FIELD___parity].NumericByRef().s4)
        {
            default:
            case SerialParity_None:
                uart_config.parity = UART_PARITY_DISABLE;
                break;

            case SerialParity_Even:
                uart_config.parity = UART_PARITY_EVEN;
                break;

            case SerialParity_Odd:
                uart_config.parity = UART_PARITY_ODD;
                break;
        }

        // stop bits
        switch ((SerialStopBitCount)pThis[FIELD___stopBits].NumericByRef().s4)
        {
            default:
            case SerialStopBitCount_One:
                uart_config.stop_bits = UART_STOP_BITS_1;
                break;

            case SerialStopBitCount_OnePointFive:
                uart_config.stop_bits = UART_STOP_BITS_1_5;
                break;

            case SerialStopBitCount_Two:
                uart_config.stop_bits = UART_STOP_BITS_2;
                break;
        }

        uart_config.rx_flow_ctrl_thresh = 120;

        bool EnableXonXoff = false;
        switch ((SerialHandshake)pThis[FIELD___handshake].NumericByRef().s4)
        {
            default:
            case SerialHandshake_None:
                uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
                break;

            case SerialHandshake_RequestToSend:
                uart_config.flow_ctrl = UART_HW_FLOWCTRL_RTS;
                uart_config.rx_flow_ctrl_thresh = 122;
                break;

            case SerialHandshake_RequestToSendXOnXOff:
                uart_config.flow_ctrl = UART_HW_FLOWCTRL_RTS;
                uart_config.rx_flow_ctrl_thresh = 122;
                EnableXonXoff = true;
                break;

            case SerialHandshake_XOnXOff:
                EnableXonXoff = true;
                uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
                break;
        }

        // Already Initialised ?
        if (GetPalUartFromUartNum(uart_num)->SerialDevice)
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

        // Don't use RTS/CTS if no hardware handshake enabled
        if (uart_config.flow_ctrl == UART_HW_FLOWCTRL_DISABLE)
        {
            rtsPin = UART_PIN_NO_CHANGE;
            ctsPin = UART_PIN_NO_CHANGE;
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

//
//  Write data into buffer
//
HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeWrite___VOID__SZARRAY_U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        NF_PAL_UART *palUart = NULL;

        uint8_t *data;
        size_t length = 0;

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
        length = (size_t)dataBuffer->m_numOfElements;

        // Get Uart number for serial device
        uart_port_t uart_num = (uart_port_t)PORT_INDEX_TO_UART_NUM(pThis[FIELD___portIndex].NumericByRef().s4);

        // get pointer to PAL UART
        palUart = GetPalUartFromUartNum(uart_num);
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
        CLR_RT_HeapBlock *outputStream = pThis[FIELD___outputStream].Dereference();
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

//
// Store - Send buffer and wait
//
HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeStore___U4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_PAL_UART *palUart = NULL;

    uint32_t estimatedDurationMiliseconds;
    size_t length = 0;
    uart_port_t uart_num;

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

    // Get Uart number for serial device
    uart_num = (uart_port_t)PORT_INDEX_TO_UART_NUM(pThis[FIELD___portIndex].NumericByRef().s4);

    // get pointer to PAL UART
    palUart = GetPalUartFromUartNum(uart_num);
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

                // Create a task to handle UART event from ISR
                char task_name[16];
                sprintf(task_name, "uart%d_tx", uart_num);

                if (xTaskCreate(UartTxWorkerTask, task_name, 2048, palUart, 12, NULL) != pdPASS)
                {
                    ESP_LOGE(TAG, "Failed to start UART TX task");
                    NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
                }

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
            uart_write_bytes(uart_num, (const char *)palUart->TxRingBuffer.Reader(), length);

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

    // null pointers and vars
    pThis = NULL;

    NANOCLR_NOCLEANUP();
}

HRESULT IRAM_ATTR
    Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeRead___U4__SZARRAY_U1__I4__I4(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array *dataBuffer;
    NF_PAL_UART *palUart = NULL;

    uint8_t *data;
    size_t dataLength = 0;

    size_t count = 0;
    size_t bytesRead = 0;
    size_t bytesToRead = 0;

    InputStreamOptions options = InputStreamOptions_None;
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

    // Get Uart number for serial device
    uart_num = (uart_port_t)PORT_INDEX_TO_UART_NUM(pThis[FIELD___portIndex].NumericByRef().s4);

    // get pointer to PAL UART
    palUart = GetPalUartFromUartNum(uart_num);
    if (palUart == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // setup timeout from the _readtimeout heap block
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTimeSpan(pThis[FIELD___readTimeout], timeoutTicks));

    // figure out what's available in the Rx ring buffer
    if (palUart->RxRingBuffer.Length() >= count)
    {
        // read from Rx ring buffer
        bytesToRead = count;

        // is the read ahead option enabled?
        if (options == InputStreamOptions_ReadAhead)
        {
            // yes
            // check how many bytes we can store in the buffer argument
            if (dataLength < palUart->RxRingBuffer.Length())
            {
                // read as many bytes has the buffer can hold
                bytesToRead = dataLength;
            }
            else
            {
                // read everything that's available in the ring buffer
                bytesToRead = palUart->RxRingBuffer.Length();
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
                    if (dataLength < palUart->RxRingBuffer.Length())
                    {
                        // read as many bytes has the buffer can hold
                        bytesToRead = dataLength;
                    }
                    else
                    {
                        // read everything that's available in the ring buffer
                        bytesToRead = palUart->RxRingBuffer.Length();
                    }
                }
                else
                {
                    // take InputStreamOptions_Partial as default and read requested quantity or what's available
                    bytesToRead = count;

                    if (count > palUart->RxRingBuffer.Length())
                    {
                        // need to adjust because there aren't enough bytes available
                        bytesToRead = palUart->RxRingBuffer.Length();
                    }
                }
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

HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeSetWatchChar___VOID(
    CLR_RT_StackFrame &stack)
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
    uart_enable_pattern_det_intr(uart_num, watchChar, 1, 10000, 10, 10);
    // Reset the pattern queue length to record at most 10 pattern positions.
    uart_pattern_queue_reset(uart_num, 10);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::get_BytesToRead___U4(
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

    // Get Uart number for serial device
    uart_num = (uart_port_t)PORT_INDEX_TO_UART_NUM(pThis[FIELD___portIndex].NumericByRef().s4);

    // get pointer to PAL UART
    palUart = GetPalUartFromUartNum(uart_num);
    if (palUart == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // return how many bytes can be read from the Rx buffer
    stack.SetResult_U4(palUart->RxRingBuffer.Length());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::
    GetDeviceSelector___STATIC__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        // declare the device selector string whose max size is "COM1,COM2,COM3" + terminator
        // COM1 is being used for VS debug, so it's not available
        char deviceSelectorString[15] = "COM2,COM3";

        // because the caller is expecting a result to be returned
        // we need set a return result in the stack argument using the appropriate SetResult according to the variable
        // type (a string here)
        stack.SetResult_String(deviceSelectorString);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
