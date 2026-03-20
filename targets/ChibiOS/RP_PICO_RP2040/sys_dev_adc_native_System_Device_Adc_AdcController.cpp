//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_adc_native_target.h>

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeOpenChannel___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int channel;
    NF_PAL_ADC_PORT_PIN_CHANNEL adcDefinition;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    channel = stack.Arg1().NumericByRef().s4;

    if (channel < 0 || channel >= AdcChannelCount)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    adcDefinition = AdcPortPinConfig[channel];

    // Initialize GPIO for ADC input (not needed for temp sensor)
    if (adcDefinition.gpio != 0xFF)
    {
        adcRPGpioInit(adcDefinition.gpio);
    }

    // Start ADC
    adcStart(&ADCD1, NULL);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeGetChannelCount___I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    stack.SetResult_I4(AdcChannelCount);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeGetMaxValue___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    // RP2040 ADC is 12-bit: max value = 4095
    stack.SetResult_I4(4095);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeGetMinValue___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    stack.SetResult_I4(0);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeIsChannelModeSupported___BOOLEAN__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int mode = stack.Arg1().NumericByRef().s4;

    // Only single-ended mode supported
    stack.SetResult_Boolean((mode == (int)AdcChannelMode_SingleEnded));

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeGetResolutionInBits___I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    // RP2040 ADC is 12-bit
    stack.SetResult_I4(12);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();

    NANOCLR_NOCLEANUP_NOLABEL();
}
