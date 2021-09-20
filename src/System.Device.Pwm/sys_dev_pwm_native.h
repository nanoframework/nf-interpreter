//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _SYS_DEV_PWM_NATIVE_H_
#define _SYS_DEV_PWM_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

typedef enum __nfpack PwmPulsePolarity
{
    PwmPulsePolarity_ActiveHigh = 0,
    PwmPulsePolarity_ActiveLow = 1,
} PwmPulsePolarity;

struct Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel
{
    static const int FIELD___frequency = 1;
    static const int FIELD___dutyCyclePercentage = 2;
    static const int FIELD___dutyCycle = 3;
    static const int FIELD___polarity = 4;
    static const int FIELD___pinNumber = 5;
    static const int FIELD___pwmTimer = 6;
    static const int FIELD___disposedValue = 7;

    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);
    NANOCLR_NATIVE_DECLARE(NativeSetDesiredFrequency___U4__U4);
    NANOCLR_NATIVE_DECLARE(NativeSetActiveDutyCyclePercentage___VOID__U4);
    NANOCLR_NATIVE_DECLARE(NativeStart___VOID);
    NANOCLR_NATIVE_DECLARE(NativeStop___VOID);
    NANOCLR_NATIVE_DECLARE(DisposeNative___VOID);

    //--//

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_Pwm;

#endif  //_SYS_DEV_PWM_NATIVE_H_
