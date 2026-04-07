//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NANO_GG_ADC_NATIVE_TARGET_H
#define NANO_GG_ADC_NATIVE_TARGET_H

#include <em_device.h>
#include <em_adc.h>
#include "em_cmu.h"

#include "nano_gg_adc_native.h"
#include <target_nano_gg_adc_config.h>

// set missing defines
#ifndef GECKO_USE_ADC0
#define GECKO_USE_ADC0 FALSE
#endif
#ifndef GECKO_USE_ADC1
#define GECKO_USE_ADC1 FALSE
#endif
#ifndef GECKO_USE_ADC2
#define GECKO_USE_ADC2 FALSE
#endif
#ifndef GECKO_USE_ADC3
#define GECKO_USE_ADC3 FALSE
#endif

typedef struct
{
    uint8_t adcIndex;
    ADC_PosSel_TypeDef posSel;
} NF_PAL_ADC_PORT_PIN_CHANNEL;

typedef struct
{
    uint8_t channelCount;
    uint32_t *dataBuffer;
    uint32_t averageCount;
    uint32_t currentIndex;
} NF_PAL_GECKO_ADC_CONTINUOUS_SCAN;


extern const NF_PAL_ADC_PORT_PIN_CHANNEL AdcPortPinConfig[];
extern const int AdcChannelCount;
extern NF_PAL_GECKO_ADC_CONTINUOUS_SCAN* ContinuousScanOperation;

#if GECKO_USE_ADC0
extern ADC_Init_TypeDef adc0Init;
extern bool adc0Initialized;
#endif
#if GECKO_USE_ADC1
extern ADC_Init_TypeDef adc1Init;
extern bool adc1Initialized;
#endif


#endif // NANO_GG_ADC_NATIVE_TARGET_H
