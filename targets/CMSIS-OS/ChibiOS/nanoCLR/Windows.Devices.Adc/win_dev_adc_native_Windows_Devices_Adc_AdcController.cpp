//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "win_dev_adc_native.h"

///////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.Adc.AdcChannelMode (in managed code) !!!    //
///////////////////////////////////////////////////////////////////////////////////////
enum AdcChannelMode
{
        SingleEnded = 0,
        Differential
};

HRESULT Library_win_dev_adc_native_Windows_Devices_Adc_AdcController::NativeOpenChannel___VOID__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        // Get channel from argument
        int channel = stack.Arg1().NumericByRef().s4;

        // get device ID
        int deviceId = pThis[FIELD___deviceId].NumericByRef().s4;

        // we are filling this bellow with the appropriate ADC port pin config
        NF_PAL_ADC_PORT_PIN_CHANNEL channelDef;
        ADCDriver* adc;

        // we should remove form the build the ADC options that aren't implemented
        // plus we have to use the default to catch invalid ADC Ids
        switch(deviceId)
        {

   #if STM32_ADC_USE_ADC1
            case 1: 
                channelDef = Adc1PortPinConfig[channel];
                adc = &ADCD1;
                break;
   #endif

   #if STM32_ADC_USE_ADC2
            case 2: 
                channelDef = Adc2PortPinConfig[channel];
                adc = &ADCD2;
                break;
   #endif

   #if STM32_ADC_USE_ADC3
            case 3: 
                channelDef = Adc3PortPinConfig[channel];
                adc = &ADCD3;
                break;
   #endif

            default: 
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        if(channelDef.portId != NULL && channelDef.pin != NULL)
        {
            palSetGroupMode(channelDef.portId, PAL_PORT_BIT(channelDef.pin), 0, PAL_MODE_INPUT_ANALOG);
        }

        // start ADC
        adcStart(adc, NULL);

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_adc_native_Windows_Devices_Adc_AdcController::NativeGetChannelCount___I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        int channelCount;

        CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

        // FIXME - Return number of single ended channels
        int deviceId = pThis[ FIELD___deviceId ].NumericByRefConst().s4;

        // we should remove form the build the ADC options that aren't implemented
        // plus we have to use the default to catch invalid ADC Ids
        switch(deviceId)
        {

   #if STM32_ADC_USE_ADC1
            case 1: 
                channelCount = Adc1ChannelCount;
                break;
   #endif

   #if STM32_ADC_USE_ADC2
            case 2: 
                channelCount = Adc2ChannelCount;
                break;
   #endif

   #if STM32_ADC_USE_ADC3
            case 3: 
                channelCount = Adc3ChannelCount;
                break;
   #endif

            default: 
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        
        // Return value to the managed application
        stack.SetResult_I4(channelCount);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_adc_native_Windows_Devices_Adc_AdcController::NativeGetMaxValue___I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // Currently fixed at 12 bit so return 4095 = ((2^12) - 1)
        stack.SetResult_I4(4095);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_adc_native_Windows_Devices_Adc_AdcController::NativeGetMinValue___I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    // Return 0 for now
    stack.SetResult_I4(0);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_adc_native_Windows_Devices_Adc_AdcController::NativeIsChannelModeSupported___BOOLEAN__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        int mode = stack.Arg1().NumericByRef().s4;
        
        // Only support Single ended mode for now
        stack.SetResult_Boolean( (mode == (int)AdcChannelMode::SingleEnded) ) ;
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_adc_native_Windows_Devices_Adc_AdcController::NativeGetResolutionInBits___I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // Fixed at 12 bit for now
        stack.SetResult_I4(12);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_adc_native_Windows_Devices_Adc_AdcController::NativeInit___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // Get device Id from argument
        int deviceId = stack.Arg1().NumericByRef().s4;

        // all required initialization for ADC are already handled in ChibiOS driver
        // this is only to check if the requested deviceId is available in hardware

        // we should remove form the build the ADC options that aren't implemented
        // plus we have to use the default to catch invalid ADC Ids
        switch(deviceId)
        {

   #if STM32_ADC_USE_ADC1
            case 1: 
                break;
   #endif

   #if STM32_ADC_USE_ADC2
            case 2: 
                break;
   #endif

   #if STM32_ADC_USE_ADC3
            case 3: 
                break;
   #endif

            default: 
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_adc_native_Windows_Devices_Adc_AdcController::GetDeviceSelector___STATIC__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
   {
       // declare the device selector string whose max size is "ADC1,ADC2,ADC3" + terminator and init with the terminator
       char deviceSelectorString[ 15 + 1] = { 0 };

   #if STM32_ADC_USE_ADC1
       strcat(deviceSelectorString, "ADC1,");
   #endif
   #if STM32_ADC_USE_ADC2
       strcat(deviceSelectorString, "ADC2,");
   #endif
   #if STM32_ADC_USE_ADC3
       strcat(deviceSelectorString, "ADC3,");
   #endif

       // replace the last comma with a terminator
       deviceSelectorString[hal_strlen_s(deviceSelectorString) - 1] = '\0';

       // because the caller is expecting a result to be returned
       // we need set a return result in the stack argument using the appropriate SetResult according to the variable type (a string here)
       stack.SetResult_String(deviceSelectorString);
    }
    NANOCLR_NOCLEANUP();
}
