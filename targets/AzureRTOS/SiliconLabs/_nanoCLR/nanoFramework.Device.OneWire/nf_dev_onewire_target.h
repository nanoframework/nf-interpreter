//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _NF_DEV_ONEWIRE_TARGET_H_
#define _NF_DEV_ONEWIRE_TARGET_H_

#include <nf_dev_onewire.h>
#include <em_device.h>
#include <em_gpio.h>

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

// struct with parameters for 1-Wire working thread
struct OneWireFindStruct
{
    bool DoReset;
    bool AlarmOnly;
    bool FindFirst;
};

// @brief   Driver state machine possible states.
typedef enum
{
    // Not initialized.
    ONEWIRE_UNINIT,
    // Stopped.
    ONEWIRE_STOP,
    // Ready.
    ONEWIRE_READY,
    // Generating random number.
    ONEWIRE_ACTIVE
} oneWireState;

// character to send on the UART to mimic 1-Wire bus signals
// 1-Wire write 0 time slot
#define IWIRE_WR0 0x00
// 1-Wire write 1 time slot
#define IWIRE_WR1 0xFF
// 1-Wire read time slot
#define IWIRE_RD 0xFF

// ROM commands
// search ROM
#define SEARCH_ROM 0xF0
// conditional search ROM
#define COND_SEARCH_ROM 0xEC

#if defined(_USART_ROUTELOC0_MASK)

// the following macro defines a function that configures the GPIO pins for an Gecko USART peripheral
// it gets called in the oneWire_lld_start function
// this is required because the USART peripherals can use multiple GPIO configuration combinations
#define INIT_ONEWIRE_CONFIG(num, usart_tx_port_location, usart_rx_location)                                            \
    void InitSpiConfig##num(NF_SpiDriver_Init_t &initSpiData, bool isHalfDuplex)                                       \
    {                                                                                                                  \
        initSpiData.port = USART##num;                                                                                 \
        initSpiData.portLocationTx = mosi_port_location;                                                               \
        initSpiData.portLocationClk = sck_port_location;                                                               \
        if (!isHalfDuplex)                                                                                             \
        {                                                                                                              \
            initSpiData.portLocationRx = miso_port_location;                                                           \
        }                                                                                                              \
    }

#else
#error "This routing configuration is not supported. Need to have _USART_ROUTELOC0_MASK."
#endif

#endif // _NF_DEV_ONEWIRE_TARGET_H_
