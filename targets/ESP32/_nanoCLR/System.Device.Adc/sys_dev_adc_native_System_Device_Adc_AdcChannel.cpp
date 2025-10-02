//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_adc_native_target.h>
#include <Esp32_DeviceMapping.h>

#if defined(CONFIG_IDF_TARGET_ESP32)
extern "C" uint8_t temperature_sens_read();
#endif

adc_oneshot_unit_handle_t GetAdcHandle(adc_unit_t unit);

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcChannel::NativeReadValue___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int channelNumber;
    adc_unit_t adcNumber;
    int reading = 0;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Get channel from _channelNumber field
    channelNumber = pThis[FIELD___channelNumber].NumericByRef().s4;
    if (channelNumber < 0 || channelNumber > TARGET_ADC_NUM_PINS)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // Calculate internal ADC number based on channel number, 0->(CONFIG_SOC_ADC_MAX_CHANNEL_NUM - 1)
    adcNumber = channelNumber < CONFIG_SOC_ADC_MAX_CHANNEL_NUM ? ADC_UNIT_1 : ADC_UNIT_2;

#if defined(CONFIG_IDF_TARGET_ESP32)
    // Handle internal channels for ESP32 only
    if (adcNumber == ADC_UNIT_1 && (channelNumber == 8 || channelNumber == 9))
    {
        if (channelNumber == 8)
        {
            // reading = temperature_sens_read();
            reading = 0;
        }
        else
        {
            // Hall sensor no longer available
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    else
#endif
    {
        if (adcNumber == ADC_UNIT_2)
        {
            // Adjust channel number for ADC2
            channelNumber -= CONFIG_SOC_ADC_MAX_CHANNEL_NUM;
        }

        // Read the value
        if (adc_oneshot_read(GetAdcHandle(adcNumber), (adc_channel_t)channelNumber, &reading) != ESP_OK)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_PIN_UNAVAILABLE);
        }
    }

    // Return value to the managed application
    stack.SetResult_I4(reading);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcChannel::NativeDisposeChannel___VOID(CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();

    // left empty on purpose, nothing to do here

    NANOCLR_NOCLEANUP_NOLABEL();
}
