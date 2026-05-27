//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef SYS_DEV_ADC_NATIVE_TARGET_H
#define SYS_DEV_ADC_NATIVE_TARGET_H

#include <hal.h>
#include <sys_dev_adc_native.h>

// RP2040 ADC: single ADC with 5 channels
// Ch 0-3 = GPIO26-GPIO29, Ch 4 = internal temperature sensor
typedef struct
{
    uint8_t adcIndex;
    uint8_t gpio;       // GPIO number (26-29) or 0xFF for internal channels
    uint32_t adcChannel;
} NF_PAL_ADC_PORT_PIN_CHANNEL;

extern const NF_PAL_ADC_PORT_PIN_CHANNEL AdcPortPinConfig[];
extern const int AdcChannelCount;

#endif // SYS_DEV_ADC_NATIVE_TARGET_H
