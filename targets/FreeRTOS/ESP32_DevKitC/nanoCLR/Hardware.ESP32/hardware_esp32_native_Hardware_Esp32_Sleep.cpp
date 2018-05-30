//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <string.h>
#include <targetPAL.h>

#include "hardware_esp32_native.h"

HRESULT Library_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::NativeEnableWakupByTimer___STATIC__nanoFrameworkHardwareEsp32ErrorsEsp__U8( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        uint64_t time_us = (uint64_t)stack.Arg0().NumericByRef().s8;

        esp_err_t err = esp_sleep_enable_timer_wakeup(time_us);

        // Return err to the managed application
        stack.SetResult_I4( (int)err ) ;

    }    
    NANOCLR_NOCLEANUP();
}

HRESULT Library_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::NativeEnableWakupByPin___STATIC__nanoFrameworkHardwareEsp32ErrorsEsp__nanoFrameworkHardwareEsp32SleepWakeupGpioPin__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        gpio_num_t gpio_num = (gpio_num_t)stack.Arg0().NumericByRef().s4;
        int  level = stack.Arg1().NumericByRef().s4;
        
        esp_err_t err = esp_sleep_enable_ext0_wakeup( gpio_num, level);

        // Return err to the managed application
        stack.SetResult_I4( (int)err ) ;
 }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::NativeEnableWakeupByMultiPins___STATIC__nanoFrameworkHardwareEsp32ErrorsEsp__nanoFrameworkHardwareEsp32SleepWakeupGpioPin__nanoFrameworkHardwareEsp32SleepWakupMode( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        uint64_t mask = (uint64_t)stack.Arg0().NumericByRef().s8;
        esp_sleep_ext1_wakeup_mode_t  mode = (esp_sleep_ext1_wakeup_mode_t)stack.Arg1().NumericByRef().s4;
        
        esp_err_t err = esp_sleep_enable_ext1_wakeup(mask, mode);

        // Return err to the managed application
        stack.SetResult_I4( (int)err ) ;
  }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::NativeEnableWakeupByTouchPad___STATIC__nanoFrameworkHardwareEsp32ErrorsEsp( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        esp_err_t err = esp_sleep_enable_touchpad_wakeup();

        // Return err to the managed application
        stack.SetResult_I4( (int)err ) ;

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::NativeStartLightSleep___STATIC__nanoFrameworkHardwareEsp32ErrorsEsp( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        esp_err_t err = esp_light_sleep_start();

        // Return err to the managed application
        stack.SetResult_I4( (int)err ) ;
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::NativeStartDeepSleep___STATIC__nanoFrameworkHardwareEsp32ErrorsEsp( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        esp_deep_sleep_start();
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::NativeGetWakeupCause___STATIC__nanoFrameworkHardwareEsp32SleepWakeupCause( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();

        // Return value to the managed application
        stack.SetResult_I4( (int32_t)cause ) ;
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::NativeGetWakeupGpioPin___STATIC__nanoFrameworkHardwareEsp32SleepWakeupGpioPin( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        int64_t pin = (int64_t)esp_sleep_get_ext1_wakeup_status();
        
        // Return value to the managed application
        stack.SetResult_I8( pin ) ;
    }    
    NANOCLR_NOCLEANUP();
}

HRESULT Library_hardware_esp32_native_nanoFramework_Hardware_Esp32_Sleep::NativeGetWakeupTouchpad___STATIC__nanoFrameworkHardwareEsp32SleepTouchPad( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        touch_pad_t touch_pad = esp_sleep_get_touchpad_wakeup_status();

        // Return value to the managed application
        stack.SetResult_I4( (int)touch_pad ) ;
    }
    NANOCLR_NOCLEANUP();
}
