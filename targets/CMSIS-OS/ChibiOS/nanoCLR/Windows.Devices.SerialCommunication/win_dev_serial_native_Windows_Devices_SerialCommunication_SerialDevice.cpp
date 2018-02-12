//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#include <hal.h>
// #include <cmsis_os.h>
// #include <LaunchCLR.h>
// #include <string.h>
#include <nanoHAL.h>
#include "win_dev_serial_native.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.SerialCommunication.SerialHandshake (in managed code) !!! //
/////////////////////////////////////////////////////////////////////////////////////////////////////

enum SerialHandshake
{
    SerialHandshake_None = 0,
    SerialHandshake_RequestToSend,
    SerialHandshake_RequestToSendXOnXOff,
    SerialHandshake_XOnXOff
};

//////////////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.SerialCommunication.SerialParity (in managed code) !!! //
//////////////////////////////////////////////////////////////////////////////////////////////////

enum SerialParity
{
    SerialParity_None = 0,
    SerialParity_Even,
    SerialParity_Mark,
    SerialParity_Odd,
    SerialParity_Space
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.SerialCommunication.SerialStopBitCount (in managed code) !!! //
////////////////////////////////////////////////////////////////////////////////////////////////////////

enum SerialStopBitCount
{
    SerialStopBitCount_One = 0,
    SerialStopBitCount_OnePointFive,
    SerialStopBitCount_Two
};

/////////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH  Windows.Storage.Streams.InputStreamOptions (in managed code) !!! //
/////////////////////////////////////////////////////////////////////////////////////////////

enum InputStreamOptions
{
    InputStreamOptions_None = 0,
    InputStreamOptions_Partial,
    InputStreamOptions_ReadAhead
};

/////////////////////////////////////////////////////////
// UART PAL strucs delcared in win_dev_serial_native.h //
/////////////////////////////////////////////////////////
#if STM32_UART_USE_USART1
    NF_PAL_UART Uart1_PAL;
#endif
#if STM32_UART_USE_USART2
    NF_PAL_UART Uart2_PAL;
#endif
#if STM32_UART_USE_USART3
    NF_PAL_UART Uart3_PAL;
#endif
#if STM32_UART_USE_UART4
    NF_PAL_UART Uart4_PAL;
#endif
#if STM32_UART_USE_UART5
    NF_PAL_UART Uart5_PAL;
#endif
#if STM32_UART_USE_USART6
    NF_PAL_UART Uart6_PAL;
#endif
#if STM32_UART_USE_UART7
    NF_PAL_UART Uart7_PAL;
#endif
#if STM32_UART_USE_UART8
    NF_PAL_UART Uart8_PAL;
#endif

// This callback is invoked when a transmission buffer has been completely read by the driver.
static void TxEnd1(UARTDriver *uartp) 
{
    (void)uartp;

    NATIVE_INTERRUPT_START

    NF_PAL_UART* palUart;

    #if STM32_UART_USE_USART1
    if (uartp == &UARTD1)
    {
        palUart = &Uart1_PAL;
    }
    #endif
    #if STM32_UART_USE_USART2
    if (uartp == &UARTD2)
    {
        palUart = &Uart2_PAL;
    }
    #endif
    #if STM32_UART_USE_USART3
    if (uartp == &UARTD3)
    {
        palUart = &Uart3_PAL;
    }
    #endif
    #if STM32_UART_USE_UART4
    if (uartp == &UARTD4)
    {
        palUart = &Uart4_PAL;
    }
    #endif
    #if STM32_UART_USE_UART5
    if (uartp == &UARTD5)
    {
        palUart = &Uart5_PAL;
    }
    #endif
    #if STM32_UART_USE_USART6
    if (uartp == &UARTD6)
    {
        palUart = &Uart6_PAL;
    }
    #endif
    #if STM32_UART_USE_UART7
    if (uartp == &UARTD7)
    {
        palUart = &Uart7_PAL;
    }
    #endif
    #if STM32_UART_USE_UART8
    if (uartp == &UARTD8)
    {
        palUart = &Uart8_PAL;
    }
    #endif

    // pop elements from ring buffer, just pop
    palUart->TxRingBuffer.Pop(palUart->TxOngoingCount);

    // reset Tx ongoing count
    palUart->TxOngoingCount = 0;

    Events_Set( SYSTEM_EVENT_FLAG_COM_OUT );

    NATIVE_INTERRUPT_END
}

// This callback is invoked when a character is received but the application was not ready to receive it, the character is passed as parameter.
static void RxChar(UARTDriver *uartp, uint16_t c) 
{
    (void)uartp;
    (void)c;

    NATIVE_INTERRUPT_START

    NF_PAL_UART* palUart;

    #if STM32_UART_USE_USART1
    if (uartp == &UARTD1)
    {
        palUart = &Uart1_PAL;
    }
    #endif
    #if STM32_UART_USE_USART2
    if (uartp == &UARTD2)
    {
        palUart = &Uart2_PAL;
    }
    #endif
    #if STM32_UART_USE_USART3
    if (uartp == &UARTD3)
    {
        palUart = &Uart3_PAL;
    }
    #endif
    #if STM32_UART_USE_UART4
    if (uartp == &UARTD4)
    {
        palUart = &Uart4_PAL;
    }
    #endif
    #if STM32_UART_USE_UART5
    if (uartp == &UARTD5)
    {
        palUart = &Uart5_PAL;
    }
    #endif
    #if STM32_UART_USE_USART6
    if (uartp == &UARTD6)
    {
        palUart = &Uart6_PAL;
    }
    #endif
    #if STM32_UART_USE_UART7
    if (uartp == &UARTD7)
    {
        palUart = &Uart7_PAL;
    }
    #endif
    #if STM32_UART_USE_UART8
    if (uartp == &UARTD8)
    {
        palUart = &Uart8_PAL;
    }
    #endif
  
    // store this into the UART Rx buffer

    // push char to ring buffer
    // don't care about the success of the operation, if it's full we are droping the char anyway
    palUart->RxRingBuffer.Push((uint8_t)c);

    // check if the requested bytes are available in the buffer 
    if(palUart->RxBytesToRead > 0 && palUart->RxRingBuffer.Length() >= palUart->RxBytesToRead)
    {
        // reset Rx bytes to read count
        palUart->RxBytesToRead = 0;

        // fire event for Rx buffer complete
        Events_Set(SYSTEM_EVENT_FLAG_COM_IN);
    }

    NATIVE_INTERRUPT_END
}

HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeDispose___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {

    #if STM32_UART_USE_USART1
        UnInit_UART1();
    #endif
    #if STM32_UART_USE_USART2
        UnInit_UART2();
    #endif
    #if STM32_UART_USE_USART3
        UnInit_UART3();
    #endif
    #if STM32_UART_USE_UART4
        UnInit_UART4();
    #endif
    #if STM32_UART_USE_UART5
        UnInit_UART5();
    #endif
    #if STM32_UART_USE_USART6
        UnInit_UART6();
    #endif
    #if STM32_UART_USE_UART7
        UnInit_UART7();
    #endif
    #if STM32_UART_USE_UART8
        UnInit_UART8();
    #endif

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeInit___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        NF_PAL_UART* palUart;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        // Choose the driver for this SerialDevice
        switch ((int)pThis[ FIELD___portIndex ].NumericByRef().s4)
        {
    #if STM32_UART_USE_USART1
            case 1 :
                Init_UART1();
                Uart1_PAL.UartDriver = &UARTD1;
                palUart = &Uart1_PAL;
                break;
    #endif
    #if STM32_UART_USE_USART2
            case 2 :
                Init_UART2();
                Uart2_PAL.UartDriver = &UARTD2;
                palUart = &Uart2_PAL;
                break;
    #endif
    #if STM32_UART_USE_USART3
            case 3 :
                Init_UART3();
                Uart3_PAL.UartDriver = &UARTD3;
                palUart = &Uart3_PAL;
                break;
    #endif
    #if STM32_UART_USE_UART4
            case 4 :
                Init_UART4();
                Uart4_PAL.UartDriver = &UARTD4;
                palUart = &Uart4_PAL;
                break;
    #endif
    #if STM32_UART_USE_UART5
            case 5 :
                Init_UART5();
                Uart5_PAL.UartDriver = &UARTD5;
                palUart = &Uart5_PAL;
                break;                
    #endif
    #if STM32_UART_USE_USART6
            case 6 :
                Init_UART6();
                Uart6_PAL.UartDriver = &UARTD6;
                palUart = &Uart6_PAL;
                break;                
    #endif
    #if STM32_UART_USE_UART7
            case 7 :
                Init_UART7();
                Uart7_PAL.UartDriver = &UARTD7;
                palUart = &Uart7_PAL;
                break;
    #endif
    #if STM32_UART_USE_UART8
            case 8 :
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

        // configure UART handlers
        palUart->Uart_cfg.txend1_cb = TxEnd1;
        palUart->Uart_cfg.rxchar_cb = RxChar;
        //palUart->Uart_cfg.rxend_cb = RxEnd;

        // call the configure
        return NativeConfig___VOID(stack);
    }
    NANOCLR_NOCLEANUP(); 
}

HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeConfig___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        NF_PAL_UART* palUart;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        // Choose the driver for this SerialDevice
        switch ((int)pThis[ FIELD___portIndex ].NumericByRef().s4)
        {
    #if STM32_UART_USE_USART1
            case 1 :
                palUart = &Uart1_PAL;
                break;
    #endif
    #if STM32_UART_USE_USART2
            case 2 :
                palUart = &Uart2_PAL;
                break;
    #endif
    #if STM32_UART_USE_USART3
            case 3 :
                palUart = &Uart3_PAL;
                break;
    #endif
    #if STM32_UART_USE_UART4
            case 4 :
                palUart = &Uart4_PAL;
                break;
    #endif
    #if STM32_UART_USE_UART5
            case 5 :
                palUart = &Uart5_PAL;
                break;                
    #endif
    #if STM32_UART_USE_USART6
            case 6 :
                palUart = &Uart6_PAL;
                break;                
    #endif
    #if STM32_UART_USE_UART7
            case 7 :
                palUart = &Uart7_PAL;
                break;
    #endif
    #if STM32_UART_USE_UART8
            case 8 :
                palUart = &Uart8_PAL;
                break;
    #endif
        }

        // setup configuration

        // data bits @ CR1:M1&M0
        // TODO

        // parity @ CR1:PS
        // TODO

        // stop bits @ CR2:STOP1&STOP0
        // clear bits for stop bits setting
        palUart->Uart_cfg.cr2 &= ~USART_CR2_STOP;

        switch ((SerialStopBitCount)pThis[ FIELD___stopBits ].NumericByRef().s4)
        {
            case SerialStopBitCount_One :
                // already set with the above
                break;
            case SerialStopBitCount_OnePointFive :
                palUart->Uart_cfg.cr2 |= USART_CR2_STOP_1 + USART_CR2_STOP_0;
                break;
            case SerialStopBitCount_Two :
                palUart->Uart_cfg.cr2 |= USART_CR2_STOP_1;
                break;
        }

        // baud rate
        palUart->Uart_cfg.speed = (int)pThis[ FIELD___baudRate ].NumericByRef().s4;
        
        // stop UART, better do this before changing configuration
        uartStop(palUart->UartDriver);

        // get pointer to PAL UART        
        switch ((int)pThis[ FIELD___portIndex ].NumericByRef().s4)
        {
    #if STM32_UART_USE_USART1
            case 1 :
                ConfigPins_UART1();
                break;
    #endif
    #if STM32_UART_USE_USART2
            case 2 :
                ConfigPins_UART2();
                break;
    #endif
    #if STM32_UART_USE_USART3
            case 3 :
                ConfigPins_UART3();
                break;
    #endif
    #if STM32_UART_USE_UART4
            case 4 :
                ConfigPins_UART4();
                break;
    #endif
    #if STM32_UART_USE_UART5
            case 5 :
                ConfigPins_UART5();
                break;                
    #endif
    #if STM32_UART_USE_USART6
            case 6 :
                ConfigPins_UART6();
                break;                
    #endif
    #if STM32_UART_USE_UART7
            case 7 :
                ConfigPins_UART7();
                break;
    #endif
    #if STM32_UART_USE_UART8
            case 8 :
                ConfigPins_UART8();
                break;
    #endif
        }

        uartStart(palUart->UartDriver, &palUart->Uart_cfg);
        uartStopSend(palUart->UartDriver);
        uartStopReceive(palUart->UartDriver);

        // null pointers and vars
        pThis = NULL;
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeWrite___VOID__SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        NF_PAL_UART* palUart;

        uint8_t* data;
        unsigned int length = 0;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        if(pThis[ FIELD___disposed ].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        // dereference the data buffer from the argument
        CLR_RT_HeapBlock_Array* dataBuffer = stack.Arg1().DereferenceArray();

        // get a the pointer to the array by using the first element of the array
        data = dataBuffer->GetFirstElement();

        // get the size of the buffer
        length = dataBuffer->m_numOfElements;

        // get pointer to PAL UART        
        switch ((int)pThis[ FIELD___portIndex ].NumericByRef().s4)
        {
    #if STM32_UART_USE_USART1
            case 1 :
                palUart = &Uart1_PAL;
                break;
    #endif
    #if STM32_UART_USE_USART2
            case 2 :
                palUart = &Uart2_PAL;
                break;
    #endif
    #if STM32_UART_USE_USART3
            case 3 :
                palUart = &Uart3_PAL;
                break;
    #endif
    #if STM32_UART_USE_UART4
            case 4 :
                palUart = &Uart4_PAL;
                break;
    #endif
    #if STM32_UART_USE_UART5
            case 5 :
                palUart = &Uart5_PAL;
                break;                
    #endif
    #if STM32_UART_USE_USART6
            case 6 :
                palUart = &Uart6_PAL;
                break;                
    #endif
    #if STM32_UART_USE_UART7
            case 7 :
                palUart = &Uart7_PAL;
                break;
    #endif
    #if STM32_UART_USE_UART8
            case 8 :
                palUart = &Uart8_PAL;
                break;
    #endif
        }

        // check if there is enough room in the buffer
        if(palUart->TxRingBuffer.Capacity() - palUart->TxRingBuffer.Length() < length)
        {
            // not enough room in the buffer
             NANOCLR_SET_AND_LEAVE(CLR_E_BUFFER_TOO_SMALL);
        }

        // push data to buffer
        size_t bytesWritten = palUart->TxRingBuffer.Push(data, length);

        // check if all requested bytes were written
        if(bytesWritten != length)
        {
            // not sure if this is the best exception to throw here...
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
        
        // need to update the _unstoredBufferLength field in the SerialDeviceOutputStream
        // get pointer to outputStream field
        CLR_RT_HeapBlock* outputStream = pThis[Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::FIELD___outputStream].Dereference();
        // get pointer to _unstoredBufferLength field and udpate field value
        outputStream[Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDeviceOutputStream::FIELD___unstoredBufferLength].NumericByRef().s4 = palUart->TxRingBuffer.Length();

        // null pointers and vars
        pThis = NULL;
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeStore___U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NF_PAL_UART* palUart;

    size_t length = 0;
    size_t totalTxlength = 0;

    CLR_RT_HeapBlock* writeTimeout;
    int64_t*  timeoutTicks;
    bool eventResult = true;
    bool txOk = false;
    
    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    if(pThis[ FIELD___disposed ].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    // Choose the driver for this SerialDevice
    switch ((int)pThis[ FIELD___portIndex ].NumericByRef().s4)
    {
#if STM32_UART_USE_USART1
        case 1 :
            palUart = &Uart1_PAL;
            break;
#endif
#if STM32_UART_USE_USART2
        case 2 :
            palUart = &Uart2_PAL;
            break;
#endif
#if STM32_UART_USE_USART3
        case 3 :
            palUart = &Uart3_PAL;
            break;
#endif
#if STM32_UART_USE_UART4
        case 4 :
            palUart = &Uart6_PAL;
            break;
#endif
#if STM32_UART_USE_UART5
        case 5 :
            palUart = &Uart5_PAL;
            break;
#endif
#if STM32_UART_USE_USART6
        case 6 :
            palUart = &Uart6_PAL;
            break;
#endif
#if STM32_UART_USE_UART7
        case 7 :
            palUart = &Uart7_PAL;
            break;
#endif
#if STM32_UART_USE_UART8
        case 8 :
            palUart = &Uart8_PAL;
            break;
#endif
    }

    // get value for _readtimeout field (pointer!)
    writeTimeout = &pThis[ Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::FIELD___writeTimeout ];

    // setup timeout
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(*writeTimeout, timeoutTicks));

    // push dummy length value onto the eval stack
    // this is going to be used to store how many bytes where buffered to Tx
    if(stack.m_customState == 1)
    {
        stack.PushValueI4(0);
        
        // bump custom state so the read value above is pushed only once
        stack.m_customState = 2;
    }

    // check if there is anything the buffer
    if(palUart->TxRingBuffer.Length() > 0)
    {
        // check if there is a TX operation ongoing
        if(palUart->TxOngoingCount == 0)
        {
            // OK to Tx
            txOk = true;
        }
        else
        {
            // need to wait for the ongoing operation to complete before starting a new one
        }
    }

    if(txOk)
    {
        // optimize buffer for sequential reading
        palUart->TxRingBuffer.OptimizeSequence();

        // get data length available in the buffer
        length = palUart->TxRingBuffer.Length();

        // push to the stack how many bytes bytes where buffered for Tx
        stack.m_evalStack[1].NumericByRef().s4 = length;
        
        // flush DMA buffer to ensure cache coherency
        // (only required for Cortex-M7)
        dmaBufferFlush(palUart->TxRingBuffer.Reader(), length);

        // set TX ongoing count
        palUart->TxOngoingCount = length;

        // because the UART can be accessed from several threads need to get exclusive access to it
        uartAcquireBus(palUart->UartDriver);

        // start sending data (DMA will read from the ring buffer)
        uartStartSend(palUart->UartDriver, length, (uint8_t*)palUart->TxRingBuffer.Reader());
        
        // done here, release the UART
        uartReleaseBus(palUart->UartDriver);
    }

    while(eventResult)
    {
        // non-blocking wait allowing other threads to run while we wait for the Tx operation to complete
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.WaitEvents( stack.m_owningThread, *timeoutTicks, CLR_RT_ExecutionEngine::c_Event_SerialPortOut, eventResult ));

        if(eventResult)
        {
            // event occurred

            // get from the eval stack how many bytes were buffered to Tx
            length = stack.m_evalStack[1].NumericByRef().s4;

            // done here
            break;
        }
        else
        {
            NANOCLR_SET_AND_LEAVE( CLR_E_TIMEOUT );
        }
    }

    // pop length heap block from stack
    stack.PopValue();

    // pop timeout heap block from stack
    stack.PopValue();

    stack.SetResult_U4(length);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeRead___U4__SZARRAY_U1__I4__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array* dataBuffer;
    NF_PAL_UART* palUart;

    uint8_t* data;
    size_t dataLength = 0;

    size_t count = 0;
    size_t bytesRead = 0;
    size_t bytesToRead = 0;

    InputStreamOptions options = InputStreamOptions_None;

    CLR_RT_HeapBlock* readTimeout;
    int64_t*  timeoutTicks;
    bool eventResult = true;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  if(!(pThis)) FAULT_ON_NULL(pThis);

    if(pThis[ FIELD___disposed ].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    // dereference the data buffer from the argument
    dataBuffer = stack.Arg1().DereferenceArray();

    // get a the pointer to the array by using the first element of the array
    data = dataBuffer->GetFirstElement();

    // get the length of the data buffer
    dataLength =  dataBuffer->m_numOfElements;

    // get how many bytes are requested to read
    count = stack.Arg2().NumericByRef().s4;

    // get the InputStreamOptions option
    options = (InputStreamOptions)stack.Arg3().NumericByRef().s4;

    // Choose the driver for this SerialDevice
    switch ((int)pThis[ FIELD___portIndex ].NumericByRef().s4)
    {
#if STM32_UART_USE_USART1
        case 1 :
            palUart = &Uart1_PAL;
            break;
#endif
#if STM32_UART_USE_USART2
        case 2 :
            palUart = &Uart2_PAL;
            break;
#endif
#if STM32_UART_USE_USART3
        case 3 :
            palUart = &Uart3_PAL;
            break;
#endif
#if STM32_UART_USE_UART4
        case 4 :
            palUart = &Uart6_PAL;
            break;
#endif
#if STM32_UART_USE_UART5
        case 5 :
            palUart = &Uart5_PAL;
            break;
#endif
#if STM32_UART_USE_USART6
        case 6 :
            palUart = &Uart6_PAL;
            break;
#endif
#if STM32_UART_USE_UART7
        case 7 :
            palUart = &Uart7_PAL;
            break;
#endif
#if STM32_UART_USE_UART8
        case 8 :
            palUart = &Uart8_PAL;
            break;
#endif
    }

    // get value for _readtimeout field (pointer!)
    readTimeout = &pThis[ Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::FIELD___readTimeout ];

    // setup timeout from the _readtimeout heap block
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks( *readTimeout, timeoutTicks ));

    // figure out what's available in the Rx ring buffer
    if(palUart->RxRingBuffer.Length() >= count)
    {
        // read from Rx ring buffer
        bytesToRead = count;

        // is the read ahead option enabled?
        if(options == InputStreamOptions_ReadAhead)
        {
            // yes
            // check how many bytes we can store in the buffer argument
            if(dataLength < palUart->RxRingBuffer.Length())
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
    }
    else
    {
        if(stack.m_customState == 1)
        {
            // not enough bytes available, have to read from UART
            palUart->RxBytesToRead = count;
            
            // clear event by getting it
            Events_Get(SYSTEM_EVENT_FLAG_COM_IN);

            // don't read anything from the buffer yet
            bytesToRead = 0;
        }  
    }

    while(eventResult)
    {
        if(stack.m_customState == 1)
        {
            if(bytesToRead > 0)
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
            NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeoutTicks, CLR_RT_ExecutionEngine::c_Event_SerialPortIn, eventResult));

            if(!eventResult)
            {
                // event timeout

                // compute how many bytes to read 
                // considering the InputStreamOptions read ahead option
                if(options == InputStreamOptions_ReadAhead)
                {
                    // yes
                    // check how many bytes we can store in the buffer argument
                    if(dataLength < palUart->RxRingBuffer.Length())
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

                    if(count > palUart->RxRingBuffer.Length())
                    {
                        // need to adjust because there aren't enough bytes available
                        bytesToRead = palUart->RxRingBuffer.Length();
                    }
                }
            }
        }
    }

    if(bytesToRead > 0)
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

HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::GetDeviceSelector___STATIC__STRING( CLR_RT_StackFrame& stack )
{
   NANOCLR_HEADER();
   {
       // declare the device selector string whose max size is "COM1,COM2,COM3,COM4,COM5,COM6,COM7,COM8," + terminator and init with the terminator
       char deviceSelectorString[ 40 + 1] = { 0 };

   #if STM32_UART_USE_USART1
       strcat(deviceSelectorString, "COM1,");
   #endif
   #if STM32_UART_USE_USART2
       strcat(deviceSelectorString, "COM2,");
   #endif
   #if STM32_UART_USE_USART3
       strcat(deviceSelectorString, "COM3,");
   #endif
   #if STM32_UART_USE_UART4
       strcat(deviceSelectorString, "COM4,");
   #endif
   #if STM32_UART_USE_UART5
       strcat(deviceSelectorString, "COM5,");
   #endif
   #if STM32_UART_USE_USART6
       strcat(deviceSelectorString, "COM6,");
   #endif
   #if STM32_UART_USE_UART7
       strcat(deviceSelectorString, "COM7,");
   #endif
   #if STM32_UART_USE_UART8
       strcat(deviceSelectorString, "COM8,");
   #endif

       // replace the last comma with a terminator
       deviceSelectorString[hal_strlen_s(deviceSelectorString) - 1] = '\0';

       // because the caller is expecting a result to be returned
       // we need set a return result in the stack argument using the appropriate SetResult according to the variable type (a string here)
       stack.SetResult_String(deviceSelectorString);
   }
   NANOCLR_NOCLEANUP();
}
