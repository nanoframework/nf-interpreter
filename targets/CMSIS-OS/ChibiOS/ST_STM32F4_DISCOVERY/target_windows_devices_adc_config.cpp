//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "win_dev_adc_native.h"

//////////
// ADC1 //
//////////

const NF_PAL_ADC_PORT_PIN_CHANNEL Adc1PortPinConfig[] = {
    {GPIOC, 1, ADC_CHANNEL_IN11},
    {GPIOC, 2, ADC_CHANNEL_IN12},
    {NULL, NULL, ADC_CHANNEL_SENSOR},
    {NULL, NULL, ADC_CHANNEL_VREFINT},
    {NULL, NULL, ADC_CHANNEL_VBAT},
};

const int Adc1ChannelCount = ARRAYSIZE(Adc1PortPinConfig);

//////////
// ADC2 //
//////////

const NF_PAL_ADC_PORT_PIN_CHANNEL Adc2PortPinConfig[] = {
    {GPIOA, 6, ADC_CHANNEL_IN6},
    {GPIOA, 7, ADC_CHANNEL_IN7},
    {GPIOB, 0, ADC_CHANNEL_IN8},
};

const int Adc2ChannelCount = ARRAYSIZE(Adc2PortPinConfig);
