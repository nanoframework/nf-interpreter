//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <hal.h>
#include "target_nf_devices_can_config.h"
#include <nf_devices_can_native_target.h>

///////////
// CAN1 //
///////////

// pin configuration for CAN1
// port for TX pin is: GPIOB
// TX pin: is GPIOB_09
// port for RX pin is: GPIOB
// RX pin: is GPIOB_08
// GPIO alternate pin function is 9 (see "Table 9. STM32F405xx and STM32F407xx alternate function mapping" in STM32F405xx/STM32F407xx datasheet)
CAN_CONFIG_PINS(1, GPIOB, GPIOB, 9, 8, 9)

// buffer
CANRxFrame Can1_MsgBuffer[CAN1_RX_BUFFER_SIZE];

// initialization for CAN1
CAN_INIT(1, CAN1_RX_BUFFER_SIZE)

///////////
// CAN2 //
///////////

// pin configuration for CAN2
// port for TX pin is: GPIOB
// TX pin: is GPIOB_06
// port for RX pin is: GPIOB
// RX pin: is GPIOB_05
// GPIO alternate pin function is 9 (see "Table 9. STM32F405xx and STM32F407xx alternate function mapping" in STM32F405xx/STM32F407xx datasheet)
CAN_CONFIG_PINS(2, GPIOB, GPIOB, 6, 5, 9)

// buffer
CANRxFrame Can2_MsgBuffer[CAN2_RX_BUFFER_SIZE];

// initialization for CAN1
CAN_INIT(2, CAN2_RX_BUFFER_SIZE)
