//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NF_DEVICE_CAN_NATIVE_TARGET_H
#define NF_DEVICE_CAN_NATIVE_TARGET_H

#include <hal.h>
#include "target_nf_device_can_config.h"
#include <nf_device_can_native.h>

// struct representing the CAN
typedef struct
{
    CANDriver *Driver;
    CANConfig Configuration;
    thread_t *ReceiverThread;

    HAL_RingBuffer<CANRxFrame> MsgRingBuffer;
    CANRxFrame *MsgBuffer;
} NF_PAL_CAN;

///////////////////////////////////////////
// declaration of the CAN PAL strucs     //
///////////////////////////////////////////
#if defined(STM32_CAN_USE_CAN1) && (STM32_CAN_USE_CAN1 == TRUE)
extern NF_PAL_CAN Can1_PAL;
#endif
#if defined(STM32_CAN_USE_CAN2) && (STM32_CAN_USE_CAN2 == TRUE)
extern NF_PAL_CAN Can2_PAL;
#endif
#if defined(STM32_CAN_USE_CAN3) && (STM32_CAN_USE_CAN3 == TRUE)
extern NF_PAL_CAN Can3_PAL;
#endif

// the following macro defines a function that configures the GPIO pins for a STM32 CAN
// it gets called in the can_lld_start function// this is required because the CAN peripherals can use multiple GPIO
// configuration combinations
#define CAN_CONFIG_PINS(num, gpio_port_tx, gpio_port_rx, tx_pin, rx_pin, alternate_function)                           \
    void ConfigPins_CAN##num()                                                                                         \
    {                                                                                                                  \
        palSetPadMode(                                                                                                 \
            gpio_port_tx,                                                                                              \
            tx_pin,                                                                                                    \
            PAL_MODE_ALTERNATE(alternate_function) | PAL_STM32_OTYPE_PUSHPULL | PAL_STM32_OSPEED_HIGHEST |             \
                PAL_STM32_MODE_ALTERNATE);                                                                             \
        palSetPadMode(                                                                                                 \
            gpio_port_rx,                                                                                              \
            rx_pin,                                                                                                    \
            PAL_MODE_ALTERNATE(alternate_function) | PAL_STM32_OTYPE_PUSHPULL | PAL_STM32_OSPEED_HIGHEST |             \
                PAL_STM32_MODE_ALTERNATE);                                                                             \
    }

///////////////////////////////////////////////////////////////////////////////////////////
// when a CAN is defined the declarations below will have the real function/configuration
// in the target folder @ target_nf_device_can_config.cpp
///////////////////////////////////////////////////////////////////////////////////////////
void ConfigPins_CAN1();
void ConfigPins_CAN2();
void ConfigPins_CAN3();

/////////////////////////////////////
// CAN Msg buffers                 //
// these live in the target folder //
/////////////////////////////////////
extern CANRxFrame Can1_MsgBuffer[];
extern CANRxFrame Can2_MsgBuffer[];
extern CANRxFrame Can3_MsgBuffer[];

//
#define CAN_INIT(num, buffer_size)                                                                                     \
    void Init_Can##num()                                                                                               \
    {                                                                                                                  \
        Can##num##_PAL.MsgBuffer = Can##num##_MsgBuffer;                                                               \
        Can##num##_PAL.MsgRingBuffer.Initialize(Can##num##_PAL.MsgBuffer, buffer_size);                                \
    }

// when a CAN is defined the declarations below will have the real function/configuration
// in the target folder @ target_nf_device_can_config.cpp
void Init_Can1();
void Init_Can2();
void Init_Can3();

#endif // NF_DEVICE_CAN_NATIVE_TARGET_H
