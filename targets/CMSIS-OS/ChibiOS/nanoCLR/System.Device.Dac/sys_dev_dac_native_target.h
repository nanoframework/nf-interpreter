//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _SYS_DEV_DAC_NATIVE_TARGET_H_
#define _SYS_DEV_DAC_NATIVE_TARGET_H_

#include <hal.h>
#include <sys_dev_dac_native.h>

struct NF_PAL_DAC_PORT_PIN_CHANNEL
{
    // Index of DAC converter
    // on most STM32 devices can be converter 1 or 2
    uint8_t dacIndex;

    // DAC channel
    // on most STM32 devices can be channel 1 or  2
    uint8_t dacChannel;

    // GPIO port ID
    stm32_gpio_t *portId;

    // GPIO pin of the DAC channel
    uint8_t pin;
};

// struct representing the DAC controller
struct NF_PAL_DAC
{
    DACDriver *Driver;
    DACConfig Config;
    uint8_t Channel;
};

////////////////////////////////////////////
// declaration of the the DAC PAL strucs //
////////////////////////////////////////////
#if STM32_DAC_USE_DAC1_CH1
extern NF_PAL_DAC Dac1_1_PAL;
#endif
#if STM32_DAC_USE_DAC1_CH2
extern NF_PAL_DAC Dac1_2_PAL;
#endif
#if STM32_DAC_USE_DAC2_CH1
extern NF_PAL_DAC Dac2_1_PAL;
#endif
#if STM32_DAC_USE_DAC2_CH2
extern NF_PAL_DAC Dac2_2_PAL;
#endif

// there is a driver for each pair DAC converter + channel
// let's code the options with DAC controller on the tens place and the channel on the ones place
// like this: channel 2 of DAC controller 1 being => (1 * 10) + 12 = 12
#define ENCODED_DAC_REF(dac, channel) (dac * 10 + channel)

extern const NF_PAL_DAC_PORT_PIN_CHANNEL DacPortPinConfig[];
extern const int DacChannelCount;

#endif //_SYS_DEV_DAC_NATIVE_TARGET_H_
