//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef SYS_DEV_ADC_NATIVE_TARGET_H
#define SYS_DEV_ADC_NATIVE_TARGET_H

#include <sys_dev_adc_native.h>
#include <em_device.h>
#include <em_adc.h>
#include "em_cmu.h"

#include <target_system_device_adc_config.h>

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

extern const NF_PAL_ADC_PORT_PIN_CHANNEL AdcPortPinConfig[];
extern const int AdcChannelCount;

#endif // SYS_DEV_ADC_NATIVE_TARGET_H
