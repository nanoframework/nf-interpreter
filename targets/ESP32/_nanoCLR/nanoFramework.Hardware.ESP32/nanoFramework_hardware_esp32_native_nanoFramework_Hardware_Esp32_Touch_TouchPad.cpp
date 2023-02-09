//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nanoFramework_hardware_esp32_native.h"
#include <nanoCLR_Hardware.h>
#include <targetPAL.h>

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

static int numberInitialzed = 0;
static bool isTouchInitialized = false;
static bool isTouchPadUsed[TOUCH_PAD_MAX];
static bool isFilterOn = false;
static bool touchValues[TOUCH_PAD_MAX];
static uint16_t thresholds[TOUCH_PAD_MAX];
static bool isTimeModeOn = false;
static uint16_t lastTouchValues[TOUCH_PAD_MAX];
static touch_fsm_mode_t measurementMode;

/*
Function used for all the interruption in the touch pad.
*/
static void IsrCallBack(void *arg)
{
    bool val;
    uint32_t padIntr = touch_pad_get_status();
    touch_pad_intr_clear();
    for (int i = 0; i < TOUCH_PAD_MAX; i++)
    {
        val = (padIntr >> i) & 0x01;
        // Check if we have a change and raise an even if yes
        if (val != touchValues[i])
        {
            PostManagedEvent(EVENT_TOUCH, 0, i, val);
        }

        touchValues[i] = val;
    }
}

/*
Resources need to be cleaned and the driver uninstalled in case of a soft reboot.
*/
void HAL_AddSoftRebootHandler()
{
    // top the task
    isTimeModeOn = false;

    // Clean the isr registration
    touch_pad_intr_disable();
    touch_pad_isr_deregister(IsrCallBack, NULL);
    // Clean filter and uninstall the driver
    touch_pad_filter_stop();
    touch_pad_filter_delete();

    // Deinitialize and clean the touch pad driver
    touch_pad_deinit();

    isFilterOn = false;
    isTouchInitialized = false;
    // Make sure all pins are not reserved
    memset(isTouchPadUsed, 0, sizeof(isTouchPadUsed));
    // Clear the pin values
    memset(touchValues, 0, sizeof(touchValues));
    memset(thresholds, 0, sizeof(thresholds));
    memset(lastTouchValues, 0, sizeof(lastTouchValues));
}

/*
This function ensure that the driver is installed for static functions.
It does initialize as well the pins table and register for the soft reboot call back.
*/
void MakeSureTouchIsInitialized()
{
    if (!isTouchInitialized)
    {
        isTouchInitialized = true;
        touch_pad_init();

        // We setup software trigger mode because the FSM one
        // is not properly working on ESP32 and we will manage a read loop
        // in a task.
        touch_pad_set_fsm_mode(TOUCH_FSM_MODE_SW);

        // Make sure all pins are not reserved
        memset(isTouchPadUsed, 0, sizeof(isTouchPadUsed));

        HAL_AddSoftRebootHandler(HAL_AddSoftRebootHandler);

        touch_pad_intr_enable();
        touch_pad_isr_register(IsrCallBack, NULL);
    }
}

#endif

HRESULT GetAndCheckTouchPadNumber(CLR_RT_StackFrame &stack, touch_pad_t *padNumber)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    // Get the stack
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);
    *padNumber =
        (touch_pad_t)(pThis[Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
                                FIELD___touchPadNumber]
                          .NumericByRef()
                          .s4);

    // Let's make sure it's a proper pin
    if ((*padNumber < 0) || (*padNumber >= TOUCH_PAD_MAX))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::NativeInit___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    touch_pad_t padNumber;
    NANOCLR_CHECK_HRESULT(GetAndCheckTouchPadNumber(stack, &padNumber));

    // We make sure the pin is not yet open
    if (isTouchPadUsed[padNumber])
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // We initalize the pin
    MakeSureTouchIsInitialized();
    if (touch_pad_io_init(padNumber) != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    if (touch_pad_config(padNumber, 0) != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    // And we increase the number and mark the pin as open
    numberInitialzed++;
    isTouchPadUsed[padNumber] = true;

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::NativeDeinit___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    if (numberInitialzed == 1)
    {
        touch_pad_deinit();
        isTouchInitialized = false;
    }

    touch_pad_t padNumber;
    NANOCLR_CHECK_HRESULT(GetAndCheckTouchPadNumber(stack, &padNumber));

    // Decrease the counter and make sure we mark this one as not used anymore
    numberInitialzed--;
    isTouchPadUsed[padNumber] = false;

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

uint16_t TouchPadRead(touch_pad_t padNumber)
{
    uint16_t touchValue;

    // Start a manual measurement if software mode
    touch_pad_sw_start();

    while (!touch_pad_meas_is_done())
    {
        ;
    }

    // If we are filtering, the function to call to read the data is different
    if (isFilterOn)
    {
        touch_pad_read_filtered(padNumber, &touchValue);
    }
    else
    {
        touch_pad_read(padNumber, &touchValue);
    }

    // Do we have to reset the event
    if (touchValue > thresholds[padNumber] && touchValues[padNumber])
    {
        PostManagedEvent(EVENT_TOUCH, 0, padNumber, 0);
        touchValues[padNumber] = false;
    }

    lastTouchValues[padNumber] = touchValue;

    return touchValue;
}

static void ReadTask(void *pvParameter)
{
    while (isTimeModeOn)
    {
        for (int i = 0; i < TOUCH_PAD_MAX; i++)
        {
            if (isTouchPadUsed[i])
            {
                TouchPadRead((touch_pad_t)i);
            }
        }

        // TODO: find the best value for this task. Different samples
        // shows different values from 10 to 200.
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

#endif

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::NativeRead___I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    touch_pad_t padNumber;
    uint16_t touchValue;
    NANOCLR_CHECK_HRESULT(GetAndCheckTouchPadNumber(stack, &padNumber));

    // If we already have the background task running
    if (isTimeModeOn)
    {
        touchValue = lastTouchValues[padNumber];
    }
    else
    {
        touchValue = TouchPadRead(padNumber);
    }

    stack.SetResult_I4(touchValue);

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeGetThreshold___U2(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    touch_pad_t padNumber;
    NANOCLR_CHECK_HRESULT(GetAndCheckTouchPadNumber(stack, &padNumber));

    uint16_t threshold;
    if (touch_pad_get_thresh(padNumber, &threshold) != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    // Make sure we store it for later usage
    thresholds[padNumber] = threshold;
    stack.SetResult_U2(threshold);

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeSetThreshold___VOID__U2(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    touch_pad_t padNumber;
    uint16_t threshold;

    NANOCLR_CHECK_HRESULT(GetAndCheckTouchPadNumber(stack, &padNumber));

    // Non static function, Arg1 is the first argument
    threshold = (uint16_t)stack.Arg1().NumericByRef().u2;
    if (touch_pad_set_thresh(padNumber, threshold) != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    // Make sure we store it for later usage
    thresholds[padNumber] = threshold;

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeGetTouchSpeed___VOID__BYREF_I4__BYREF_I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    touch_pad_t padNumber;
    touch_cnt_slope_t slope;
    touch_tie_opt_t opt;

    NANOCLR_CHECK_HRESULT(GetAndCheckTouchPadNumber(stack, &padNumber));
    if (touch_pad_get_cnt_mode(padNumber, &slope, &opt) != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    // Arg1 as we are in a non static function.
    CLR_RT_HeapBlock bhSlope;
    CLR_RT_HeapBlock bhOpt;
    bhSlope.SetInteger(slope);
    NANOCLR_CHECK_HRESULT(bhSlope.StoreToReference(stack.Arg1(), 0));
    bhOpt.SetInteger(opt);
    NANOCLR_CHECK_HRESULT(bhOpt.StoreToReference(stack.Arg2(), 0));

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeSetChargeSpeed___VOID__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    touch_pad_t padNumber;
    touch_cnt_slope_t slope;
    touch_tie_opt_t opt;

    NANOCLR_CHECK_HRESULT(GetAndCheckTouchPadNumber(stack, &padNumber));
    // Non static function, Arg1 is the first argument
    slope = (touch_cnt_slope_t)stack.Arg1().NumericByRef().u4;
    opt = (touch_tie_opt_t)stack.Arg2().NumericByRef().u4;
    if (touch_pad_set_cnt_mode(padNumber, slope, opt) != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeCalibrate___I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    touch_pad_t padNumber;
    int count;
    double average = 0;

    NANOCLR_CHECK_HRESULT(GetAndCheckTouchPadNumber(stack, &padNumber));
    // Non static function, Arg1 is the first argument
    count = (int)stack.Arg1().NumericByRef().s4;

    // We will read the values and make an average
    for (int i = 0; i < count; i++)
    {
        average = (average * i + TouchPadRead(padNumber)) / (i + 1);
    }

    // And we return the average
    stack.SetResult_I4((int)average);

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeGetGpioNumberFromTouchNumber___STATIC__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    int padNumber = (int)(stack.Arg0().NumericByRef().u4);
    int gpioNumber = -1;

    // Let's make sure it's a proper pin
    if ((padNumber < 0) || (padNumber >= TOUCH_PAD_MAX))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    /*!< Touch pad channel 0 is GPIO4(ESP32) */
    /*!< Touch pad channel 1 is GPIO0(ESP32) / GPIO1(ESP32-S2) */
    /*!< Touch pad channel 2 is GPIO2(ESP32) / GPIO2(ESP32-S2) */
    /*!< Touch pad channel 3 is GPIO15(ESP32) / GPIO3(ESP32-S2) */
    /*!< Touch pad channel 4 is GPIO13(ESP32) / GPIO4(ESP32-S2) */
    /*!< Touch pad channel 5 is GPIO12(ESP32) / GPIO5(ESP32-S2) */
    /*!< Touch pad channel 6 is GPIO14(ESP32) / GPIO6(ESP32-S2) */
    /*!< Touch pad channel 7 is GPIO27(ESP32) / GPIO7(ESP32-S2) */
    /*!< Touch pad channel 8 is GPIO33(ESP32) / GPIO8(ESP32-S2) */
    /*!< Touch pad channel 9 is GPIO32(ESP32) / GPIO9(ESP32-S2) */
    /*!< Touch channel 10 is GPIO10(ESP32-S2) */
    /*!< Touch channel 11 is GPIO11(ESP32-S2) */
    /*!< Touch channel 12 is GPIO12(ESP32-S2) */
    /*!< Touch channel 13 is GPIO13(ESP32-S2) */
    /*!< Touch channel 14 is GPIO14(ESP32-S2) */

    // This is the ESP32-S2 series
    if (SOC_TOUCH_SENSOR_NUM > 10)
    {
        // In this case it's simple
        gpioNumber = padNumber;
    }
    else
    {
        // This is traditional ESP32
        switch (padNumber)
        {
            case TOUCH_PAD_NUM0:
                gpioNumber = 4;
                break;

            case TOUCH_PAD_NUM1:
                gpioNumber = 0;
                break;

            case TOUCH_PAD_NUM2:
                gpioNumber = 2;
                break;

            case TOUCH_PAD_NUM3:
                gpioNumber = 15;
                break;

            case TOUCH_PAD_NUM4:
                gpioNumber = 13;
                break;

            case TOUCH_PAD_NUM5:
                gpioNumber = 12;
                break;

            case TOUCH_PAD_NUM6:
                gpioNumber = 14;
                break;

            case TOUCH_PAD_NUM7:
                gpioNumber = 27;
                break;

            case TOUCH_PAD_NUM8:
                gpioNumber = 33;
                break;

            case TOUCH_PAD_NUM9:
                gpioNumber = 32;
                break;
        }
    }

    stack.SetResult_I4(gpioNumber);

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeGetTriggerMode___STATIC__nanoFrameworkHardwareEsp32TouchTouchTriggerMode(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    // For static functions, we need to make sure that we have the touch pad initialized
    MakeSureTouchIsInitialized();

    touch_trigger_mode_t triggerMode;
    if (touch_pad_get_trigger_mode(&triggerMode) != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    stack.SetResult_I4(triggerMode);

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeSetTriggerMode___STATIC__VOID__nanoFrameworkHardwareEsp32TouchTouchTriggerMode(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    // For static functions, we need to make sure that we have the touch pad initialized
    MakeSureTouchIsInitialized();
    touch_trigger_mode_t triggerMode = (touch_trigger_mode_t)stack.Arg0().NumericByRef().s4;

    if (touch_pad_set_trigger_mode(triggerMode) != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeGetTriggerSource___STATIC__nanoFrameworkHardwareEsp32TouchWakeUpSource(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    // For static functions, we need to make sure that we have the touch pad initialized
    MakeSureTouchIsInitialized();
    touch_trigger_src_t triggerSource;
    if (touch_pad_get_trigger_source(&triggerSource) != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    stack.SetResult_I4(triggerSource);

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeSetTriggerSource___STATIC__VOID__nanoFrameworkHardwareEsp32TouchWakeUpSource(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    // For static functions, we need to make sure that we have the touch pad initialized
    MakeSureTouchIsInitialized();
    touch_trigger_src_t triggerSource = (touch_trigger_src_t)stack.Arg0().NumericByRef().s4;
    if (touch_pad_set_trigger_source(triggerSource) != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeGetMeasurementMode___STATIC__nanoFrameworkHardwareEsp32TouchMeasurementMode(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    // For static functions, we need to make sure that we have the touch pad initialized
    MakeSureTouchIsInitialized();

    stack.SetResult_I4(measurementMode);

    NANOCLR_NOCLEANUP_NOLABEL();

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
    NANOCLR_NOCLEANUP();
#endif

}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeSetMeasurementMode___STATIC__VOID__nanoFrameworkHardwareEsp32TouchMeasurementMode(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    // For static functions, we need to make sure that we have the touch pad initialized
    MakeSureTouchIsInitialized();
    measurementMode = (touch_fsm_mode_t)stack.Arg0().NumericByRef().s4;

    // TODO: once we'll use IDF 5.0 or a version where the FSM start is present
    // This section will have to be adjusted.

    // Do we need to stop the task?
    if (measurementMode == TOUCH_FSM_MODE_SW)
    {
        isTimeModeOn = false;
    }

    // As the ESP32 in this current version of IDF does not have a task to start the measurement,
    // we do it in a task and don't change the mode.
    if (measurementMode == TOUCH_FSM_MODE_TIMER)
    {
        if (!isTimeModeOn)
        {
            isTimeModeOn = true;
            // Start a task to show what pads have been touched
            xTaskCreate(&ReadTask, "ReadTask", 4096, NULL, 5, NULL);
        }
    }

    NANOCLR_NOCLEANUP_NOLABEL();

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
    NANOCLR_NOCLEANUP();
#endif

}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeSetVoltage___STATIC__VOID__nanoFrameworkHardwareEsp32TouchTouchHighVoltage__nanoFrameworkHardwareEsp32TouchTouchLowVoltage__nanoFrameworkHardwareEsp32TouchTouchHighVoltageAttenuation(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    // For static functions, we need to make sure that we have the touch pad initialized
    MakeSureTouchIsInitialized();
    touch_high_volt_t refh = (touch_high_volt_t)stack.Arg0().NumericByRef().s4;
    touch_low_volt_t refl = (touch_low_volt_t)stack.Arg1().NumericByRef().s4;
    touch_volt_atten_t atten = (touch_volt_atten_t)stack.Arg2().NumericByRef().s4;

    if (touch_pad_set_voltage(refh, refl, atten) != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeGetVoltage___STATIC__VOID__BYREF_nanoFrameworkHardwareEsp32TouchTouchHighVoltage__BYREF_nanoFrameworkHardwareEsp32TouchTouchLowVoltage__BYREF_nanoFrameworkHardwareEsp32TouchTouchHighVoltageAttenuation(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    touch_high_volt_t refh;
    touch_low_volt_t refl;
    touch_volt_atten_t atten;

    // Get the voltage
    if (touch_pad_get_voltage(&refh, &refl, &atten) != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    CLR_RT_HeapBlock bhRefh;
    CLR_RT_HeapBlock bhRefl;
    CLR_RT_HeapBlock bhAtten;
    bhRefh.SetInteger(refh);
    NANOCLR_CHECK_HRESULT(bhRefh.StoreToReference(stack.Arg0(), 0));
    bhRefl.SetInteger(refl);
    NANOCLR_CHECK_HRESULT(bhRefl.StoreToReference(stack.Arg1(), 0));
    bhAtten.SetInteger(atten);
    NANOCLR_CHECK_HRESULT(bhAtten.StoreToReference(stack.Arg2(), 0));

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeStartFilter___STATIC__VOID__U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    // Static function, argument 0 is the first argument
    uint32_t period = (uint32_t)stack.Arg0().NumericByRef().u4;

    if (touch_pad_filter_start(period) != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    isFilterOn = true;
    
#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeStopFilter___STATIC__VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    touch_pad_filter_stop();
    isFilterOn = false;
    NANOCLR_NOCLEANUP_NOLABEL();

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
    NANOCLR_NOCLEANUP();
#endif

}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeSetMeasurementTime___STATIC__VOID__U2__U2(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    // Static function, argument 0 is the first argument
    uint16_t sleep = (uint32_t)stack.Arg0().NumericByRef().u2;
    uint16_t meas = (uint32_t)stack.Arg1().NumericByRef().u2;

    if (touch_pad_set_meas_time(sleep, meas) != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeGetMeasurementTime___STATIC__VOID__BYREF_U2__BYREF_U2(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    // Static function, argument 0 is the first argument
    uint16_t sleep;
    uint16_t meas;

    if (touch_pad_get_meas_time(&sleep, &meas) != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    // Arg0 as we are in a non static function.
    CLR_RT_HeapBlock bhSleep;
    CLR_RT_HeapBlock bhMeas;
    bhSleep.SetInteger(sleep);
    NANOCLR_CHECK_HRESULT(bhSleep.StoreToReference(stack.Arg0(), 0));
    bhMeas.SetInteger(meas);
    NANOCLR_CHECK_HRESULT(bhMeas.StoreToReference(stack.Arg1(), 0));

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}
