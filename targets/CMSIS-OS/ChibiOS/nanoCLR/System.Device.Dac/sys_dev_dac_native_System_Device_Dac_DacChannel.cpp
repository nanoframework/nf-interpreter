//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_dac_native_target.h"

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

HRESULT Library_sys_dev_dac_native_System_Device_Dac_DacChannel::NativeWriteValue___VOID__U2(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *dacController = NULL;

    // we are filling this below with the appropriate DAC port pin config and DAC driver
    NF_PAL_DAC_PORT_PIN_CHANNEL dacDefinition;
    NF_PAL_DAC *palDac;

    dacsample_t sample;
    int channelNumber;
    int controllerId;

    // assign the value to the buffer
    sample = (dacsample_t)stack.Arg1().NumericByRef().u2;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Get channel from _channelNumber field
    channelNumber = pThis[FIELD___channelNumber].NumericByRef().s4;

    // need to get the controllerId for the DAC controller of this channel
    // get pointer to DacController field
    dacController = pThis[FIELD___dacController].Dereference();

    // get pointer to _controllerId field in DacController
    controllerId = dacController[Library_sys_dev_dac_native_System_Device_Dac_DacController::FIELD___controllerId]
                       .NumericByRef()
                       .s4;

    // only one DAC controller for now, but check it anyways
    if (controllerId == 1)
    {
        dacDefinition = DacPortPinConfig[channelNumber];

        // we should remove form the build the DAC controller & channel combinations that aren't implemented
        // plus we have to use the default to catch invalid DAC Ids
        switch (ENCODED_DAC_REF(dacDefinition.dacIndex, dacDefinition.dacChannel))
        {
#if STM32_DAC_USE_DAC1_CH1
            case 11:
                palDac = &Dac1_1_PAL;
                break;
#endif

#if STM32_DAC_USE_DAC1_CH2
            case 12:
                palDac = &Dac1_2_PAL;
                break;
#endif

#if STM32_DAC_USE_DAC2_CH1
            case 21:
                palDac = &Dac2_1_PAL;
                break;
#endif

#if STM32_DAC_USE_DAC2_CH2
            case 22:
                palDac = &Dac2_2_PAL;
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

    // perform the conversion
    dacPutChannelX(palDac->Driver, palDac->Channel, sample);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_dac_native_System_Device_Dac_DacChannel::NativeDispose___VOID__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_PAL_DAC_PORT_PIN_CHANNEL dacDefinition;
    int channelNumber;
    bool disposeController = false;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get disposeController
    disposeController = (bool)stack.Arg0().NumericByRef().u1;

    // Get channel from _channelNumber field
    channelNumber = pThis[FIELD___channelNumber].NumericByRef().s4;

    if (disposeController)
    {
        dacDefinition = DacPortPinConfig[channelNumber];

        switch (ENCODED_DAC_REF(dacDefinition.dacIndex, dacDefinition.dacChannel))
        {
#if STM32_DAC_USE_DAC1_CH1
            case 11:
                dacStop(&DACD1);
                break;
#endif

#if STM32_DAC_USE_DAC1_CH2
            case 12:
                dacStop(&DACD2);
                break;
#endif

#if STM32_DAC_USE_DAC2_CH1
            case 21:
                dacStop(&DACD3);
                break;
#endif

#if STM32_DAC_USE_DAC2_CH2
            case 22:
                dacStop(&DACD4);
                break;
#endif

            default:
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }

    NANOCLR_NOCLEANUP();
}
