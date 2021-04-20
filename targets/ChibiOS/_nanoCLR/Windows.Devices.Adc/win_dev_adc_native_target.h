//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _WIN_DEV_ADC_NATIVE_TARGET_H_
#define _WIN_DEV_ADC_NATIVE_TARGET_H_

#include <hal.h>
#include <win_dev_adc_native.h>

typedef struct
{
    uint8_t adcIndex;
    stm32_gpio_t *portId;
    uint8_t pin;
    uint32_t adcChannel;
} NF_PAL_ADC_PORT_PIN_CHANNEL;

extern const NF_PAL_ADC_PORT_PIN_CHANNEL AdcPortPinConfig[];
extern const int AdcChannelCount;

#endif //_WIN_DEV_ADC_NATIVE_TARGET_H_
