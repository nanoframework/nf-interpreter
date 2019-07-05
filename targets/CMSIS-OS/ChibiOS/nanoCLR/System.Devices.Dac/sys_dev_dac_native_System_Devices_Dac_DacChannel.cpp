//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_dac_native_target.h"

// this has to be an array because ChibiOS DAC API expects that
static dacsample_t sampleBuffer[1 * 1];

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

// not used, just left here if needed for debugging purposes
static void dacerrorcallback(DACDriver *dacp, dacerror_t err)
{
  (void)dacp;
  (void)err;
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif


HRESULT Library_win_dev_dac_native_System_Devices_Dac_DacChannel::NativeWriteValue___VOID__U2( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock* dacController = NULL;

    // we are filling this below with the appropriate DAC port pin config and DAC driver
    NF_PAL_DAC_PORT_PIN_CHANNEL dacDefinition;
    DACDriver* dacDriver = NULL;
    DACConversionGroup dacConversionGroupConfig;
    int channelNumber;
    int controllerId;

    // assign the value to the buffer
    sampleBuffer[0] = stack.Arg1().NumericByRef().r4;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

    // Get channel from _channelNumber field
    channelNumber = pThis[FIELD___channelNumber].NumericByRef().s4;

    // need to get the controllerId for the DAC controller of this channel
    // get pointer to DacController field
    dacController = pThis[FIELD___dacController].Dereference();

    // get pointer to _controllerId field in DacController
    controllerId = dacController[Library_win_dev_dac_native_System_Devices_Dac_DacController::FIELD___controllerId].NumericByRef().s4;

    // only one DAC controller for now, but check it anyways
    if(controllerId == 1)
    {
        dacDefinition = DacPortPinConfig[channelNumber];

        // we should remove form the build the DAC options that aren't implemented
        // plus we have to use the default to catch invalid DAC Ids
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

    // need to setup the conversion group parameters
    dacConversionGroupConfig = {
        .num_channels = 1U,
        .end_cb       = NULL,
        .error_cb     = NULL,
        .trigger      = DAC_TRG(0) // should be settable
    };

    // perform the conversion
    dacConvert(dacDriver, &dacConversionGroupConfig, sampleBuffer, 1);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_dac_native_System_Devices_Dac_DacChannel::NativeDisposeChannel___VOID( CLR_RT_StackFrame& stack )
{
    (void)stack;

    NANOCLR_HEADER();

    // left empty on purpose, nothing to do here

    NANOCLR_NOCLEANUP_NOLABEL();
}
