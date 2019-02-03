//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _WIN_DEV_PWM_NATIVE_H_
#define _WIN_DEV_PWM_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>

enum PwmPulsePolarity
{
        /// <summary>
        /// Configures the PWM signal to start in the active high state.
        /// </summary>
        ActiveHigh,
        /// <summary>
        /// Configures the PWM signal to start in the active low state.
        /// </summary>
        ActiveLow
};

struct Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmController
{
    static const int FIELD___controllerId = 1;
    static const int FIELD___actualFrequency = 2;
    static const int FIELD___pwmTimer = 3;

    NANOCLR_NATIVE_DECLARE(get_MaxFrequency___R8);
    NANOCLR_NATIVE_DECLARE(get_MinFrequency___R8);
    NANOCLR_NATIVE_DECLARE(get_PinCount___I4);
    NANOCLR_NATIVE_DECLARE(NativeSetDesiredFrequency___U4__U4);
    NANOCLR_NATIVE_DECLARE(GetDeviceSelector___STATIC__STRING);

    //--//

};

struct Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmControllerManager
{
    static const int FIELD_STATIC___syncLock = 0;
    static const int FIELD_STATIC__s_controllersCollection = 1;


    //--//

};

struct Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmPin
{
    static const int FIELD___syncLock = 1;
    static const int FIELD___pwmController = 2;
    static const int FIELD___polarity = 3;
    static const int FIELD___dutyCyclePercentage = 4;
    static const int FIELD___dutyCycle = 5;
    static const int FIELD___pinNumber = 6;
    static const int FIELD___pwmTimer = 7;
    static const int FIELD___isStarted = 8;
    static const int FIELD___disposed = 9;

    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);
    NANOCLR_NATIVE_DECLARE(NativeSetActiveDutyCyclePercentage___VOID__U4);
    NANOCLR_NATIVE_DECLARE(NativeSetPolarity___VOID__U1);
    NANOCLR_NATIVE_DECLARE(NativeStart___VOID);
    NANOCLR_NATIVE_DECLARE(NativeStop___VOID);
    NANOCLR_NATIVE_DECLARE(DisposeNative___VOID);

    //--//
	
	static int GetChannel (int pin, int timerId, bool create);
    static uint32_t CalculateDuty(int timerId, uint32_t dutyCycle, PwmPulsePolarity polarity);
    static HRESULT  ConfigureAndStart(CLR_RT_HeapBlock* pThis, bool create, bool noStart);

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Devices_Pwm;

#endif  //_WIN_DEV_PWM_NATIVE_H_
