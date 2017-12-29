//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#include <ch.h>
#include <hal.h>
// #include <cmsis_os.h>
// #include <LaunchCLR.h>
// #include <string.h>
// #include <targetPAL.h>
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

// UART driver configuration structure
#if defined(STM32F0xx_MCUCONF) || defined(STM32F7xx_MCUCONF)

static UARTConfig uart_cfg = {
  NULL, //void_handler,
  NULL, //void_handler,
  NULL, //void_handler,
  NULL, //rxchar,
  NULL, //rxerr,
  0,
  0,
  9600,
  0,
  0,
  0
};

#elif defined(STM32F4xx_MCUCONF)

static UARTConfig uart_cfg = {
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  9600,
  0,
  0,
  0
};

#else
    #error "Error with STM32 series option"
#endif

// define this type here to make it shorter and improve code readability
typedef Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice _serialDevice;

HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeDispose___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeInit___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        UARTDriver* uartDriver;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        // setup configuration

        // data bits @ CR1:M1&M0

        // parity @ CR1:PS

        // stop bits @ CR2:STOP1&STOP0
        // clear bits for stop bits setting
        uart_cfg.cr2 &= ~USART_CR2_STOP;

        switch ((SerialStopBitCount)pThis[ FIELD___stopBits ].NumericByRef().s4)
        {
            case SerialStopBitCount_One :
                // already set with the above
                break;
            case SerialStopBitCount_OnePointFive :
                uart_cfg.cr2 |= USART_CR2_STOP_1 + USART_CR2_STOP_0;
                break;
            case SerialStopBitCount_Two :
                uart_cfg.cr2 |= USART_CR2_STOP_1;
                break;
        }

        // Choose the driver that is mapped to the chosen bus
        switch ((int)pThis[ FIELD___portIndex ].NumericByRef().s4)
        {
#if STM32_UART_USE_USART1
            case 1 :
                uartDriver = &UARTD1;
                break;
#endif
#if STM32_UART_USE_USART2
            case 2 :
                uartDriver = &UARTD2;
                break;
#endif
#if STM32_UART_USE_USART3
            case 3 :
                uartDriver = &UARTD3;
                break;
#endif
#if STM32_UART_USE_UART4
            case 4 :
                uartDriver = &UARTD4;
                break;
#endif
#if STM32_UART_USE_UART5
            case 5 :
                uartDriver = &UARTD5;
                break;
#endif
#if STM32_UART_USE_USART6
            case 6 :
                uartDriver = &UARTD6;
                break;
#endif
#if STM32_UART_USE_UART7
            case 7 :
                uartDriver = &UARTD7;
                break;
#endif
#if STM32_UART_USE_UART8
            case 8 :
                uartDriver = &UARTD8;
                break;
#endif
        }

        // stop UART
        uartStop(uartDriver);


        uartStart(uartDriver, &uart_cfg);
        palSetPadMode(GPIOC, 6, PAL_MODE_ALTERNATE(8));
        palSetPadMode(GPIOC, 7, PAL_MODE_ALTERNATE(8));


        // null pointers and vars
        pThis = NULL;
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeWrite___VOID__SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

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
