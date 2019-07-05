//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#ifndef _SYS_DEV_DAC_NATIVE_TARGET_H_
#define _SYS_DEV_DAC_NATIVE_TARGET_H_

#include <hal.h>
#include <sys_dev_dac_native.h>

struct NF_PAL_DAC_PORT_PIN_CHANNEL
{
    uint8_t         dacIndex;
    stm32_gpio_t*   portId;
    uint8_t         pin;
    uint32_t        dacChannel;
};

extern const NF_PAL_DAC_PORT_PIN_CHANNEL DacPortPinConfig[];
extern const int DacChannelCount;

#endif  //_SYS_DEV_DAC_NATIVE_TARGET_H_
