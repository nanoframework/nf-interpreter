//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nano_gg_adc_native_target.h>

// PE11- ADC0 Port 4X Channel 11 (Expansion Header Pin 6)

const NF_PAL_ADC_PORT_PIN_CHANNEL AdcPortPinConfig[] = {

    // ADC0
    {0, adcPosSelAPORT4XCH11},

    // these are the internal sources
    {0, adcPosSelTEMP},
    {0, adcPosSelAVDD},    
};

const int AdcChannelCount = ARRAYSIZE(AdcPortPinConfig);
