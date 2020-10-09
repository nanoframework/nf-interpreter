//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <win_dev_adc_native_target.h>

const NF_PAL_ADC_PORT_PIN_CHANNEL AdcPortPinConfig[] = {
    
    // ADC1
    {1, GPIOA, 3, ADC_CHANNEL_IN3},
    {1, GPIOA, 4, ADC_CHANNEL_IN4},
    {1, GPIOB, 0, ADC_CHANNEL_IN8},
    {1, GPIOB, 1, ADC_CHANNEL_IN9},
    {1, GPIOC, 2, ADC_CHANNEL_IN12},
    {1, GPIOC, 3, ADC_CHANNEL_IN13},

    // ADC3
    {3, GPIOF, 8, ADC_CHANNEL_IN6},
    {3, GPIOF, 9, ADC_CHANNEL_IN7},

    // these are the internal sources, available only at ADC1
    {1, NULL, 0, ADC_CHANNEL_SENSOR},
    {1, NULL, 0, ADC_CHANNEL_VREFINT},
    {1, NULL, 0, ADC_CHANNEL_VBAT},
};

const int AdcChannelCount = ARRAYSIZE(AdcPortPinConfig);
