//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef WIN_DEV_ADC_NATIVE_TARGET_H
#define WIN_DEV_ADC_NATIVE_TARGET_H

#include <hal.h>
#include <win_dev_adc_native.h>

typedef struct
{
    uint8_t adcIndex;
    stm32_gpio_t *portId;
    uint8_t pin;
    uint32_t adcChannel;
} NF_PAL_ADC_PORT_PIN_CHANNEL__;

extern const NF_PAL_ADC_PORT_PIN_CHANNEL__ AdcPortPinConfig__[];
extern const int AdcChannelCount__;

#endif // WIN_DEV_ADC_NATIVE_TARGET_H
