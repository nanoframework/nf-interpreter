//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "win_dev_serial_native_target.h"

/////////////////////////////////////////////////////////
// UART PAL strucs delcared in win_dev_serial_native.h //
/////////////////////////////////////////////////////////

static LPUART_Type *const lpuart_bases[] = LPUART_BASE_PTRS;

// Static UART config/data structs. 
static NF_PAL_UART Uart_PAL1, Uart_PAL2, Uart_PAL3, Uart_PAL4, Uart_PAL5, Uart_PAL6, Uart_PAL7, Uart_PAL8;

// Array of pointers to above config UART structs. 
NF_PAL_UART *const Uart_PAL[]{NULL, &Uart_PAL1, &Uart_PAL2, &Uart_PAL3, &Uart_PAL4, &Uart_PAL5, &Uart_PAL6, &Uart_PAL7, &Uart_PAL8};

// Task firing up event after receiving wanted amount of bytes 
static void vREvent(void *pvParameters)
{
  // This is main UART struct, holding config, handlers, and the rest of variables.d 
  (void)pvParameters;
  uint32_t ulNotifiedValue;

  while (1) 
  {
    xTaskNotifyWait(pdFALSE,          // Don't clear bits on entry. 
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
    xTaskNotifyWait(pdFALSE,          // Don't clear bits on entry. 
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
    // don't care about the success of the operation, if it's full we are droping the char anyway 
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
  }

  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  NATIVE_INTERRUPT_END
}

// Override of the default MIMXRT1060 UART interrupt routines to simple UART_Handle function, which
// reads 1 byte of input data to RTOS stream buffer, if theres buffer overflow it drops data and clears interrupts.

extern "C" 
{
// LPUART8 is currently used for debugging, disable it or will collide with debugger 
// void LPUART1_IRQHandler(void) { UART_Handle(LPUART1, 1); }
void LPUART2_IRQHandler(void) { UART_Handle(LPUART2, 2); }
void LPUART3_IRQHandler(void) { UART_Handle(LPUART3, 3); }
void LPUART4_IRQHandler(void) { UART_Handle(LPUART4, 4); }
void LPUART5_IRQHandler(void) { UART_Handle(LPUART5, 5); }
void LPUART6_IRQHandler(void) { UART_Handle(LPUART6, 6); }
void LPUART7_IRQHandler(void) { UART_Handle(LPUART7, 7); }
void LPUART8_IRQHandler(void) { UART_Handle(LPUART8, 8); }
}

// Deinitialize serial port and allocated free memory 
HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeDispose___VOID(CLR_RT_StackFrame &stack) 
{
  NANOCLR_HEADER();
  {
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    uint8_t uartNum = 0;
    LPUART_Type *base = NULL;

    uartNum = pThis[FIELD___portIndex].NumericByRef().s4;
    base = lpuart_bases[uartNum];

    // Quit if parameters or device is invalid or out of range 
    if (uartNum >= (sizeof(Uart_PAL) / sizeof(Uart_PAL[0])) || base == NULL) 
    {
      NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    NF_PAL_UART *palUart = Uart_PAL[uartNum];

    // Free ring buffers memory 
    free(palUart->TxBuffer);
    free(palUart->RxBuffer);

    // Deinitialize device and delete FreeRTOS idle tasks 
    LPUART_Deinit(base);
    vTaskDelete(palUart->xRTaskToNotify);
    vTaskDelete(palUart->xWTaskToNotify);
  }
  NANOCLR_NOCLEANUP();
}

// Initialise a new Serial port, allocate buffer memory and create FreeRTOS idle tasks 
HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeInit___VOID(CLR_RT_StackFrame &stack) 
{
  NANOCLR_HEADER();
  {
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    status_t status = 0;
    uint8_t uartNum = 0;
    lpuart_config_t *config = NULL;
    LPUART_Type *base = NULL;

    uartNum = pThis[FIELD___portIndex].NumericByRef().s4;
    config = &Uart_PAL[uartNum]->uartCfg;
    base = lpuart_bases[uartNum];

    // Quit if parameters or device is invalid or out of range 
    if (uartNum >= (sizeof(Uart_PAL) / sizeof(Uart_PAL[0])) || config == NULL || base == NULL) 
    {
      NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    NF_PAL_UART *palUart = Uart_PAL[uartNum];

    // Allocate memory for TX and RX circular buffer 
    palUart->TxBuffer = (uint8_t *)malloc(UART_TX_BUFER_SIZE * sizeof(uint8_t));
    if (palUart->TxBuffer == NULL) 
    {
      NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    palUart->RxBuffer = (uint8_t *)malloc(UART_RX_BUFER_SIZE * sizeof(uint8_t));
    if (palUart->RxBuffer == NULL) 
    {
      free(palUart->TxBuffer);
      NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    // Initialize TX and RX buffer 
    palUart->TxRingBuffer.Initialize(palUart->TxBuffer, UART_TX_BUFER_SIZE);
    palUart->TxOngoingCount = 0;
    palUart->WatchChar = 0;
    palUart->RxRingBuffer.Initialize(palUart->RxBuffer, UART_RX_BUFER_SIZE);
    palUart->RxBytesToRead = 0;

    // Get default config structure for initializing given UART peripheral and enable TX, RX 
    LPUART_GetDefaultConfig(config);
    config->enableRx = true;
    config->enableTx = true;

    // FreeRTOS Task needs parameter data survive after this function finish, so write this parameter to Uart_PAL structure. 
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
    BaseType_t xReturned =
        xTaskCreate(vREvent, "UART Read Event", configMINIMAL_STACK_SIZE, (void *)palUart, configMAX_PRIORITIES - 1, &palUart->xRTaskToNotify);
    if (xReturned == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) 
    {
      NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    xReturned = xTaskCreate(vWEvent, "UART Read Event", configMINIMAL_STACK_SIZE, (void *)palUart, configMAX_PRIORITIES, &palUart->xWTaskToNotify);
    if (xReturned == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) 
    {
      NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }
  }
  NANOCLR_NOCLEANUP();
}

// Set up serial port Configuration 
HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeConfig___VOID(CLR_RT_StackFrame &stack) 
{
  NANOCLR_HEADER();
  {
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    status_t status = 0;
    uint8_t uartNum = 0;
    lpuart_config_t *config = NULL;
    LPUART_Type *base = NULL;

    uartNum = pThis[FIELD___portIndex].NumericByRef().s4;
    config = &Uart_PAL[uartNum]->uartCfg;
    base = lpuart_bases[uartNum];

    if (uartNum >= (sizeof(Uart_PAL) / sizeof(Uart_PAL[0])) || config == NULL || base == NULL) 
    {
      NANOCLR_SET_AND_LEAVE(CLR_E_IO);
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
    case SerialParity_None:
      config->parityMode = kLPUART_ParityDisabled;
      break;
    case SerialParity_Even:
      config->parityMode = kLPUART_ParityEven;
      break;
    case SerialParity_Odd:
      config->parityMode = kLPUART_ParityOdd;
      break;
    }

    switch (pThis[FIELD___stopBits].NumericByRef().s4) 
    {
    default:
      NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
      break;
    case SerialStopBitCount_One:
      config->stopBitCount = kLPUART_OneStopBit;
      break;
    case SerialStopBitCount_Two:
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
  }
  NANOCLR_NOCLEANUP();
}

// Write data into buffer 
HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeWrite___VOID__SZARRAY_U1(CLR_RT_StackFrame &stack) 
{
  NANOCLR_HEADER();
  {
    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    uint8_t *data = NULL;
    uint8_t uartNum = 0;
    size_t length = 0;

    if (pThis[FIELD___disposed].NumericByRef().u1 != 0) 
    {
      NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    // Get UART device number 
    uartNum = pThis[FIELD___portIndex].NumericByRef().s4;

    // Quit if parameters or device is invalid or out of range 
    if (uartNum >= (sizeof(Uart_PAL) / sizeof(Uart_PAL[0]))) 
    {
      NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    NF_PAL_UART *palUart = Uart_PAL[uartNum];

    // dereference the data buffer from the argument 
    CLR_RT_HeapBlock_Array *dataBuffer = stack.Arg1().DereferenceArray();

    // get a  the pointer to the array by using the first element of the array 
    data = dataBuffer->GetFirstElement();

    // get the size of the buffer 
    length = dataBuffer->m_numOfElements;

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
      NANOCLR_SET_AND_LEAVE(CLR_E_IO);
    }

    // need to update the _unstoredBufferLength field in the SerialDeviceOutputStream
    // get pointer to outputStream field 
    CLR_RT_HeapBlock *outputStream = pThis[Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::FIELD___outputStream].Dereference();
    // get pointer to _unstoredBufferLength field and udpate field value 
    outputStream[Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDeviceOutputStream::FIELD___unstoredBufferLength].NumericByRef().s4 = palUart->TxRingBuffer.Length();
  }
  NANOCLR_NOCLEANUP();
}

// Store - Send buffer and wait 
HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeStore___U4(CLR_RT_StackFrame &stack) 
{
  NANOCLR_HEADER();
  {
    NF_PAL_UART *palUart = NULL;

    size_t length = 0;

    CLR_RT_HeapBlock *writeTimeout;
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

    uint8_t uartNum = pThis[FIELD___portIndex].NumericByRef().s4;

    // Quit if parameters or device is invalid or out of range 
    if (uartNum >= (sizeof(Uart_PAL) / sizeof(Uart_PAL[0]))) 
    {
      NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    palUart = Uart_PAL[uartNum];

    // as we are transsmiting data, we can discard recive buffer
    // assuming that we can't RX and TX simultaneously 
    size_t rx_size = 0;
    palUart->RxRingBuffer.Push(1);
    rx_size = palUart->RxRingBuffer.Length();
    palUart->RxRingBuffer.Pop(rx_size);

    // get value for _readtimeout field (pointer!) 
    writeTimeout = &pThis[Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::FIELD___writeTimeout];

    // setup timeout 
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(*writeTimeout, timeoutTicks));

    // push dummy length value onto the eval stack
    // this is going to be used to store how many bytes where buffered to Tx 
    if (stack.m_customState == 1) 
    {
      stack.PushValueI4(0);
      // bump custom state so the read value above is pushed only once 
      stack.m_customState = 2;
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
      // Optimize buffer for sequential reading 
      palUart->TxRingBuffer.OptimizeSequence();

      // Get data length available in the buffer 
      length = palUart->TxRingBuffer.Length();

      // Push to the stack how many bytes bytes where buffered for Tx 
      stack.m_evalStack[1].NumericByRef().s4 = length;

      // Set TX ongoing count 
      palUart->TxOngoingCount = length;

      // Set transfer structure to nano ring buffer 
      palUart->xfer.data = (uint8_t *)palUart->TxRingBuffer.Reader();
      palUart->xfer.dataSize = length;
      // Notify task that we want to transmit data. 
      xTaskNotify(palUart->xWTaskToNotify, 0x01, eSetBits);
    }

    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeoutTicks, CLR_RT_ExecutionEngine::c_Event_SerialPortOut, eventResult));

    if (eventResult) 
    {
      // Notify the task that the transmission is complete. 
      // pop elements from ring buffer, just pop 
      palUart->TxRingBuffer.Pop(palUart->TxOngoingCount);

      // reset Tx ongoing count 
      palUart->TxOngoingCount = 0;

      length = stack.m_evalStack[1].NumericByRef().s4;
    } 
    else 
    {
      palUart->TxRingBuffer.Pop(palUart->TxOngoingCount);
      palUart->TxOngoingCount = 0;
      NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
    }

    // pop length and timeout heap block from stack 
    stack.PopValue();
    stack.PopValue();

    stack.SetResult_U4(length);
  }
  NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeRead___U4__SZARRAY_U1__I4__I4(CLR_RT_StackFrame &stack) 
{
  NANOCLR_HEADER();
  {
    CLR_RT_HeapBlock_Array *dataBuffer = NULL;
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);
    int64_t *timeoutTicks;
    InputStreamOptions options = InputStreamOptions_None;
    CLR_RT_HeapBlock *readTimeout;

    bool eventResult = true;

    uint8_t uartNum = 0;
    size_t bytesRead = 0;
    size_t bytesToRead = 0;
    size_t dataLength = 0;

    uint8_t *data = NULL;

    size_t count = 0;

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

    NF_PAL_UART *palUart = Uart_PAL[uartNum];

    // get how many bytes are requested to read 
    count = stack.Arg2().NumericByRef().s4;

    // Dereference the data buffer from the argument 
    dataBuffer = stack.Arg1().DereferenceArray();

    // get a the pointer to the array by using the first element of the array 
    data = dataBuffer->GetFirstElement();

    // get the length of the data buffer 
    dataLength = dataBuffer->m_numOfElements;

    // Get the InputStreamOptions option 
    // TODO: Implement transfer options. 
    options = (InputStreamOptions)stack.Arg3().NumericByRef().s4;

    // Set up timeout. 
    readTimeout = &pThis[Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::FIELD___readTimeout];
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(*readTimeout, timeoutTicks));

    // Check what's avaliable in Rx ring buffer 
    if (palUart->RxRingBuffer.Length() >= count) 
  {
      // read from Rx ring buffer 
      bytesToRead = count;

      // is the read ahead option enabled? 
      if (options == InputStreamOptions_ReadAhead) {

        // yes, check how many bytes we can store in the buffer argument 
        if (dataLength < palUart->RxRingBuffer.Length()) {
          // read as many bytes has the buffer can hold
          bytesToRead = dataLength;
        } else {
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

      if (stack.m_customState == 1) {

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
        {          // need to read from the UART
                   // update custom state 
          stack.m_customState = 2;
        }
      } 
      else 
      {
        // wait for event 
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeoutTicks, CLR_RT_ExecutionEngine::c_Event_SerialPortIn, eventResult));
        if (!eventResult)
        {
          // event timeout
          // compute how many bytes to read
          // considering the InputStreamOptions read ahead option 
          if (options == InputStreamOptions_ReadAhead)
          {
            // yes, check how many bytes we can store in the buffer argument 
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
          if (options == InputStreamOptions_Partial) 
          {
            // take InputStreamOptions_Partial as default and read requested quantity or what's available 
            bytesToRead = count;

            if (count > palUart->RxRingBuffer.Length()) {
              // need to adjust because there aren't enough bytes available 
              bytesToRead = palUart->RxRingBuffer.Length();
            }
          }
          // If InputStreamOptions is set to None return timeout exception 
          NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
        }
      }
    }

    if (bytesToRead > 0) 
    {
      // pop the requested bytes from the ring buffer 
      bytesRead = palUart->RxRingBuffer.Pop(data, bytesToRead);
    }

    // pop timeout heap block from stack and return how many bytes were read 
    stack.PopValue();
    stack.SetResult_U4(bytesRead);
  }
  NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeSetWatchChar___VOID(CLR_RT_StackFrame &stack) 
{
  NANOCLR_HEADER();
  {
    // get a pointer to the managed object instance and check that it's not NULL 
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Choose the driver for this SerialDevice 
    uint8_t uartNum = (uint8_t)pThis[FIELD___portIndex].NumericByRef().s4;

    // Quit if parameters or device is invalid or out of range 
    if (uartNum >= (sizeof(Uart_PAL) / sizeof(Uart_PAL[0]))) 
    {
      NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    NF_PAL_UART *palUart = Uart_PAL[uartNum];

    // set watch char 
    palUart->WatchChar = (uint8_t)pThis[FIELD___watchChar].NumericByRef().u1;
  }
  NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::get_BytesToRead___U4(CLR_RT_StackFrame &stack) 
{
  NANOCLR_HEADER();
  {

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    uint8_t uartNum = 0;
    size_t read_count = 0;

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

    NF_PAL_UART *palUart = Uart_PAL[uartNum];
    read_count = palUart->RxBytesToRead;

    stack.SetResult_U4(read_count);
  }
  NANOCLR_NOCLEANUP();
}

// Return available devices 
HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::GetDeviceSelector___STATIC__STRING(CLR_RT_StackFrame &stack) 
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
  // we need set a return result in the stack argument using the a ppropriate SetResult according to the variable type (a string here) 
  stack.SetResult_String(deviceSelectorString);
  NANOCLR_NOCLEANUP_NOLABEL();
}
