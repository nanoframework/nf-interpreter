//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_dac_native_target.h"

extern dac_oneshot_handle_t dacChannelHandle[DacChannelCount];

HRESULT Library_sys_dev_dac_native_System_Device_Dac_DacChannel::NativeWriteValue___VOID__U2(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *dacController = nullptr;
        int channelNumber;
        int controllerId;
        dac_channel_t dacChannel = DAC_CHAN_0;

        // Get value argumant and mask to 0 - 255 range
        uint16_t value = (stack.Arg1().NumericByRefConst().u2 & 0xff);

        // get a pointer to the managed object instance and check that it's not nullptr
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

        // only one DAC controller, but check it anyways
        if (controllerId != 1)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        switch (channelNumber)
        {
            case 0:
                dacChannel = DAC_CHAN_0;
                break;

            case 1:
                dacChannel = DAC_CHAN_1;
                break;

            default:
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        dac_oneshot_output_voltage(dacChannelHandle[dacChannel], value);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_dac_native_System_Device_Dac_DacChannel::NativeDispose___VOID__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int channelNumber;
    int dacChannel;
    bool disposeController = false;

    // get a pointer to the managed object instance and check that it's not nullptr
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get disposeController flag
    disposeController = (bool)stack.Arg1().NumericByRef().u1;

    if (disposeController)
    {
        // Get channel from _channelNumber field
        channelNumber = pThis[FIELD___channelNumber].NumericByRef().s4;

        switch (channelNumber)
        {
            case 1:
                dacChannel = DAC_CHAN_0;
                break;

            case 2:
                dacChannel =DAC_CHAN_1;
                break;

            default:
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        dac_oneshot_del_channel(dacChannelHandle[dacChannel]);
    }

    NANOCLR_NOCLEANUP();
}
