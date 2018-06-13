//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "win_dev_adc_native.h"

const NF_PAL_ADC_PORT_PIN_CHANNEL AdcPortPinConfig[] = {
    
    // ADC1
    {1, GPIOA, 3, ADC_CHANNEL_IN3},
    {1, GPIOC, 0, ADC_CHANNEL_IN10},
    {1, GPIOC, 3, ADC_CHANNEL_IN13},

    // ADC3
    {3, GPIOF, 3, ADC_CHANNEL_IN9},
    {3, GPIOF, 5, ADC_CHANNEL_IN15},
    {3, GPIOF, 10, ADC_CHANNEL_IN8},

    // ADC2/ADC3
    // Fill up with rest of set ADC pins
    {2, GPIOB, 1, ADC_CHANNEL_IN9},
    {2, GPIOC, 2, ADC_CHANNEL_IN12},
    {3, GPIOF, 4, ADC_CHANNEL_IN14},
    
    // these are the internal sources, available only at ADC1
    {1, NULL, 0, ADC_CHANNEL_SENSOR},
    {1, NULL, 0, ADC_CHANNEL_VREFINT},
    {1, NULL, 0, ADC_CHANNEL_VBAT},
};

const int AdcChannelCount = ARRAYSIZE(AdcPortPinConfig);
