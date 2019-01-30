//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _NF_DEVICES_ONEWIRE_NATIVE_H_
#define _NF_DEVICES_ONEWIRE_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <hal.h>
#include <hal_nf_community.h>

struct Library_nf_devices_onewire_native_nanoFramework_Devices_OneWire_OneWireController
{
    static const int FIELD___syncLock = 1;
    static const int FIELD___serialNumber = 2;

    NANOCLR_NATIVE_DECLARE(TouchReset___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(TouchBit___BOOLEAN__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(TouchByte___U1__U1);
    NANOCLR_NATIVE_DECLARE(WriteByte___U1__U1);
    NANOCLR_NATIVE_DECLARE(ReadByte___U1);
    NANOCLR_NATIVE_DECLARE(FindFirstDevice___BOOLEAN__BOOLEAN__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(FindNextDevice___BOOLEAN__BOOLEAN__BOOLEAN);

    //--//

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Devices_OneWire;

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

#endif  //_NF_DEVICES_ONEWIRE_NATIVE_H_
