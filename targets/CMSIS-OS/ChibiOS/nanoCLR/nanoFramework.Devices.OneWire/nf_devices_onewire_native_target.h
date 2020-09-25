//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _NF_DEVICES_ONEWIRE_NATIVE_TARGET_H_
#define _NF_DEVICES_ONEWIRE_NATIVE_TARGET_H_

#include <nf_devices_onewire_native.h>
#include <hal.h>
#include <hal_nf_community.h>

// struct with parameters for 1-Wire working thread
struct OneWireFindStruct
{
    bool DoReset;
    bool AlarmOnly;
};

// the following macro defines a function that configures the GPIO pins for a STM32 UART/USART
// it gets called in the oneWire_lld_start function
// this is required because the UART/USART peripherals can use multiple GPIO configuration combinations
#define UART_CONFIG_PINS(num, gpio_port_tx, tx_pin, alternate_function) void ConfigPins_UART##num() { \
    palSetPadMode(gpio_port_tx, tx_pin, PAL_MODE_ALTERNATE(alternate_function) | PAL_STM32_OTYPE_OPENDRAIN \
    | PAL_MODE_INPUT_PULLUP | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_MODE_ALTERNATE); \
}

#endif  //_NF_DEVICES_ONEWIRE_NATIVE_TARGET_H_
