//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <string.h>
#include <nanoCLR_Hardware.h>
#include <targetPAL.h>

#include "nanoFramework_hardware_esp32_native.h"

#if defined(CONFIG_IDF_TARGET_ESP32)
static bool CalibrateTouchPad(touch_pad_t pad, int calibrationCount);
#endif

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::
    NativeEnableWakeupByTimer___STATIC__nanoFrameworkHardwareEsp32EspNativeError__U8(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        uint64_t time_us = (uint64_t)stack.Arg0().NumericByRef().s8;

        esp_err_t err = esp_sleep_enable_timer_wakeup(time_us);

        // Return err to the managed application
        stack.SetResult_I4((int)err);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::
    NativeEnableWakeupByPin___STATIC__nanoFrameworkHardwareEsp32EspNativeError__nanoFrameworkHardwareEsp32SleepWakeupGpioPin__I4(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if SOC_PM_SUPPORT_EXT_WAKEUP

    gpio_num_t gpio_num;
    esp_err_t err;

    uint64_t mask = (uint64_t)stack.Arg0().NumericByRef().s8;
    int level = stack.Arg1().NumericByRef().s4;

    // Extract pin number from mask value
    gpio_num = GPIO_NUM_0;
    err = 0;

    if (mask) // Only enable pin if other than Pin.None
    {
        for (size_t i = 0; i < GPIO_NUM_MAX; i++)
        {
            if (mask & 0x01)
            {
                gpio_num = (gpio_num_t)i;
            }
            mask >>= 1;
        }
        err = esp_sleep_enable_ext0_wakeup(gpio_num, level);
    }

    // Return err to the managed application
    stack.SetResult_I4((int)err);

    NANOCLR_NOCLEANUP_NOLABEL();

#else

    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);

    NANOCLR_NOCLEANUP();

#endif
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::
    NativeEnableWakeupByMultiPins___STATIC__nanoFrameworkHardwareEsp32EspNativeError__nanoFrameworkHardwareEsp32SleepWakeupGpioPin__nanoFrameworkHardwareEsp32SleepWakeupMode(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if SOC_PM_SUPPORT_EXT_WAKEUP

    uint64_t mask = (uint64_t)stack.Arg0().NumericByRef().s8;
    esp_sleep_ext1_wakeup_mode_t mode = (esp_sleep_ext1_wakeup_mode_t)stack.Arg1().NumericByRef().s4;

    esp_err_t err = esp_sleep_enable_ext1_wakeup(mask, mode);

    // Return err to the managed application
    stack.SetResult_I4((int)err);

    NANOCLR_NOCLEANUP_NOLABEL();

#else

    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);

    NANOCLR_NOCLEANUP();

#endif
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::
    NativeEnableWakeupByTouchPad___STATIC__nanoFrameworkHardwareEsp32EspNativeError(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if SOC_PM_SUPPORT_EXT_WAKEUP
    esp_err_t err;

#if defined(CONFIG_IDF_TARGET_ESP32)
    int pad1;
    int pad2;
    // Setup the sleep mode
    touch_pad_init();
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);

    pad1 = stack.Arg0().NumericByRef().s4;
    pad2 = stack.Arg0().NumericByRef().s4;

    // Check that the configuration is correct
    if ((pad1 < 0) && (pad2 < 0))
    {
        // We can't have both pads negative
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }
    else if ((pad1 >= TOUCH_PAD_MAX) || (pad2 >= TOUCH_PAD_MAX))
    {
        // We can't have any pad more than the maximum pad number
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }
    else if (pad1 < 0)
    {
        // If we have pad1 negative but pad 2 positive, swap them
        int padTmp = pad2;
        pad2 = pad1;
        pad1 = padTmp;
        // Set the source on 1 pad only
        touch_pad_set_trigger_source(TOUCH_TRIGGER_SOURCE_SET1);
    }
    else
    {
        // Both positives, both in the norm, then both sources
        touch_pad_set_trigger_source(TOUCH_TRIGGER_SOURCE_BOTH);
    }

    // Set pad 1 and calibrate it
    touch_pad_config((touch_pad_t)pad1, 0);
    if (!CalibrateTouchPad((touch_pad_t)pad1, 20))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    if (pad2 >= 0)
    {
        // Set pad 2 and calibrate it if it's a valid one
        touch_pad_config((touch_pad_t)pad2, 0);
        if (!CalibrateTouchPad((touch_pad_t)pad2, 20))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }
    }

#else
#endif

    err = esp_sleep_enable_touchpad_wakeup();
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);

    // Return err to the managed application
    stack.SetResult_I4((int)err);

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::
    NativeStartLightSleep___STATIC__nanoFrameworkHardwareEsp32EspNativeError(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        esp_err_t err = esp_light_sleep_start();

        // Return err to the managed application
        stack.SetResult_I4((int)err);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::
    NativeStartDeepSleep___STATIC__nanoFrameworkHardwareEsp32EspNativeError(CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();

    // set flags to stop debug...
    CLR_EE_DBG_SET(RebootPending);
    // ... reboot CLR so execution ends gracefully ...
    CLR_EE_REBOOT_CLR;
    // ... and set power level to OFF
    g_CLR_HW_Hardware.m_powerLevel = PowerLevel__Off;

    // no error here, always OK
    stack.SetResult_I4(0);

    //////////////////////////////////////////////////////////
    // developer notes:
    // - the "goto sleep" is actually handled in ClrStartup() loop when calling CPU_SetPowerMode()
    // - the execution engine has to exit the current thread execution to be able to check the reboot flags set above
    // and shutdown gracefully
    // - a clean approach to accomplish this is to add a Thread.Sleep(Inifinite) in the caller
    //////////////////////////////////////////////////////////

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::
    NativeGetWakeupCause___STATIC__nanoFrameworkHardwareEsp32SleepWakeupCause(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if SOC_PM_SUPPORT_EXT_WAKEUP

    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();

    // Return value to the managed application
    stack.SetResult_I4((int32_t)cause);

    NANOCLR_NOCLEANUP_NOLABEL();

#else

    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);

    NANOCLR_NOCLEANUP();

#endif
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::
    NativeGetWakeupGpioPin___STATIC__nanoFrameworkHardwareEsp32SleepWakeupGpioPin(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if SOC_PM_SUPPORT_EXT_WAKEUP

    int64_t pin = (int64_t)esp_sleep_get_ext1_wakeup_status();

    // Return value to the managed application
    stack.SetResult_I8(pin);

    NANOCLR_NOCLEANUP_NOLABEL();

#else

    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);

    NANOCLR_NOCLEANUP();

#endif
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::
    NativeGetWakeupTouchpad___STATIC__nanoFrameworkHardwareEsp32SleepTouchPad(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if SOC_PM_SUPPORT_EXT_WAKEUP

    touch_pad_t touch_pad = esp_sleep_get_touchpad_wakeup_status();
    int retValue = (int)touch_pad;
    // We have to remap this enum.
    if (touch_pad == TOUCH_PAD_MAX)
    {
        retValue = -1;
    }

    // Return value to the managed application
    stack.SetResult_I4(retValue);

    NANOCLR_NOCLEANUP_NOLABEL();

#else

    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);

    NANOCLR_NOCLEANUP();

#endif
}

#if defined(CONFIG_IDF_TARGET_ESP32)
static bool CalibrateTouchPad(touch_pad_t pad, int calibrationCount)
{
    double avg = 0;
    const int minReading = 300;
    uint16_t val;
    for (int i = 0; i < calibrationCount; i++)
    {
        touch_pad_read(pad, &val);
        avg = (avg * i + val) / (i + 1);
    }

    if (avg < minReading)
    {
        touch_pad_config(pad, 0);
        return false;
    }
    else
    {
        // TODO: adjust sensitivity, 2/3 seems to work as well.
        int threshold = avg * 2 / 3;
        touch_pad_config(pad, threshold);
    }

    return true;
}
#endif