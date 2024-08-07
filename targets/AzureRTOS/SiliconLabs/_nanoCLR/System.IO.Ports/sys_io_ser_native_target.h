//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef SYS_IO_SER_NATIVE_TARGET_H
#define SYS_IO_SER_NATIVE_TARGET_H

#include <target_system_io_ports_config.h>
#include <sys_io_ser_native.h>

#include <em_device.h>
#include <em_cmu.h>
#include <em_usart.h>

// set missing defines
#if defined(USART0)
#ifndef GECKO_USE_USART0
#define GECKO_USE_USART0 FALSE
#endif
#else
#define GECKO_USE_USART0 FALSE
#endif

#if defined(USART1)
#ifndef GECKO_USE_USART1
#define GECKO_USE_USART1 FALSE
#endif
#else
#define GECKO_USE_USART1 FALSE
#endif

#if defined(USART2)
#ifndef GECKO_USE_USART2
#define GECKO_USE_USART2 FALSE
#endif
#else
#define GECKO_USE_USART2 FALSE
#endif

#if defined(USART3)
#ifndef GECKO_USE_USART3
#define GECKO_USE_USART3 FALSE
#endif
#else
#define GECKO_USE_USART3 FALSE
#endif

#if defined(USART4)
#ifndef GECKO_USE_USART4
#define GECKO_USE_USART4 FALSE
#endif
#else
#define GECKO_USE_USART4 FALSE
#endif

#if defined(USART5)
#ifndef GECKO_USE_USART5
#define GECKO_USE_USART5 FALSE
#endif
#else
#define GECKO_USE_USART5 FALSE
#endif

// struct representing the UART
typedef struct
{
    USART_TypeDef *Usart;
    USART_InitAsync_TypeDef UsartInit;

    uint8_t *TxBuffer;
    uint16_t TxOngoingCount;

    HAL_RingBuffer<uint8_t> RxRingBuffer;
    uint8_t *RxBuffer;
    uint16_t RxBytesToRead;

    uint8_t WatchChar;
    uint8_t NewLineChar;
    uint32_t ReceivedBytesThreshold;

    bool SignalLevelsInverted;

} NF_PAL_UART;

////////////////////////////////////////////
// declaration of the the UART PAL structs //
////////////////////////////////////////////
#if defined(GECKO_USE_USART0) && (GECKO_USE_USART0 == TRUE)
extern NF_PAL_UART Uart0_PAL;
#endif
#if defined(GECKO_USE_USART1) && (GECKO_USE_USART1 == TRUE)
extern NF_PAL_UART Uart1_PAL;
#endif
#if defined(GECKO_USE_USART2) && (GECKO_USE_USART2 == TRUE)
extern NF_PAL_UART Uart2_PAL;
#endif
#if defined(GECKO_USE_USART3) && (GECKO_USE_USART3 == TRUE)
extern NF_PAL_UART Uart3_PAL;
#endif
#if defined(GECKO_USE_USART_UART4) && (GECKO_USE_USART_UART4 == TRUE)
extern NF_PAL_UART Uart4_PAL;
#endif
#if defined(GECKO_USE_USART_UART5) && (GECKO_USE_USART_UART5 == TRUE)
extern NF_PAL_UART Uart5_PAL;
#endif

// the following macro defines a function that configures the GPIO pins for a Gecko USART
// it gets called in the System_IO_Ports_SerialPort::NativeConfig function
// this is required because the UART/USART peripherals can use multiple GPIO configuration combinations
#define UART_INIT_CONFIG(num, gpio_port_tx, tx_pin, tx_location, gpio_port_rx, rx_pin, rx_location)                    \
    void InitConfig_USART##num()                                                                                       \
    {                                                                                                                  \
        CMU_ClockEnable(cmuClock_USART##num, true);                                                                    \
        GPIO_PinModeSet(gpio_port_rx, rx_pin, gpioModeInput, 0);                                                       \
        GPIO_PinModeSet(gpio_port_tx, tx_pin, gpioModePushPull, 1);                                                    \
        USART_InitAsync(USART##num, &Uart##num##_PAL.UsartInit);                                                       \
        NVIC_ClearPendingIRQ(USART##num##_RX_IRQn);                                                                    \
        NVIC_EnableIRQ(USART##num##_RX_IRQn);                                                                          \
        NVIC_ClearPendingIRQ(USART##num##_TX_IRQn);                                                                    \
        NVIC_EnableIRQ(USART##num##_TX_IRQn);                                                                          \
        Uart##num##_PAL.Usart = USART##num;                                                                            \
        USART##num->ROUTELOC0 = rx_location | tx_location;                                                             \
        USART##num->ROUTEPEN |= USART_ROUTEPEN_TXPEN | USART_ROUTEPEN_RXPEN;                                           \
        Uart##num##_PAL.TxBuffer = NULL;                                                                               \
        Uart##num##_PAL.TxOngoingCount = 0;                                                                            \
        Uart##num##_PAL.RxBuffer = NULL;                                                                               \
        Uart##num##_PAL.WatchChar = 0;                                                                                 \
        Uart##num##_PAL.NewLineChar = 0;                                                                               \
        Uart##num##_PAL.SignalLevelsInverted = false;                                                                  \
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
// when a UART/USART is defined the declarations below will have the real function/configuration
// in the target folder @ target_system_io_ports_config.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
void InitConfig_USART0();
void InitConfig_USART1();
void InitConfig_USART2();
void InitConfig_USART3();
void InitConfig_USART4();
void InitConfig_USART5();

// the following macro defines a function that un initializes an UART struct
// it gets called in the System_IO_Ports_SerialPort::NativeDispose function
#define UART_UNINIT(num, gpio_port_tx, tx_pin, tx_location, gpio_port_rx, rx_pin, rx_location)                         \
    void UnInit_UART##num()                                                                                            \
    {                                                                                                                  \
        USART_Reset(USART##num);                                                                                       \
        GPIO_PinModeSet(gpio_port_rx, rx_pin, gpioModeDisabled, 0);                                                    \
        GPIO_PinModeSet(gpio_port_tx, tx_pin, gpioModeDisabled, 0);                                                    \
        CMU_ClockEnable(cmuClock_USART##num, false);                                                                   \
        platform_free(Uart##num##_PAL.RxBuffer);                                                                       \
        Uart##num##_PAL.TxBuffer = NULL;                                                                               \
        Uart##num##_PAL.RxBuffer = NULL;                                                                               \
        Uart##num##_PAL.Usart = NULL;                                                                                  \
        return;                                                                                                        \
    }

// when a UART/USART is defined the declarations below will have the real function/configuration
// in the target folder @ target_system_io_ports_config.cpp
void UnInit_UART0();
void UnInit_UART1();
void UnInit_UART2();
void UnInit_UART3();
void UnInit_UART4();
void UnInit_UART5();

#endif // SYS_IO_SER_NATIVE_TARGET_H
