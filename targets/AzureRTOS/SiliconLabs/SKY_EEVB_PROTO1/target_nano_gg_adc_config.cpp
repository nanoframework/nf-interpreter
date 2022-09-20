//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nano_gg_adc_native_target.h>

// PA15: ADC0 APORT2X Channel 15
// PE15: ADC0 APORT4X Channel 15
// PE14: ADC0 APORT4Y Channel 14
// PE13: ADC0 APORT4X Channel 13
// PE12: ADC0 APORT4Y Channel 12
// PE11: ADC0 APORT4X Channel 11

const NF_PAL_ADC_PORT_PIN_CHANNEL AdcPortPinConfig[] = {

    // MCU_ADC1
    {0, adcPosSelAPORT2XCH15},
    // MCU_ADC2
    {0, adcPosSelAPORT4XCH15},
    // MCU_ADC3
    {0, adcPosSelAPORT4YCH14},
    // MCU_ADC4
    {0, adcPosSelAPORT4XCH13},
    // MCU_ADC5
    {0, adcPosSelAPORT4YCH12},
    // MCU_ADC6
    {0, adcPosSelAPORT4XCH11},

    // these are the internal sources
    {0, adcPosSelTEMP},
    {0, adcPosSelAVDD},    
};

const int AdcChannelCount = ARRAYSIZE(AdcPortPinConfig);
