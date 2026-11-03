//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_adc_native_target.h>

const NF_PAL_ADC_PORT_PIN_CHANNEL c_AdcPortPinConfig[] = {

    // ADC1
    {1, GPIOC, 0, ADC_CHANNEL_IN10},
    {1, GPIOA, 0, ADC_CHANNEL_IN16},
    {1, GPIOF, 11, ADC_CHANNEL_IN2},
};

const int c_AdcChannelCount = ARRAYSIZE(c_AdcPortPinConfig);
