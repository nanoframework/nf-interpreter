//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_dac_native_target.h"

////////////////////////////////////////////////////////////
// DAC PAL strucs delcared in sys_dev_dac_native_target.h //
////////////////////////////////////////////////////////////
#if STM32_DAC_USE_DAC1_CH1
NF_PAL_DAC Dac1_1_PAL;
#endif
#if STM32_DAC_USE_DAC1_CH2
NF_PAL_DAC Dac1_2_PAL;
#endif
#if STM32_DAC_USE_DAC2_CH1
NF_PAL_DAC Dac2_1_PAL;
#endif
#if STM32_DAC_USE_DAC2_CH2
NF_PAL_DAC Dac2_2_PAL;
#endif

HRESULT Library_sys_dev_dac_native_System_Devices_Dac_DacController::NativeOpenChannel___VOID__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    
    // we are filling this below with the appropriate ADC port pin config and ADC driver
    NF_PAL_DAC_PORT_PIN_CHANNEL dacDefinition;
    NF_PAL_DAC* palDac;
    int controllerId;

    // Get channel from argument
    int channel = stack.Arg1().NumericByRef().s4;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    // get controller ID
    controllerId = pThis[FIELD___controllerId].NumericByRef().s4;

    // only one DAC controller for now, but check it anyways
    if(controllerId == 1)
    {
        dacDefinition = DacPortPinConfig[channel];

        switch(ENCODED_DAC_REF(dacDefinition.dacIndex, dacDefinition.dacChannel))
        {
          #if STM32_DAC_USE_DAC1_CH1
            case 11:
                Dac1_1_PAL.Driver = &DACD1;
                palDac = &Dac1_1_PAL;
                // unlike STM documentation, ChibiOS uses a 0 index for DAC channels
                palDac->Channel = 1 - 1;
                break;
          #endif

          #if STM32_DAC_USE_DAC1_CH2
            case 12:
                Dac1_2_PAL.Driver = &DACD2;
                palDac = &Dac1_2_PAL;
                // unlike STM documentation, ChibiOS uses a 0 index for DAC channels
                palDac->Channel = 2 - 1;
                break;
          #endif

          #if STM32_DAC_USE_DAC2_CH1
            case 21:
                Dac2_1_PAL.Driver = &DACD3;
                palDac = &Dac2_1_PAL;
                // unlike STM documentation, ChibiOS uses a 0 index for DAC channels
                palDac->Channel = 1 - 1;
                break;
          #endif

          #if STM32_DAC_USE_DAC2_CH2
            case 22:
                Dac2_2_PAL.Driver = &DACD4;
                palDac = &Dac2_2_PAL;
                // unlike STM documentation, ChibiOS uses a 0 index for DAC channels
                palDac->Channel = 2 - 1;
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

    // start DAC driver if it's not already started
    if(palDac->Driver->state < DAC_READY)
    {
        if(dacDefinition.portId != NULL)
        {
            palSetPadMode(dacDefinition.portId, dacDefinition.pin, PAL_MODE_INPUT_ANALOG);
        }

        // set DAC config: 12 bits data mode, others registers to defaults
        memset(&palDac->Config, 0, sizeof(DACConfig));

        palDac->Config.init      = 0;
        palDac->Config.datamode  = DAC_DHRM_12BIT_RIGHT;
        palDac->Config.cr        = 0;

        // start DAC
        dacStart(palDac->Driver, &palDac->Config);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_dac_native_System_Devices_Dac_DacController::NativeGetChannelCount___I4( CLR_RT_StackFrame& stack )
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

HRESULT Library_sys_dev_dac_native_System_Devices_Dac_DacController::NativeGetResolutionInBits___I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // Fixed at 12 bit
        stack.SetResult_I4(12);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_dac_native_System_Devices_Dac_DacController::NativeInit___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    int controllerId;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    // get pointer to _controllerId field in DacController
    controllerId = pThis[FIELD___controllerId].NumericByRef().s4;

    // all required initialization for DAC is already handled in ChibiOS driver

    switch(controllerId)
    {
        case 1: 
            break;

        default: 
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_dac_native_System_Devices_Dac_DacController::GetDeviceSelector___STATIC__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
       // because the caller is expecting a result to be returned
       // we need set a return result in the stack argument using the appropriate SetResult according to the variable type (a string here)
       stack.SetResult_String("DAC1");
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
