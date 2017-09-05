//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>
#include <string.h>
#include <targetPAL.h>

#include "win_dev_pwm_native.h"

///////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.I2c.I2cSharingMode (in managed code) !!!    //
///////////////////////////////////////////////////////////////////////////////////////
enum PwmPulsePolarity 
    { 
        ActiveHigh, 
        ActiveLow 
    };




HRESULT Library_win_dev_pwm_native_Windows_Devices_PWM_PwmDevice::NativeInit___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    NANOCLR_NOCLEANUP();
}
    
HRESULT Library_win_dev_pwm_native_Windows_Devices_PWM_PwmDevice::DisposeNative___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    NANOCLR_NOCLEANUP();
}