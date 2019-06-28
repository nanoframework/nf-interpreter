//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_dac_native_target.h>


const NF_PAL_DAC_PORT_PIN_CHANNEL DacPortPinConfig[] = {
    
    // ADC1
    {1, GPIOC, 0, ADC_CHANNEL_IN10},
    {1, GPIOC, 1, ADC_CHANNEL_IN11},

    // ADC2
    {2, GPIOC, 2, ADC_CHANNEL_IN14},
    {2, GPIOC, 3, ADC_CHANNEL_IN15},

    // ADC3
    {3, GPIOC, 4, ADC_CHANNEL_IN12},
    {3, GPIOC, 5, ADC_CHANNEL_IN13},

    // these are the internal sources, available only at ADC1
    {1, NULL, 0, ADC_CHANNEL_SENSOR},
    {1, NULL, 0, ADC_CHANNEL_VREFINT},
    {1, NULL, 0, ADC_CHANNEL_VBAT},
};

const int DacChannelCount = ARRAYSIZE(DacPortPinConfig);
