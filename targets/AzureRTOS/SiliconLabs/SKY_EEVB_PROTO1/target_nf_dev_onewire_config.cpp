//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "target_nf_dev_onewire_config.h"
#include <nf_dev_onewire_target.h>

///////////
// UART0 //
///////////

// pin configuration for UART0
// UART0 TX: PE10, location 1
// UART0 TX: PE11, location 1
// EMC encoded "port location", see Alternate Functionality Overview table in MCU datasheet

UART_CONFIG_PINS(0, 0, 0)
