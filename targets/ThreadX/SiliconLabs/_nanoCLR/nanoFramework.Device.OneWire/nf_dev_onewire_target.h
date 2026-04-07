//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _NF_DEV_ONEWIRE_TARGET_H_
#define _NF_DEV_ONEWIRE_TARGET_H_

#include <nf_dev_onewire.h>
#include <em_device.h>
#include <em_gpio.h>

#include <tx_api.h>

// set missing defines
#if defined(USART0)
#ifndef NF_ONEWIRE_USE_USART0
#define NF_ONEWIRE_USE_USART0 FALSE
#endif
#else
#define NF_ONEWIRE_USE_USART0 FALSE
#endif

#if defined(USART1)
#ifndef NF_ONEWIRE_USE_USART1
#define NF_ONEWIRE_USE_USART1 FALSE
#endif
#else
#define NF_ONEWIRE_USE_USART1 FALSE
#endif

#if defined(USART2)
#ifndef NF_ONEWIRE_USE_USART2
#define NF_ONEWIRE_USE_USART2 FALSE
#endif
#else
#define NF_ONEWIRE_USE_USART2 FALSE
#endif

#if defined(USART3)
#ifndef NF_ONEWIRE_USE_USART3
#define NF_ONEWIRE_USE_USART3 FALSE
#endif
#else
#define NF_ONEWIRE_USE_USART3 FALSE
#endif

#if defined(USART4)
#ifndef NF_ONEWIRE_USE_USART4
#define NF_ONEWIRE_USE_USART4 FALSE
#endif
#else
#define NF_ONEWIRE_USE_USART4 FALSE
#endif

#if defined(USART5)
#ifndef NF_ONEWIRE_USE_USART5
#define NF_ONEWIRE_USE_USART5 FALSE
#endif
#else
#define NF_ONEWIRE_USE_USART5 FALSE
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

#include "sl_iostream.h"
#ifdef __cplusplus
extern "C" {
#endif

sl_status_t sl_iostream_usart_init_onewire(void);

#ifdef __cplusplus
}
#endif

#endif // _NF_DEV_ONEWIRE_TARGET_H_
