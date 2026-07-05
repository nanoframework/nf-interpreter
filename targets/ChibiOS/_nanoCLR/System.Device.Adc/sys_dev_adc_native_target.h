//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef SYS_DEV_ADC_NATIVE_TARGET_H
#define SYS_DEV_ADC_NATIVE_TARGET_H

#include <hal.h>
#include <sys_dev_adc_native.h>

#if defined(RP_ADC_USE_ADC1)

// RP2040 ADC: single ADC with static channel map.
typedef struct
{
    uint8_t adcIndex;
    uint8_t gpio; // GPIO number (26-29) or 0xFF for internal channels.
    uint32_t adcChannel;
} NF_PAL_ADC_PORT_PIN_CHANNEL;

#else

typedef struct
{
    uint8_t adcIndex;
    stm32_gpio_t *portId;
    uint8_t pin;
    uint32_t adcChannel;
} NF_PAL_ADC_PORT_PIN_CHANNEL;

#endif

extern const NF_PAL_ADC_PORT_PIN_CHANNEL c_AdcPortPinConfig[];
extern const int c_AdcChannelCount;

#if !defined(RP_ADC_USE_ADC1)
extern NF_PAL_ADC_PORT_PIN_CHANNEL *RuntimeAdcPortPinConfig;
extern uint8_t RuntimeAdcChannelCount;
#endif

#endif // SYS_DEV_ADC_NATIVE_TARGET_H
