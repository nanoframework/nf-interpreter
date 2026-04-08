//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_adc_native_target.h>

// RP2040 Pico ADC channel configuration
// Ch 0: GPIO26 (ADC0) - Pico pin 31
// Ch 1: GPIO27 (ADC1) - Pico pin 32
// Ch 2: GPIO28 (ADC2) - Pico pin 34
// Ch 3: GPIO29 (ADC3) - Pico pin  (used for VSYS/3 on Pico board)
// Ch 4: Internal temperature sensor
const NF_PAL_ADC_PORT_PIN_CHANNEL AdcPortPinConfig[] = {
    {1, 26, ADC_CHANNEL_IN0},
    {1, 27, ADC_CHANNEL_IN1},
    {1, 28, ADC_CHANNEL_IN2},
    {1, 29, ADC_CHANNEL_IN3},
    {1, 0xFF, ADC_CHANNEL_TEMPSENSOR},
};

const int AdcChannelCount = sizeof(AdcPortPinConfig) / sizeof(AdcPortPinConfig[0]);
