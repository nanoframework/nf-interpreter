//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "win_dev_adc_native.h"

//////////
// ADC1 //
//////////

const NF_PAL_ADC_PORT_PIN_CHANNEL Adc1PortPinConfig[] = {
    {GPIOA, 6, ADC_CHANNEL_IN6},
    {GPIOA, 4, ADC_CHANNEL_IN4},
    {GPIOC, 2, ADC_CHANNEL_IN12},
    {GPIOF, 10, ADC_CHANNEL_IN8},
    {NULL, NULL, ADC_CHANNEL_SENSOR},
    {NULL, NULL, ADC_CHANNEL_VREFINT},
    {NULL, NULL, ADC_CHANNEL_VBAT},
};

const int Adc1ChannelCount = ARRAYSIZE(Adc1PortPinConfig);

//////////
// ADC3 //
//////////

const NF_PAL_ADC_PORT_PIN_CHANNEL Adc3PortPinConfig[] = {
    {GPIOF, 8, ADC_CHANNEL_IN6},
    {GPIOB, 8, ADC_CHANNEL_IN7},
};

const int Adc3ChannelCount = ARRAYSIZE(Adc3PortPinConfig);
