//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_adc_native_target.h>

// ESP32 ADC controller
//
// Contains ADC1 and ADC2
// ADC1 with 8 channels ( GPIO 32 - 39 ) 0=36, 1=37, 2=38, 3=39, 4=32, 5=33, 6=34, 7=35
//                      channel 8/9 are logical channles for temperture sensor and is the built in Esp32 Hall sensor.
//                      For hall sensor there is a restriction channel 0 and 3 must be unused
// ADC2 with 10 channels (GPIO 0,2,4,12-15 and 25-27)
// Note : ADC2 cannot be used if Wifi started and other restrictions as pins used for other things.
//
// ESP_DevKitC Gpio 0 cannot be used
// ESP-Wrover_kit V3 GPIO 0,2,4 and 15 can't be used

// Typical marked inputs on boards
// VP - GPIO36 ( channel 0 )
// VN - GPIO39 ( channel 3 )

// From managed code we treat all ADC channels as 1 logical ADC unit (ADC1)
// Logical channels
// ADC1 / 0 - 7     - 8 channels - ( GPIO 32 - 39 ) 0=36, 1=37, 2=38, 3=39, 4=32, 5=33, 6=34, 7=35
//  8               - Internal Temperture sensor
//  9               - Internal Hall Sensor
// ADC2 / 10 - 19   - 10 Channels -( GPIOs 0, 2, 4, 12 - 15 and 25 - 27. )

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeOpenChannel___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int channelNumber;
    int adcUnit;
    esp_err_t result;
    // default to MAX bit width for SoC
    adc_bits_width_t width_bit = (adc_bits_width_t)SOC_ADC_MAX_BITWIDTH;
    adc_atten_t atten = ADC_ATTEN_DB_11;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Get channel from argument
    channelNumber = stack.Arg1().NumericByRef().s4;

    if (channelNumber < ADC_CHANNEL_0 || channelNumber > 19)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // Get ADC device number from channel
    adcUnit = channelNumber <= 9 ? 1 : 2;

    adc_power_acquire(); // Make sure powered on

    switch (adcUnit)
    {
        case 1:
            // Normal channel 0-7 ?
            if (channelNumber <= 7)
            {
                adc1_config_width(width_bit);

                result = adc1_config_channel_atten((adc1_channel_t)channelNumber, atten);
                if (result != ESP_OK)
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_PIN_UNAVAILABLE);
                }
            }
            break;

        case 2:
            // Adjust for ADC2
            channelNumber -= 10;
            result = adc2_config_channel_atten((adc2_channel_t)channelNumber, atten);

            if (result != ESP_OK)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_PIN_UNAVAILABLE);
            }
            break;
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeGetChannelCount___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int channelCount = 20;

    // Return value to the managed application
    stack.SetResult_I4(channelCount);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeGetMaxValue___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    // Currently fixed 12 bit so return 4095
    stack.SetResult_I4(4095);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeGetMinValue___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    // Return 0 for now, is this signed ?
    stack.SetResult_I4(0);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeIsChannelModeSupported___BOOLEAN__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int mode = stack.Arg1().NumericByRef().s4;

    // Only support Single ended mode for now
    stack.SetResult_Boolean((mode == (int)AdcChannelMode_SingleEnded));

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeGetResolutionInBits___I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    // Fixed at 12 bit for now
    stack.SetResult_I4(12);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();

    // all required initialization for ADC are already handled

    NANOCLR_NOCLEANUP_NOLABEL();
}
