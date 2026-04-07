//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_adc_native_target.h>
#include <Esp32_DeviceMapping.h>

// Details for ESP32 ADC controllers
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

// ESP32 ADC1 with 8 channels (GPIO 36,37,38,39,32,33,34,35)
//            plus 2 channels (Temp sensor, Hall sensor) ( no longer available with IDF 5.x)
//            plus 10 channels (GPIO 4,0,2,12,13,14,15,27,25,26)
// ESP32_S2 ADC1 with 10 channels (GPIO 1,2,3,4,5,6,7,8,9,10)
//               plus 10 channels (GPIO 11,12,13,14, XTAL_32K_P, XTAL_32K_N, DAC_1, DAC_2, 19, 20)
// ESP32_S3 ADC1 with 10 channels (GPIO 1,2,3,4,5,6,7,8,9,10)
//               plus 10 channels (GPIO 11,12,13,14,15,16,17,18,19,20)
// ESP32_P4 ADC1 with 8 channels (GPIO 16,17,18,19,20,21,22,23)
//               plus 6 channels (GPIO 49,50,51,52,53,54)
// ESP32_C3 ADC1 with 5 channels (GPIO 0,1,2,3,4)
//               plus 1 channel (GPIO 5)
// ESP32_C6 ADC1 with 7 channels (GPIO  X32K_P(0),X32K_N(1),2,3,4,5,6)
// ESP32_H2 ADC1 with 5 channels (GPIO 1,2,3,4,5)

// Config parameters from sdkconfig
// CONFIG_SOC_ADC_SUPPORTED
// CONFIG_SOC_ADC_PERIPH_NUM
// CONFIG_SOC_ADC_MAX_CHANNEL_NUM

// The ESP32 still uses the legacy ADC driver for now as dependency between ADC and I2S due to internal DAC etc
#if defined(CONFIG_IDF_TARGET_ESP32)

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeOpenChannel___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int channelNumber;
    int adcUnit;
    esp_err_t result;
    // default to MAX bit width for SoC
    adc_bits_width_t width_bit = (adc_bits_width_t)SOC_ADC_RTC_MAX_BITWIDTH;
    adc_atten_t atten = ADC_ATTEN_DB_12;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Get channel from argument
    channelNumber = stack.Arg1().NumericByRef().s4;

    if (channelNumber < ADC_CHANNEL_0 || channelNumber > TARGET_ADC_NUM_PINS)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // Get ADC device number from channel
    adcUnit = channelNumber < CONFIG_SOC_ADC_MAX_CHANNEL_NUM ? 1 : 2;

    switch (adcUnit)
    {
#if (CONFIG_SOC_ADC_PERIPH_NUM >= 1)
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
#endif

#if (CONFIG_SOC_ADC_PERIPH_NUM >= 2)
        case 2:
            // Adjust for ADC2
            channelNumber -= CONFIG_SOC_ADC_MAX_CHANNEL_NUM;
            result = adc2_config_channel_atten((adc2_channel_t)channelNumber, atten);

            if (result != ESP_OK)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_PIN_UNAVAILABLE);
            }
            break;
#endif
        default:
            NANOCLR_SET_AND_LEAVE(CLR_E_PIN_UNAVAILABLE);
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

#else

adc_oneshot_unit_handle_t adc_handles[SOC_ADC_PERIPH_NUM] = {NULL};

adc_oneshot_unit_handle_t GetAdcHandle(adc_unit_t unit)
{
    if (unit < 0 || unit >= SOC_ADC_PERIPH_NUM)
    {
        return NULL;
    }

    return adc_handles[unit];
}

void AdcController_Initialize(adc_unit_t unit)
{
    if (adc_handles[unit] != NULL)
    {
        // already initialized
        return;
    }

    adc_oneshot_unit_init_cfg_t init_config1 = {};
    init_config1.unit_id = unit;
    init_config1.ulp_mode = ADC_ULP_MODE_DISABLE;

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc_handles[unit]));
}

void AdcController_delete(adc_unit_t unit)
{
    if (adc_handles[unit] != NULL)
    {
        ESP_ERROR_CHECK(adc_oneshot_del_unit(adc_handles[unit]));
        adc_handles[unit] = NULL;
    }
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeOpenChannel___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int channelNumber;
    adc_unit_t adcUnit;
    esp_err_t result;

    // Set up channel configuration
    adc_oneshot_chan_cfg_t config = {};

    config.bitwidth = ADC_BITWIDTH_12;
    config.atten = ADC_ATTEN_DB_12;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Get channel from argument
    channelNumber = stack.Arg1().NumericByRef().s4;

    if (channelNumber < ADC_CHANNEL_0 || channelNumber > TARGET_ADC_NUM_PINS)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // Get ADC device number from channel
    adcUnit = channelNumber < CONFIG_SOC_ADC_MAX_CHANNEL_NUM ? ADC_UNIT_1 : ADC_UNIT_2;

#if (CONFIG_SOC_ADC_PERIPH_NUM == 1)
    // Only ADC1 supported
    if (adcUnit == ADC_UNIT_2)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }
#endif

    // Initialize ADC unit if required
    AdcController_Initialize(adcUnit);

    if (adcUnit == ADC_UNIT_2)
    {
        // Adjust channel number for ADC2
        channelNumber -= CONFIG_SOC_ADC_MAX_CHANNEL_NUM;
    }

#if defined(CONFIG_IDF_TARGET_ESP32)
    // Handle internal channels for ESP32 only, no longer used with IDF 5.x
    if (adcUnit == ADC_UNIT_1 && (channelNumber == 8 || channelNumber == 9))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }
#endif

    result = adc_oneshot_config_channel(GetAdcHandle(adcUnit), (adc_channel_t)channelNumber, &config);
    if (result != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_PIN_UNAVAILABLE);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeGetChannelCount___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int channelCount = CONFIG_SOC_ADC_MAX_CHANNEL_NUM * CONFIG_SOC_ADC_PERIPH_NUM;

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
#endif // #if defined(CONFIG_IDF_TARGET_ESP32)