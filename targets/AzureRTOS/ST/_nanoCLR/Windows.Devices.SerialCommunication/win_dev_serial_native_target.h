//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef WIN_DEV_SERIAL_NATIVE_TARGET_H
#define WIN_DEV_SERIAL_NATIVE_TARGET_H

#include <target_windows_devices_serialcommunication_config.h>
#include <win_dev_serial_native.h>
#include <hal.h>

// struct representing the UART
typedef struct
{
    UARTDriver *UartDriver;
    UARTConfig Uart_cfg;

    HAL_RingBuffer<uint8_t> TxRingBuffer;
    uint8_t *TxBuffer;
    uint16_t TxOngoingCount;

    HAL_RingBuffer<uint8_t> RxRingBuffer;
    uint8_t *RxBuffer;
    uint16_t RxBytesToRead;

    uint8_t WatchChar;
} NF_PAL_UART__;

////////////////////////////////////////////
// declaration of the the UART PAL strucs //
////////////////////////////////////////////
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART1) && (NF_SERIAL_COMM_STM32_UART_USE_USART1 == TRUE)
extern NF_PAL_UART__ Uart1_PAL__;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART2) && (NF_SERIAL_COMM_STM32_UART_USE_USART2 == TRUE)
extern NF_PAL_UART__ Uart2_PAL__;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART3) && (NF_SERIAL_COMM_STM32_UART_USE_USART3 == TRUE)
extern NF_PAL_UART__ Uart3_PAL__;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART4) && (NF_SERIAL_COMM_STM32_UART_USE_UART4 == TRUE)
extern NF_PAL_UART__ Uart4_PAL__;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART5) && (NF_SERIAL_COMM_STM32_UART_USE_UART5 == TRUE)
extern NF_PAL_UART__ Uart5_PAL__;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART6) && (NF_SERIAL_COMM_STM32_UART_USE_USART6 == TRUE)
extern NF_PAL_UART__ Uart6_PAL__;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART7) && (NF_SERIAL_COMM_STM32_UART_USE_UART7 == TRUE)
extern NF_PAL_UART__ Uart7_PAL__;
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART8) && (NF_SERIAL_COMM_STM32_UART_USE_UART8 == TRUE)
extern NF_PAL_UART__ Uart8_PAL__;
#endif

// the following macro defines a function that configures the GPIO pins for a STM32 UART/USART
// it gets called in the Windows_Devices_SerialCommunication_SerialDevice::NativeConfig function
// this is required because the UART/USART peripherals can use multiple GPIO configuration combinations
#define UART_CONFIG_PINS__(num, gpio_port_tx, gpio_port_rx, tx_pin, rx_pin, alternate_function)                        \
    void ConfigPins_UART##num##__()                                                                                    \
    {                                                                                                                  \
        palSetPadMode(gpio_port_tx, tx_pin, PAL_MODE_ALTERNATE(alternate_function));                                   \
        palSetPadMode(gpio_port_rx, rx_pin, PAL_MODE_ALTERNATE(alternate_function));                                   \
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
// when a UART/USART is defined the declarations below will have the real function/configuration
// in the target folder @ target_windows_devices_serialcommunication_config.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
void ConfigPins_UART1__();
void ConfigPins_UART2__();
void ConfigPins_UART3__();
void ConfigPins_UART4__();
void ConfigPins_UART5__();
void ConfigPins_UART6__();
void ConfigPins_UART7__();
void ConfigPins_UART8__();

/////////////////////////////////////
// UART Tx buffers                 //
// these live in the target folder //
/////////////////////////////////////
extern uint8_t Uart1_TxBuffer__[];
extern uint8_t Uart2_TxBuffer__[];
extern uint8_t Uart3_TxBuffer__[];
extern uint8_t Uart4_TxBuffer__[];
extern uint8_t Uart5_TxBuffer__[];
extern uint8_t Uart6_TxBuffer__[];
extern uint8_t Uart7_TxBuffer__[];
extern uint8_t Uart8_TxBuffer__[];

/////////////////////////////////////
// UART Rx buffers                 //
// these live in the target folder //
/////////////////////////////////////
extern uint8_t Uart1_RxBuffer__[];
extern uint8_t Uart2_RxBuffer__[];
extern uint8_t Uart3_RxBuffer__[];
extern uint8_t Uart4_RxBuffer__[];
extern uint8_t Uart5_RxBuffer__[];
extern uint8_t Uart6_RxBuffer__[];
extern uint8_t Uart7_RxBuffer__[];
extern uint8_t Uart8_RxBuffer__[];

// the following macro defines a function that initializes an UART struct
// it gets called in the Windows_Devices_SerialCommunication_SerialDevice::NativeInit function

#if defined(STM32F7XX) || defined(STM32F0XX)

// STM32F7 and STM32F0 use UART driver v2
#define UART_INIT__(num, tx_buffer_size, rx_buffer_size)                                                               \
    void Init_UART##num##__()                                                                                          \
    {                                                                                                                  \
        Uart##num##_PAL__.Uart_cfg.txend2_cb = NULL;                                                                   \
        Uart##num##_PAL__.Uart_cfg.rxend_cb = NULL;                                                                    \
        Uart##num##_PAL__.Uart_cfg.rxerr_cb = NULL;                                                                    \
        Uart##num##_PAL__.Uart_cfg.timeout_cb = NULL;                                                                  \
        Uart##num##_PAL__.Uart_cfg.timeout = 0;                                                                        \
        Uart##num##_PAL__.Uart_cfg.speed = 9600;                                                                       \
        Uart##num##_PAL__.Uart_cfg.cr1 = 0;                                                                            \
        Uart##num##_PAL__.Uart_cfg.cr2 = 0;                                                                            \
        Uart##num##_PAL__.Uart_cfg.cr3 = 0;                                                                            \
        Uart##num##_PAL__.TxBuffer = Uart##num##_TxBuffer__;                                                           \
        Uart##num##_PAL__.TxRingBuffer.Initialize(Uart##num##_PAL__.TxBuffer, tx_buffer_size);                         \
        Uart##num##_PAL__.TxOngoingCount = 0;                                                                          \
        Uart##num##_PAL__.RxBuffer = Uart##num##_RxBuffer__;                                                           \
        Uart##num##_PAL__.RxRingBuffer.Initialize(Uart##num##_PAL__.RxBuffer, rx_buffer_size);                         \
        Uart##num##_PAL__.WatchChar = 0;                                                                               \
    }

#else

// all other STM32F use UART driver v1 which has a different UARTConfig struct
#define UART_INIT__(num, tx_buffer_size, rx_buffer_size)                                                               \
    void Init_UART##num##__()                                                                                          \
    {                                                                                                                  \
        Uart##num##_PAL__.Uart_cfg.txend2_cb = NULL;                                                                   \
        Uart##num##_PAL__.Uart_cfg.rxend_cb = NULL;                                                                    \
        Uart##num##_PAL__.Uart_cfg.rxerr_cb = NULL;                                                                    \
        Uart##num##_PAL__.Uart_cfg.speed = 9600;                                                                       \
        Uart##num##_PAL__.Uart_cfg.cr1 = 0;                                                                            \
        Uart##num##_PAL__.Uart_cfg.cr2 = 0;                                                                            \
        Uart##num##_PAL__.Uart_cfg.cr3 = 0;                                                                            \
        Uart##num##_PAL__.TxBuffer = Uart##num##_TxBuffer__;                                                           \
        Uart##num##_PAL__.TxRingBuffer.Initialize(Uart##num##_PAL__.TxBuffer, tx_buffer_size);                         \
        Uart##num##_PAL__.TxOngoingCount = 0;                                                                          \
        Uart##num##_PAL__.RxBuffer = Uart##num##_RxBuffer__;                                                           \
        Uart##num##_PAL__.RxRingBuffer.Initialize(Uart##num##_PAL__.RxBuffer, rx_buffer_size);                         \
        Uart##num##_PAL__.WatchChar = 0;                                                                               \
    }

#endif

// when a UART/USART is defined the declarations below will have the real function/configuration
// in the target folder @ target_windows_devices_serialcommunication_config.cpp
void Init_UART1__();
void Init_UART2__();
void Init_UART3__();
void Init_UART4__();
void Init_UART5__();
void Init_UART6__();
void Init_UART7__();
void Init_UART8__();

// the following macro defines a function that un initializes an UART struct
// it gets called in the Windows_Devices_SerialCommunication_SerialDevice::NativeDispose function
#define UART_UNINIT__(num)                                                                                             \
    void UnInit_UART##num##__()                                                                                        \
    {                                                                                                                  \
        return;                                                                                                        \
    }

// when a UART/USART is defined the declarations below will have the real function/configuration
// in the target folder @ target_windows_devices_serialcommunication_config.cpp
void UnInit_UART1__();
void UnInit_UART2__();
void UnInit_UART3__();
void UnInit_UART4__();
void UnInit_UART5__();
void UnInit_UART6__();
void UnInit_UART7__();
void UnInit_UART8__();

#endif // WIN_DEV_SERIAL_NATIVE_TARGET_H
