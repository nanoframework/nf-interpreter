//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_dac_native_target.h"

HRESULT Library_win_dev_dac_native_System_Devices_Dac_DacController::NativeOpenChannel___VOID__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    
    // we are filling this below with the appropriate ADC port pin config and ADC driver
    NF_PAL_DAC_PORT_PIN_CHANNEL dacDefinition;
    DACDriver* dacDriver = NULL;
    volatile int controllerId;

    // Get channel from argument
    volatile int channel = stack.Arg1().NumericByRef().s4;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    // get controller ID
    controllerId = pThis[FIELD___controllerId].NumericByRef().s4;

    // only one DAC controller for now, but check it anyways
    if(controllerId == 1)
    {
        dacDefinition = DacPortPinConfig[channel];

        // we should remove form the build the ADC options that aren't implemented
        // plus we have to use the default to catch invalid ADC Ids
        switch(dacDefinition.dacIndex)
        {
#if STM32_DAC_USE_DAC1_CH1
            case 1: 
                dacDriver = &DACD1;
                break;
#endif

#if STM32_DAC_USE_DAC1_CH2
            case 2:
                dacDriver = &DACD2;
                break;
#endif

#if STM32_DAC_USE_DAC2_CH1
            case 3: 
                dacDriver = &DACD3;
                break;
#endif

#if STM32_DAC_USE_DAC2_CH2
            case 4:
                dacDriver = &DACD4;
                break;
#endif
            default: 
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);            
        }

    }
    else
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    if(dacDefinition.portId != NULL)
    {
        palSetGroupMode(dacDefinition.portId, PAL_PORT_BIT(dacDefinition.pin), 0, PAL_MODE_INPUT_ANALOG);
    }

    // start DAC
    dacStart(dacDriver, NULL);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_dac_native_System_Devices_Dac_DacController::NativeGetChannelCount___I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    int channelCount;
    int controllerId;

    CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

    controllerId = pThis[ FIELD___controllerId ].NumericByRefConst().s4;

    switch(controllerId)
    {
        case 1: 
            channelCount = DacChannelCount;
            break;

        default: 
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }
    
    // Return value to the managed application
    stack.SetResult_I4(channelCount);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_dac_native_System_Devices_Dac_DacController::NativeGetResolutionInBits___I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // Fixed at 12 bit
        stack.SetResult_I4(12);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_win_dev_dac_native_System_Devices_Dac_DacController::NativeInit___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    int controllerId;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    // get pointer to _controllerId field in AdcController
    controllerId = pThis[FIELD___controllerId].NumericByRef().s4;

    // all required initialization for ADC is already handled in ChibiOS driver

    switch(controllerId)
    {
        case 1: 
            break;

        default: 
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_dac_native_System_Devices_Dac_DacController::GetDeviceSelector___STATIC__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
       // because the caller is expecting a result to be returned
       // we need set a return result in the stack argument using the appropriate SetResult according to the variable type (a string here)
       stack.SetResult_String("DAC1");
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
