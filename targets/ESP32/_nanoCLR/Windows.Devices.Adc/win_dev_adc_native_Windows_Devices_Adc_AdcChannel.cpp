//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "win_dev_adc_native_target.h"

#if defined(IDF_TARGET_ESP32)
extern "C" uint8_t temprature_sens_read();
#endif

HRESULT Library_win_dev_adc_native_Windows_Devices_Adc_AdcChannel::NativeReadValue___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int reading = 0;
        esp_err_t result;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // Get channel from _channelNumber field
        int channelNumber = pThis[FIELD___channelNumber].NumericByRef().s4;

        // Calculate internal ADC number based on channel number
        int adcNumber = channelNumber <= 9 ? 1 : 2;

        if (adcNumber == 1)
        {
            switch (channelNumber)
            {

#if defined(IDF_TARGET_ESP32)
                case 8:
                    reading = temperature_sens_read();
                    break;

                case 9:
                    reading = hall_sensor_read();
                    break;
#endif

                default:
                    reading = adc1_get_raw((adc1_channel_t)channelNumber);
                    break;
            }
        }
        else if (adcNumber == 2)
        {
            // Adjust channel number for ADC2
            channelNumber -= 10;
            result = adc2_get_raw((adc2_channel_t)channelNumber, (adc_bits_width_t)SOC_ADC_MAX_BITWIDTH, &reading);

            if (result != ESP_OK)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_PIN_UNAVAILABLE);
            }
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        stack.SetResult_I4(reading);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_adc_native_Windows_Devices_Adc_AdcChannel::NativeDisposeChannel___VOID(CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();

    // left empty on purpose, nothing to do here

    NANOCLR_NOCLEANUP_NOLABEL();
}
