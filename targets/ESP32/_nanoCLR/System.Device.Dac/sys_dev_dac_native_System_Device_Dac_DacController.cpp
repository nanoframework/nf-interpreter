//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_dac_native_target.h"

HRESULT Library_sys_dev_dac_native_System_Device_Dac_DacController::NativeOpenChannel___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int controllerId;
        int channelNumber;
        dac_channel_t dacChannel = DAC_CHANNEL_1;

        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // Get channel from argument
        channelNumber = stack.Arg1().NumericByRef().s4;

        // get controller ID
        controllerId = pThis[FIELD___controllerId].NumericByRef().s4;

        // only one DAC controller for now, but check it anyways
        if (controllerId != 1)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        switch (channelNumber)
        {
            case 0:
                dacChannel = DAC_CHANNEL_1;
                break;

            case 1:
                dacChannel = DAC_CHANNEL_2;
                break;

            default:
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // Enable DAC for this channel
        dac_output_enable(dacChannel);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_dac_native_System_Device_Dac_DacController::NativeGetChannelCount___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int controllerId;
        int channelCount = 0;

        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        controllerId = pThis[FIELD___controllerId].NumericByRefConst().s4;

        switch (controllerId)
        {
            case 1:
                channelCount = DacChannelCount;
                break;

            default:
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // Return value to the managed application
        stack.SetResult_I4(channelCount);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_dac_native_System_Device_Dac_DacController::NativeGetResolutionInBits___I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        // Fixed at 8 bit
        stack.SetResult_I4(DacResolutionInBits);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_dac_native_System_Device_Dac_DacController::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int controllerId;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get pointer to _controllerId field in AdcController
    controllerId = pThis[FIELD___controllerId].NumericByRef().s4;

    // all required initialization for DAC is already handled in ChibiOS driver

    switch (controllerId)
    {
        case 1:
            break;

        default:
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_dac_native_System_Device_Dac_DacController::GetDeviceSelector___STATIC__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        // because the caller is expecting a result to be returned
        // we need set a return result in the stack argument using the appropriate SetResult according to the variable
        // type (a string here)
        stack.SetResult_String("DAC1");
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
