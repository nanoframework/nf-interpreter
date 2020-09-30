//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _WIN_DEV_SERIAL_NATIVE_TARGET_H_
#define _WIN_DEV_SERIAL_NATIVE_TARGET_H_

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
} NF_PAL_UART;

////////////////////////////////////////////
// declaration of the the UART PAL strucs //
////////////////////////////////////////////
#if NF_SERIAL_COMM_STM32_UART_USE_USART1
extern NF_PAL_UART Uart1_PAL;
#endif
#if NF_SERIAL_COMM_STM32_UART_USE_USART2
extern NF_PAL_UART Uart2_PAL;
#endif
#if NF_SERIAL_COMM_STM32_UART_USE_USART3
extern NF_PAL_UART Uart3_PAL;
#endif
#if NF_SERIAL_COMM_STM32_UART_USE_UART4
extern NF_PAL_UART Uart4_PAL;
#endif
#if NF_SERIAL_COMM_STM32_UART_USE_UART5
extern NF_PAL_UART Uart5_PAL;
#endif
#if NF_SERIAL_COMM_STM32_UART_USE_USART6
extern NF_PAL_UART Uart6_PAL;
#endif
#if NF_SERIAL_COMM_STM32_UART_USE_UART7
extern NF_PAL_UART Uart7_PAL;
#endif
#if NF_SERIAL_COMM_STM32_UART_USE_UART8
extern NF_PAL_UART Uart8_PAL;
#endif

// the following macro defines a function that configures the GPIO pins for a STM32 UART/USART
// it gets called in the Windows_Devices_SerialCommunication_SerialDevice::NativeConfig function
// this is required because the UART/USART peripherals can use multiple GPIO configuration combinations
#define UART_CONFIG_PINS(num, gpio_port_tx, gpio_port_rx, tx_pin, rx_pin, alternate_function)                          \
    void ConfigPins_UART##num()                                                                                        \
    {                                                                                                                  \
        palSetPadMode(gpio_port_tx, tx_pin, PAL_MODE_ALTERNATE(alternate_function));                                   \
        palSetPadMode(gpio_port_rx, rx_pin, PAL_MODE_ALTERNATE(alternate_function));                                   \
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
// when a UART/USART is defined the declarations below will have the real function/configuration
// in the target folder @ target_windows_devices_serialcommunication_config.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
void ConfigPins_UART1();
void ConfigPins_UART2();
void ConfigPins_UART3();
void ConfigPins_UART4();
void ConfigPins_UART5();
void ConfigPins_UART6();
void ConfigPins_UART7();
void ConfigPins_UART8();

/////////////////////////////////////
// UART Tx buffers                 //
// these live in the target folder //
/////////////////////////////////////
extern uint8_t Uart1_TxBuffer[];
extern uint8_t Uart2_TxBuffer[];
extern uint8_t Uart3_TxBuffer[];
extern uint8_t Uart4_TxBuffer[];
extern uint8_t Uart5_TxBuffer[];
extern uint8_t Uart6_TxBuffer[];
extern uint8_t Uart7_TxBuffer[];
extern uint8_t Uart8_TxBuffer[];

/////////////////////////////////////
// UART Rx buffers                 //
// these live in the target folder //
/////////////////////////////////////
extern uint8_t Uart1_RxBuffer[];
extern uint8_t Uart2_RxBuffer[];
extern uint8_t Uart3_RxBuffer[];
extern uint8_t Uart4_RxBuffer[];
extern uint8_t Uart5_RxBuffer[];
extern uint8_t Uart6_RxBuffer[];
extern uint8_t Uart7_RxBuffer[];
extern uint8_t Uart8_RxBuffer[];

// the following macro defines a function that initializes an UART struct
// it gets called in the Windows_Devices_SerialCommunication_SerialDevice::NativeInit function

#if defined(STM32F7XX) || defined(STM32F0XX)

// STM32F7 and STM32F0 use UART driver v2
#define UART_INIT(num, tx_buffer_size, rx_buffer_size)                                                                 \
    void Init_UART##num()                                                                                              \
    {                                                                                                                  \
        Uart##num##_PAL.Uart_cfg.txend2_cb = NULL;                                                                     \
        Uart##num##_PAL.Uart_cfg.rxend_cb = NULL;                                                                      \
        Uart##num##_PAL.Uart_cfg.rxerr_cb = NULL;                                                                      \
        Uart##num##_PAL.Uart_cfg.timeout_cb = NULL;                                                                    \
        Uart##num##_PAL.Uart_cfg.timeout = 0;                                                                          \
        Uart##num##_PAL.Uart_cfg.speed = 9600;                                                                         \
        Uart##num##_PAL.Uart_cfg.cr1 = 0;                                                                              \
        Uart##num##_PAL.Uart_cfg.cr2 = 0;                                                                              \
        Uart##num##_PAL.Uart_cfg.cr3 = 0;                                                                              \
        Uart##num##_PAL.TxBuffer = Uart##num##_TxBuffer;                                                               \
        Uart##num##_PAL.TxRingBuffer.Initialize(Uart##num##_PAL.TxBuffer, tx_buffer_size);                             \
        Uart##num##_PAL.TxOngoingCount = 0;                                                                            \
        Uart##num##_PAL.RxBuffer = Uart##num##_RxBuffer;                                                               \
        Uart##num##_PAL.RxRingBuffer.Initialize(Uart##num##_PAL.RxBuffer, rx_buffer_size);                             \
        Uart##num##_PAL.WatchChar = 0;                                                                                 \
    }

#else

// all other STM32F use UART driver v1 which has a different UARTConfig struct
#define UART_INIT(num, tx_buffer_size, rx_buffer_size)                                                                 \
    void Init_UART##num()                                                                                              \
    {                                                                                                                  \
        Uart##num##_PAL.Uart_cfg.txend2_cb = NULL;                                                                     \
        Uart##num##_PAL.Uart_cfg.rxend_cb = NULL;                                                                      \
        Uart##num##_PAL.Uart_cfg.rxerr_cb = NULL;                                                                      \
        Uart##num##_PAL.Uart_cfg.speed = 9600;                                                                         \
        Uart##num##_PAL.Uart_cfg.cr1 = 0;                                                                              \
        Uart##num##_PAL.Uart_cfg.cr2 = 0;                                                                              \
        Uart##num##_PAL.Uart_cfg.cr3 = 0;                                                                              \
        Uart##num##_PAL.TxBuffer = Uart##num##_TxBuffer;                                                               \
        Uart##num##_PAL.TxRingBuffer.Initialize(Uart##num##_PAL.TxBuffer, tx_buffer_size);                             \
        Uart##num##_PAL.TxOngoingCount = 0;                                                                            \
        Uart##num##_PAL.RxBuffer = Uart##num##_RxBuffer;                                                               \
        Uart##num##_PAL.RxRingBuffer.Initialize(Uart##num##_PAL.RxBuffer, rx_buffer_size);                             \
        Uart##num##_PAL.WatchChar = 0;                                                                                 \
    }

#endif

// when a UART/USART is defined the declarations below will have the real function/configuration
// in the target folder @ target_windows_devices_serialcommunication_config.cpp
void Init_UART1();
void Init_UART2();
void Init_UART3();
void Init_UART4();
void Init_UART5();
void Init_UART6();
void Init_UART7();
void Init_UART8();

// the following macro defines a function that un initializes an UART struct
// it gets called in the Windows_Devices_SerialCommunication_SerialDevice::NativeDispose function
#define UART_UNINIT(num)                                                                                               \
    void UnInit_UART##num()                                                                                            \
    {                                                                                                                  \
        return;                                                                                                        \
    }

// when a UART/USART is defined the declarations below will have the real function/configuration
// in the target folder @ target_windows_devices_serialcommunication_config.cpp
void UnInit_UART1();
void UnInit_UART2();
void UnInit_UART3();
void UnInit_UART4();
void UnInit_UART5();
void UnInit_UART6();
void UnInit_UART7();
void UnInit_UART8();

#endif //_WIN_DEV_SERIAL_NATIVE_TARGET_H_
