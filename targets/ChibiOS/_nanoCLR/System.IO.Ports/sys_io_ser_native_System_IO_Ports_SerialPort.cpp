//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <hal.h>
#include <nanoHAL.h>
#include "sys_io_ser_native_target.h"

/////////////////////////////////////////////////////////
// UART PAL strucs delcared in win_dev_serial_native.h //
/////////////////////////////////////////////////////////
// TODO: uncomment when Windows.Deveices.SerialCommunication will be removed
// #if defined(NF_SERIAL_COMM_STM32_UART_USE_USART1) && (NF_SERIAL_COMM_STM32_UART_USE_USART1 == TRUE)
// NF_PAL_UART Uart1_PAL;
// #endif
// #if defined(NF_SERIAL_COMM_STM32_UART_USE_USART2) && (NF_SERIAL_COMM_STM32_UART_USE_USART2 == TRUE)
// NF_PAL_UART Uart2_PAL;
// #endif
// #if defined(NF_SERIAL_COMM_STM32_UART_USE_USART3) && (NF_SERIAL_COMM_STM32_UART_USE_USART3 == TRUE)
// NF_PAL_UART Uart3_PAL;
// #endif
// #if defined(NF_SERIAL_COMM_STM32_UART_USE_UART4) && (NF_SERIAL_COMM_STM32_UART_USE_UART4 == TRUE)
// NF_PAL_UART Uart4_PAL;
// #endif
// #if defined(NF_SERIAL_COMM_STM32_UART_USE_UART5) && (NF_SERIAL_COMM_STM32_UART_USE_UART5 == TRUE)
// NF_PAL_UART Uart5_PAL;
// #endif
// #if defined(NF_SERIAL_COMM_STM32_UART_USE_USART6) && (NF_SERIAL_COMM_STM32_UART_USE_USART6 == TRUE)
// NF_PAL_UART Uart6_PAL;
// #endif
// #if defined(NF_SERIAL_COMM_STM32_UART_USE_UART7) && (NF_SERIAL_COMM_STM32_UART_USE_UART7 == TRUE)
// NF_PAL_UART Uart7_PAL;
// #endif
// #if defined(NF_SERIAL_COMM_STM32_UART_USE_UART8) && (NF_SERIAL_COMM_STM32_UART_USE_UART8 == TRUE)
// NF_PAL_UART Uart8_PAL;
// #endif

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

    // pop elements from ring buffer, just pop
    palUart->TxRingBuffer.Pop(palUart->TxOngoingCount);

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
    {
        NF_PAL_UART *palUart;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // Choose the driver for this SerialDevice
        switch ((int)pThis[FIELD___portIndex].NumericByRef().s4)
        {
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART1) && (NF_SERIAL_COMM_STM32_UART_USE_USART1 == TRUE)
            case 1:
                palUart = &Uart1_PAL;
                break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART2) && (NF_SERIAL_COMM_STM32_UART_USE_USART2 == TRUE)
            case 2:
                palUart = &Uart2_PAL;
                break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART3) && (NF_SERIAL_COMM_STM32_UART_USE_USART3 == TRUE)
            case 3:
                palUart = &Uart3_PAL;
                break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART4) && (NF_SERIAL_COMM_STM32_UART_USE_UART4 == TRUE)
            case 4:
                palUart = &Uart4_PAL;
                break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART5) && (NF_SERIAL_COMM_STM32_UART_USE_UART5 == TRUE)
            case 5:
                palUart = &Uart5_PAL;
                break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART6) && (NF_SERIAL_COMM_STM32_UART_USE_USART6 == TRUE)
            case 6:
                palUart = &Uart6_PAL;
                break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART7) && (NF_SERIAL_COMM_STM32_UART_USE_UART7 == TRUE)
            case 7:
                palUart = &Uart7_PAL;
                break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART8) && (NF_SERIAL_COMM_STM32_UART_USE_UART8 == TRUE)
            case 8:
                palUart = &Uart8_PAL;
                break;
#endif
            default:
                // this COM port is not valid
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                break;
        }

        // get length of Rx ring buffer
        stack.SetResult_U4(palUart->RxRingBuffer.Length());
    }
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
            // stop UART
            uartStop(&UARTD1);
            Uart1_PAL.UartDriver = NULL;
            break;
#endif

#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART2) && (NF_SERIAL_COMM_STM32_UART_USE_USART2 == TRUE)
        case 2:
            UnInit_UART2();
            // stop UART
            uartStop(&UARTD2);
            Uart2_PAL.UartDriver = NULL;
            break;
#endif

#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART3) && (NF_SERIAL_COMM_STM32_UART_USE_USART3 == TRUE)
        case 3:
            UnInit_UART3();
            // stop UART
            uartStop(&UARTD3);
            Uart3_PAL.UartDriver = NULL;
            break;
#endif

#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART4) && (NF_SERIAL_COMM_STM32_UART_USE_UART4 == TRUE)
        case 4:
            UnInit_UART4();
            // stop UART
            uartStop(&UARTD4);
            Uart4_PAL.UartDriver = NULL;
            break;
#endif

#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART5) && (NF_SERIAL_COMM_STM32_UART_USE_UART5 == TRUE)
        case 5:
            UnInit_UART5();
            // stop UART
            uartStop(&UARTD5);
            Uart5_PAL.UartDriver = NULL;
            break;
#endif

#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART6) && (NF_SERIAL_COMM_STM32_UART_USE_USART6 == TRUE)
        case 6:
            UnInit_UART6();
            // stop UART
            uartStop(&UARTD6);
            Uart6_PAL.UartDriver = NULL;
            break;
#endif

#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART7) && (NF_SERIAL_COMM_STM32_UART_USE_UART7 == TRUE)
        case 7:
            UnInit_UART7();
            // stop UART
            uartStop(&UARTD7);
            Uart7_PAL.UartDriver = NULL;
            break;
#endif

#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART8) && (NF_SERIAL_COMM_STM32_UART_USE_UART8 == TRUE)
        case 8:
            UnInit_UART8();
            // stop UART
            uartStop(&UARTD8);
            Uart8_PAL.UartDriver = NULL;
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
    {
        NF_PAL_UART *palUart;

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

        // configure UART handlers
        palUart->Uart_cfg.txend1_cb = TxEnd1;
        palUart->Uart_cfg.rxchar_cb = RxChar;
        // palUart->Uart_cfg.rxend_cb = RxEnd;

        // call the configure
        return NativeConfig___VOID(stack);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeConfig___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        NF_PAL_UART *palUart = NULL;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // Choose the driver for this SerialDevice
        switch ((int)pThis[FIELD___portIndex].NumericByRef().s4)
        {
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART1) && (NF_SERIAL_COMM_STM32_UART_USE_USART1 == TRUE)
            case 1:
                palUart = &Uart1_PAL;
                break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART2) && (NF_SERIAL_COMM_STM32_UART_USE_USART2 == TRUE)
            case 2:
                palUart = &Uart2_PAL;
                break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART3) && (NF_SERIAL_COMM_STM32_UART_USE_USART3 == TRUE)
            case 3:
                palUart = &Uart3_PAL;
                break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART4) && (NF_SERIAL_COMM_STM32_UART_USE_UART4 == TRUE)
            case 4:
                palUart = &Uart4_PAL;
                break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART5) && (NF_SERIAL_COMM_STM32_UART_USE_UART5 == TRUE)
            case 5:
                palUart = &Uart5_PAL;
                break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART6) && (NF_SERIAL_COMM_STM32_UART_USE_USART6 == TRUE)
            case 6:
                palUart = &Uart6_PAL;
                break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART7) && (NF_SERIAL_COMM_STM32_UART_USE_UART7 == TRUE)
            case 7:
                palUart = &Uart7_PAL;
                break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART8) && (NF_SERIAL_COMM_STM32_UART_USE_UART8 == TRUE)
            case 8:
                palUart = &Uart8_PAL;
                break;
#endif
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
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeWrite___VOID__SZARRAY_U1__I4__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        NF_PAL_UART *palUart = NULL;

        uint8_t *data;
        unsigned int length = 0;
        size_t count = 0;
        int16_t writeOffset = 0;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        if (pThis[FIELD___disposed].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        // dereference the data buffer from the argument
        CLR_RT_HeapBlock_Array *dataBuffer = stack.Arg1().DereferenceArray();
        writeOffset = stack.Arg2().NumericByRef().s4;
        count = stack.Arg3().NumericByRef().s4;

        // get a the pointer to the array by using the first element of the array
        data = dataBuffer->GetElement(writeOffset);

        // get the size of the buffer
        length = dataBuffer->m_numOfElements;

        if (count > length)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_BUFFER_TOO_SMALL);
        }

        // get pointer to PAL UART
        switch ((int)pThis[FIELD___portIndex].NumericByRef().s4)
        {
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART1) && (NF_SERIAL_COMM_STM32_UART_USE_USART1 == TRUE)
            case 1:
                palUart = &Uart1_PAL;
                break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART2) && (NF_SERIAL_COMM_STM32_UART_USE_USART2 == TRUE)
            case 2:
                palUart = &Uart2_PAL;
                break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART3) && (NF_SERIAL_COMM_STM32_UART_USE_USART3 == TRUE)
            case 3:
                palUart = &Uart3_PAL;
                break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART4) && (NF_SERIAL_COMM_STM32_UART_USE_UART4 == TRUE)
            case 4:
                palUart = &Uart4_PAL;
                break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART5) && (NF_SERIAL_COMM_STM32_UART_USE_UART5 == TRUE)
            case 5:
                palUart = &Uart5_PAL;
                break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART6) && (NF_SERIAL_COMM_STM32_UART_USE_USART6 == TRUE)
            case 6:
                palUart = &Uart6_PAL;
                break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART7) && (NF_SERIAL_COMM_STM32_UART_USE_UART7 == TRUE)
            case 7:
                palUart = &Uart7_PAL;
                break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART8) && (NF_SERIAL_COMM_STM32_UART_USE_UART8 == TRUE)
            case 8:
                palUart = &Uart8_PAL;
                break;
#endif
        }

        // check if there is enough room in the buffer
        if (palUart->TxRingBuffer.Capacity() - palUart->TxRingBuffer.Length() < count)
        {
            // not enough room in the buffer
            NANOCLR_SET_AND_LEAVE(CLR_E_BUFFER_TOO_SMALL);
        }

        // push data to buffer
        size_t bytesWritten = palUart->TxRingBuffer.Push(data, count);

        // check if all requested bytes were written
        if (bytesWritten != count)
        {
            // not sure if this is the best exception to throw here...
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }

        // null pointers and vars
        pThis = NULL;
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeStore___U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_PAL_UART *palUart = NULL;

    size_t length = 0;

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

    // Choose the driver for this SerialDevice
    switch ((int)pThis[FIELD___portIndex].NumericByRef().s4)
    {
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART1) && (NF_SERIAL_COMM_STM32_UART_USE_USART1 == TRUE)
        case 1:
            palUart = &Uart1_PAL;
            break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART2) && (NF_SERIAL_COMM_STM32_UART_USE_USART2 == TRUE)
        case 2:
            palUart = &Uart2_PAL;
            break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART3) && (NF_SERIAL_COMM_STM32_UART_USE_USART3 == TRUE)
        case 3:
            palUart = &Uart3_PAL;
            break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART4) && (NF_SERIAL_COMM_STM32_UART_USE_UART4 == TRUE)
        case 4:
            palUart = &Uart4_PAL;
            break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART5) && (NF_SERIAL_COMM_STM32_UART_USE_UART5 == TRUE)
        case 5:
            palUart = &Uart5_PAL;
            break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART6) && (NF_SERIAL_COMM_STM32_UART_USE_USART6 == TRUE)
        case 6:
            palUart = &Uart6_PAL;
            break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART7) && (NF_SERIAL_COMM_STM32_UART_USE_UART7 == TRUE)
        case 7:
            palUart = &Uart7_PAL;
            break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART8) && (NF_SERIAL_COMM_STM32_UART_USE_UART8 == TRUE)
        case 8:
            palUart = &Uart8_PAL;
            break;
#endif
    }

    CLR_RT_HeapBlock hbTimeout;
    hbTimeout.SetInteger((CLR_INT64)pThis[FIELD___writeTimeout].NumericByRef().s4 * TIME_CONVERSION__TO_MILLISECONDS);
    // setup timeout
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeoutTicks));

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
        // optimize buffer for sequential reading
        palUart->TxRingBuffer.OptimizeSequence();

        // get data length available in the buffer
        length = palUart->TxRingBuffer.Length();

        // push to the stack how many bytes bytes where buffered for Tx
        stack.m_evalStack[1].NumericByRef().s4 = length;

        // flush DMA buffer to ensure cache coherency
        // (only required for Cortex-M7)
        cacheBufferFlush(palUart->TxRingBuffer.Reader(), length);

        // set TX ongoing count
        palUart->TxOngoingCount = length;

        // because the UART can be accessed from several threads need to get exclusive access to it
        uartAcquireBus(palUart->UartDriver);

        // start sending data (DMA will read from the ring buffer)
        uartStartSend(palUart->UartDriver, length, (uint8_t *)palUart->TxRingBuffer.Reader());
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

    // pop length heap block from stack
    stack.PopValue();

    // pop timeout heap block from stack
    stack.PopValue();

    stack.SetResult_U4(length);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeRead___U4__SZARRAY_U1__I4__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array *dataBuffer;
    NF_PAL_UART *palUart = NULL;

    uint8_t *data;

    size_t count = 0;
    size_t bytesRead = 0;
    size_t readOffset = 0;
    size_t bytesToRead = 0;

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
    // The offset to start filling the buffer
    readOffset = stack.Arg2().NumericByRef().s4;

    // get a the pointer to the array by using the first element of the array
    data = dataBuffer->GetElement(readOffset);

    // get how many bytes are requested to read
    count = stack.Arg3().NumericByRef().s4;

    // Choose the driver for this SerialDevice
    switch ((int)pThis[FIELD___portIndex].NumericByRef().s4)
    {
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART1) && (NF_SERIAL_COMM_STM32_UART_USE_USART1 == TRUE)
        case 1:
            palUart = &Uart1_PAL;
            break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART2) && (NF_SERIAL_COMM_STM32_UART_USE_USART2 == TRUE)
        case 2:
            palUart = &Uart2_PAL;
            break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART3) && (NF_SERIAL_COMM_STM32_UART_USE_USART3 == TRUE)
        case 3:
            palUart = &Uart3_PAL;
            break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART4) && (NF_SERIAL_COMM_STM32_UART_USE_UART4 == TRUE)
        case 4:
            palUart = &Uart4_PAL;
            break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART5) && (NF_SERIAL_COMM_STM32_UART_USE_UART5 == TRUE)
        case 5:
            palUart = &Uart5_PAL;
            break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART6) && (NF_SERIAL_COMM_STM32_UART_USE_USART6 == TRUE)
        case 6:
            palUart = &Uart6_PAL;
            break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART7) && (NF_SERIAL_COMM_STM32_UART_USE_UART7 == TRUE)
        case 7:
            palUart = &Uart7_PAL;
            break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART8) && (NF_SERIAL_COMM_STM32_UART_USE_UART8 == TRUE)
        case 8:
            palUart = &Uart8_PAL;
            break;
#endif
    }

    CLR_RT_HeapBlock hbTimeout;
    hbTimeout.SetInteger((CLR_INT64)pThis[FIELD___writeTimeout].NumericByRef().s4 * TIME_CONVERSION__TO_MILLISECONDS);
    // setup timeout
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
    {
        NF_PAL_UART *palUart;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // Choose the driver for this SerialDevice
        switch ((int)pThis[FIELD___portIndex].NumericByRef().s4)
        {
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART1) && (NF_SERIAL_COMM_STM32_UART_USE_USART1 == TRUE)
            case 1:
                palUart = &Uart1_PAL;
                break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART2) && (NF_SERIAL_COMM_STM32_UART_USE_USART2 == TRUE)
            case 2:
                palUart = &Uart2_PAL;
                break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART3) && (NF_SERIAL_COMM_STM32_UART_USE_USART3 == TRUE)
            case 3:
                palUart = &Uart3_PAL;
                break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART4) && (NF_SERIAL_COMM_STM32_UART_USE_UART4 == TRUE)
            case 4:
                palUart = &Uart4_PAL;
                break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART5) && (NF_SERIAL_COMM_STM32_UART_USE_UART5 == TRUE)
            case 5:
                palUart = &Uart5_PAL;
                break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART6) && (NF_SERIAL_COMM_STM32_UART_USE_USART6 == TRUE)
            case 6:
                palUart = &Uart6_PAL;
                break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART7) && (NF_SERIAL_COMM_STM32_UART_USE_UART7 == TRUE)
            case 7:
                palUart = &Uart7_PAL;
                break;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART8) && (NF_SERIAL_COMM_STM32_UART_USE_UART8 == TRUE)
            case 8:
                palUart = &Uart8_PAL;
                break;
#endif
            default:
                // this COM port is not valid
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                break;
        }

        // set watch char
        palUart->WatchChar = (uint8_t)pThis[FIELD___watchChar].NumericByRef().u1;
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::GetDeviceSelector___STATIC__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
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
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
