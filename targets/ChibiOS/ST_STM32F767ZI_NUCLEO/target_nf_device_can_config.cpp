//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <hal.h>
#include "target_nf_device_can_config.h"
#include <nf_device_can_native_target.h>

///////////
// CAN1 //
///////////

// pin configuration for CAN1
// port for TX pin is: GPIOD
// TX pin: is GPIOD_01
// port for RX pin is: GPIOD
// RX pin: is GPIOD_00
// GPIO alternate pin function is 9 (see alternate function mapping table in device datasheet)
CAN_CONFIG_PINS(1, GPIOD, GPIOD, 1, 0, 9)

// buffer
CANRxFrame Can1_MsgBuffer[CAN1_RX_BUFFER_SIZE];

// initialization for CAN1
CAN_INIT(1, CAN1_RX_BUFFER_SIZE)

///////////
// CAN2 //
///////////

// NOT AVAILABLE
