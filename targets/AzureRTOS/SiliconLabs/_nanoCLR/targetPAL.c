//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <em_device.h>
#include <em_gpio.h>

// required for Azure RTOS TX_INTERRUPT_SAVE_AREA implementation at platform level
unsigned int interrupt_save;
