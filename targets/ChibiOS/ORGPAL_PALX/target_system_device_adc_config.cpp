//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_adc_native_target.h>

const NF_PAL_ADC_PORT_PIN_CHANNEL AdcPortPinConfig[] = {

    // ADC1
    {1, GPIOA, 2, ADC_CHANNEL_IN2}, // Channel 0, Thermistor port Input
    {1, GPIOB, 0, ADC_CHANNEL_IN8}, // Channel 1, Battery port input.
    {1, GPIOC, 2, ADC_CHANNEL_IN12}, // Channel 2, 420MA Current sensor port input.
    {1, GPIOC, 3, ADC_CHANNEL_IN13}, // Channel 3, Onboard PCB temperature.

    // These are the internal sources, available only at ADC1
    // and are added to the end.
    {1, NULL, 0, ADC_CHANNEL_SENSOR}, // Channel 4, internal MCU temperature.
    {1, NULL, 0, ADC_CHANNEL_VREFINT}, //Channel 5, internal MCU voltage reference
    {1, NULL, 0, ADC_CHANNEL_VBAT}, // Channel 6, internal MCU vbat reference.
};

const int AdcChannelCount = ARRAYSIZE(AdcPortPinConfig);
