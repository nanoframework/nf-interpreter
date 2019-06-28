//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_dac_native_target.h"

const NF_PAL_DAC_PORT_PIN_CHANNEL DacPortPinConfig[] = {
    
    // ADC1
    {1, GPIOA, 6, ADC_CHANNEL_IN6},
    {1, GPIOA, 4, ADC_CHANNEL_IN4},
    {1, GPIOC, 2, ADC_CHANNEL_IN12},
    {1, GPIOF, 10, ADC_CHANNEL_IN8},

    // ADC3
    {3, GPIOF, 8, ADC_CHANNEL_IN6},

    // these are the internal sources, available only at ADC1
    {1, NULL, 0, ADC_CHANNEL_SENSOR},
    {1, NULL, 0, ADC_CHANNEL_VREFINT},
    {1, NULL, 0, ADC_CHANNEL_VBAT},
};

const int DacChannelCount = ARRAYSIZE(DacPortPinConfig);
