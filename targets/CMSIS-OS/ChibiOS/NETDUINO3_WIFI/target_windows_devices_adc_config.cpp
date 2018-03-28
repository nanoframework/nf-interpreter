//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "win_dev_adc_native.h"

//////////
// ADC1 //
//////////

const NF_PAL_ADC_PORT_PIN_CHANNEL Adc1PortPinConfig[] = {
    {GPIOC, 0, ADC_CHANNEL_IN10},
    {GPIOC, 1, ADC_CHANNEL_IN11},
    {NULL, NULL, ADC_CHANNEL_SENSOR},
    {NULL, NULL, ADC_CHANNEL_VREFINT},
    {NULL, NULL, ADC_CHANNEL_VBAT},
};

const int Adc1ChannelCount = ARRAYSIZE(Adc1PortPinConfig);

//////////
// ADC2 //
//////////

const NF_PAL_ADC_PORT_PIN_CHANNEL Adc2PortPinConfig[] = {
    {GPIOC, 2, ADC_CHANNEL_IN14},
    {GPIOC, 3, ADC_CHANNEL_IN15},
};

const int Adc2ChannelCount = ARRAYSIZE(Adc2PortPinConfig);

//////////
// ADC3 //
//////////

const NF_PAL_ADC_PORT_PIN_CHANNEL Adc3PortPinConfig[] = {
    {GPIOC, 4, ADC_CHANNEL_IN12},
    {GPIOC, 5, ADC_CHANNEL_IN13},
};

const int Adc3ChannelCount = ARRAYSIZE(Adc3PortPinConfig);
